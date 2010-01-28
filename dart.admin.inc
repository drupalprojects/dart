<?php
// $Id$

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
    '#description' => t('The value to use when no "site/zone" is specified on a given page. Example: sports/yankees.'),
    '#maxlength' => 32,
  );
  $form['dart_vals'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Defaults'),
    '#default_value' => variable_get('dart_vals',array('tile' => 'tile','ord' => 'ord')),
    '#options' => array(
      'tile'  => 'tile (value auto-increments on each tag)',
      'ord'   => 'ord (value changes on each page request)',
    ),
    '#description' => t('Include these values in all of your ad tags.'),
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


  return system_settings_form($form);
}

/**
 * Form builder; Create an ad tag.
 *
 * @ingroup forms
 * @see dart_admin_tag_validate()
 * @see dart_admin_tag_submit()
 */
function dart_admin_tag(&$form_state, $edit=array()){
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => t('Tag Name'),
    '#description' => t('A unique name for this ad tag.'),
    '#required' => TRUE,
    '#maxlength' => 128,
    '#default_value' => isset($edit['name']) ? $edit['name'] : '',
  );
  $form['overrides'] = array(
    '#type' => 'fieldset',
    '#title' => t('DART Overrides'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#description' => t('Override the default settings for this ad tag.'),
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

function dart_admin_tag_validate($form, &$form_state){
  // Ensure that "name" is unique (for new submissions only).
  if ($form_state['values']['tag_name'] && !isset($form_state['values']['dtid'])) {
    if(db_result(db_query("SELECT count(*) FROM {dart_tags WHERE name = '%s'}", $form_state['values']['tag_name']))){
      form_set_error('tag_name', t('The tag name %name already exists. Please choose another name for this tag.', array('%name' => $form_state['values']['tag_name'])));
    }
  }
}

function dart_admin_tag_submit($form, &$form_state){
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
 * List all tags in a table
 */
function dart_admin_tag_list(){
  $header = array(
    array('data' => t('Tag ID'), 'field' => 'dtid'),
    array('data' => t('Tag Name'), 'field' => 'name'),
    array('data' => t('Position (pos)'), 'field' => 'pos'),
    array('data' => t('Size (sz)')),
    array('data' => t('Operations')),
  );
  $result = db_query("SELECT dtid, name, pos, sz FROM {dart_tags}" . tablesort_sql($header));
  $rows = array();

  while ($tag = db_fetch_array($result)) {
    $tag[] = l(t('Edit'), 'admin/build/adtag/' . $tag['dtid'] . '/edit');
    $rows[] = $tag;
  }
  if (empty($rows)) {
    $rows[] = array(array('data' => '<em>'. t('There are currently no DART tags.') .'</em>', 'colspan' => 5));
  }
  return theme('table', $header, $rows);
}
