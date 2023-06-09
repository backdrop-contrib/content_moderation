<?php
/**
 * @file
 *  Administrative functions for Content Moderation.
 */

/**
 * Administration form for states.
 *
 * Administrators can use this form to add, delete, reorder, and
 * update the description for states.
 */
function content_moderation_admin_states_form($form, &$form_state) {
  $form['states'] = array(
    '#tree' => TRUE,
  );

  // List existing states.
  $states = content_moderation_states();
  foreach ($states as $state) {
    $form['states'][$state['name']]['state'] = array(
      '#type' => 'value',
      '#value' => $state,
    );
    $form['states'][$state['name']]['name'] = array(
      '#type' => 'value',
      '#value' => $state['name'],
    );
    $form['states'][$state['name']]['label'] = array(
      '#type' => 'textfield',
      '#default_value' => $state['label'],
      '#maxlength' => 255,
      '#size' => 30,
    );
    $form['states'][$state['name']]['machine_name'] = array(
      '#markup' => check_plain($state['name']),
    );
    $form['states'][$state['name']]['description'] = array(
      '#type' => 'textfield',
      '#default_value' => $state['description'],
    );
    $form['states'][$state['name']]['weight'] = array(
      '#type' => 'weight',
      '#default_value' => $state['weight'],
    );
    $form['states'][$state['name']]['delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete'),
      '#title_display' => 'invisible',
      '#default_value' => FALSE,
    );
    if ($state['name'] == content_moderation_state_published() || $state == content_moderation_state_none()) {
      $form['states'][$state['name']]['delete']['#disabled'] = TRUE;
    }
  }

  // Provide fields to create a new state.
  $new_state['label'] = array(
    '#title' => t('New state'),
    '#type' => 'textfield',
    '#description' => t('Enter a name for the new state.'),
    '#maxlength' => 255,
    '#size' => 30,
  );
  $new_state['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 255,
    '#size' => 30,
    '#required' => FALSE,
    '#machine_name' => array(
      'exists' => 'content_moderation_state_load',
      'source' => array('states', '0', 'label'),
    ),
  );
  $new_state['description'] = array(
    '#type' => 'textfield',
    '#maxlength' => 255,
    '#title' => '&nbsp;',
    '#description' => t('Enter a description of the new state.'),
  );
  $new_state['weight'] = array(
    '#type' => 'weight',
  );
  $form['states'][] = $new_state;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Transforms the states administration form into a reorderable table.
 */
function theme_content_moderation_admin_states_form($variables) {
  $form = $variables['form'];

  backdrop_add_tabledrag('content-moderation-states', 'order', 'sibling', 'content-moderation-state-weight');

  $header = array(
    t('Name'),
    t('Machine name'),
    t('Description'),
    array('data' => t('Delete'), 'class' => array('checkbox')),
    t('Weight'),
  );

  $rows = array();
  foreach (element_children($form['states']) as $key) {
    $element = &$form['states'][$key];

    $row = array(
      'data' => array(),
      'class' => array('draggable'),
    );
    $row['data']['label'] = backdrop_render($element['label']);
    $row['data']['name'] = backdrop_render($element['name']) . backdrop_render($element['machine_name']);
    $row['data']['description'] = backdrop_render($element['description']);
    $row['data']['delete'] = backdrop_render($element['delete']);

    $element['weight']['#attributes']['class'] = array('content-moderation-state-weight');
    $row['data']['weight'] = backdrop_render($element['weight']);

    $rows[] = $row;
  }

  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('id' => 'content-moderation-states'),
  ));
  $output .= backdrop_render_children($form);

  return $output;
}

/**
 * Form submit handler for moderation state administration.
 */
function content_moderation_admin_states_form_submit($form, &$form_state) {
  foreach ($form_state['values']['states'] as $key => $info) {
    if (!empty($info['delete'])) {
      content_moderation_state_delete($info['state']);
      backdrop_set_message(t('Moderation state %label (%name) has been deleted.',
        array(
          '%label' => $info['label'],
          '%name' => $info['name'],
        )
      ));
    }
    elseif (!empty($info['name'])) {
      $state = array(
        'name' => $info['name'],
        'label' => $info['label'],
        'description' => $info['description'],
        'weight' => $info['weight'],
      );
      content_moderation_state_save($state);
      if (module_exists('i18n_string')) {
        i18n_string_update(array(
          'content_moderation',
          'moderation_state',
          $info['name'],
          'label',
        ), $info['label']);
        i18n_string_update(array(
          'content_moderation',
          'moderation_state',
          $info['name'],
          'description',
        ), $info['description']);
      }
    }
  }

  backdrop_set_message(t('Your settings have been saved.'));

  // Warn users that changes here may need require reconfiguring Views that use
  // Workbench.
  $views_message = t('Depending on the changes you have made it may be necessary to <a href="@views">reconfigure Views</a> that leverage Content Moderation such as <a href="@content_moderation">content_moderation</a>', array(
    '@views' => url('admin/structure/views'),
    '@content_moderation' => url('admin/structure/views/edit/content_moderation'),
  ));
  backdrop_set_message($views_message, $type = 'warning', $repeat = FALSE);
}

/**
 * Administration form to create and delete moderation transitions.
 *
 * Transition ordering is based on state ordering.
 */
function content_moderation_admin_transitions_form($form, &$form_state) {
  $form['transitions'] = array(
    '#tree' => TRUE,
  );

  // List existing transitions.
  $transitions = content_moderation_transitions();

  foreach ($transitions as $transition) {
    $element = array();

    $element['transition'] = array(
      '#type' => 'value',
      '#value' => $transition,
    );
    $element['name'] = array(
      '#type' => 'value',
      '#value' => $transition['name'],
    );
    $element['label'] = array(
      '#markup' => check_plain($transition['label']),
    );
    $element['machine_name'] = array(
      '#markup' => check_plain($transition['name']),
    );
    $element['from_name'] = array(
      '#markup' => check_plain(content_moderation_state_label($transition['from_name'])),
    );
    $element['to_name'] = array(
      '#markup' => check_plain(content_moderation_state_label($transition['to_name'])),
    );
    $element['delete'] = array(
      '#type' => 'checkbox',
      '#title' => t('Delete'),
      '#title_display' => 'invisible',
      '#default_value' => FALSE,
    );

    $form['transitions'][] = $element;
  }

  // Provide fields to create a new transition.
  $options = content_moderation_state_labels();

  array_unshift($options, t('- Choose state -'));
  $element = array();
  $element['label'] = array(
    '#type' => 'textfield',
    '#title' => t('Transition Label'),
    '#maxlength' => 255,
    '#size' => 30,
  );
  $element['name'] = array(
    '#type' => 'machine_name',
    '#maxlength' => 255,
    '#size' => 30,
    '#required' => FALSE,
    '#machine_name' => array(
      'exists' => 'content_moderation_transition_load',
      'source' => array('transitions', 'new', 'label'),
    ),
  );
  $element['from_name'] = array(
    '#type' => 'select',
    '#title' => t('New transition'),
    '#options' => $options,
  );
  $element['to_name'] = array(
    '#type' => 'select',
    '#title' => '&nbsp;',
    '#options' => $options,
  );
  $form['transitions']['new'] = $element;

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Transforms the transitions administration form into a table.
 */
function theme_content_moderation_admin_transitions_form($variables) {
  $form = $variables['form'];

  $header = array(
    t('Name'),
    t('Machine name'),
    t('From'),
    '',
    t('To'),
    array('data' => t('Delete'), 'class' => array('checkbox')),
  );

  $rows = array();
  foreach (element_children($form['transitions']) as $key) {
    $element = &$form['transitions'][$key];
    $row = array('data' => array());
    $row['data']['label'] = backdrop_render($element['label']);
    $row['data']['name'] = backdrop_render($element['name']) . backdrop_render($element['machine_name']);
    $row['data']['from'] = backdrop_render($element['from_name']);
    $row['data'][] = '&gt;';
    $row['data']['to'] = backdrop_render($element['to_name']);
    $row['data']['delete'] = backdrop_render($element['delete']);
    $rows[] = $row;
  }
  // @TODO: change this css call.
  backdrop_add_css(backdrop_get_path('module', 'content_moderation') . '/css/content_moderation.css');
  $output = theme('table', array(
    'header' => $header,
    'rows' => $rows,
    'attributes' => array('class' => array('width-auto')),
  ));
  $output .= backdrop_render_children($form);

  return $output;
}

/**
 * Form validation handler for the transitions form.
 */
function content_moderation_admin_transitions_form_validate($form, &$form_state) {
  $new = $form_state['values']['transitions']['new'];
  if (!empty($new['from_name']) && !empty($new['to_name'])) {
    if ($new['from_name'] == $new['to_name']) {
      form_set_error('transitions][new', t('To and from states of a new transition must be different.'));
    }
    else {
      foreach (content_moderation_transitions() as $t) {
        if ($new['from_name'] == $t['from_name'] && $new['to_name'] == $t['to_name']) {
          form_set_error('transitions][new', t('This transition already exists.'));
          break;
        }
      }
    }
  }
}

/**
 * Form submit handler for moderation transitions.
 */
function content_moderation_admin_transitions_form_submit($form, &$form_state) {
  foreach ($form_state['values']['transitions'] as $transition) {
    if (!empty($transition['delete'])) {
      content_moderation_transition_delete($transition['transition']);
      backdrop_set_message(t('%from --&gt; %to has been deleted.',
        array(
          '%from' => content_moderation_state_label($transition['transition']['from_name']),
          '%to' => content_moderation_state_label($transition['transition']['to_name']),
        )
      ));
    }
    elseif (!empty($transition['label']) && !empty($transition['from_name']) && !empty($transition['to_name'])) {
      $new_transition = array(
        'name' => $transition['name'],
        'label' => $transition['label'],
        'from_name' => $transition['from_name'],
        'to_name' => $transition['to_name'],
      );
      content_moderation_transition_save($new_transition);
      backdrop_set_message(t('%label: %from --&gt; %to has been created.',
        array(
          '%label' => $new_transition['label'],
          '%from' => content_moderation_state_label($new_transition['from_name']),
          '%to' => content_moderation_state_label($new_transition['to_name']),
        )
      ));
    }
  }
}

/**
 * Check proper permissions for moderation roles.
 *
 * This helper administration form checks whether roles have the appropriate
 * permissions to use moderation as intended.
 */
function content_moderation_admin_check_role_form($form, &$form_state) {
  $form = array();

  $roles = user_roles();
  $form['role'] = array(
    '#type' => 'select',
    '#title' => t('Backdrop role'),
    '#description' => t('Select a role to check.'),
    '#options' => $roles,
  );

  $form['moderation_task'] = array(
    '#type' => 'select',
    '#title' => t('Moderation task'),
    '#description' => t('Select a moderation task that the role should be able to perform.'),
    '#options' => array(
      'author' => t('Author moderated content'),
      'editor' => t('Edit moderated content'),
      'moderator' => t('Moderate content'),
      'publisher' => t('Publish moderated content'),
    ),
  );

  $types = backdrop_map_assoc(content_moderation_moderate_node_types());
  $all_types = node_type_get_types();
  foreach ($types as $type) {
    $types[$type] = $all_types[$type]->name;
  }
  $form['types'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Content types'),
    '#description' => t('Select any content types on which the user should have the ability to perform the moderation task.'),
    '#options' => $types,
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Check permissions'),
  );

  if (empty($types)) {
    backdrop_set_message(t('Moderation is not enabled for any content types. Visit the <a href="@content_type_administration_page">content type administration page</a> to enable moderation for one or more types.', array('@content_type_administration_page' => url('admin/structure/types'))), 'warning');
    $form['#disabled'] = TRUE;
  }

  return $form;
}

/**
 * Form submit handler for moderation role tests.
 */
function content_moderation_admin_check_role_form_submit($form, &$form_state) {
  $rid = $form_state['values']['role'];
  $role = user_role_load($rid);

  $types = array_filter($form_state['values']['types']);

  $moderation_task = $form_state['values']['moderation_task'];
  $recommended_permissions = content_moderation_recommended_permissions($types);
  $recommended_permissions = $recommended_permissions[$moderation_task];

  // Get a full list of this role's permissions.
  $actual_permissions = user_role_permissions(array_filter(array(
    $rid => TRUE,
    BACKDROP_AUTHENTICATED_ROLE => ($rid != BACKDROP_ANONYMOUS_ROLE),
  )));

  // The results of user_role_permissions() are in a nested array:
  // array(rid => array("permission name" => TRUE))
  // Check each for our recommended permissions.
  foreach ($actual_permissions as $permissions) {
    $recommended_permissions = array_diff($recommended_permissions, array_keys(array_filter($permissions)));
  }

  if (empty($recommended_permissions)) {
    // All of the recommended permissions were accounted for.
    backdrop_set_message(t('The @role role should be a qualified @moderation_task.', array(
      '@role' => $role->name,
      '@moderation_task' => $moderation_task,
    )), 'status');
  }
  else {
    // The specified role didn't have some of the recommended permissions. Print
    // a list for the user.
    $all_permissions = module_invoke_all('permission');
    foreach ($recommended_permissions as $permission) {
      backdrop_set_message(t('The @role role may need the "!permission_label" permission in order to be a qualified @moderation_task of @types content.', array(
        '@role' => $role->name,
        '@moderation_task' => $moderation_task,
        '@types' => implode(t(' and '), array(
          implode(', ', array_slice($types, 0, -1)), end($types))
        ), // @TODO: Context senstive translation bug
        '!permission_label' => $all_permissions[$permission]['title'],
      )), 'error');
    }
    // Provide links to node and moderation permissions.
    backdrop_set_message(t('View <a href="@node_permissions">node permissions</a> or <a href="@moderation_permissions">moderation permissions</a> for the @role role.', array(
      '@node_permissions' => url('admin/people/permissions/' . $rid, array('fragment' => 'module-node')),
      '@moderation_permissions' => url('admin/people/permissions/' . $rid, array('fragment' => 'module-content_moderation')),
      '@role' => $role->name,
    )), 'error');

    // Note that we don't cover all configurations.
    backdrop_set_message(t('The @role role may be a qualified @moderation_task regardless of these notices if it has liberal overall node and moderation permissions, like "Administer content" and "Bypass moderation restrictions".', array(
      '@role' => $role->name,
      '@moderation_task' => $moderation_task,
    )), 'warning');
  }

  // Note that we don't cover all configurations.
  backdrop_set_message(t('You must check manually that this role has the appropriate transition permissions for your workflow. <a href="@moderation_permissions_link">View moderation permissions for this role.</a>', array('@moderation_permissions_link' => url('admin/people/permissions/' . $rid, array('fragment' => 'module-content_moderation')))), 'warning');

  // Repopulate the form with the submitted values.
  $form_state['rebuild'] = TRUE;
}

/**
 * Recommended permissions for typical moderation roles.
 */
function content_moderation_recommended_permissions($types = array()) {
  $permissions = array(
    'author' => array(
      // Node.
      "access content",
      "view own unpublished content",
      "view revisions",

      // Content moderation.
      "view moderation messages",
      "use content_moderation my drafts tab",
    ),
    'editor' => array(
      // Node.
      "access content",
      "view revisions",
      "revert revisions",

      // Content.
      "view all unpublished content",

      // Content moderation.
      "view moderation messages",
      "view moderation history",
      "use content_moderation my drafts tab",
      "use content_moderation needs review tab",
    ),
    'moderator' => array(
      // Node.
      "access content",
      "view revisions",

      // Content.
      "view all unpublished content",

      // Content moderation.
      "view moderation messages",
      "view moderation history",
      "use content_moderation needs review tab",
    ),
    'publisher' => array(
      // Node.
      "access content",
      "view revisions",
      "revert revisions",

      // Content.
      "view all unpublished content",

      // Content moderation.
      "view moderation messages",
      "view moderation history",
      "use content_moderation needs review tab",
    ),
  );

  foreach ($types as $type) {
    $permissions['author'][] = "create $type content";
    $permissions['author'][] = "edit own $type content";
    $permissions['editor'][] = "edit any $type content";
    $permissions['moderator'][] = "edit any $type content";
    $permissions['publisher'][] = "edit any $type content";
  }

  return $permissions;
}
