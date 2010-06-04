<?php
// $Id$

/**
 * @file dart-tag.tpl.php
 * Display the js call to display a DART ad tag.
 *
 * Variables available:
 * - $tag: The full tag object or NULL. If it's NULL, all other 
 *         vars listed below will be NULL as well.
 * - $src: The src path for a noscrip ad call.
 *
 * @see template_preprocess_dart_noscript_tag()
 */
?>
  
<noscript><a href="<?php print $src; ?>"><img src="<?php print $src; ?>" alt="" /></a></noscript>