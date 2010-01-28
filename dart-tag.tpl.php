<?php
// $Id$

/**
 * @file dart-tag.tpl.php
 * Display the js call to display a DART ad tag.
 *
 * Variables available:
 * - tag: The full tag object or NULL. If it's NULL, all other 
 *        vars listed below will be NULL as well
 * - $pos: The position (pos).
 * - $sz: The size (sz).
 * - $vars: A js object with information needed by the tag.
 *          Example:
 *          {
 *            dartSiteZone : 'over.ride/value',
 *            dartVars : {
 *                         key1 : {val:'foo', eval:true},
 *                         key2 : {val:'bar', eval:false},
 *                         key3 : {val:'foobar', eval:true}
 *                       },
 *          }
 *
 * @see template_preprocess_dart_tag()
 */
?>

<?php if ($tag) { ?>
  <script type="text/javascript">Drupal.DART.tag('<?php print $pos; ?>', '<?php print $sz; ?>', <?php print $vars; ?>);</script>
<?php } ?>