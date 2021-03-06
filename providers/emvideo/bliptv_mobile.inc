<?php
// $Id: bliptv_mobile.inc,v 1.1.2.7 2010/11/01 15:26:20 aaron Exp $

/**
 * @file media_bliptv_mobile/providers/emvideo/bliptv_mobile.inc
 * This include processes blip.tv media files for use by emfield.module.
 */

function emvideo_bliptv_mobile_info() {
  $features = array(
    array(t('Autoplay'), t('Yes'), ''),
    array(t('RSS attachment'), t('Yes'), t('When displaying in an RSS feed, these videos will be attached to the feed. (The feed reader or aggregator must support enclosure tags.)')),
    array(t('Thumbnails'), t('Yes'), ''),
  );
  return array(
    'provider' => 'bliptv_mobile',
    'name' => t('Blip.tv Mobile'),
    'url' => MEDIA_BLIPTV_MOBILE_MAIN_URL,
    'settings_description' => t('These settings specifically affect videos displayed from <a href="@provider" target="_blank">Blip.tv</a>.', array('@provider' => MEDIA_BLIPTV_MOBILE_MAIN_URL)),
    'supported_features' => $features,
  );
}

function emvideo_bliptv_mobile_settings() {
  module_load_include('inc', 'media_bliptv_mobile', 'includes/media_bliptv_mobile.admin');
  return media_bliptv_mobile_admin_form();
}

/**
 *  Implements emvideo_PROVIDER_validate hook.
 */
function emvideo_bliptv_mobile_validate($value, $error_field) {
  if ($value == 'BLIP_TV_ERROR_EMBED') {
    form_set_error($error_field, t('Please do not use the Embed Code from Blip.TV. You must instead paste the URL from the video page.'));
  }
}

/**
 *  Implement hook emvideo_PROVIDER_data_version().
 */
function emvideo_bliptv_mobile_data_version() {
  return MEDIA_BLIPTV_MOBILE_DATA_VERSION;
}

function emvideo_bliptv_mobile_data($field, $item) {
  $data = array();

  // we added new data fields to keep from having to reload the api/rss
  // because the data is only stored on update/insert, however, we have to know which data type we're using
  // this will just be an integer, increased when we make a change to data
  $data['emvideo_bliptv_mobile_data_version'] = $data['emvideo_data_version'] = MEDIA_BLIPTV_MOBILE_DATA_VERSION;

  // use the page id, since we'll have that in most cases (except in embed pastes, which gets parsed during extraction)
  // we use this to get an rss feed w/ all the info for the video. interesting reading ;)
  // some of our data we'll get from the api, like available files
  $rss = emvideo_bliptv_mobile_request($item['value'], TRUE, 'rss');
  $api = emvideo_bliptv_mobile_request($item['value'], TRUE, 'api');

  // special handling for show/feature pages
  // TODO: this is disabled for now.
//   if (empty($rss) && empty($api) && preg_match('@http\://([^\.]+)\.blip\.tv/@i', $item['embed'], $matches)) {
//     $show_rss = emvideo_bliptv_mobile_request($item['value'], TRUE, 'rss', TRUE);
//     $data['thumbnail']['url'] = $rss['IMAGE']['URL'][0];
//     $data['flv'] = array();
//     $data['showpage'] = "http://{$item['value']}.blip.tv/";
//     $data['is_show'] = TRUE;
//     return $data;
//   }

  $data['is_show'] = FALSE;

  // get our thumbnail url
  $data['thumbnail']['url'] = $rss['ITEM']['MEDIA:THUMBNAIL'][1]['URL'];
  if(!preg_match("/a\.images\.blip\.tv/",$data['thumbnail']['url']))
    $data['thumbnail']['url'] = "http://a.images.blip.tv".$data['thumbnail']['url'];
  // this gets sent to the player
  foreach ((array)$api['MEDIA']['LINK'] as $x => $link) {
    $x--;
    $video_type = '';
    switch ($link['TYPE']) {
      case 'video/x-flv':
        $video_type = 'flv';
        break;
      case 'video/x-m4v':
        $video_type = 'm4v';
        break;
      case 'video/quicktime':
        $video_type = 'mov';
        break;
      case 'video/3gpp':
        $video_type = '3gp';
        break;
    }
    if ($video_type) {
      $video_info = array();
      $video_info['url'] = $link['HREF'];
      $video_info['width'] = $api['MEDIA']['WIDTH'][$x];
      $video_info['height'] = $api['MEDIA']['HEIGHT'][$x];
      $video_info['duration'] = $api['MEDIA']['DURATION'][$x];
      $video_info['size'] = $api['MEDIA']['SIZE'][$x];
      $video_info['mime'] = $link['TYPE'];
      $video_info['embed_code'] = $rss['ITEM']['BLIP:EMBEDLOOKUP'];

      // we only store the last video of a particular type, for instance if two roles use .mov
      $data[$video_type] = $video_info;

      // however, we store the whole thing under role, so the information is still saved
      // but our arrays may be out of synch...
      $y = $x + 1;
      if ($api['MEDIA']['ROLE'][$y]) {
        $data[$video_type]['role'] = $api['MEDIA']['ROLE'][$y];
        $data[$api['MEDIA']['ROLE'][$y]] = $video_info;
        $data[$api['MEDIA']['ROLE'][$y]]['role'] = $api['MEDIA']['ROLE'][$y];
      }
    }
  }
  if (!$data['flv']) {
    $data['flv'] = array();
  }
  if (!$data['flv']['url']) {
    $data['flv']['url'] = $rss['ITEM']['ENCLOSURE'][1]['URL'];
  }
  $data['title'] = $api['en']['TITLE'][0] ? $api['en']['TITLE'][0] : $rss['2.0']['CHANNEL']['TITLE'][0];
  $data['description'] = $api['en']['DESCRIPTION'][0] ? $api['en']['DESCRIPTION'][0] : $rss['ITEM']['BLIP:PUREDESCRIPTION'][0];
  $data['blip_user']['uid'] = $api['CREATEDBY']['UID'][0] ? $api['CREATEDBY']['UID'][0] : $rss['ITEM']['BLIP:USERID'][0];
  $data['blip_user']['username'] = $api['CREATEDBY']['LOGIN'][0] ? $api['CREATEDBY']['LOGIN'][0] : $rss['ITEM']['BLIP:USER'][0];
  $data['blip_user']['url'] = $api['CREATEDBY']['LINKS']['LINK'][1] ? $api['CREATEDBY']['LINKS']['LINK'][1] : 'http://blip.tv/users/view/'. $data['blip_user']['username'];

  $data['showpage'] = $rss['ITEM']['BLIP:SHOWPAGE'][0];
  $data['embed_code'] = $rss['ITEM']['BLIP:EMBEDLOOKUP'];

  // This is the code actually used by the player, even though it's different
  // than for the page.
  if ($rss['ITEM']['BLIP:POSTS_ID'][0]) {
    $data['post_id'] = $rss['ITEM']['BLIP:POSTS_ID'][0];
  }
  return $data;
}

function emvideo_bliptv_mobile_request($code, $cacheable = TRUE, $skin = 'rss', $show = FALSE) {
  $args = array();
  $file = $show ? "http://$code.blip.tv/file/?skin=" . $skin : "http://blip.tv/file/$code?skin=" . $skin;
  return module_invoke('emfield', 'request_xml', 'bliptv_mobile', $file, $args, $cacheable, FALSE, $code .':'. $skin . ($show ? ':show' : ':post'));
}

function emvideo_bliptv_mobile_extract($embed) {
  // @TODO: Until #293153 is resolved, don't allow pasting embed code from Blip.TV.
  preg_match('@blip\.tv/play/([^"\&\?/]+)@i', $embed, $matches);
  if ($matches[1]) {
    return 'BLIP_TV_ERROR_EMBED';
  }
  return array(
    '@blip\.tv/rss/flash/([^"\&\?/]+)@i',
    '@blip\.tv/file/view/([^"\&\?/]+)@i',
    '@blip\.tv/file/([^"\&\?/]+)@i',
  );
}

/**
 * hook emvideo_PROVIDER_duration($item)
 * Returns the duration of the video in seconds.
 *  @param $item
 *    The video item itself, which needs the $data array.
 *  @return
 *    The duration of the video in seconds.
 */
function emvideo_bliptv_mobile_duration($item) {
  if (!isset($item['data']['emvideo_bliptv_mobile_data_version']) || $item['data']['emvideo_bliptv_mobile_data_version'] < 2) {
    $item['data'] = emvideo_bliptv_mobile_data(NULL, $item);
  }
  return isset($item['data']['flv']['duration']) ? $item['data']['flv']['duration'] : 0;
}

/**
 * hook emvideo_PROVIDER_embedded_link($video_code)
 * returns a link to view the video at the provider's site.
 *  @param $video_code
 *    The string containing the video to watch.
 *  @return
 *    A string containing the URL to view the video at the original provider's site.
 */
function emvideo_bliptv_mobile_embedded_link($video_code) {
  return media_bliptv_mobile_video_url($video_code);
}

/**
 * Look for a file for an enclosure of the preferred tag.
 *
 * @param array $data
 *  The data from the provider video item.
 * @param string $preferred_tag
 *  The preferred tag from the data array, for instance 'web' or 'mov'.
 * @return mixed
 *  If the preferred tag is 'override' or 'none', the return that for possible
 *  later overriding. Otherwise, if we have a file in the data array for that
 *  tag, then return the file array. Otherwise return NULL, so we'll specify
 *  the fallback 'flv' tag, if it exists.
 */
function _media_bliptv_mobile_get_video_info_from_preferred_tag($data, $preferred_tag) {
  if (in_array($preferred_tag, array('override', 'none'))) {
    return $preferred_tag;
  }

  if ($data[$preferred_tag]['url']) {
    return $data[$preferred_tag];
  }
}

/**
 * Implementation of hook emvideo_PROVIDER_rss().
 *
 * Providers may supply an enclosure for rss feeds. This expects something in
 * a file format, so would be an object in the format of $file->filepath,
 * $file->filesize, and $file->filemime.
 */
function emvideo_bliptv_mobile_rss($item, $teaser = NULL) {
  if ($item['value']) {
    // If this item is of a really old version, we'll try to compensate.
    if ($item['data']['emvideo_bliptv_mobile_data_version'] >= 1) {
      $data = $item['data'];
    }
    else {
      $data = emvideo_bliptv_mobile_data(NULL, $item);
    }

    // Get the preferred type for the rss feed.
    $video_info = _media_bliptv_mobile_get_video_info_from_preferred_tag($data, media_bliptv_mobile_variable_get('rss_tag'));

    // Grab the preferred filetype rather than tag, so .mov rather than 'web'.
    if ($video_info == 'override') {
      $video_info = _media_bliptv_mobile_get_video_info_from_preferred_tag($data, media_bliptv_mobile_variable_get('rss_format'));
    }

    // Default to flv if there's no available clip format.
    if (is_null($video_info) || ($video_info == 'override')) {
      $video_info = $data['flv'];
    }

    // Begin building file object.
    if (is_array($video_info)) {
      $file = array();
      // Create temporary name/path for newly uploaded files.
      $file['filepath'] = $video_info['url'];
      $file['filesize'] = $video_info['size'];
      $file['filemime'] = $video_info['mime'];

      // Additional data for Y! MRSS.
      $file['thumbnail']['filepath'] = $data['thumbnail']['url'];
      $file['width'] = ($video_info['width']) ? $video_info['width'] : 0;
      $file['height'] = ($video_info['height']) ? $video_info['height'] : 0;
      $file['duration'] = ($video_info['duration']) ? $video_info['duration'] : FALSE;
      // @TODO Media:credit role="author" ...

      return $file;
    }
  }
}

function theme_media_bliptv_mobile_flash($code, $width, $height, $field, $item, $autoplay, $flv, $thumbnail, $options = array()) {
  static $count;
  if ($code) {
    $count++;
    $id = isset($options['id']) ? $options['id'] : "emfield_videocck_player_bliptv_mobile_$count";
    $autostart = $autoplay ? 'autostart=true' : 'autostart=false';

    $rss = $item['data']['showpage'] ? "&amp;feedurl={$item['data']['showpage']}/rss" : '';
    $post_id = $item['data']['post_id'];
    // @TODO: Use the following if/when we allow featured shows to be embedded.
    // $file = $item['data']['is_show'] ? "http://$code.blip.tv/rss/flash/" : 'http://blip.tv/rss/flash/'. $item['data']['post_id'];
    if (isset($item['data']['flv']['embed_code']['0'])) {
      $embed_code = $item['data']['flv']['embed_code']['0'];
    }
    else if (isset($item['data']['m4v']['embed_code']['0'])) {
      $embed_code = $item['data']['m4v']['embed_code']['0'];
    }
    $file = 'http://blip.tv/rss/flash/'. $item['data']['post_id'];
    $embed_file = 'http://blip.tv/scripts/flash/showplayer.swf?'. $autostart .'&amp;enablejs=true'. $rss .'&amp;file='. $file .'&amp;showplayerpath=http://blip.tv/scripts/flash/showplayer.swf';

    if ($item['data']['video_cck_bliptv_mobile_data_version'] == 1) {
      // If this is an old version of the data, then we'll attempt to display.
      // Bad on the admin, however; they need to run emfield's update.
      $autostart = $autoplay ? 'autostart=true' : 'autostart=false';
      $embed_file = "http://blip.tv/scripts/flash/blipplayer.swf?$autostart&amp;file=$flv%3Fsource%3D3";
      // Now we'll resave the node so the data is saved properly in the future.
      // @TODO: Do this in cron, or even in update?
      // @NOTE: We now do this in an update.
      // Query would be something like:
      // SELECT nid FROM content_type_video WHERE field_video_provider = 'bliptv_mobile' AND field_video_data LIKE '%%"video_cck_bliptv_mobile_data_version";i:1;%%';
      // except that we don't know types and fields...
/*      if (($node = $options['node']) && $node->nid) {
        emfield_operations_reload(array($options['node']->nid), FALSE);
      }*/
    }

    if (preg_match("/^m\./i",$_SERVER["HTTP_HOST"])>0 or
	preg_match("/\.mobi$/i",$_SERVER["HTTP_HOST"])>0) {
	  $height -= 30;
      $useragent = $_SERVER["HTTP_USER_AGENT"];
	  if(preg_match("/ipad/i",$useragent)>0 or
      preg_match("/iphone/i",$useragent)>0 or
      preg_match("/ipod/i",$useragent)>0) {
        if(!empty($item['data']['m4v'])) {
          $output .= '<p class="nomobile">Note: iPhone embeds are currently not working properly. We are working on a solution.</p><embed src="http://blip.tv/play/'. (empty($embed_code)?$code:$embed_code).'%2Em4v" type="application/x-shockwave-flash" width="'. ($width/2) .'" height="'. ($height/2) .'" allowscriptaccess="always" allowfullscreen="true" wmode="opaque" style="margin:0 auto"></embed>';
		} else {
          $output .= '<p class="nomobile">Sorry, but this video has no iPhone version available at this time.</p>';
		}
      } else if(preg_match("/android/i",$useragent)>0) {
        // Embed codes end with A-D for autoplay, and I-L for none
        $embed_code = substr($embed_code,0,-1);
        $output .= '<script src="http://blip.tv/play/'.($embed_code+($autoplay?"A":"I")).'.js?width='.($width/2).'&height='.($height/2).'" type="text/javascript"></script><noscript>'.emvideo_bliptv_mobile_mobile_embed($item).'</noscript>';
	  } else if(!empty($item['data']['3gp']) or
                !empty($item['data']['m4v'])) {
        $output .= emvideo_bliptv_mobile_mobile_embed($item);
      } else {
        $output .= '<p class="nomobile">Sorry, but this video has no mobile version available at this time.</p>';
      }
    } else {
      $output .= '<object classid="clsid:D27CDB6E-AE6D-11cf-96B8-444553540000" codebase="http://download.macromedia.com/pub/shockwave/cabs/flash/swflash.cab#version=6,0,40,0" width="' . $width .'" height="'. $height .'">
      <param name="movie" value="http://blip.tv/play/'. $embed_code . '" />
      <param name="quality" value="high" />
      <param name="bgcolor" value="#FFFFFF" />
      <param name="wmode" value="transparent" />
      <param name="allowfullscreen" value="true" />
      '.($autostart?'<param name="flashvars" value="'.$autostart.'" />':'').'

      <!--[if !IE]> <-->
      <object data="http://blip.tv/play/'. $embed_code.'" width="'. $width .'" height="'. $height .'" type="application/x-shockwave-flash">
      <param name="quality" value="high" />
      <param name="bgcolor" value="#FFFFFF" />
      <param name="wmode" value="transparent" />
      <param name="allowfullscreen" value="true" />
      '.($autostart?'<param name="flashvars" value="'.$autostart.'" />':'').'
      <param name="pluginurl" value="http://www.adobe.com/go/getflashplayer" />
      <p class="noflash">It seems you don\'t have a recent version of Adobe Flash.  Try <a href="http://www.adobe.com/go/getflashplayer">downloading it here</a>, then reload the page.</p>
      </object>
      <!--> <![endif]-->
      </object>';
    }
  }

  return $output;
}

function emvideo_bliptv_mobile_mobile_embed($item) {
  $returndata = '';
  $refdata = '?referrer='.emvideo_bliptv_mobile_current_page_escaped();
  // $returndata .= '<img src="'.$item['data']['thumbnail']['url'].'" style="font-size:10px;width:'.($width/4).'px;height:'.($height/4).'px;" title="'.$item['data']['title'].' Mobile Video Thumbnail" alt="'.$item['data']['title'].' Mobile Video Thumbnail" width="'.($width/4).'" height="'.($height/4).'"/>';
  $width = $item['data']['3gp']['width'];
  $height = $item['data']['3gp']['height'];
  if(!empty($item['data']['3gp'])) {
    $file3gp = $item['data']['3gp']['url'];
    $sizearray3gp = emvideo_bliptv_mobile_get_video_size($item['data']['3gp']['size']);
    $returndata .= '<p style="text-align:left;"><a href="'.$file3gp.$refdata.'&ext=.3gp">';
    $returndata .= 'Play Video (3GPP, '.round($sizearray3gp[0],1).' '.$sizearray3gp[1].'bytes)';
    $returndata .= "</a></p>";
  }
  if(!empty($item['data']['m4v'])) {
    $filem4v = $item['data']['m4v']['url'];
    $sizearraym4v = emvideo_bliptv_mobile_get_video_size($item['data']['m4v']['size']);
    $returndata .= '<p style="text-align:left;"><a href="'.$filem4v.$refdata.'&ext=.m4v">';
    $returndata .= 'Play Video (MPEG-4, '.round($sizearraym4v[0],1).' '.$sizearraym4v[1].'bytes)';
    $returndata .= '</a></p>';
  }
  return $returndata;
}

function emvideo_bliptv_mobile_thumbnail($field, $item, $formatter, $node, $width, $height, $options = array()) {
  if (($item['data']['emvideo_bliptv_mobile_data_version'] >= 1) || isset($item['data']['thumbnail']['url'])) {
    $tn = $item['data']['thumbnail']['url'];
  }
  else {
    $tn = $item['data']['thumbnail'];
  }
  return $tn;
}

function emvideo_bliptv_mobile_video($code, $width, $height, $field, $item, $node, $autoplay, $options = array()) {
  if (($item['data']['emvideo_bliptv_mobile_data_version'] >= 1) || isset($item['data']['flv']['url'])) {
    $flv = $item['data']['flv']['url'];
  }
  else {
    $flv = $item['data']['flv'];
  }
  $thumbnail = emvideo_bliptv_mobile_thumbnail($field, $item, NULL, $node, $width, $height, $options);

  $output = theme('media_bliptv_mobile_flash', $code, $width, $height, $field, $item, $autoplay, $flv, $thumbnail);
  return $output;
}

function emvideo_bliptv_mobile_preview($code, $width, $height, $field, $item, $node, $autoplay, $options = array()) {
  if ($item['data']['emvideo_bliptv_mobile_data_version'] >= 1) {
    $flv = $item['data']['flv']['url'];
    $thumbnail = $item['data']['thumbnail']['url'];
  }
  else {
    $flv = $item['data']['flv'];
    $thumbnail = $item['data']['thumbnail'];
  }
  $output = theme('media_bliptv_mobile_flash', $code, $width, $height, $field, $item, $autoplay, $flv, $thumbnail);
  return $output;
}

/**
 * Implement hook_emvideo_PROVIDER_content_generate().
 */
function emvideo_bliptv_mobile_content_generate() {
  return array(
    'http://blip.tv/file/836041/',
    'http://blip.tv/file/1492835/',
    'http://blip.tv/file/2015750/',
    'http://blip.tv/file/2312026/',
  );
}



/**
 * Construct a referral URL when accessing 3GPP videos
 */
function emvideo_bliptv_mobile_current_page_escaped() {
  $pageurl = "http";
  if ($_SERVER["HTTPS"]=="on") $pageurl.="s";
  $pageurl.="://";
  if($_SERVER["SERVER_PORT"]!="80" && $_SERVER["SERVER_PORT"]!="443")
    $pageurl.=$_SERVER["HTTP_HOST"].":".$_SERVER["SERVER_PORT"].$_SERVER["REQUEST_URI"];
  else
    $pageurl.=$_SERVER["HTTP_HOST"].$_SERVER["REQUEST_URI"];
  $urltrans = array(':'=>'%3A','/'=>'%2F');
  return strtr($pageurl,$urltrans);
}

/**
 * Get the filesize of the 3GPP video in plain English
 */
function emvideo_bliptv_mobile_get_video_size($size) {
  $byteprefix = Array('Kilo','Mega','Giga','Tera','Peta','Exa','Zetta','Yotta');
  foreach ($byteprefix as $prefix) {
    $size = $size/1024;
    if ($size<1024) {
      $byteprefix = $prefix;
      break;
    }
  }
  return array($size,$byteprefix);
}
