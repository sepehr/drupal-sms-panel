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
  
  // Introducing {smses} table.
  $data['smses']['table']['group'] = t('SMS');
  
  $data['smses']['table']['base'] = array(
    'field' => 'sid',
    'title' => t('SMS'),
    'help' => t('SMS messages sent or recieved through the website.'),
  );
  
  $data['smses']['sid'] = array(
    'title' => t('SMS identifier'),
    'help' => t('The SMS message ID.'),
    'field' => array(
      'handler' => 'views_handler_field_numeric',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_numeric',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_numeric',
    ),
  );    
  
  $data['smses']['uid'] = array(
    'title' => t('User identifier'),
    'help' => t('The user ID of the SMS message sender.'),
    'field' => array(
      'handler' => 'sms_panel_views_handler_field_user',
      'click sortable' => TRUE,
    ),
    'relationship' => array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'label' => t('user'),
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'title' => t('Name'),
      'handler' => 'views_handler_filter_user_name',
    ),
    'argument' => array(
      'handler' => 'views_handler_argument_uid',
      'name field' => 'name',
    ),
  );
  
  $data['smses']['direction'] = array(
    'title' => t('SMS Outgoing'),
    'help' => t('Whether the SMS message is incoming or outgoing.'),
    'field' => array(
      'handler' => 'views_handler_field_boolean',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_boolean_operator',
      'label' => t('Outgoing SMS'),
      'type' => 'yes-no',
    ),
  );
  
  $data['smses']['number'] = array(
    'title' => t('Phone number'),
    'help' => t('SMS message sender or reciever phone number.'),
    'field' => array(
      'handler' => 'sms_panel_views_handler_field_number',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  
  $data['smses']['message'] = array(
    'title' => t('SMS text'),
    'help' => t('SMS message body text.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
  
  $data['smses']['gateway'] = array(
    'title' => t('SMS gateway'),
    'help' => t('Default SMSFramework gateway used.'),
    'field' => array(
      'handler' => 'views_handler_field_xss',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
  );
    
  $data['smses']['timestamp'] = array(
    'title' => t('SMS operation timestamp.'),
    'help' => t('The date that a SMS message has been sent or recieved.'),
    'field' => array(
      'handler' => 'views_handler_field_date',
      'click sortable' => TRUE,
    ),
    'sort' => array(
      'handler' => 'views_handler_sort_date',
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_date',
    ),
  );
  
  $data['smses']['view_sms'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a link to the SMS message.'),
      'handler' => 'sms_panel_views_handler_field_sms_link',
    ),
  );
  
  $data['smses']['delete_sms'] = array(
    'field' => array(
      'title' => t('Delete Link'),
      'help' => t('Provide a link to delete the SMS message.'),
      'handler' => 'sms_panel_views_handler_field_sms_link_delete',
    ),
  );
  
  // Introducing {sms_user} table,
  // on behalf of sms_user.module, Views your majesty!
  $data['sms_user']['table']['group'] = t('SMS User');
  
  $data['sms_user']['table']['base'] = array(
    'field' => 'uid',
    'title' => t('SMS User'),
    'help' => t('Registered users who filled their cell number.'),
  );
  
  $data['sms_user']['table']['join'] = array(
    'users' => array(
      'left_field' => 'uid',
      'field' => 'uid',
    ),
  );
  
  $data['sms_user']['number'] = array(
    'title' => t('User mobile number'),
    'help' => t('The mobile phone number of the user.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter_string',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  $data['sms_user']['status'] = array(
    'title' => t('User mobile number status'),
    'help' => t('The status of the user mobile phone number.'),
    'field' => array(
      'handler' => 'views_handler_field',
      'click sortable' => TRUE,
    ),
    'filter' => array(
      'handler' => 'views_handler_filter',
    ),
    'sort' => array(
      'handler' => 'views_handler_sort',
    ),
  );
  
  return $data;
}

/**
 * Implementation of hook_views_handlers().
 */
function sms_panel_views_views_handlers() {
  $handlers = array();
  
  $handlers['info'] = array(
    'path' => drupal_get_path('module', 'sms_panel_views') . '/views/handlers',
  );
  
  $handlers['handlers'] = array(
    'sms_panel_views_handler_field_user' => array(
      'parent' => 'views_handler_field_user',
    ),
    'sms_panel_views_handler_field_number' => array(
      'parent' => 'views_handler_field_xss',
    ),
    'sms_panel_views_handler_field_sms_link' => array(
      'parent' => 'views_handler_field',
    ),
    'sms_panel_views_handler_field_sms_link_delete' => array(
      'parent' => 'views_handler_field',
    ),
  );
  
  return $handlers;
}
