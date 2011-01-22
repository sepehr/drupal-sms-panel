<?php
// $Id$

/**
 * @file
 * Implements Views module hooks.
 */

/**
 * Implements hook_views_data().
 */
function sms_panel_views_views_data() {
  $data = array();

  // Introducing {sms_panel_bulk} table.
  $data['sms_panel_bulk']['table']['group'] = t('Bulk SMS');

  $data['sms_panel_bulk']['table']['base'] = array(
    'field' => 'bid',
    'title' => t('Bulk SMS'),
    'help' => t('Bulk SMS messages sent through the website.'),
  );

  $data['sms_panel_bulk']['bid'] = array(
    'title' => t('Bulk SMS identifier'),
    'help' => t('The Bulk SMS message ID.'),
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

  $data['sms_panel_bulk']['uid'] = array(
    'title' => t('User identifier'),
    'help' => t('The user ID of the Bulk SMS message sender.'),
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

  $data['sms_panel_bulk']['note'] = array(
    'title' => t('Bulk SMS note'),
    'help' => t('Bulk SMS message note.'),
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

  $data['sms_panel_bulk']['message'] = array(
    'title' => t('Bulk SMS text'),
    'help' => t('Bulk SMS message body text.'),
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

  $data['sms_panel_bulk']['gateway'] = array(
    'title' => t('SMS gateway'),
    'help' => t('Default SMSFramework gateway used to send the Bulk SMS message.'),
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

  $data['sms_panel_bulk']['timestamp'] = array(
    'title' => t('SMS operation timestamp.'),
    'help' => t('The date that the bulk SMS message has been sent.'),
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

  $data['sms_panel_bulk']['view_bulk_sms'] = array(
    'field' => array(
      'title' => t('Link'),
      'help' => t('Provide a link to the Bulk SMS message.'),
      'handler' => 'sms_panel_bulk_handler_field_sms_link',
    ),
  );

  $data['sms_panel_bulk']['resend_sms'] = array(
    'field' => array(
      'title' => t('Resend Link'),
      'help' => t('Provide a link to resend the Bulk SMS message.'),
      'handler' => 'sms_panel_bulk_handler_field_sms_link_resend',
    ),
  );

  $data['sms_panel_bulk']['forward_sms'] = array(
    'field' => array(
      'title' => t('Forward Link'),
      'help' => t('Provide a link to forward the SMS message.'),
      'handler' => 'sms_panel_bulk_handler_field_sms_link_forward',
    ),
  );

  $data['sms_panel_bulk']['delete_sms'] = array(
    'field' => array(
      'title' => t('Delete Link'),
      'help' => t('Provide a link to delete the SMS message.'),
      'handler' => 'sms_panel_bulk_handler_field_sms_link_delete',
    ),
  );

  return $data;
}

/**
 * Implements hook_views_handlers().
 */
function sms_panel_bulk_views_handlers() {
  $handlers = array();
  $handlers['info'] = array(
    'path' => drupal_get_path('module', 'sms_panel_bulk') . '/views/handlers',
  );

  $handlers['handlers'] = array(
    'sms_panel_bulk_handler_field_user' => array(
      'parent' => 'views_handler_field_user',
    ),
    'sms_panel_bulk_handler_field_number' => array(
      'parent' => 'views_handler_field_xss',
    ),
    'sms_panel_bulk_handler_field_sms_link' => array(
      'parent' => 'views_handler_field',
    ),
    'sms_panel_bulk_handler_field_sms_link_resend' => array(
      'parent' => 'views_handler_field',
    ),
    'sms_panel_bulk_handler_field_sms_link_forward' => array(
      'parent' => 'views_handler_field',
    ),
    'sms_panel_bulk_handler_field_sms_link_delete' => array(
      'parent' => 'views_handler_field',
    ),
  );

  return $handlers;
}

