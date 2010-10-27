<?php
// $Id$

/**
 * @file
 * Contains SMS Panel UI forms, menu pages and callbacks.
 */

/**
 * Menu callback for Dashboard page.
 */
function sms_panel_ui_page_dashboard() {
  // TODO: Build dashboard page.
}

/**
 * Menu callback for the SMS page.
 */
function sms_panel_ui_page_sms($sms) {
  drupal_add_css(drupal_get_path('module', 'sms_panel_ui') . '/css/sms-panel-ui-sms.css', 'module');
  return drupal_get_form('sms_panel_ui_form_sms_info', $sms);
}

/**
 * Theme callback SMS number.
 *
 * @param $number
 *   The SMS message number.
 *
 * @return
 *   Themed SMS number.
 */
function sms_panel_ui_theme_number($number, $show_username) {
  $uid = sms_user_get_uid($number);
  if (!$uid) {
    return $number;    
  }
  
  if ($show_username) {
    $user = user_load($uid);
    $number = $number . ' (' . $user->name . ')';
  }
  
  return l($number, 'user/' . $uid);
}

/**
 * Form callback for SMS message info page.
 *
 * @param $sms
 *   The SMS object.
 *
 * @return
 *   The form.
 */
function sms_panel_ui_form_sms_info(&$form_state, $sms) {
  $form = array();
  
  $form['sms_info'] = array(
    '#type' => 'fieldset',
    '#title' => t('Message information'),
    '#description' => t('All the information we have about this SMS message.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );
  
  $form['sms_info']['sms_info_sid'] = array(
    '#type' => 'item',
    '#title' => t('SMS ID'),
    '#value' => $sms->sid,
  );      
  
  if ($sms->direction) {
    $sender = user_load($sms->uid);
    $form['sms_info']['sms_info_user'] = array(
      '#type' => 'item',
      '#title' => t('Sender'),
      '#value' => l($sender->name, 'user/' . $sender->uid),
    );
  }
  
  $form['sms_info']['sms_info_number'] = array(
    '#type' => 'item',
    '#title' => $sms->direction ? t('Recipient') : t('Sender'),
    '#value' => theme('sms_number', $sms->number, TRUE),
  );
  
  $form['sms_info']['sms_info_direction'] = array(
    '#type' => 'item',
    '#title' => t('SMS Direction'),
    '#value' => $sms->direction ? t('Outgoing') : t('Incoming'),
  );
  
  $form['sms_info']['sms_info_timestamp'] = array(
    '#type' => 'item',
    '#title' => $sms->direction ? t('Sent Date') : t('Recieve Date'),
    '#value' => format_date($sms->timestamp, 'long'),
  );
  
  $form['sms_info']['sms_info_gateway'] = array(
    '#type' => 'item',
    '#title' => $sms->direction ? t('Sent through') : t('Recieved via'),
    '#value' => $sms->gateway . ' ' . t('gateway'),
  );
  
  $form['sms_info']['sms_info_message'] = array(
    '#type' => 'markup', 
    '#prefix' => '<div class="form-item sms-info-message-wrapper">',
    '#value' => '<label>' . t('Message text:') . '</label><div class="sms-info-message">' . $sms->message . '</div>',
    '#suffix' => '</div>',
  );
  
  $form['buttons']['resend'] = array(
    '#type' => 'button',
    '#value' => t('Resend'),
  );
  
  $form['buttons']['delete'] = array(
    '#type' => 'button',
    '#value' => t('Delete'),
  );

  return $form;
}

/**
 * Form validation callback for SMS message info page.
 */
function sms_panel_ui_form_sms_info_validate($form, &$form_state) {
  switch ($form_state['values']['op']) {
    case 'Delete':
      // TODO: Find a better way to go to delete confirm page.
      drupal_goto(sms_url($form['sms_info']['sms_info_sid']['#value']) . '/delete');
      exit();
    
    case 'Resend':
      
      break;
  }
}

/**
 * Form callback for SMS delete confirmation.
 */
function sms_panel_ui_sms_delete_confirm(&$form_state, $sms) {
  $form = array();
  
  $form['sid'] = array(
    '#type' => 'value',
    '#value' => $sms,
  );

  return confirm_form($form,
    t('Are you sure you want to delete the %title log?', array('%title' => sms_title($sms))),
    isset($_GET['destination']) ? $_GET['destination'] : sms_url($sms),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Form submission callback for SMS delete confirm form.
 */
function sms_panel_ui_sms_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    sms_delete($form_state['values']['sid']);
  }
  $form_state['redirect'] = 'admin/smsframework/panel';
}

/**
 * Callback for user phone numbers autocomplete widget.
 *
 * Returns a JSON object containing autocomplete suggestions.
 */
function sms_number_autocomplete($string = '') {
  $matches = array();
  if ($string) {
    $result = db_query_range("SELECT number FROM {sms_user} WHERE number LIKE '%s%%'", $string, 0, 10);
    while ($user = db_fetch_object($result)) {
      $matches[$user->number] = check_plain($user->number);
    }
  }

  drupal_json($matches);
}

/**
 * Callback for SMS gateways autocomplete widget.
 *
 * Returns a JSON object containing autocomplete suggestions.
 */
function sms_gateway_autocomplete($string = '') {
  $matches = array();
  if ($string) {
    $gateways = sms_gateways();
    $gateways = _sms_gateway_autocomplete_match($gateways, $string);

    foreach ($gateways as $gateway) {
      $matches[$gateway] = check_plain($gateway);
    }
  }

  drupal_json($matches);
}

/**
 * Helper function to match part of a string against passed array keys.
 *
 * @param $array
 *   The array to match against.
 * @param $string
 *   Search string.
 *
 * @return
 *   Refined array.
 */
function _sms_gateway_autocomplete_match($array, $search) {
  foreach ($array as $key => $value) {
    sscanf($key, "$search%s", $result);
    if ($result == FALSE) {
      unset($array[$key]);
    }
  }
  
  return array_keys($array);
}
