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
function sms_panel_ui_page_sms($sid) {
  $sms = sms_load($sid);
  return 'asd';//theme('sms', $sms);
} 

/**
 * Menu callback for Dashboard page.
 */
function sms_panel_ui_callback_sms_info($sid) {
  $sms = sms_load($sid);
  return 'asd2';//theme('sms_info', $sms);
}
 
/**
 * Form definition for SMS delete confirmation.
 */
function sms_panel_ui_sms_delete_confirm(&$form_state, $sid) {
  $form = array();
  
  $form['sid'] = array(
    '#type' => 'value',
    '#value' => $sid,
  );

  return confirm_form($form,
    t('Are you sure you want to delete the %title log?', array('%title' => sms_title($sid))),
    isset($_GET['destination']) ? $_GET['destination'] : sms_url($sid),
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Submition callback for SMS delete confirm form.
 */
function sms_panel_ui_sms_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    sms_delete($form_state['values']['sid']);
  }

  $form_state['redirect'] = 'admin/smsframework/panel';
}
