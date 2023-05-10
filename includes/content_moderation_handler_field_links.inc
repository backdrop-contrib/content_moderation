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
      $node = node_load($values->{$this->aliases['nid']}, $values->{$this->aliases['vid']});
      return theme('links', array(
        'links' => content_moderation_get_moderation_links($node,
          array(
            'html' => TRUE,
            'query' => array('destination' => $_GET['q']),
          ),
        ),
      ));
    }
    return '';
  }
}
