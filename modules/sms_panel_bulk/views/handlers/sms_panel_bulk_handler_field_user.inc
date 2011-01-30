<?php
// $Id$

/**
 * @file
 * Implements SMS Panel Bulk SMS user handler.
 */

/**
 * Field handler to provide a UID and Username replacement. Also linking to the user profile.
 */
class sms_panel_bulk_handler_field_user extends views_handler_field {
  /**
   * Override init function to provide generic option.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
    if (!empty($this->options['uid_link_replace'])) {
      $this->additional_fields['uid'] = 'uid';
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['uid_link_replace'] = array('default' => TRUE);
    return $options;
  }

  /**
   * Extends field options form.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['uid_link_replace'] = array(
      '#title' => t('Link this field to its user and replace the UID with username'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['uid_link_replace'],
    );
  }

  /**
   * Replaces a UID with its Username.
   */
  function render_replace($uid) {
    $user = user_load($uid);
    return $user->name;
  }

  /**
   * Makes the user link.
   */
  function render_link($data, $values) {
    if (!empty($this->options['uid_link_replace']) && user_access('access user profiles') && $values->{$this->aliases['uid']} && $data !== NULL && $data !== '') {
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = "user/" . $values->{$this->aliases['uid']};
    }
    return $this->render_replace($values->{$this->aliases['uid']});
  }

  /**
   * Render triger.
   */
  function render($values) {
    return $this->render_link(check_plain($values->{$this->field_alias}), $values);
  }
}

