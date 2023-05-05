/**
 * @file
 * Sets the summary for Workbench moderation on vertical tabs.
 */

(function ($) {

Backdrop.behaviors.workbenchModerationSettingsSummary = {
  attach: function(context) {
    $('fieldset.node-form-options', context).backdropSetSummary(function (context) {
      var vals = [];

      $('input:checked', context).parent().each(function () {
        vals.push(Backdrop.checkPlain($.trim($(this).text())));
      });

      if ($('select[name="workbench_moderation_state_new"]', context).val()) {
        vals.push(Backdrop.checkPlain($('select[name="workbench_moderation_state_new"] option:selected').text()));
      }
      return vals.join(', ');
    });
  }
};

})(jQuery);
