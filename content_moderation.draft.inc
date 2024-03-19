<?php
/**
 * @file
 * Hook implementations and API functions for facilitating the handling of draft
 * revisions, aka forward revisions.
 * Incorporates Drafty module.
 */

/**
 * Rearrange the registry of modules implementing content_moderation hooks.
 *
 * @param array $implementations
 */
function content_moderation_rearrange_module_registry(&$implementations) {
  if (isset($implementations['content_moderation'])) {
    $group = $implementations['content_moderation'];
    unset($implementations['content_moderation']);
    $implementations['content_moderation'] = $group;
  }
}

/**
 * Set revision to publish.
 *
 * @param EntityInterface $entity
 */
function content_moderation_set_revision_to_publish($entity) {
  $recursion_level = &backdrop_static('content_moderation_recursion_level', 0);
  if (!$recursion_level && !$entity->isNew() && !$entity->is_active_revision) {
    // Since this is a draft revision, after saving we want the current,
    // published revision to remain in place in the base entity table and
    // field_data_*() tables. Set the revision to publish once the draft entity
    // has been written to the database.
    $id = $entity->id();
    $type = $entity->entityType();
    $vid = content_moderation_draft_tracker()->getPublishedRevisionId($type, $id);
    content_moderation_draft_tracker()->setRevisionToBePublished($type, $id, $vid);
  }
  $recursion_level++;
}

/**
 * Restore published revision.
 */
function content_moderation_restore_published_revision() {
  $recursion_level = &backdrop_static('content_moderation_recursion_level', 0);
  if ($recursion_level == 1) {
    // Doing this in hook_entity_update() so that the entire process is
    // completed within entity saving. However this results in two entity saves
    // within entity insert. The other option is hook_exit(), which is not
    // better since for example that would happen outside the transaction.
    content_moderation_draft_tracker()->restorePublishedRevisions();
  }
  $recursion_level--;
}

/**
 * Reduce recursion level
 */
function content_moderation_reduce_recursion_level() {
  $recursion_level = &backdrop_static('content_moderation_recursion_level', 0);
  $recursion_level--;
}

/**
 * Get revision requested
 *
 * @param array $entities
 * @param int $age
 *   FIELD_LOAD_CURRENT or FIELD_LOAD_REVISION
 */
function content_moderation_get_revision_requested($entities, $age) {
  // Entity Plus provides the function entity_revision_is_default() to determine
  // whether an entity was loaded with the default revision or not. However this
  // is not sufficient for two reasons.
  //  - It relies on entity_load() for core entities, which makes it unsafe to
  //    call within hook_entity_load() implementations. This can be useful when
  //    allowing drafts to be previewed in context such as listings.
  //  - The Entity Plus implementation only tells you whether the entity was
  //    loaded with that revision or not, but not whether it was requested
  //    with the ID or with the revision explicitly specified.

  // Note that hook_field_attach_load() is the only hook in core where it is
  // possible to determine whether calling code requested a revision or not,
  // this information is not available to hook_entity_load(). Also note that
  // hook_field_attach_load() is cached when entities are loaded only be ID, but
  // since revision loads don't use the field cache it works fine for our
  // purposes.
  foreach ($entities as $entity) {
    $entity->content_moderation_revision_requested = $age;
  }
}

/**
 * Callback function to delete a single temporary-draft revision.
 *
 * Could be called either from a batch job or from a cron-queue.
 */
function content_moderation_queue_delete_revision($content_moderation_queue_item, &$context = NULL) {
  try {
    // Trigger hook_content_moderation_predelete_revision(). Allow other modules to
    // preserve information on the old revision before it's deleted.
    module_invoke_all('content_moderation_predelete_revision', $content_moderation_queue_item['entity_type'], $content_moderation_queue_item['entity_id'], $content_moderation_queue_item['revision_id'], $content_moderation_queue_item['replaced_by']);

    // Delete the old revision.
    entity_plus_revision_delete($content_moderation_queue_item['entity_type'], $content_moderation_queue_item['revision_id']);

    // Document the successful batch operation for the batch-finished function
    // to use.
    if (!empty($context) && isset($context['results'])){
      $context['results'][] = $content_moderation_queue_item;
    }
  }
  catch (Exception $e){
    // Log the problem.
    $message = 'An error occurred while deleting temporary revision: @args. Exception details: @error';
    $args = array(
      '@args' => print_r($content_moderation_queue_item, TRUE),
      '@error' => print_r($e, TRUE),
    );
    watchdog('content_moderation', $message, $args, WATCHDOG_ERROR);
    throw $e;  // Batch API will pass $success = FALSE.
  }
}

/**
 * Add a delete revision cron queue
 *
 * @return array
 *   The cron queue for deleting revisions.
 */
function content_moderation_revision_delete_cron_queue() {
  return array(
    'content_moderation_revision_delete' => array(
      'worker callback' => 'content_moderation_queue_delete_revision',
      'time' => 60,
    )
  );
}

/**
 * Batch completion callback for when draft-deletes happen in a batch job.
 * $results contains info about successfully deleted revisions, and
 * exception objects for revision-delete options that caused problems.
 * $operations contains the operations that remained unprocessed.
 */
function content_moderation_delete_batch_finished($success, $results, $operations) {
  // TODO: Display a message about temporary-revision-cleanup here?
  // Note that logging of failed deletes is handled by the operation callback.
}

/**
 * Factory function for the ContentModerationDraftTracker class.
 */
function content_moderation_draft_tracker() {
  $tracker = &backdrop_static(__FUNCTION__);
  if (!isset($tracker)) {
    $tracker = new ContentModerationDraftTracker();
  }
  return $tracker;
}
