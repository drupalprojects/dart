<?php
// $Id$

/**
 * @file dart-tag.tpl.php
 * Display the js call to display a DART ad tag.
 *
 * Variables available:
 * - $tag: The full tag object or NULL. If it's NULL, all other 
 *         vars listed below will be NULL as well.
 * - $site: The DART site.
 * - $zone: The DART zone.
 * - $dart_url: The default URL to contact DART.
 * - $dart_prefix: The DART prefix.
 * - $dart_vars: An array of key|val pairs in the form:
 *               array('key' => array('val' => 'xyz', 'eval' => TRUE))
 *
 * - $tile: Either an integer or NULL.
 * - $ord: Random number followed by a question mark or NULL.
 *
 * @see template_preprocess_dart_noscript_tag()
 */
?>

<?php
  $src  = $dart_url . '/jump/' . $dart_prefix . '.' . $site . '/' . $zone . ';';
  $src .= 'pos=' . $tag->pos . ';';
  $src .= 'sz=' . $tag->sz . ';';
  foreach ($dart_vars as $key=>$val){
    if (!$val['eval']){
      $src .= $key . '=' . $val['val'] .';';
    }
  }
  if ($tile) {
    $src .= 'tile=' . $tile .';';
  }
  if ($ord) {
    $src .= 'tile=' . $ord;
  }
  
?>
  
<noscript><a href="<?php print $src; ?>"><img src="<?php print $src; ?>" alt="" /></a></noscript>