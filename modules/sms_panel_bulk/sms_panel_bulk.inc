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

  $options = _sms_panel_bulk_field_options('roles');
  if (!empty($options)) {
    $form['recipients']['roles']['sms_panel_bulk_recipients_roles'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Recipient User Roles'),
      '#description' => t('Check which user roles you want to include as SMS recipients.'),
      '#options' => $options,
      '#return_value' => 1,
    );
  }
  // There were no roles defined.
  else {
    $form['recipients']['roles']['sms_panel_bulk_recipients_roles_notice'] = array(
      '#type' => 'item',
      '#title' => t('Notice'),
      '#value' => t('There were no user roles defined in this system. You might want to <a href="!link">add one</a>.',
        array('!link' => url('admin/user/roles'))
      ),
    );
  }

  // User queues.
  $form['recipients']['queues'] = array(
    '#type' => 'fieldset',
    '#title' => t('User Queues'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $options = _sms_panel_bulk_field_options('queues');
  if (!empty($options)) {
    $form['recipients']['queues']['sms_panel_bulk_recipients_queues'] = array(
      '#type' => 'checkboxes',
      '#title' => t('Recipient User Queues'),
      '#description' => t('Check queues you want to include all of its users as SMS recipients.'),
      '#options' => $options,
      '#return_value' => 1,
    );
  }
  // There were no userqueues.
  else {
    $form['recipients']['queues']['sms_panel_bulk_recipients_queues_notice'] = array(
      '#type' => 'item',
      '#title' => t('Notice'),
      '#value' => t('There were no user queues defined. You might want to <a href="!link">define one</a> and then push a few users in.',
        array('!link' => url('admin/user/userqueue'))
      ),
    );
  }

  // Individual users.
  $form['recipients']['individuals'] = array(
    '#type' => 'fieldset',
    '#title' => t('Individual Users'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $options = _sms_panel_bulk_field_options('users');
  if (!empty($options)) {
    $form['recipients']['individuals']['sms_panel_bulk_recipients_users'] = array(
      '#type' => 'checkboxes',
      '#options' => $options,
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
  }
  // There were no verified sms users.
  else {
    $form['recipients']['individuals']['sms_panel_bulk_recipients_users_notice'] = array(
      '#type' => 'item',
      '#title' => t('Notice'),
      '#value' => t('There were no user with a verified number.',
        array('!link' => url('admin/user/userqueue'))
      ),
    );
  }

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

  // Dispatch methods.
  $form['dispatch_method'] = array(
    '#type' => 'fieldset',
    '#title' => t('Dispatch Method'),
    '#collapsible' => TRUE,
    '#collapsed' => TRUE,
  );

  $form['dispatch_method']['sms_panel_bulk_dispatch_method'] = array(
    '#type' => 'radios',
    '#title' => t('Choose how to send this message'),
    '#required' => TRUE,
    '#default_value' => 'batch',
    '#attributes' => array(
      'class' => 'sms-panel-bulk-dispatch-methods',
    ),
    '#options' => array(
      'batch' => t('Batch Operation (Default)'),
      'regular' => t('Regular (Not recommended)'),
      'cron_queue' => t('Dispatch on cron (Requires <a href="!link">Job Queue</a>)',
        array('!link' => 'http://drupal.org/project/job_queue')
      ),
    ),
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
 */
function sms_panel_bulk_form_compose_validate($form, &$form_state) {
  // Check number of selected recipients.
  // roles, queues, users and numbers. all.
  if (!_sms_panel_bulk_count_recipients($form_state['values'])) {
    form_set_error('sms_panel_bulk_recipients_roles', t('You need to specify one recipient role, queue, user or number at least.'));
  }

  // Validate recipient numbers.
  // TODO: Validate the number using the smsframework's
  // sms_validate_number() when it's fixed in its forthcoming release.
  $form_state['values']['number'] = array_map('trim', explode(',', $form_state['values']['number']));
  foreach ($form_state['values']['number'] as $number) {
    if ($number != sms_formatter($number)) {
      form_set_error('sms_panel_bulk_recipients_numbers', t('!number is not a valid phone number.', array('!number' => $number)));
    }
  }

  // Validate new userqueue creation parameters.
  if ($form_state['values']['sms_panel_bulk_individuals_queue'] &&
      empty($form_state['values']['sms_panel_bulk_individuals_queue_name'])) {
    form_set_error('sms_panel_bulk_individuals_queue_name', t('You need to specify a user queue name in order to save your selections.'));
  }

  // Check for duplicate userqueue names.
  $queue_names = array();
  $queues = array_values(userqueue_load_queues(FALSE));
  foreach ($queues as $queue) {
    $queue_names[] = strtolower($queue->title);
  }
  if (in_array(strtolower($form_state['values']['sms_panel_bulk_individuals_queue_name']), $queue_names)) {
    form_set_error('sms_panel_bulk_individuals_queue_name', t('There were a user queue created with this name before, please choose another name.'));
  }

  // Validate dispatch method.
  if ($form_state['values']['sms_panel_bulk_dispatch_method'] == 'cron_queue' && !module_exists('job_queue')) {
    form_set_error('sms_panel_bulk_dispatch_method', t('You need to install <a href="!link">Job Queue</a> first, so we can add the operation to the cron queue.'));
  }
}

/**
 * Submission callback for bulk SMS compose form.
 */
function sms_panel_bulk_form_compose_submit($form, &$form_state) {
  $numbers = array();

  // Process user roles.
  $selections = _sms_panel_bulk_checkboxes_selections($form_state['values']['sms_panel_bulk_recipients_roles'], FALSE);
  if (!empty($selections)) {
    foreach ($selections as $rid) {
      $results = db_query("SELECT number FROM {users_roles} ur
                          LEFT JOIN {sms_user} su ON ur.uid = su.uid
                          WHERE ur.rid = '%d' AND su.delta = 0 AND su.status = 2", $rid);
      while ($recipient = db_fetch_object($results)) {
        $numbers[] = $recipient->number;
      }
    }
    // Manual garbage collecting.
    // TODO: Profile and see if it's necessary.
    unset($recipient, $results, $rid);
  }

  // Process user queues.
  $selections = _sms_panel_bulk_checkboxes_selections($form_state['values']['sms_panel_bulk_recipients_queues'], FALSE);
  if (!empty($selections)) {
    foreach ($selections as $uqid) {
      $queue_users = userqueue_get_users($uqid, TRUE);

      foreach ($queue_users as $uid => $queue_user) {
        if ($queue_user->sms_user[0]['number'] && $queue_user->sms_user[0]['status'] == 2) {
          $numbers[] = $queue_user->sms_user[0]['number'];
        }
      }
    }
    // Manual garbage collecting.
    // TODO: Profile and see if it's necessary.
    unset($queue_users, $queue_user, $uqid);
  }

  // Process individual users.
  $selections = _sms_panel_bulk_checkboxes_selections($form_state['values']['sms_panel_bulk_recipients_users'], FALSE);
  if (!empty($selections)) {
    // Create a new userqueue if requested so.
    if ($form_state['values']['sms_panel_bulk_individuals_queue']) {
      $queue = new stdClass();
      $queue->size = 0;
      $queue->title = $form_state['values']['sms_panel_bulk_individuals_queue_name'];
      drupal_write_record('userqueue', $queue);

      // Get the new queue ID.
      $queue->uqid = db_result(db_query("SELECT LAST_INSERT_ID()"));
    }

    // Per each selection, save the number
    // and add the uselection to new queue if available.
    foreach ($selections as $uid) {
      $user = user_load($uid);
      $numbers[] = $user->sms_user[0]['number'];

      // Add user to the new queue.
      if ($queue->uqid) {
        $queue_user = new stdClass();
        $queue_user->uid = $user->uid;
        $queue_user->uqid = $queue->uqid;
        drupal_write_record('userqueue_user', $queue_user);
      }
    }
    unset($queue, $uid, $user, $queue_user);
  }

  // Process anonymous numbers.
  $selections = array_map('trim', explode(',', $form_state['values']['sms_panel_bulk_recipients_numbers']));
  foreach ($selections as $number) {
    $numbers[] = $number;
  }

  // Remove empties and dupes, check length validity.
  $numbers = array_unique(array_filter($numbers));
  if (empty($numbers) || !is_array($numbers)) {
    drupal_set_message('Could not find any numbers to dispatch the send operation against. Please try again.', 'warning');
    return;
  }

  // Dispatch bulk sending.
  sms_panel_bulk_dispatch(
    $numbers,
    $form_state['values']['sms_panel_bulk_message'],
    $form_state['values']['sms_panel_bulk_note'],
    $form_state['values']['sms_panel_bulk_dispatch_method']
  );
}

/**
 * Theme callback for bulk compose form.
 *
 * @todo
 * - Implement tablesort API.
 * - Add pagination & preserve user selections.
 */
function theme_sms_panel_bulk_form_compose($form) {
  if (isset($form['recipients']['individuals']['sms_panel_bulk_recipients_users']) &&
      !empty($form['recipients']['individuals']['sms_panel_bulk_recipients_users'])) {
    // Per each checkbox option, remove the title
    // and move the naked checkbox to a table cell!
    $rows = array();
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
  }

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
      $results = db_query("SELECT uid FROM {sms_user} WHERE status = 2");
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
  return count($values['sms_panel_bulk_recipients_numbers']) +
         _sms_panel_bulk_checkboxes_selections($values['sms_panel_bulk_recipients_roles']) +
         _sms_panel_bulk_checkboxes_selections($values['sms_panel_bulk_recipients_users']) +
         _sms_panel_bulk_checkboxes_selections($values['sms_panel_bulk_recipients_queues']);
}

/**
 * Helper function to count the number of a form checkbox list selection.
 *
 * @param $array
 *   An array of checkbox values.
 * @param $count
 *   Whether to return the count or selections themeselves.
 *
 * @return
 *   The count of selected checkboxes.
 */
function _sms_panel_bulk_checkboxes_selections($array, $count = TRUE) {
  if (!is_array($array)) {
    return 0;
  }

  $selecions = array_filter($array,
    create_function(
      '$checkbox_value',
      'return $checkbox_value != 0;'
    )
  );
  return $count ? intval(count($selecions)) : $selecions;
}

