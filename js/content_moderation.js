/**
 * @file
 * Sets the summary for Content moderation on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.contentModerationSettingsSummary = {
  attach: function(context) {
    var $context = $(context);
    $context.find('fieldset#edit-content-moderation').backdropSetSummary(function () {
      var vals = [];

      if ($context.find('input[name="moderation_enabled"]:checked').length) {
        vals.push(Backdrop.t('Moderation enabled'));
      }
      else {
        vals.push(Backdrop.t('Moderation disabled'));
      }

      if ($('select[name="content_moderation_state_new"]', context).val()) {
        vals.push(Backdrop.checkPlain($('select[name="content_moderation_state_new"] option:selected').text()));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
