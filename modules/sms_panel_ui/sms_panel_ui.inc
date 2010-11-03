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
 * Proxy callback for SMS send form.
 *
 * @param $op
 *   SMS operation.
 * @param $sms
 *   SMS object.
 *
 * @return
 *   Prepopulated SMS send form.
 */
function sms_panel_ui_callback_sms_actions($sms, $op) {
  switch ($op) {
    case 'reply':
      return drupal_get_form('sms_panel_ui_form_sms_send', $sms->number, NULL);
      
    case 'resend':
      return drupal_get_form('sms_panel_ui_form_sms_send', $sms->number, $sms->message);
    
    case 'forward':
      return drupal_get_form('sms_panel_ui_form_sms_send', NULL, $sms->message);
  }
}

/**
 * Form callback for SMS panel configuration options.
 *
 * @ingroup forms
 * @see system_settings_form()
 */
function sms_panel_ui_form_configure() {
  $form = array();
  
  $form['general_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );
  
  $form['dashboard_settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dashboard Settings'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );    
    
  if (module_exists('sms_panel_ui_datepicker')) {
    $form['datepicker_settings'] = array(
      '#type' => 'fieldset',
      '#title' => t('Datepicker Settings'),
      '#collapsible' => TRUE,
      '#collapsed' => TRUE,
    );
  }
  
  return system_settings_form($form);
}

/**
 * Form callback for SMS message sending.
 *
 * @param $form_state
 *   The form state.
 * @param $number
 *   Phone number to be prepopulated.
 * @param $message
 *   SMS message text to be prepopulated.
 *
 * @todo
 *   Add Token integration.
 *
 * @ingroup forms
 */
function sms_panel_ui_form_sms_send(&$form_state, $number = NULL, $message = NULL) {
  $form = sms_send_form();

  $form['recipients'] = array(
    '#type' => 'fieldset',
    '#title' => t('Recipients'),
    '#description' => t('Choose which users and/or phone numbers to send the SMS message to.'),
    '#collapsible' => TRUE,
    '#collapsed' => FALSE,
  );    
  
  $form['number']['#title'] = t('Recipient numbers');
  $form['number']['#autocomplete_path'] = 'sms/numbers/autocomplete';
  $form['number']['#default_value'] = (is_null($number) || empty($number)) ? '' : $number;
  $form['number']['#description'] = t('Enter a comma separated list of valid phone numbers to send the SMS message to.');
  $form['recipients']['number'] = $form['number'];
  unset($form['number']);
  
  $form['recipients']['user'] = array(
    '#type' => 'textfield',
    '#title' => t('Recipient users'),
    '#description' => t('Enter a comma separated list of phone activated users to send the SMS message to.'),
    '#autocomplete_path' => 'sms/users/autocomplete',
  );
  
  $form['message'] = array(
    '#type' => 'textarea',
    '#title' => t('SMS Message'),
    '#description' => t('SMS text message to send.'),
    '#default_value' => is_null($message) ? '' : check_plain($message),
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );
  
  return $form;
}

/**
 * Form validation callback for SMS message sending.
 */
function sms_panel_ui_form_sms_send_validate($form, &$form_state) {
  if (empty($form_state['values']['message'])) {
    form_set_error('message', t('Please fill out the SMS message.'));
    return;
  }
  
  if (empty($form_state['values']['number']) && empty($form_state['values']['user'])) {
    form_set_error('number', t('As a minimum, you need to set a number or user as recipient.'));
    return;
  }
  
  // Validate recipient numbers.
  // TODO: Validate the number using the smsframework's
  // sms_validate_number() when it's fixed in its coming verison.
  $form_state['values']['number'] = explode(',', $form_state['values']['number']);
  $form_state['values']['number'] = array_map('trim', $form_state['values']['number']);
  foreach ($form_state['values']['number'] as $number) {
    if ($number != sms_formatter($number)) {
      form_set_error('number', t('!number is not a valid phone number.', array('!number' => $number)));
    }
  }
  
  // Validate recipient users.
  $query = "SELECT u.name, su.number 
            FROM {sms_user} su
            LEFT JOIN {users} u ON su.uid = u.uid
            WHERE su.status = 2 AND u.name = '%s'";
  $form_state['values']['user'] = explode(',', $form_state['values']['user']);
  $form_state['values']['user'] = array_map('trim', $form_state['values']['user']);
  foreach ($form_state['values']['user'] as $username) {
    $account = db_fetch_object(db_query($query, $username));
    if ($account->name != $username) {
      form_set_error('user', t('<em>!user</em> has no activated phone number associated with his/her account.', array('!user' => $username)));
      return;
    }
    else {
      $form_state['values']['number'][] = $account->number;
    }
  }
}

/**
 * Form submission callback for SMS message sending.
 */
function sms_panel_ui_form_sms_send_submit($form, &$form_state) {
  $form_state['values']['number'] = array_filter($form_state['values']['number']);
  $form_state['values']['number'] = array_unique($form_state['values']['number']);
  
  foreach ($form_state['values']['number'] as $number) {
    sms_send($number, $form_state['values']['message']);
  }
}

/**
 * Form callback for SMS message info page.
 *
 * @param $sms
 *   The SMS object.
 *
 * @return
 *   The form.
 *
 * @ingroup forms
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
  
  $sms_user = $sms->uid ? user_load($sms->uid) : t('System');
  $form['sms_info']['sms_info_user'] = array(
    '#type' => 'item',
    '#title' => is_object($sms_user) ? t('Sender') : t('Recipient'),
    '#value' => is_object($sms_user) ? l($sms_user->name, 'user/' . $sms_user->uid) : $sms_user,
  );
    
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
    '#title' => $sms->direction ? t('Sent Date') : t('Receive Date'),
    '#value' => format_date($sms->timestamp, 'long'),
  );
  
  $form['sms_info']['sms_info_gateway'] = array(
    '#type' => 'item',
    '#title' => $sms->direction ? t('Sent through') : t('Received via'),
    '#value' => $sms->gateway . ' ' . t('gateway'),
  );
  
  $form['sms_info']['sms_info_message'] = array(
    '#type' => 'markup', 
    '#prefix' => '<div class="form-item sms-info-message-wrapper">',
    '#value' => '<label>' . t('Message text:') . '</label><div class="sms-info-message">' . $sms->message . '</div>',
    '#suffix' => '</div>',
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
  // TODO: Find a better way to go to delete confirm page.
  if ($form_state['values']['op'] == t('Delete')) {
    drupal_goto(sms_url($form['sms_info']['sms_info_sid']['#value']) . '/delete');
    exit();
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
 * Callback for user phone number autocomplete widget.
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
 * Callback for multi user phone numbers autocomplete widget.
 *
 * Returns a JSON object containing autocomplete suggestions.
 */
function sms_numbers_autocomplete($string = '') {
  $matches = array();
  
  if ($string) {
    $numbers = explode(',', $string);
    $numbers = array_map('trim', $numbers);
    
    $number = array_pop($numbers);
    $query = "SELECT su.number
              FROM {sms_user} su
              WHERE su.number LIKE '%s%%'
              UNION
              SELECT s.number
              FROM {smses} s
              WHERE s.number LIKE '%s%%'";
    $result = db_query_range($query, $number, $number, 0, 10);
    while ($user = db_fetch_object($result)) {
      if (count($numbers)) {
        $matches[implode(', ', $numbers) . ', ' . $user->number] = check_plain($user->number);
      }
      else {
        $matches[$user->number] = check_plain($user->number);
      }
    }
  }

  drupal_json($matches);
}

/**
 * Callback for multi username autocomplete widget.
 *
 * Returns a JSON object containing autocomplete suggestions.
 */
function sms_users_autocomplete($string = '') {
  $matches = array();
  
  if ($string) {
    $names = explode(',', $string);
    $names = array_map('trim', $names);
    
    $name = array_pop($names);
    $query = "SELECT u.name
              FROM {sms_user} su
              LEFT JOIN {users} u ON su.uid = u.uid
              WHERE su.status = 2 AND LOWER(name) LIKE '%s%%'";
    $result = db_query_range($query, $name, 0, 10);
    while ($user = db_fetch_object($result)) {
      if (count($names)) {
        $matches[implode(', ', $names) . ', ' . $user->name] = check_plain($user->name);
      }
      else {
        $matches[$user->name] = check_plain($user->name);
      }
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
