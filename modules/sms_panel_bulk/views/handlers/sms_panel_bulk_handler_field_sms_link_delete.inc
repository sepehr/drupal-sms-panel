<?php
// $Id$

require_once 'sms_panel_bulk_handler_field_sms_link.inc';

/**
 * Field handler to provide a link to delete a SMS message.
 */
class sms_panel_bulk_handler_field_sms_link_delete extends sms_panel_bulk_handler_field_sms_link {
  /**
   * Class constructor.
   */
  function construct() {
    parent::construct();
  }

  /**
   * Field renderer.
   */
  function render($values) {
    if (!user_access('send bulk sms messages')) {
      return;
    }

    $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');
    return l($text, 'admin/smsframework/panel/sms/bulk' . $values->{$this->aliases['bid']} . '/delete', array(
      'query' => drupal_get_destination(),
    ));
  }
}

