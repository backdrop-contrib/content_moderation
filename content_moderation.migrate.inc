<?php
/**
 * @file
 * Handlers for Migrate module.
 */

/**
 * Provide a destination handler for Migrate module.
 */
class ContentModerationMigrateDestinationHandler extends MigrateDestinationHandler {

  /**
   * @inheritdoc
   */
  public function __construct() {}

  /**
   * @inheritdoc
   */
  public function handlesType($destination) {
    return ($destination == 'Node');
  }

  /**
   * @inheritdoc
   */
  public function fields($entity_type, $bundle_type) {
     $fields = array();

    if (content_moderation_node_type_moderated($bundle_type)) {
      $fields['content_moderation_state_new'] = t('Moderation state');
    }
    return $fields;
  }
}
