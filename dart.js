// $Id$

/**
 * Create a DART object to handle tagging functionality
 */
Drupal.DART = {};

/**
 * Using document.write, add a DART tag to the page
 */
Drupal.DART.tag = function (pos, sz, vars) {
  var localDartVars = dartVars;
  var dartSiteZone = (dartSite in vars ? vars.dartSite : dartSite) + "/" + (dartZone in vars ? vars.dartZone : dartZone);

  ad  = '<script type="text/javascript" src="';
  ad += vars.dartURL + "/adj/";
  ad += dartPrefix + '.' + dartSiteZone + ";";
  ad += this.keyVal('pos', pos, false);
  ad += this.keyVal('sz', sz, false);

  ad += this.keyVals(localDartVars);
  ad += this.keyVals(vars.dartVars);

  // If ord exists, add it last.
  if (typeof ord !== "undefined"){
    ad += this.keyVal('ord', 'ord', true);
  }

  ad += '"></script>';

  document.write(ad);
  //console.log('-----------------'+pos+'------------------');
  //console.log(sz);  console.log(vars);  console.log(dartVars);  console.log(dartSite+"/"+dartZone);
  //console.log(ad);
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
    val = vals[key];
    if (typeof val['val'] === 'object') {
      for(var i in val['val']){
        ad += this.keyVal(key, val['val'][i], val['eval']);
      }
    } else {
      ad += this.keyVal(key, val['val'], val['eval']);
    }
  }
  return ad;
}