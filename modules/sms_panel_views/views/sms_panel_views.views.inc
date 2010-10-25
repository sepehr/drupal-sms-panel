<?php
// $Id$

/**
 * @file
 * Implements Views module hooks.
 */

/**
 * Implementation of hook_views_data().
 */
function sms_panel_views_views_data() {
  $data = array();

  $data['sms_panel']['table']['group'] = t('SMS');
  
  $data['sms_panel']['sid'] = array(
    'title' => t('SMS identifier'),
    'help' => t('The SMS message identifier.'),
    'field' => array(
    
    ),
    'sort' => array(
    
    ),
    'filter' => array(
    
    ),
    'argument' => array(
    
    ),    
  );
  
  // TODO: Introduce other {sms_messages} fields.
  
  return $data;
}

// TODO: Implement hook_views_default_views().
