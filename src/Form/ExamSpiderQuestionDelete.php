<?php

namespace Drupal\exam_spider\Form;

use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Form\ConfirmFormBase;
use Drupal\Core\Url;
use Drupal\exam_spider\Controller\ExamSpider;

/**
 * Class ExamSpiderQuestionDelete.
 */
class ExamSpiderQuestionDelete extends ConfirmFormBase {

  /**
   * The question ID.
   *
   * @var integer
   */
  public $questionid;

  /**
   * Delete Question form.
   */
  public function getFormId() {
    return 'delete_question_form';
  }

  /**
   * Delete Question confirm text.
   */
  public function getQuestion() {
    $examspider_service = new ExamSpider();
    $questionid = $this->id;
    $question_data = $examspider_service->examSpiderGetQuestion($questionid);
    return t('Do you want to delete @question question?', ['@question' => $question_data['question']]);
  }

  /**
   * Delete Question cancel url.
   */
  public function getCancelUrl() {
    return new Url('exam_spider.exam_spider_dashboard');
  }

  /**
   * Delete Question Description text.
   */
  public function getDescription() {
    return $this->t('This action cannot be undone.');
  }

  /**
   * Delete button text.
   */
  public function getConfirmText() {
    return $this->t('Delete it!');
  }

  /**
   * Cancel button text.
   */
  public function getCancelText() {
    return $this->t('Cancel');
  }

  /**
   * Delete Question form.
   */
  public function buildForm(array $form, FormStateInterface $form_state, $questionid = NULL) {
    $this->id = $questionid;
    return parent::buildForm($form, $form_state);
  }

  /**
   * Delete Question form submit callbacks.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $examspider_service = new ExamSpider();
    $questionid = $this->id;
    $question_data = $examspider_service->examSpiderGetQuestion($questionid);
    $examid = $question_data['examid'];
    db_delete('exam_questions')
      ->condition('id', $questionid)
      ->execute();
    drupal_set_message(t('%question_name question has been deleted.', ['%question_name' => $question_data['question']]));
    $form_state->setRedirect('exam_spider.exam_spider_add_question', ['examid' => $examid]);
  }

}
