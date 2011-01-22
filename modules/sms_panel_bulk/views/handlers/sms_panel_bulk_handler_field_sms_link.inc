<?php
// $Id$

/**
 * Field handler to provide a link to the SMS message.
 */
class sms_panel_bulk_handler_field_sms_link extends views_handler_field {
  /**
   * Class constructor.
   */
  function construct() {
    parent::construct();
    $this->additional_fields['bid'] = 'bid';
  }

  function option_definition() {
    $options = parent::option_definition();
    $options['text'] = array('default' => '', 'translatable' => TRUE);
    return $options;
  }

  function options_form(&$form, &$form_state) {
    parent::options_form($form, $form_state);
    $form['text'] = array(
      '#type' => 'textfield',
      '#title' => t('Text to display'),
      '#default_value' => $this->options['text'],
    );
  }

  function query() {
    $this->ensure_my_table();
    $this->add_additional_fields();
  }

  function render($values) {
    $text = !empty($this->options['text']) ? $this->options['text'] : t('view');
    $bid = $values->{$this->aliases['bid']};
    return l($text, 'admin/smsframework/panel/sms/bulk/' . $bid);
  }
}

