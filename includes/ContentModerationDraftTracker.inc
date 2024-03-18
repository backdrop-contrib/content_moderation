<?php

/**
 * Handles tracking, selecting and publishing revisions.
 */
class ContentModerationDraftTracker {

  /**
   * A list of entity types, ids and version IDs to be published.
   */
  protected $revisionsToPublish = array();

  public function wasRevisionRequested($entity) {
    return isset($entity->_content_moderation_revision_requested) && $entity->_content_moderation_revision_requested === FIELD_LOAD_REVISION;
  }

  public function isDraftRevision($entity) {
    return !empty($entity->is_active_revision);
  }

  /**
   * Get the current published revision for an entity.
   *
   * @param $type
   *   The entity type.
   * @param $id
   *   The entity ID.
   *
   * @return A version ID.
   */
  public function getPublishedRevisionId($type, $id) {
    $info = entity_get_info();
    // Get the version ID of the published revision directly from the database.
    // It is not possible to rely on $entity->original here since that does not
    // guarantee being the published revision. Also avoid loading the entity
    // because we may be in the process of saving it.
    $query = db_select($info[$type]['base table'], 'b');
    $query->addField('b', $info[$type]['entity keys']['revision']);
    $query->condition($info[$type]['entity keys']['id'], $id);
    $vid = $query->execute()->fetchField();

    return $vid;
  }

  /**
   * Add a revision to be published to the tracker.
   *
   * @param $type
   *   The entity type.
   * @param $id
   *   The entity ID.
   * @param $vid
   *   The entity version ID.
   *
   * @return $this
   */
  public function setRevisionToBePublished($type, $id, $vid) {
    // Only one revision can be published during a request, so just overwrite
    // and for now last one wins.
    $this->revisionsToPublish[$type][$id] = $vid;

    return $this;
  }

  /**
   * Publish a revision.
   *
   * @param $type
   *   The entity type.
   * @param $vid
   *   The entity version ID.
   *
   * @return The newly published revision.
   */
  function publishRevision($type, $id, $vid) {
    // Title module assumes that the current content language is used when
    // saving an entity. This is OK for the new draft revision, but it does not
    // work when publishing a revision. Therefore, ensure that
    // title_active_language() reflects the original language of the entity.
    // Without this, title may overwrite {$title}_field in the original language
    // with the contents of the legacy field.
    // @todo: this might not be necessary after one or both of these patches
    // lands:
    // https://www.drupal.org/node/2267251
    // https://www.drupal.org/node/2098097
    if (module_exists('title')) {
      $entity = entity_load($type, $id);
      $langcode = entity_language($type, $entity);
      title_active_language($langcode);
    }
    $revision = reset(entity_load_multiple($type, array(), array('vid' => $vid)));

    // Publishing a revision sometimes happens within hook_entity_update(). When
    // we do that, set $entity->original to the entity we're in the process of
    // saving. i.e. the draft we're in the process of creating and need to
    // replace with the published version again.
    $revision->is_active_revision = TRUE;

    return $this->saveRevisionAsNew($type, $revision);
  }

  /**
   * Save a revision as new.
   *
   * @param $type
   *   The entity type.
   * @param $revision
   *   An entity object.
   *
   * @return The newly saved revision.
   */
  public function saveRevisionAsNew($type, $revision) {
    list($id) = entity_extract_ids($type, $revision);
    entity_get_controller($type)->resetCache();
    $original = entity_load($type, $id);
    $revision->original = $original;
    // @todo: entity API function?
    $revision->revision = TRUE;
    $revision->is_new_revision = TRUE;
    $revision->default_revision = TRUE;
    $revision->save();

    return $revision;
  }

  /**
   * Publish revisions previously set with setRevisionToBePublished().
   */
  public function restorePublishedRevisions() {
    $delete_old_revisions = settings_get('content_moderation_delete_old_revisions', FALSE);
    $delete_with_cron = settings_get('content_moderation_delete_with_cron', TRUE);
    $queue = NULL;
    $operations = array();
    foreach ($this->revisionsToPublish as $type => $value) {
      foreach ($value as $id => $vid) {
        unset($this->revisionsToPublish[$type][$id]);
        $published_revision = $this->publishRevision($type, $id, $vid);
        // Now that the revision is deleted, there are two identical copies of
        // the revision in the system. The original 'draft' revision and the
        // newly saved published revision.
        if ($delete_old_revisions) {
          list (, $replaced_by) = entity_extract_ids($type, $published_revision);
          // Deletion must be done on a different request thread, or any other
          // hooks called for the revision-to-be-deleted will fail.
          $content_moderation_queue_item = array(
            'entity_type' => $type,
            'entity_id' => $id,
            'revision_id' => $vid,
            'replaced_by' => $replaced_by,
          );
          if ($delete_with_cron) {
            if (empty($queue)) {
              $queue = DrupalQueue::get('content_moderation_revision_delete');
            }
            $queue->createItem($content_moderation_queue_item);
          }
          else {
            $operations[] = array(
              'content_moderation_queue_delete_revision',
              array($content_moderation_queue_item),
            );
          }
        }
        // @todo: when restoring a published revision, should the revision
        // timestamp be set to the old value?
      }
    }
    if (!empty($operations)) {
      $batch = array(
        'operations' => $operations,
        'finished' => 'content_moderation_delete_batch_finished',
      );
      batch_set($batch);
    }
  }

}
