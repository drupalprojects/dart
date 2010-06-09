// $Id$

/**
 * Create a DART object to handle tagging functionality
 */
Drupal.DART = {};

/**
 * Using document.write, add a DART tag to the page
 */
Drupal.DART.tag = function (pos, sz, tag) {
  ad  = '<script type="text/javascript" src="';
  ad += Drupal.DART.dart_url + "/" + tag.options.method + "/";
  ad += tag.prefix + '.' + tag.site + "/" + tag.zone + ";";
  ad += this.keyVal('pos', pos, false);
  ad += this.keyVal('sz', sz, false);
  ad += this.keyVals(tag.options.keyvals);

  // If ord exists, add it last.
  if (typeof Drupal.DART.ord !== "undefined"){
    ad += this.keyVal('ord', Drupal.DART.ord, true);
  }

  ad += '"></script>';

  document.write(ad);
  //console.log('-----------------'+pos+'------------------');
  //console.log(tag);
}

/**
 * Format a key|val pair into a dart tag key|val pair.
 */
Drupal.DART.keyVal = function(key, val, useEval){
  kvp  = key + "=";
  kvp += useEval ? eval(val) : val;
  kvp += key == "ord" ? "?" : ";";
  return(kvp);
}

/**
 * Loop through an object and create kay|val pairs.
 * 
 * @param vals
 *   an object in this form:
 *   {
 *     key1 : {val:'foo', eval:true},
 *     key2 : {val:'bar', eval:false},
 *     key3 : {val:'foobar', eval:true}
 *   }
 */
Drupal.DART.keyVals = function(vals){
  var ad = '';
  for(var key in vals){
    value = vals[key];
    for(var val in value){
      v = value[val];
      ad += this.keyVal(key, v['val'], v['eval']);
    }
  }
  return ad;
}