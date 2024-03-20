<?php
/**
 * @file
 *  Content moderation views integration for Content.
 */

/**
 * Implements hook_views_data().
 */
function content_moderation_views_data() {
  $data = array();

  $data['content_moderation_node_history']['table']['group'] = t('Content Moderation');
  $data['content_moderation_node_history']['table']['join'] = array(
    'node' => array(
      'left_field' => 'nid',
      'field' => 'nid',
      'type' => 'INNER',
    ),
    'node_revision' => array(
      'left_field' => 'vid',
      'field' => 'vid',
      'type' => 'INNER',
    ),
  );

  $data['content_moderation_node_history']['hid'] = array(
    'title' => t('Moderation history ID'),
    'help' => t('Content moderation history record ID.'),
    'filter' => array(
      'handler' => 'views_handler_filter',
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument',
    ),
  );

  $data['content_moderation_node_history']['nid'] = array(
    'title' => t('Node'),
    'help' => t('The node according to the Content Moderation.'),
    'relationship' => array(
      'base' => 'node',
      'field' => 'nid',
      'handler' => 'views_handler_relationship',
      'label' => t('Node'),
    ),
  );

  $data['content_moderation_node_history']['state'] = array(
    'title' => t('State'),
    'help' => t('Content moderation state of the node revisision.'),
    'filter' => array(
      'handler' => 'content_moderation_handler_filter_state',
    ),
    'field' => array(
      'handler' => 'content_moderation_handler_field_state',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'name table' => 'content_moderation_states',
      'name field' => 'label',
    ),
  );
  $data['content_moderation_node_history']['from_state'] = array(
    'title' => t('From state'),
    'help' => t('Previous content moderation state of the node revisision.'),
    'filter' => array(
      'handler' => 'content_moderation_handler_filter_state',
    ),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_string',
      'name table' => 'content_moderation_states',
      'name field' => 'label',
    ),
  );
  $data['content_moderation_node_history']['uid'] = array(
    'title' => t('User'),
    'help' => t('User who moderated this revision.'),
    'relationship' => array(
      'title' => t('User'),
      'label' => t('moderator user'),
      'base' => 'users',
      'base field' => 'uid',
    ),
  );
  $data['content_moderation_node_history']['stamp'] = array(
    'title' => t('Timestamp'),
    'help' => t('The date this revision was moderated.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  $data['content_moderation_node_history']['published'] = array(
    'title' => t('Published'),
    'help' => t('Whether or not this revision is published.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Published'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['content_moderation_node_history']['is_current'] = array(
    'title' => t('Current'),
    'help' => t('Whether or not this is the active revision.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Current'),
      'type' => 'yes-no',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  $data['content_moderation_node_history']['moderation_actions'] = array(
    'title' => t('Moderation actions'),
    'help' => t('Content moderation state of the node revisision.'),
    'real field' => 'state',
    'field' => array(
      'title' => t('Moderation links'),
      'handler' => 'content_moderation_handler_field_links',
      'click sortable' => FALSE,
      'additional fields' => array('nid', 'vid', 'is_current'),
    ),
    'filter' => array(
      'title' => t('User can moderate'),
      'handler' => 'content_moderation_handler_filter_user_can_moderate',
      'label' => t('Can moderate'),
    ),
  );

  $data['content_moderation_states']['table']['group'] = t('Content Moderation');
  $data['content_moderation_states']['table']['join'] = array(
    // Add a join for labeling the 'state' and 'from_state' arguments on
    // content_moderation_node_history.
    'content_moderation_node_history' => array(
      'left_field' => 'state',
      'field' => 'name',
    ),
    'node' => array(
      'left_table' => 'content_moderation_node_history',
      'left_field' => 'state',
      'field' => 'name',
    ),
    'node_revision' => array(
      'left_table' => 'content_moderation_node_history',
      'left_field' => 'state',
      'field' => 'name',
    ),
  );
  $data['content_moderation_states']['description'] = array(
    'title' => t('Moderation state description'),
    'help' => t('Shows a description of the moderation state.'),
    'field' => array(
      'handler' => 'views_handler_field',
    ),
  );
  $data['content_moderation_states']['weight'] = array(
    'title' => t('Moderation state order'),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );

  // Declare virtual fields on the {node} table.
  $data['node']['content_moderation_moderated_type'] = array(
    'group' => t('Content Moderation'),
    'title' => t('Type is moderated'),
    'help' => t('Whether the content type is moderated.'),
    'real field' => 'type',
    'filter' => array(
      'handler' => 'content_moderation_handler_filter_moderated_type',
      'label' => t('Type is moderated'),
      'type' => 'yes-no',
    ),
  );
  $data['node']['content_moderation_history_link'] = array(
    'group' => t('Content Moderation'),
    'title' => t('Moderation history'),
    'help' => t('A link to the moderation history page of the node'),
    'real field' => 'nid',
    'field' => array(
      'title' => t('Moderation history link'),
      'handler' => 'content_moderation_handler_field_history_link',
      'click sortable' => FALSE,
    ),
  );

  return $data;
}
