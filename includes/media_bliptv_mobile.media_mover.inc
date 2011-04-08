<?php
// $Id: media_bliptv_mobile.media_mover.inc,v 1.1.2.1 2010/09/23 19:19:08 aaron Exp $

/**
 * @file
 * Functions to implement Media Mover behavior for Media: BlipTV Mobile.
 */

/**
 * Upload a video to BlipTV through Media Mover.
 */
function media_bliptv_mobile_upload_video($file, $configuration) {
  drupal_set_message(t('This feature is not yet implemented.'), 'error');
//   _media_bliptv_mobile_set_include_path();
//
//   $path = media_bliptv_mobile_zend_path();
//   Zend_Loader::loadClass('Zend_Gdata_ClientLogin', $path);
//   Zend_Loader::loadClass('Zend_Gdata_BlipTV_Mobile', $path);
//   Zend_Loader::loadClass('Zend_Gdata_App_Exception', $path);
//   Zend_Loader::loadClass('Zend_Gdata_App_HttpException', $path);
//
//   $title = $configuration['media_bliptv_mobile_default_title'];
//   $description = $configuration['media_bliptv_mobile_default_description'];
//   $tags = '';
//   $node = NULL;
//
//   if ($file['nid']) {
//     $node = node_load($file['nid']);
//     if ($node) {
//       $title = $node->title;
//       if (strip_tags($node->body)) {
//         $description = strip_tags($node->body);
//       }
//
//       $tags = array();
//       if ($node->taxonomy) {
//         foreach ($node->taxonomy as $term) {
//           if ($configuration['media_bliptv_mobile_vocabs'][$term->vid]) {
//             $ntags = explode(' ', $term->name);
//             $tags = array_merge($tags, $ntags);
//           }
//         }
//       }
//       $tags = array_filter($tags, '_media_bliptv_mobile_filter_tags');
//       $tags = implode(', ', $tags);
//     }
//     else {
//       watchdog('media_bliptv_mobile', 'File !file belongs to node !nid which does not exist anymore.', array('!file' => $filepath, '!nid' => $file['nid']), WATCHDOG_ERROR);
//     }
//   }
//   if ($tags == '') {
//     $tags = $configuration['media_bliptv_mobile_default_tags'];
//   }
//
//   $username = $configuration['media_bliptv_mobile_username'];
//   $password = $configuration['media_bliptv_mobile_password'];
//   $devkey   = $configuration['media_bliptv_mobile_key'];
//   $category = $configuration['media_bliptv_mobile_category'];
//
//   $filepath = media_mover_api_config_current_file($file);
//   $fileinfo = pathinfo($filepath);
//
//   try {
//     $httpclient = Zend_Gdata_ClientLogin::getHttpClient($username, $password, 'bliptv_mobile', NULL, MEDIA_BLIPTV_MOBILE_APP_ID, NULL, NULL, MEDIA_BLIPTV_MOBILE_AUTH_URL);
//   }
//   catch (Exception $e) {
//     watchdog('media_bliptv_mobile', 'Cannot authenticate. Error: @error', array('@error' => strip_tags($e->getMessage())), WATCHDOG_ERROR);
//     return FALSE;
//   }
//
//   $bliptv_mobile = new Zend_Gdata_BlipTV_Mobile($httpclient, MEDIA_BLIPTV_MOBILE_APP_ID, NULL, $devkey);
//   $upload = new Zend_Gdata_BlipTV_Mobile_VideoEntry();
//
//   $filesource = $bliptv_mobile->newMediaFileSource($filepath);
//   $filesource->setContentType('video/avi');
//   $filesource->setSlug($filepath);
//
//   $upload->setMediaSource($filesource);
//   $upload->setVideoTitle($title);
//   $upload->setVideoDescription($description);
//   $upload->setVideoCategory($category);
//   $upload->setVideoTags($tags);
//
//   try {
//     $video = $bliptv_mobile->insertEntry($upload, MEDIA_BLIPTV_MOBILE_UPLOAD_URL, 'Zend_Gdata_BlipTV_Mobile_VideoEntry');
//     $url = media_bliptv_mobile_video_url($video->getVideoId());
//     watchdog('media_bliptv_mobile', 'Uploaded file !file to BlipTV: !video', array('!file' => $fileinfo['basename'], '!video' => l($url, $url)), WATCHDOG_NOTICE, l($title, 'http://www.bliptv.com/watch?v='. $story_id, array('absolute' => TRUE)));
//     return $url;
//   }
//   catch (Zend_Gdata_App_HttpException $httpException) {
//     watchdog('media_bliptv_mobile', 'Upload failed. Error: @error. Exception: @exception.', array('@error' => strip_tags($httpException->getMessage()), '@exception' => strip_tags($httpException->getRawResponseBody())), WATCHDOG_ERROR, $node ? l($node->title, 'node/' . $node->nid) : NULL);
//     return FALSE;
//   }
//   catch (Zend_Gdata_App_Exception $e) {
//     watchdog('media_bliptv_mobile', print_r($file, TRUE));
//     watchdog('media_bliptv_mobile', 'Upload failed. Error: @error.', array('@error' => strip_tags($e->getMessage())), WATCHDOG_ERROR, $node ? l($node->title, 'node/' . $node->nid) : NULL);
//     return FALSE;
//   }
}

/**
 * Media Mover configuration form element for Media: BlipTV Mobile.
 *
 * @see media_bliptv_mobile_validate_configuration().
 */
// function media_bliptv_mobile_config($configuration) {
//   $form['media_bliptv_mobile_conf'] = array(
//     '#type' => 'fieldset',
//     '#title' => t('Upload to BlipTV configuration'),
//     '#element_validate' => array('media_bliptv_mobile_validate_configuration', array('media_bliptv_mobile_conf'))
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_username'] = array(
//     '#title' => t('Username'),
//     '#type' => 'textfield',
//     '#default_value' => $configuration['media_bliptv_mobile_username'] ? $configuration['media_bliptv_mobile_username'] : '',
//     '#description' => t('Your BlipTV username.'),
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_password'] = array(
//     '#title' => t('Password'),
//     '#type' => 'password',
//     '#default_value' => $configuration['media_bliptv_mobile_password'] ? $configuration['media_bliptv_mobile_password'] : '',
//     '#description' => t('Your BlipTV password.'),
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_key'] = array(
//     '#title' => t('Developer Key'),
//     '#type' => 'textfield',
//     '#default_value' => $configuration['media_bliptv_mobile_key'] ? $configuration['media_bliptv_mobile_key'] : '',
//     '#description' => t('Your BlipTV developer key.'),
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_category'] = array(
//     '#title' => t('Category'),
//     '#type' => 'textfield',
//     '#default_value' => $configuration['media_bliptv_mobile_category'] ? $configuration['media_bliptv_mobile_category'] : '',
//     '#description' => t('The category where videos will be added. This has to be a BlipTV category.'),
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_default_title'] = array(
//     '#title' => t('Default title'),
//     '#type' => 'textfield',
//     '#default_value' => $configuration['media_bliptv_mobile_default_title'] ? $configuration['media_bliptv_mobile_default_title'] : '',
//     '#description' => t('Videos which do not belong to a node will be given this title.'),
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_default_description'] = array(
//     '#title' => t('Default description'),
//     '#type' => 'textfield',
//     '#default_value' => $configuration['media_bliptv_mobile_default_description'] ? $configuration['media_bliptv_mobile_default_description'] : 'Default description',
//     '#description' => t('Videos which do not belong to a node will be given this description.'),
//   );
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_default_tags'] = array(
//     '#title' => t('Default tags'),
//     '#type' => 'textfield',
//     '#default_value' => $configuration['media_bliptv_mobile_default_tags'] ? $configuration['media_bliptv_mobile_default_tags'] : '',
//     '#description' => t('Videos which do not belong to a node or which do not have tags will be given these tags. Separate them by space.'),
//   );
//
//   $vocabs = taxonomy_get_vocabularies();
//   $options = array();
//   foreach ($vocabs as $vocab) {
//     $options[$vocab->vid] = $vocab->name;
//   }
//
//   $form['media_bliptv_mobile_conf']['media_bliptv_mobile_vocabs'] = array(
//     '#type' => 'checkboxes',
//     '#title' => t('Take tags from'),
//     '#description' => t('Tags will be taken from the selected vocabularies.'),
//     '#options' => $options,
//     '#default_value' => $configuration['media_bliptv_mobile_vocabs'] ? $configuration['media_bliptv_mobile_vocabs'] : array(),
//   );
//
//   return $form;
// }
