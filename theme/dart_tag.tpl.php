<?php
// $Id$

/**
 * @file
 * Display the js call to display a DART ad tag.
 *
 * Variables available:
 * - $tag: The full tag object or NULL. If it's NULL, all other
 *        vars listed below will be NULL as well
 * - $json_tag: a js version of $tag.
 * - $attributes: any attributes that should be displayed on teh outer-most div.
 * - $show_script_tag: boolean.
 * - $show_noscript_tag: boolean.
 * - $noscript: the <noscript> tag for this DART tag, or empty string.
 *
 * @see template_preprocess_dart_tag()
 */
?>

<div <?php print drupal_attributes($attributes); ?>>
  <?php if ($tag->slug) { ?><span class="slug"><?php print $tag->slug; ?></span><?php } ?>
  <?php if ($show_script_tag) { ?>
  <script type="text/javascript">Drupal.DART.tag('<?php print $json_tag; ?>');</script>
  <?php } ?>
  <?php print $noscript; ?>
</div>