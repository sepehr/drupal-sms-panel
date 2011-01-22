<?php
// $Id$

/**
 * @file
 * SMS Panel Bulk helpers, menu & form callbacks.
 */

/**
 * Form callback for bulk SMS message compose.
 *
 * @ingroup forms
 */
function sms_panel_bulk_form_compose() {
  $form = array();

  // Bulk SMS recipients.
  $form['recipients'] = array(
    '#type' => 'fieldset',
    '#title' => t('Recipients'),
    '#collapsible' => TRUE,
  );

  // User roles.
  $form['recipients']['roles'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Roles'),
    '#collapsible' => TRUE,
  );

  $form['recipients']['roles']['sms_panel_bulk_recipients_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Recipient User Roles'),
    '#description' => t('Check which user roles you want to include as SMS recipients.'),
    '#options' => _sms_panel_bulk_form_helper('roles'),
    '#return_value' => 1,
  );

  // User queues.
  $form['recipients']['queues'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Queues'),
    '#collapsible' => TRUE,
  );

  $form['recipients']['queues']['sms_panel_bulk_recipients_queues'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Recipient User Queues'),
    '#description' => t('Check queues you want to include all of its users as SMS recipients.'),
    '#options' => _sms_panel_bulk_form_helper('queues'),
    '#return_value' => 1,
  );

  // Individual users.
  $form['recipients']['individuals'] = array(
    '#type' => 'fieldset',
    '#title' => t('Individuals'),
    '#collapsible' => TRUE,
  );

  // Bulk composition details.
  $form['sms_panel_bulk_message'] = array(
    '#type' => 'textarea',
    '#title' => t('SMS Message'),
    '#description' => t('SMS text message to send.'),
    '#required' => TRUE,
  );

  $form['sms_panel_bulk_note'] = array(
    '#type' => 'textfield',
    '#title' => t('Optional Note'),
    '#description' => t('Enter an optional hint self-note for this Bulk SMS composition which might help you find sth. in possible future!'),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send'),
  );

  return $form;
}

/**
 * Validation callback for bulk SMS compose form.
 */
function sms_panel_bulk_form_compose_validate($form, &$form_state) {

}

/**
 * Submission callback for bulk SMS compose form.
 */
function sms_panel_bulk_form_compose_submit($form, &$form_state) {

}

/**
 * Helper function to generate required data for administration form.
 *
 * @param $type
 *   Type of the requested data:
 *   - roles
 *   - users
 *   - queues
 *
 * @return
 *   Either the requested data (mixed) or FALSE on fail.
 */
function _sms_panel_bulk_form_helper($type) {
  switch ($type) {
    case 'roles':
      return user_roles(TRUE);

    case 'users':
      $results = db_query("SELECT uid FROM {users}");
      return;

    case 'queues':
      $queues = array();
      $queue_objects = userqueue_load_queues(FALSE);
      foreach ($queue_objects as $queue) {
        $queues[$queue->uqid] = $queue->title;
      }
      return $queues;
  }

  return FALSE;
}

