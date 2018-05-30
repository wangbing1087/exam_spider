<?php

namespace Drupal\exam_spider\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Url;
use Drupal\Core\Render\Element;
use Drupal\exam_spider\Controller\ExamSpider;

/**
 * Class ExamSpiderResultsDelete.
 *
 * @package Drupal\exam_spider\Form
 */
class ExamSpiderResultsDelete extends ConfirmFormBase {
  /**
   * Delete Result form.
   */
  public function getFormId() {
    return 'delete_result_form';
  }

  public $resultid;
  /**
   * Delete Result confirm text.
   */
  public function getQuestion() { 
    $examspider_service = new ExamSpider();
    $resultid = $this->id;
    return t('Do you want to delete REG - @resultid result?', array('@resultid' => $resultid));
  }
  /**
   * Delete Result cancel url.
   */
  public function getCancelUrl() {
    return new Url('exam_spider.exam_spider_exam_results');
  }
  /**
   * Delete Result Description text.
   */
  public function getDescription() {
    return t('This action cannot be undone.');
  }
  /**
   * Delete button text.
   */
  public function getConfirmText() {
    return t('Delete it!');
  }
  /**
   * Cancel button text.
   */
  public function getCancelText() {
    return t('Cancel');
  }
  /**
   * Delete Result form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $resultid = NULL) {
    $this->id = $resultid;
    return parent::buildForm($form, $form_state);
  }
  /**
   * Delete Result form validate callback.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);
  }
  /**
   * Delete Result form submit callbacks.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $resultid = $this->id;
    $query = \Drupal::database();
    db_delete('exam_results')
      ->condition('id', $resultid)
      ->execute();
    drupal_set_message(t('REG - @resultid result has been deleted successfully.', array('@resultid' => $resultid)));
    $form_state->setRedirect('exam_spider.exam_spider_exam_results');
  }
}
