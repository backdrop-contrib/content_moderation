<?php

class ContentModerationMigrateDestinationHandler extends MigrateDestinationHandler {

  public function __construct() {}

  public function handlesType($destination) {
    return ($destination == 'Node');
  }

  public function fields($entity_type, $bundle_type) {
     $fields = array();

    if (content_moderation_node_type_moderated($bundle_type)) {
      $fields['content_moderation_state_new'] = t('Moderation state');
    }
    return $fields;
  }
}
