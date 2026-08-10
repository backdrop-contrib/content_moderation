<?php
/**
 * @file
 * Provides moderation fields for Views.
 */

/**
 * Moderation links.
 */
class content_moderation_handler_field_links extends views_handler_field {
  /**
   * @inheritdoc
   */
  public function render($values) {
    if ($values->{$this->aliases['is_current']}) {
      // Get the current row.
      $node = node_load($values->{$this->aliases['nid']}, $values->{$this->aliases['vid']});
      $current_state = $node->content_moderation['current']->state;

      $settings = array(
        'html' => TRUE,
        'query' => array('destination' => $_GET['q']),
      );
      $links = content_moderation_get_moderation_links($node, $settings);

      // Get allowed next links.
      $account = $GLOBALS['user'];
      $next = content_moderation_states_next($current_state, $account, $node);

      // Get all the states (including the current one).
      $all_states = content_moderation_states();

      // Create an order for the drop-button links:
      $start_here = FALSE;
      $ordered_states = array();
      foreach ($all_states as $state => $label) {
        if ($start_here) {
          $ordered_states[$state] = $label;
          unset($all_states[$state]);
        }
        if ($current_state == $state) { // Removes the current state.
          unset($all_states[$state]);
          $start_here = TRUE; // Starts the list of links.
        }
      }
      // Add remanining states to end of list.
      foreach ($all_states as $state => $label) {
        $ordered_states[$state] = $label;
        unset($all_states[$state]);
      }

      // Key all current links by state for easier retreival.
      foreach ($links as $key => $link) {
        $state = $link['state'];
        $links[$state] = $link;
        unset($links[$key]);
      }

      // Create links for the drop-button based on the order of $ordered_states.
      $drop_links = array();
      foreach ($ordered_states as $state => $label) {
        // Since there may be extra states here, check first.
        if (array_key_exists($state, $links)) {
          $drop_links[$state] = $links[$state];
        }
      }

      // Build drop-button links with the most likely next link at top.
      $element['moderation_links'] = array(
        '#type' => 'dropbutton',
        '#links' => $drop_links,
      );

      return backdrop_render($element);
    }
    return '';
  }
}
