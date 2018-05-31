<?php

namespace Drupal\exam_spider\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

/**
 * Form builder for the exam Spider settings form.
 *
 * @package Drupal\exam_spider\Form
 */
class ExamSpiderSettingsForm extends ConfigFormBase {

  /**
   * Get exam Spider settings.
   */
  public function getFormId() {
    return 'exam_spider_settings_form';
  }

  /**
   * Get edit exam Spider settings.
   */
  protected function getEditableConfigNames() {
    return [
      'exam_spider.settings',
    ];
  }

  /**
   * Build exam Spider settings form.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('exam_spider.settings');
    $form['exam_spider_exam_name'] = [
      '#type' => 'textfield',
      '#title' => $this->t('@examSpiderExamTitle Name', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]),
      '#default_value' => $config->get('exam_spider_exam_name'),
      '#description' => $this->t('Please enter exam name to update'),
      '#required' => TRUE,
    ];
    $form['exam_spider_exam_url'] = [
      '#type' => 'textfield',
      '#title' => $this->t('@examSpiderExamTitle URL', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]),
      '#default_value' => $config->get('exam_spider_exam_url'),
      '#description' => $this->t('Please enter exam URL to update'),
      '#required' => TRUE,
    ];
    return parent::buildForm($form, $form_state);
  }

  /**
   * Add/Update exam settings validate callbacks.
   */
  public function validateForm(array &$form, FormStateInterface $form_state) {
    $exam_spider_exam_name = $form_state->getValue('exam_spider_exam_name');
    $exam_spider_exam_url = $form_state->getValue('exam_spider_exam_url');
    if (preg_match('/[^a-z]+$/i', $exam_spider_exam_name)) {
      $form_state->setErrorByName('exam_spider_exam_name', $this->t('Please use only charcters to update name.'));
    } 
    elseif (preg_match('/[^a-z]+$/', $exam_spider_exam_url)) {
      $form_state->setErrorByName('exam_spider_exam_url', $this->t('Please use only lowercase charcters to update path.'));
    }
    $updated_path = '/admin/structure/' . $exam_spider_exam_url;
    $pathvalidator = \Drupal::service('path.validator');
    $url_object = $pathvalidator->getUrlIfValid($updated_path);
    if (!empty($url_object) && ($exam_spider_exam_url != EXAM_SPIDER_EXAM_URL)) {
      $form_state->setErrorByName('exam_spider_exam_url', $this->t('A path already exists for the source path @source.', ['@source' => $updated_path]));
    } 
    else {
      drupal_flush_all_caches();
    }
  }

  /**
   * Exam settings submit callbacks.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    // Retrieve the configuration.
    $exam_spider_exam_name = $form_state->getValue('exam_spider_exam_name');
    $exam_spider_exam_url = $form_state->getValue('exam_spider_exam_url');
    // $updated_path = '/admin/structure/' . $exam_spider_exam_url;.
    $this->configFactory->getEditable('exam_spider.settings')
      ->set('exam_spider_exam_name', $exam_spider_exam_name)
      ->set('exam_spider_exam_url', $exam_spider_exam_url)
      ->save();
    parent::submitForm($form, $form_state);
    // drupal_flush_all_caches();
    $form_state->setRedirect('exam_spider.exam_spider_dashboard');
  }

}
