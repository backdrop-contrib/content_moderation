<?php

/**
 * @file
 * Internationalization functionality.
 */

/**
 * Translate state label
 *
 * @param string $machine_name
 * @param string $label
 *
 * @return string
 */
function content_moderation_translate_state_label($machine_name, $label) {
  return i18n_string_translate(array(
    'content_moderation',
    'moderation_state',
    $machine_name,
    'label',
  ), $label);
}
