<?php
/**
 * @file
 * Views: Filter based on moderation state.
 */

/**
 * Filter based on moderation state.
 */
class content_moderation_handler_filter_state extends views_handler_filter_in_operator {
  /**
   * @inheritdoc
   */
  public function get_value_options() {
    if (!isset($this->value_options)) {
      $this->value_title = t('Moderation status');
      $this->value_options = array_map('check_plain', content_moderation_state_labels());
    }
  }
}
