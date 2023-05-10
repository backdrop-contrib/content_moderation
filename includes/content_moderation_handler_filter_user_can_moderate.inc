<?php
/**
 * @file
 *  Provides moderation filters for Views.
 */

/**
 * Filter based on moderation privileges.
 */
class content_moderation_handler_filter_user_can_moderate extends views_handler_filter {
  /**
   * @inheritdoc
   */
  public function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    backdrop_set_message(t("This filter isn't even possible right now since content moderation permissions are incomplete--there's no way to figure out what transitions a user may make for a particular type of content."), 'error');
  }

  /**
   * @inheritdoc
   */
  public function query() {
    // Add table, add node table, add where statments like "(n.type = 'blog' AND
    // moderation.state IN ('review', 'approve')) OR (n.type = 'article' AND
    // moderation.state IN ('review'))"
    // $this->ensure_my_table();
    // $node_alias = $this->query->ensure_table('node');
    // $this->query->add_where();
  }
}
