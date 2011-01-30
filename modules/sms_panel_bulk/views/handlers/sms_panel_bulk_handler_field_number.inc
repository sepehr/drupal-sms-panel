<?php
// $Id$

/**
 * @file
 * Implements SMS Panel Bulk number handler.
 */

/**
 * Field handler to provide a UID and Username replacement. Also linking to the user profile.
 */
class sms_panel_bulk_handler_field_number extends views_handler_field {
  /**
   * Override init function to provide generic option.
   */
  function init(&$view, &$data) {
    parent::init($view, $data);
    if (!empty($this->options['link_user_number'])) {
      $this->additional_fields['number'] = 'number';
    }
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['link_user_number'] = array('default' => TRUE);
    return $options;
  }

  /**
   * Extends field options form.
   */
  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['link_user_number'] = array(
      '#title' => t('Link this number to its user if possible.'),
      '#description' => t('This will override any other link you have set.'),
      '#type' => 'checkbox',
      '#default_value' => $this->options['link_user_number'],
    );
  }

  /**
   * Makes the number link where possible.
   */
  function render_link($data, $values) {
    if (!empty($this->options['link_user_number']) && user_access('access user profiles') && $values->{$this->aliases['number']} && $data !== NULL && $data !== '') {
      $uid = sms_user_get_uid($values->{$this->aliases['number']});
      if ($uid) {
        $this->options['alter']['make_link'] = TRUE;
        $this->options['alter']['path'] = 'user/' . $uid;
      }
    }
    return $values->{$this->aliases['number']};
  }

  /**
   * Render triger.
   */
  function render($values) {
    return $this->render_link(check_plain($values->{$this->field_alias}), $values);
  }
}

