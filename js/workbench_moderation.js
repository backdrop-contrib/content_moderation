/**
 * @file
 * Sets the summary for Workbench moderation on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.workbenchModerationSettingsSummary = {
  attach: function(context) {
    var $context = $(context);
    $context.find('fieldset#edit-workbench-moderation').backdropSetSummary(function () {
      var vals = [];

      if ($context.find('input[name="moderation_enabled"]:checked').length) {
        vals.push(Backdrop.t('Moderation enabled'));
      }
      else {
        vals.push(Backdrop.t('Moderation disabled'));
      }

      if ($('select[name="workbench_moderation_state_new"]', context).val()) {
        vals.push(Backdrop.checkPlain($('select[name="workbench_moderation_state_new"] option:selected').text()));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
