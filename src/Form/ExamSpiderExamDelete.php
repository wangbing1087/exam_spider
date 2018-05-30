<?php

namespace Drupal\exam_spider\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Url;
use Drupal\Core\Render\Element;
use Drupal\exam_spider\Controller\ExamSpider;

/**
 * Class ExamSpiderExamDelete.
 *
 * @package Drupal\exam_spider\Form.
 */
class ExamSpiderExamDelete extends ConfirmFormBase {
  /**
   * Delete Exam form.
   */
  public function getFormId() {
    return 'delete_exam_form';
  }

  public $examid;

  /**
   * Delete Exam confirm text.
   */
  public function getQuestion() {
    $examspider_service = new ExamSpider();
    $exam_id = $this->id;
    $exam_data = $examspider_service->exam_spider_get_exam($exam_id);
    return t('Do you want to delete @exam_name @examSpiderExamTitle ?', array('@exam_name' => $exam_data['exam_name'], '@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE));
  }

  /**
   * Delete Exam cancel url.
   */
  public function getCancelUrl() {
    return new Url('exam_spider.exam_spider_dashboard');
  }

  /**
   * Delete Exam Description text.
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
   * Delete Exam form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $examid = NULL) {
    $this->id = $examid;
    return parent::buildForm($form, $form_state);
  }

  /**
   * Delete Exam form validate callback.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    parent::validateForm($form, $form_state);
  }

  /**
   * Delete Exam form submit callbacks.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $examspider_service = new ExamSpider();
    $exam_id = $this->id;
    $exam_data = $examspider_service->exam_spider_get_exam($exam_id);
    db_delete('exam_list')
      ->condition('id', $exam_id)
      ->execute();
    db_delete('exam_questions')
      ->condition('examid', $exam_id)
      ->execute();
    db_delete('exam_results')
      ->condition('examid', $exam_id)
      ->execute();
    drupal_set_message(t('@exam_name has been deleted successfully.', ['@exam_name' => $exam_data['exam_name']]));
    $form_state->setRedirect('exam_spider.exam_spider_dashboard');
  }

}
