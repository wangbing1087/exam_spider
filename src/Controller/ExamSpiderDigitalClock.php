<?php

/**
 * @file
 * Generate time clock.
 */

/**
 * ExamSpiderDigitalClock Class.
 */
class ExamSpiderDigitalClock {
  private $noLimit;

  /**
   * Constructor for ExamSpiderDigitalClock.
   */
  public function __construct() {
    $this->noLimit = FALSE;
  }

  /**
   * Implementation of ExamSpiderDigitalClock::showClock().
   */
  public function showClock($for) {
    drupal_add_js(array('getTimeLimit' => $this->getTimeLimit(), 'forForm' => $for), 'setting');
    drupal_add_js(drupal_get_path('module', EXAM_SPIDER_EXAM_SPIDER) . '/js/exam_spider.js');
    drupal_add_js('jQuery(document).ready(function () { jQuery.countdownto(); });', array('type' => 'inline'));
  }

  /**
   * Get time limit function.
   */
  private function getTimeLimit() {
    $values = examSpiderGetExam(arg(1));
    $timer = time() + intval($values['exam_duration'] * 60);
    return date('r', $timer);
  }

}
