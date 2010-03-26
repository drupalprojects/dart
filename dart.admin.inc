<?php
// $Id$

/**
 * Event date format for scheduling DART tags.
 */
define('DART_EVENT_DATE_FORMAT', 'Y-m-d');

/**
 * Form builder; Configure dart settings for this site.
 *
 * @ingroup forms
 */
function dart_admin_settings() {
  $form['dart_prefix'] = array(
    '#type' => 'textfield',
    '#title' => t('DART prefix'),
    '#default_value' => variable_get('dart_prefix',''),
    '#description' => t('Typically a short version of your company/organization\'s name (provided by Doubleclick).'),
    '#maxlength' => 32,
  );
  $form['dart_sitezone'] = array(
    '#type' => 'textfield',
    '#title' => t('Default DART site/zone'),
    '#default_value' => variable_get('dart_sitezone',''),
    '#required' => TRUE,
    '#description' => t('The value to use when no "site/zone" is specified on a given page. Example: sports/yankees.'),
    '#maxlength' => 32,
  );
  $form['dart_slug'] = array(
    '#type' => 'textfield',
    '#title' => t('Slug'),
    '#default_value' => variable_get('dart_slug','<none>'),
    '#required' => FALSE,
    '#description' => t('Slug all ads with the following with this label. Use <none> for no slug. Example: Advertisement'),
    '#maxlength' => 64,
  );
  $form['dart_vals'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Tag defaults'),
    '#default_value' => variable_get('dart_vals',array('tile' => 'tile','ord' => 'ord')),
    '#options' => array(
      'tile'  => 'tile (value auto-increments on each tag)',
      'ord'   => 'ord (value changes on each page request)',
    ),
    '#description' => t('Include these values in all of your ad tags.'),
  );
  $form['dart_noscript'] = array(
    '#type' => 'checkbox',
    '#title' => t('display &lt;noscript&gt; tags'),
    '#default_value' => variable_get('dart_noscript', '1'),
    '#description' => t('Check this to display &lt;noscript&gt; tags along with you javascript DART tags.'),
  );
  $form['dart_js'] = array(
    '#type' => 'textarea',
    '#title' => t('Inject javascript'),
    '#description' => t('Inject this javascript into the &lt;head&gt; on each page request. You do not need to add code to create a random number for use as the ORD if you checked "Include ord value" above. <br/>This javascript will be injected above the key|value pairs you enter in the field below, so you can declare variables here that you will include as part of a key|value pair there.'),
    '#default_value' => variable_get('dart_js',''),
    '#rows' => 5,
  );
  $form['dart_kvs'] = array(
    '#type' => 'textarea',
    '#title' => t('key|value pairs'),
    '#description' => t('Add key|value pairs that should be included on every ad tag (one per line). If you need a value to be evaluated by javascript append "|eval" to the pair. <br/>Examples: <ul><li>dcopt|ist</li><li>randomNum|Math.random()|eval</li><li>var|varDefinedIn<em>InjectJavascript</em>FieldAbove|eval</li></ul>'),
    '#default_value' => variable_get('dart_kvs',''),
    '#rows' => 5,
  );
  $roles = user_roles();
  $form['dart_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Hide ads from the following users'),
    '#default_value' => variable_get('dart_roles',array()),
    '#options' => $roles,
  );

  return system_settings_form($form);
}

/**
 * List all tags in a table.
 */
function dart_admin_tag_list() {
  $output = '';

  $header = array(
    array('data' => t('Active')),
    array('data' => t('Tag ID'), 'field' => 'dtid'),
    array('data' => t('Tag Name'), 'field' => 'name'),
    array('data' => t('Pos'), 'field' => 'pos'),
    array('data' => t('Size')),
    array('data' => t('Display Code')),
    array('data' => t('Operation')),
  );
  $result = db_query("SELECT active, dtid, name, pos, sz FROM {dart_tags}" . tablesort_sql($header));
  $rows = array();

  while ($tag = db_fetch_array($result)) {
    $tag['name'] = l($tag['name'], 'admin/build/adtag/' . $tag['dtid'] . '/edit');
    $tag['active'] = $tag['active'] ? '&#10004;' : '&#10006;';
    $tag[] = "<code>" . check_plain("print dart_tag(" . $tag['dtid'] . ");") . "</code>";
    $tag[] = l(t('edit'), 'admin/build/adtag/' . $tag['dtid'] . '/edit');


    $classes = $tag['active'] ? 'active' : 'inactive';
    $rows[] = array(
      'data' => $tag,
      'class' => $classes,
    );
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => '<em>'. t('There are currently no DART tags.') .'</em>', 'colspan' => 7));
  } else {
    $output .= '<p>' . t('In order to display any of the tags below you must add the provided display code to one of your .tpl files or theme override functions positioned in your markup wherever you would like the tag to appear. If you have chosen to display the tag as a block you can go to !admin-blocks instead of using the provided display code.', array('!admin-blocks' => l('admin/build/blocks', 'admin/build/blocks'))) . '</p>';
  }

  return $output . theme('table', $header, $rows);
}

/**
 * Form builder; Create an ad tag.
 *
 * @ingroup forms
 * @see dart_admin_tag_validate()
 * @see dart_admin_tag_submit()
 */
function dart_admin_tag(&$form_state, $edit=array()) {
  // Set some messages to explain why tags may not be displaying.
  if (empty($form_state['post']) && isset($edit['display']) && !$edit['display']) {
    global $user;
    if (!$edit['active']) {
      drupal_set_message('This tag is currently set as inactive.','warning');
    }
    if (!_dart_display_to_user($user)) {
      drupal_set_message(t('DART is configured to hide tags from you based on your user role(s). !click_here to change those settings.', array('!click_here' => l('Click here', 'admin/settings/dart'))), 'warning'); 
    }
    if (!_dart_display_on_domain($edit)) {
      drupal_set_message('This tag is configured to prevent it from displaying on the current domain. Change those settings below.','warning');
    }
  }
  
  $form['active'] = array(
    '#type' => 'checkbox',
    '#title' => t('Check here to make this tag active.'),
    '#default_value' => isset($edit['active']) ? $edit['active'] : '',
  );
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag Name'),
    '#description' => t('A unique name for this ad tag.'),
    '#required' => TRUE,
    '#maxlength' => 128,
    '#default_value' => isset($edit['name']) ? $edit['name'] : '',
  );
  $form['pos'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag Position (pos)'),
    '#description' => t('Example: rt_rail_box1'),
    '#required' => TRUE,
    '#maxlength' => 32,
    '#default_value' => isset($edit['pos']) ? $edit['pos'] : '',
  );
  $form['sz'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag Size (sz)'),
    '#description' => t('Example: 300x600,300x250'),
    '#required' => TRUE,
    '#maxlength' => 64,
    '#default_value' => isset($edit['sz']) ? $edit['sz'] : '',
  );
  $form['tag_options_group'] = array(
    '#type' => 'fieldset',
    '#title' => t('Tag Options'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  $form['tag_options_group']['block'] = array(
    '#type' => 'checkbox',
    '#title' => t('Create a block for this tag'),
    '#description' => t('Checking this will create a block (configurable on admin/build/block) in which this ad tag is displayed.'),
    '#default_value' => isset($edit['block']) ? $edit['block'] : '',
  );
  $form['tag_options_group']['scriptless'] = array(
    '#type' => 'checkbox',
    '#title' => t('This tag is scriptless'),
    '#description' => t('Check this if this tag should be rendered without javascript. Use this option for email based tags.'),
    '#default_value' => isset($edit['block']) ? $edit['block'] : '',
  );
  if (module_exists('domain')) {
    $available_domains = domain_domains();
    foreach ($available_domains as $key => $domain) {
      $options[$domain['domain_id'] === 0 ? 'zero' : $domain['domain_id']] = $domain['subdomain'];
    }
    $form['tag_options_group']['domain_group'] = array(
      '#type' => 'fieldset',
      '#title' => t('Domains/Subdomains'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
      '#description' => t('Choose which domains/subdomains this tag should appear on.'),
    );
    $form['tag_options_group']['domain_group']['domains'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Domains/Subdomains'),
      '#default_value' => isset($edit['data']['domains']) ? $edit['data']['domains'] : array_keys($options),
      '#options' => $options,
    );
  }
  $form['overrides'] = array(
    '#type' => 'fieldset',
    '#title' => t('DART Overrides'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Override the !settings for this ad tag.', array('!settings' => l('default settings', 'admin/configure/dart'))),
  );
  $form['overrides']['site'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag Site (site)'),
    '#description' => t('In rare circumstances you may want a tag to use an alternate DART site regardless of the page it appears on.'),
    '#required' => FALSE,
    '#maxlength' => 32,
    '#default_value' => isset($edit['site']) ? $edit['site'] : '',
  );
  $form['overrides']['zone'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag Zone (zone)'),
    '#description' => t('In rare circumstances you may want a tag to always use an alternate DART zone regardless of the page it appears on.'),
    '#required' => FALSE,
    '#maxlength' => 32,
    '#default_value' => isset($edit['zone']) ? $edit['zone'] : '',
  );
  $form['overrides']['slug'] = array(
    '#type' => 'textfield',
    '#title' => t('Slug'),
    '#description' => t('Override the default slug for this ad tag. Use !none for no slug. Leave this field empty to use the default slug. Example: Advertisement', array('!none' => check_plain('<none>'))),
    '#required' => FALSE,
    '#maxlength' => 64,
    '#default_value' => isset($edit['slug']) ? $edit['slug'] : '',
  );
  $form['kvs'] = array(
    '#type' => 'textarea',
    '#title' => t('Tag specific key|value pairs'),
    '#description' => t('Add key|value pairs that should be included on <strong>this tag only</strong> (one per line). If you need a value to be evaluated by javascript append "|eval" to the pair. <br/>Examples: <ul><li>dcopt|ist</li><li>randomNum|Math.random()|eval</li><li>var|varDefinedIn<em>InjectJavascript</em>FieldAbove|eval</li></ul>'),
    '#default_value' => isset($edit['kvs']) ? $edit['kvs'] : '',
    '#rows' => 5,
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Create Tag')
  );

  if (isset($edit['dtid'])) {
    $form['submit']['#value'] = t('Save Tag');
    $form['delete'] = array(
      '#type' => 'submit', 
      '#value' => t('Delete Tag')
    );
    $form['dtid'] = array(
      '#type' => 'value', 
      '#value' => $edit['dtid']
    );
  }

  return $form;
}

/**
 * Form validation function for admin tag form.
 */
function dart_admin_tag_validate($form, &$form_state) {
  // Ensure that "name" is unique (for new submissions only).
  if ($form_state['values']['tag_name'] && !isset($form_state['values']['dtid'])) {
    if(db_result(db_query("SELECT count(*) FROM {dart_tags WHERE name = '%s'}", $form_state['values']['tag_name']))){
      form_set_error('tag_name', t('The tag name %name already exists. Please choose another name for this tag.', array('%name' => $form_state['values']['tag_name'])));
    }
  }
}

/**
 * Form submit function for admin tag form.
 */
function dart_admin_tag_submit($form, &$form_state) {
  // Adjust the Data field appropriately.
  $form_state['values']['data'] = array();
  if (module_exists('domain')) {
    $form_state['values']['data']['domains'] = $form_state['values']['domains'];
  }
  $form_state['values']['data'] = serialize($form_state['values']['data']);

  switch($form_state['values']['op']){
    case t('Create Tag'):
      drupal_write_record('dart_tags', $form_state['values']);
      drupal_set_message(t('The DART tag has been created.'));
      watchdog('DART', 'Created DART tag: %name.', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
      break;
    case t('Save Tag'):
      drupal_write_record('dart_tags', $form_state['values'], 'dtid');
      drupal_set_message(t('The DART tag has been saved.'));
      watchdog('DART', 'Edited DART tag: %name.', array('%name' => $form_state['values']['name']), WATCHDOG_NOTICE);
      break;

  }
  $form_state['redirect'] = 'admin/build/adtag';
}

/**
 * Form builder for the DART tag delete confirmation form.
 *
 * @ingroup forms
 * @see dart_tag_confirm_delete_submit()
 */
function dart_admin_tag_confirm_delete(&$form_state, $dart_tag) {
  $form['dtid'] = array(
    '#type' => 'value',
    '#value' => $dart_tag->dtid,
  );
  $question = t("Are you sure you want to delete the '%name' DART tag?", array('%name' => $dart_tag->name));
  $path = 'admin/build/adtag';
  $description = t('This action cannot be undone.');
  return confirm_form($form, $question, $path, $description, t('Delete Tag'), t('Cancel'));
}

/**
 * Submit handler to delete a DART tag after confirmation.
 *
 * @see dart_tag_confirm_delete()
 */
function dart_admin_tag_confirm_delete_submit($form, &$form_state) {
  dart_tag_delete($form_state['values']['dtid']);
  drupal_set_message(t('Deleted DART tag.'));
  watchdog('DART', 'Deleted DART tag: %dtid.', array('%dtid' => $form_state['values']['dtid']), WATCHDOG_NOTICE);
  $form_state['redirect'] = 'admin/build/adtag';
  return;
}

/**
 * List all the scheduled DART tag events.
 */
function dart_admin_event_list() {
  $header = array(
    array('data' => t('Tag Name'), 'field' => 'name'),
    array('data' => t('Event Type'), 'field' => 'event_type'),
    array('data' => t('Event Date'), 'field' => 'event_time'),
    array('data' => t('Operations')),
  );
  $result = db_query("SELECT dt.name, dts.* FROM {dart_tags_schedule} as dts LEFT JOIN {dart_tags} as dt ON dts.dtid = dt.dtid" . tablesort_sql($header));
  $rows = array();

  while ($tag = db_fetch_array($result)) {
    $row = array(
      'name' => $tag['name'],
      'event_type' => $tag['event_type'],
      'event_time' => date(DART_EVENT_DATE_FORMAT, strtotime($tag['event_time'])),
      'op' => l(t('Edit'), 'admin/build/adtag/schedule/' . $tag['dtsid'] . '/edit'), 
    );
    $rows[] = $row;
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => '<em>'. t('There are currently no scheduled events.') .'</em>', 'colspan' => 4));
  }
  
  $create_link = l('<input type="button" value="Schedule a new event" />', 'admin/build/adtag/schedule/create', array('html' => TRUE));
  return theme('table', $header, $rows) . $create_link;
}

/**
 * Form builder; Create a scheduled event.
 */
function dart_admin_event(&$form_state, $edit=array()) {
  // Create an array of all tags to use as options.
  $dtids = array();
  $result = db_query('SELECT dtid, name FROM {dart_tags} ORDER BY name');
  while ($row = db_fetch_array($result)) {
    $dtids[$row['dtid']] = $row['name'] . ' [' . $row['dtid'] . ']';
  }

  // Create an array of event types to use as options.
  $event_types = array(
    'activate' => 'activate',
    'deactivate' => 'deactivate',
  );

  $form['dtid'] = array(
    '#type' => 'select',
    '#options' => $dtids,
    '#title' => t('Tag'),
    '#required' => TRUE,
    '#default_value' => isset($edit['dtid']) ? $edit['dtid'] : '',
  );
  $form['event_type'] = array(
    '#type' => 'select',
    '#options' => $event_types,
    '#title' => t('Type'),
    '#required' => TRUE,
    '#default_value' => isset($edit['event_type']) ? $edit['event_type'] : 'activate',
  );
  $form['event_time'] = array(
    '#type' => 'textfield',
    '#title' => t('When'),
    '#required' => TRUE,
    '#default_value' => isset($edit['event_time']) ? $edit['event_time'] : '',
    '#description' => t('Format: YYYY-MM-DD.'),
  );
  if (module_exists('date_popup')) {
    $form['event_time']['#type'] = 'date_popup';
    $form['event_time']['#date_format'] = DART_EVENT_DATE_FORMAT;
    $form['event_time']['#date_year_range'] = '0:+10';
    unset($form['event_time']['#description']);
  }

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Schedule Event'),
  );

  if (isset($edit['dtsid'])) {
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Reschedule Event'),
    );
    $form['delete'] = array(
      '#type' => 'submit', 
      '#value' => t('Cancel Event'),
    );
    $form['dtsid'] = array(
      '#type' => 'value', 
      '#value' => $edit['dtsid'],
    );
  }

  return $form;
}

/**
 * Form validation function for admin event form.
 */
function dart_admin_event_validate($form, &$form_state) {
  // Ensure that new events don't happen in the past.
  if ($form_state['values']['event_time'] && !isset($form_state['values']['dtsid'])) {
    if (strtotime($form_state['values']['event_time']) < strtotime(date(DART_EVENT_DATE_FORMAT))) {
      form_set_error('event_time', t('You cannot schedule an event in the past.'));
    }
  }
}

/**
 * Form submit function for admin event form.
 */
function dart_admin_event_submit($form, &$form_state) {
  switch($form_state['values']['op']){
    case t('Schedule Event'):
      drupal_write_record('dart_tags_schedule', $form_state['values']);
      drupal_set_message(t('The DART tag event has been scheduled.'));
      break;
    case t('Reschedule Event'):
      drupal_write_record('dart_tags_schedule', $form_state['values'], 'dtsid');
      drupal_set_message(t('The DART tag event has been rescheduled.'));
      break;
  }
  $form_state['redirect'] = 'admin/build/adtag/schedule';
}

/**
 * Form builder for the DART event delete confirmation form.
 */
function dart_admin_event_confirm_delete(&$form_state, $dart_event) {
  $form['dtsid'] = array(
    '#type' => 'value',
    '#value' => $dart_event->dtsid,
  );
  $question = t("Are you sure you want to cancel this event?");
  $path = 'admin/build/adtag/schedule';
  $description = t('This action cannot be undone.');
  return confirm_form($form, $question, $path, $description, t('Cancel Event'), t('Never Mind'));
}

/**
 * Submit handler to delete a DART event after confirmation.
 */
function dart_admin_event_confirm_delete_submit($form, &$form_state) {
  dart_event_delete($form_state['values']['dtsid']);
  drupal_set_message(t('Canceled this scheduled event.'));
  $form_state['redirect'] = 'admin/build/adtag/schedule';
  return;
}
