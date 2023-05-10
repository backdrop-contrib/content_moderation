<?php
/**
 * @file
 * API documentation file for Content Moderation.
 */

/**
 * Allows modules to alter moderation access.
 *
 * @param bool &$access
 *   A boolean access declaration. Passed by reference.
 * @param string $op
 *   The operation being performed. May be 'view', 'update', 'delete',
 *   'view revisions' or 'moderate'.
 * @param Node $node
 *   The node being acted upon.
 */
function hook_content_moderation_access_alter(&$access, $op, Node $node) {
  global $user;
  // If the node is marked private, only let its owner moderate it.
  if (empty($node->private) || $op != 'moderate') {
    return;
  }
  if ($user->uid != $node->uid) {
    $access = FALSE;
  }
}

/**
 * Allows modules to alter the list of possible next states for a node.
 *
 * @param array &$states
 *   An array of possible state changes, or FALSE if none were found before
 *   invoking this hook. Passed by reference.
 * @param string $current_state
 *   The current moderation state.
 * @param array $context
 *   An associative array containing:
 *   - 'account': The user object being checked.
 *   - 'node': The node object being acted upon.
 *
 * @see content_moderation_states_next()
 */
function hook_content_moderation_states_next_alter(array &$states, $current_state, array $context) {
  // Do not permit users to give final approval to their own nodes, even if
  // they would otherwise have rights to do so.
  $published = content_moderation_state_published();
  if (isset($states[$published]) && ($context['account']->uid == $context['node']->uid)) {
    unset($states[$published]);
  }
}

/**
 * Allows modules to respond to state transitions.
 *
 * @param Node $node
 *   The node that is being transitioned.
 * @param string $previous_state
 *   The state of the revision before the transition occurred.
 * @param string $new_state
 *   The new state of the revision.
 */
function hook_content_moderation_transition(Node $node, $previous_state, $new_state) {
  // Your code here.
}

/**
 * Allows modules to respond when a transition is saved.
 *
 * @param array $state
 *   The state which was just saved.
 */
function hook_content_moderation_state_save(array $state) {
  // Add data to a custom table for each new transition.
  db_insert('mytable')
    ->fields(array(
      'state' => $state['name'],
    ))
    ->execute();
}

/**
 * Allows modules to respond when a transition is saved.
 *
 * @param array $transition
 *   The transition which was just saved.
 */
function hook_content_moderation_transition_save(array $transition) {
  // Add data to a custom table for each new transition.
  db_insert('mytable')
    ->fields(array(
      'from_state' => $transition['from_name'],
      'to_state' => $transition['to_name'],
    ))
    ->execute();
}

/**
 * Allows modules to respond when a state is deleted.
 *
 * @param array $state
 *   The state which was just deleted.
 */
function hook_content_moderation_state_delete(array $state) {
  // Remove data from a custom table which refers to the old state.
  db_delete('mytable')
    ->condition('state', $state['name'])
    ->execute();
}

/**
 * Allows modules to respond when a transition is deleted.
 *
 * @param array $transition
 *   The transition which was just deleted.
 */
function hook_content_moderation_transition_delete(array $transition) {
  // Remove data from a custom table which refers to the old state.
  db_delete('mytable')
    ->condition('from_state', $transition['from_name'])
    ->condition('to_state', $transition['to_name'])
    ->execute();
}
