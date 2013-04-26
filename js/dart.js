(function($) {

/**
 * Create a DART object to handle tagging functionality
 */
Drupal.DART = {};

/**
 * Overridable settings.
 */
Drupal.DART.settings = {
  "writeTags": true
};

/**
 * Using document.write, add a DART tag to the page
 */
Drupal.DART.tag = function(tag) {
  tag = typeof(tag) == 'string' ? eval('(' + tag + ')') : tag;

  var tagname = tag.settings.options.method == 'adj' ? 'script' : 'iframe';
  var options = tag.settings.options.method == 'adj' ? 'type="text/javascript"' : 'frameborder="0" scrolling="no" width="' + tag.sz.split("x")[0] + '" height="' + tag.sz.split("x")[1] + '"';

  // Allow other modules to include js that can manipulate the tag object.
  var processed_tag = ($ !== undefined) ? $(document).triggerHandler('dart_tag_process', [tag]) : undefined;
  tag = processed_tag !== undefined ? processed_tag : tag;

  ad = '<' + tagname + ' ' + options + ' src="';
  ad += dart_url + "/";
  ad += tag.network_id !== '' ? tag.network_id + "/" : "";
  ad += tag.settings.options.method + "/";
  ad += tag.prefix + '.' + tag.site + "/" + tag.zone + ";";
  ad += this.keyVals(tag.key_vals);

  // Allow other modules to include js that can manipulate the concatenated tag string.
  rendered_ad = ($ !== undefined) ? $(document).triggerHandler('dart_tag_render', [ad]) : undefined;
  ad = rendered_ad !== undefined ? rendered_ad : ad; ad += '"></' + tagname + '>';

  if (Drupal.DART.settings.writeTags) {
    document.write(ad);
  }

  // console.log('-----------------'+tag.pos+'------------------');
  // console.log(tag);

  return ad;
};

/**
 * Format a key|val pair into a dart tag key|val pair.
 */
Drupal.DART.keyVal = function(key, val, useEval) {
  if (key != "<none>") {
    kvp  = key + "=";
    kvp += useEval ? eval(val) : val;
    kvp += key == "ord" ? "?" : ";";
  }
  else {
    kvp = useEval ? eval(val) : val;
  }

  return(kvp);
};

/**
 * Loop through an object and create kay|val pairs.
 *
 * @param vals
 *   an object in this form:
 *   {
 *     key1 : {{val:'foo', eval:true}, {val:'foo2', eval:false}}
 *     key2 : {{val:'bar', eval:false}},
 *     key3 : {{val:'foobar', eval:true}}
 *   }
 */
Drupal.DART.keyVals = function(vals) {
  var ad = '';
  for(var key in vals) {
    value = vals[key];
    for(var val in value) {
      v = value[val];
      ad += this.keyVal(key, v['val'], v['eval']);
    }
  }
  return ad;
};

/**
 * If there are tags in the loadLastTags, then load them where they belong.
 */
Drupal.behaviors.DART = {
  attach: function(context) {
    if (typeof(Drupal.DART.settings.loadLastTags) == 'object') {
      var ord = Math.floor((Math.random() * 10000000000) + 1);
      $('.dart-tag:visible').not('.dart-processed').each(function() {
        var _this = $(this);
        var regex = /dart-name-(\w+)$/;
        var result = regex.exec(_this.attr('class'));
        if (result != null) {
          var scriptTag = Drupal.DART.tag(Drupal.DART.settings.loadLastTags[result[1]]);
          var scriptTag = scriptTag.replace(/;ord=\d+/, ';ord='+ord);
          if (typeof(postscribe) == 'function') {
            postscribe(_this, scriptTag, function () { _this.addClass('.dart-processed'); });
          }
          else if (typeof(_this.writeCapture) == 'function') {
            _this.writeCapture().append(scriptTag).addClass('dart-processed');
          }
        }
      });
    }
  }
};

})(jQuery);
