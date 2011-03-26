<?php

/**
 * @file
 * Admin form handling for the DART module.
 */

/**
 * Form builder; Configure dart settings for this site.
 *
 * @ingroup forms
 */
function dart_admin_settings() {
  $form['#validate'][] = 'dart_admin_settings_validate';
  
  // Default tag settings.
  $form['default_tag_settings'] = array(
    '#theme'        => 'dart_global_defaults_form',
    '#description'  => 'Enter the global values that DART tags should use for prefix, site & zone. For example, groups.drupal.org might use gdo.group/general; drupal.org might use drupal.default (notice there is no zone); '
  );
  $form['default_tag_settings']['dart_global_prefix'] = array(
    '#type'           => 'textfield',
    '#title'          => t('DART prefix'),
    '#default_value'  => variable_get('dart_global_prefix', ''),
    '#required'       => TRUE,
    '#description'    => t('Typically this is a short version of your organization\'s name (provided by Doubleclick).'),
    '#maxlength'      => 32,
  );
  $form['default_tag_settings']['dart_global_site'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Default DART site'),
    '#default_value'  => variable_get('dart_global_site', ''),
    '#required'       => TRUE,
    '#description'    => t('The value to use when no "site" is specified on a given page.'),
    '#maxlength'      => 32,
  );
  $form['default_tag_settings']['dart_global_zone'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Default DART zone'),
    '#default_value'  => variable_get('dart_global_zone', ''),
    '#required'       => TRUE,
    '#description'    => t('The value to use when no "zone" is specified on a given page.'),
    '#maxlength'      => 32,
  );
  $form['dart_global_slug'] = array(
    '#type'           => 'textfield',
    '#title'          => t('Global Slug'),
    '#default_value'  => variable_get('dart_global_slug', '<none>'),
    '#required'       => FALSE,
    '#description'    => t('Slug all ad tags with the following with this label. Use @tag for no slug. Example: Advertisement', array('@tag' => '<none>')),
    '#maxlength'      => 64,
  );

  // Global display options.
  $form['display_options'] = array(
    '#type'         => 'fieldset',
    '#title'        => t('Global Display Options'),
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
  );
  $form['display_options']['dart_noscript'] = array(
    '#type'           => 'checkbox',
    '#title'          => t('display @tag tags', array('@tag' => '<noscript>')),
    '#default_value'  => variable_get('dart_noscript', '1'),
    '#description'    => t('Display @tag tags along with the standard javascript DART tags.', array('@tag' => '<noscript>')),
  );

  // Gloabal key/val pairs
  $form['global_key_vals'] = array(
    '#type'         => 'fieldset',
    '#title'        => t('Global Key|Value Pairs'),
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
  );
  $form['global_key_vals']['dart_special_key_vals'] = array(
    '#type'           => 'checkboxes',
    '#title'          => t('Special key|val pairs'),
    '#default_value'  => variable_get('dart_special_key_vals', array('tile' => 'tile', 'ord' => 'ord')),
    '#options'        => array(
                          'tile' => t('tile (value auto-increments on each tag)'),
                          'ord'  => t('ord (value changes on each page request)')),
    '#description' => t('Include these special values in all of your ad tags.'),
  );

  // Container for the key_value forms.
  ctools_include('plugins');
  ctools_plugin_load_class('ctools', 'export_ui', 'dart_ctools_export_ui', 'handler');
  $key_vals = variable_get('dart_key_vals', array());
  $key_val_count = max(DART_MAX_KEY_VALS, count($key_vals) + 5);
  $form['global_key_vals']['dart_kvs'] = array(
    '#theme'  => 'dart_key_vals_form',
  );
  for ($i = 0; $i < $key_val_count; $i++) {
    $form['global_key_vals']['dart_kvs'][$i] = _dart_key_val_form($i, $key_vals[$i]);
  }

  // Javascript.
  $form['javascript'] = array(
    '#type'         => 'fieldset',
    '#title'        => t('Inject Javascript'),
    '#collapsible'  => TRUE,
    '#collapsed'    => FALSE,
    '#weight'       => 20,
  );
  $form['javascript']['dart_js'] = array(
    '#type'           => 'textarea',
    '#title'          => t('Inject javascript'),
    '#description'    => t('Inject this javascript into the @tag on each page request. You do not need to add code to create a random number for use as the ORD if you checked "Include ord value" above. This javascript will be injected above the key|value pairs you enter in the field below, so you can declare variables here that you will include as part of a key|value pair there.', array('@tag' => '<head>')),
    '#default_value'  => variable_get('dart_js', ''),
    '#rows'           => 5,
  );

  // Add vertical tabs display if available.
  $form['#pre_render'][] = 'vertical_tabs_form_pre_render';

  return system_settings_form($form);
}

function dart_admin_settings_validate($form, &$form_state) {
  // Validate "key_vals" before its name is changed.
  _dart_key_val_form_validate($form, $form_state);

  // Rename the key_vals value to dart_key_vals to avoid namespace conflicts.
  $form_state['values']['dart_key_vals'] = $form_state['values']['key_vals'];
  unset($form_state['values']['key_vals']);
}

/**
 * Theme the dart settings form for global prefix.site/zone.
 */
function theme_dart_global_defaults_form($form) {
  // Removie titles.
  unset($form['dart_global_prefix']['#title'], $form['dart_global_site']['#title'], $form['dart_global_zone']['#title']);
  // Set widths.
  $form['dart_global_prefix']['#attributes']['style'] = 'width: 60px;';
  $form['dart_global_site']['#attributes']['style'] = 'width: 200px;';
  $form['dart_global_zone']['#attributes']['style'] = 'width: 200px;';

  $replacements = array(
    '!prefix' => drupal_render($form['dart_global_prefix']),
    '!site' => drupal_render($form['dart_global_site']),
    '!zone' => drupal_render($form['dart_global_zone']),
  );
  $form['dart_global_prefix_sit_zone'] = array(
    '#type'   => 'markup',
    "#prefix" => '<div class="form-item container-inline"><label>prefix . site / zone</label><br/>',
    "#suffix" => '<p class="description">' . $form['#description'] . '</p></div>',
    '#value'  => t('!prefix . !site / !zone', $replacements),
  );
  unset($form['dart_global_prefix']);
  unset($form['dart_global_site']);
  unset($form['dart_global_zone']);

  return drupal_render($form);
}
