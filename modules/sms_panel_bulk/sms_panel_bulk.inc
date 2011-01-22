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

  $form['sms_panel_bulk_details'] = array(
    '#type' => 'fieldset',
    '#title' => t('Bulk SMS Details'),
    '#collapsible' => TRUE,
  );

  $form['sms_panel_bulk_details']['sms_panel_bulk_message'] = array(
    '#type' => 'textarea',
    '#title' => t('SMS Message'),
    '#description' => t('SMS text message to send.'),
  );

  $form['sms_panel_bulk_details']['sms_panel_bulk_note'] = array(
    '#type' => 'textfield',
    '#title' => t('Optional Hint'),
    '#description' => t('Enter an optional hint self-note for this Bulk SMS composition.'),
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

