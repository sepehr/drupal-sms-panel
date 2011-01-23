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
    '#collapsed' => TRUE,
  );

  $form['recipients']['roles']['sms_panel_bulk_recipients_roles'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Recipient User Roles'),
    '#description' => t('Check which user roles you want to include as SMS recipients.'),
    '#options' => _sms_panel_bulk_field_options('roles'),
    '#return_value' => 1,
  );

  // User queues.
  $form['recipients']['queues'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Queues'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['recipients']['queues']['sms_panel_bulk_recipients_queues'] = array(
    '#type' => 'checkboxes',
    '#title' => t('Recipient User Queues'),
    '#description' => t('Check queues you want to include all of its users as SMS recipients.'),
    '#options' => _sms_panel_bulk_field_options('queues'),
    '#return_value' => 1,
  );

  // Individual users.
  $form['recipients']['individuals'] = array(
    '#type' => 'fieldset',
    '#title' => t('Individual Users'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['recipients']['individuals']['sms_panel_bulk_recipients_users'] = array(
    '#type' => 'checkboxes',
    '#options' => _sms_panel_bulk_field_options('users'),
    '#return_value' => 1,
  );

  $form['recipients']['individuals']['sms_panel_bulk_recipients_users_title'] = array(
    '#type' => 'markup',
    '#value' => '<div class="form-item"><label>' . t('Individual recipients:') . '</label></div>',
  );

  // Save selected individual users as a new userqueue.
  $form['recipients']['individuals']['save_queue'] = array(
    '#type' => 'fieldset',
    '#title' => t('Save User Queue'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
    '#prefix' => '<br />',
  );

  $form['recipients']['individuals']['save_queue']['sms_panel_bulk_individuals_queue'] = array(
    '#type' => 'checkbox',
    '#title' => t('Save selections as a new user queue named below.'),
    '#description' => t('If checked selected users will be saved into a user queue.'),
    '#return_value' => 1,
  );

  $form['recipients']['individuals']['save_queue']['sms_panel_bulk_individuals_queue_name'] = array(
    '#type' => 'textfield',
    '#title' => t('User Queue Name'),
    '#description' => t('If checked the above checkbox, this is a mandatory field.'),
  );

  // Anonymous numbers.
  $form['recipients']['anonymous'] = array(
    '#type' => 'fieldset',
    '#title' => t('Anonymous Numbers'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['recipients']['anonymous']['sms_panel_bulk_recipients_numbers'] = array(
    '#type' => 'textfield',
    '#title' => t('Numbers'),
    '#description' => t('Comma separated list of anonymous numbers to include in recipients list.'),
    '#autocomplete_path' => 'sms/numbers/autocomplete',
  );

  // Bulk composition details.
  // TODO: Token support.
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
 *
 * @todo Validate exact recipients quantity.
 */
function sms_panel_bulk_form_compose_validate($form, &$form_state) {
  // Check minimum number of selected recipients.
  if (!_sms_panel_bulk_count_recipients($form_state['values'])) {
    form_set_error('sms_panel_bulk_recipients_roles', t('You need to specify one recipient role, queue, user or number at least.'));
  }
}

/**
 * Submission callback for bulk SMS compose form.
 */
function sms_panel_bulk_form_compose_submit($form, &$form_state) {

}

/**
 * Theme callback for bulk compose form.
 *
 * @todo
 * - Add tablesort ability.
 * - Add table pagination.
 */
function theme_sms_panel_bulk_form_compose($form) {
  $rows = array();

  // Per each checkbox option, remove the title
  // and move the naked checkbox to a table cell!
  $options = element_children($form['recipients']['individuals']['sms_panel_bulk_recipients_users']);

  foreach ($options as $uid) {
    $title = $form['recipients']['individuals']['sms_panel_bulk_recipients_users'][$uid]['#title'];
    unset($form['recipients']['individuals']['sms_panel_bulk_recipients_users'][$uid]['#title']);

    $user = user_load($uid);
    $rows[] = array(
      array('data' => drupal_render($form['recipients']['individuals']['sms_panel_bulk_recipients_users'][$uid])),
      theme('username', $user),
      check_plain($user->sms_user[0]['number']),
      check_plain($user->mail),
      format_date($user->login, 'long'),
    );
  }

  // Table header.
  $headers = array(
    '',
    t('Username'),
    t('Number'),
    t('Email Address'),
    t('Last Login'),
  );

  // Inject the table into the form and render.
  $form['recipients']['individuals']['sms_panel_bulk_recipients_users_title']['#suffix'] = theme('table', $headers, $rows);
  return drupal_render($form);
}

/**
 * Helper function to generate required options data for bulk compose form.
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
function _sms_panel_bulk_field_options($type) {
  switch ($type) {
    case 'roles':
      $roles = array();
      foreach (user_roles(TRUE) as $rid => $name) {
        $count = db_result(db_query("SELECT COUNT(uid) FROM {users_roles} WHERE rid = '%d'", $rid));
        if ($count > 0) {
          $roles[$rid] = ucwords($name) . ' ' . t('<small>(!count)</small>',
            array('!count' => format_plural($count, 'one user', '@count users'))
          );
        }
      }
      return $roles;

    case 'queues':
      $queues = array();
      $queue_objects = userqueue_load_queues(TRUE);
      foreach ($queue_objects as $queue) {
        $count = count($queue->users);
        if ($count > 0) {
          $queues[$queue->uqid] = ucwords($queue->title) . ' ' . t('<small>(!count)</small>',
            array('!count' => format_plural($count, 'one user', '@count users'))
          );
        }
      }
      return $queues;

    case 'users':
      $sms_users = array();
      $results = db_query("SELECT uid FROM {sms_user} WHERE status = '2'");
      while ($uid = db_fetch_object($results)) {
        $user = user_load($uid->uid);
        $sms_users[$uid->uid] = check_plain($user->name);
      }
      return $sms_users;
  }

  return FALSE;
}

/**
 * Helper function to count selected recipients of the compose form.
 *
 * @param $values
 *   Form state 'values' subarray.
 *
 * @return
 *   Count of selected recipients including users, roles, userqueues, etc.
 */
function _sms_panel_bulk_count_recipients($values) {
  $values['sms_pquaanel_bulk_recipients_numbers'] = array_map('trim', explode(
    ',',
    $values['sms_panel_bulk_recipients_numbers']
  ));

  // Calculate recipient selection count, not the exact number quantity.
  return _sms_panel_bulk_count_checkboxes($values['sms_panel_bulk_recipients_roles'])  +
         _sms_panel_bulk_count_checkboxes($values['sms_panel_bulk_recipients_users'])  +
         _sms_panel_bulk_count_checkboxes($values['sms_panel_bulk_recipients_queues']) +
         _sms_panel_bulk_count_checkboxes($values['sms_panel_bulk_recipients_numbers']);
}

/**
 * Helper function to count the number of a form checkbox list selection.
 *
 * @param $array
 *   An array of checkbox values.
 *
 * @return
 *   The count of selected checkboxes.
 */
function _sms_panel_bulk_count_checkboxes($array) {
  return intval(count(array_filter($array,
    create_function(
      '$checkbox_value',
      'return $checkbox_value != 0;'
    ))
  ));
}

