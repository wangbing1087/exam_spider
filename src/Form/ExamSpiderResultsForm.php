<?php

namespace Drupal\exam_spider\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\exam_spider\Controller\ExamSpider;

/**
 * Form builder for the exam results form.
 *
 * @package Drupal\exam_spider\Form
 */
class ExamSpiderResultsForm extends FormBase {
  public function getFormId() {
    return 'exam_results_form';
  }
  public function buildForm(array $form, FormStateInterface $form_state) {
  	$examspider_service = new ExamSpider();
	  $form = array();
	  $form['#attached']['library'][] = 'exam_spider/exam_spider';
	   $output = NULL;
	  $exam_names = array();
	  $exams_data = $examspider_service->exam_spider_get_exam();
	  $examresults_url = Url::fromRoute('exam_spider.exam_spider_exam_results');
	  $link_options = array(
			  'attributes' => array(
			    'class' => array(
			      'button',
			    ),
			  ),
			);
		$examresults_url->setOptions($link_options);
 		$examresults_link = Link::fromTextAndUrl($this->t('Reset'), $examresults_url)->toString();
	  $form['#method'] = 'get';
	  if ($exams_data) {
	    foreach ($exams_data as $exam_name) {
	      $exam_names[$exam_name->id] = $exam_name->exam_name;
	    }
	  
	    $form['filter'] = array(
	      '#type' => 'details',
	      '#title'       => $this->t('Filter option'),
	      '#attributes'  => array('class' => array('container-inline')),
	    );

	    $form['filter']['exam_name'] = array(
	      '#type'          => 'select',
	      '#title'         => $this->t('@examSpiderExamTitle Name', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
	      '#options'       => $exam_names,
	      '#default_value' => isset($_GET['exam_name']) ? $_GET['exam_name'] : NULL,
	    );

	    $form['filter']['submit'] = array(
	      '#type'  => 'submit',
	      '#value' => $this->t('Filter'),
	    );
	    $form['filter']['reset_button'] = array(
	     '#markup' => $examresults_link,
	    );
	  }
    $exam_spider_exam_results = $examspider_service->exam_spider_exam_results($default_sel);
	  $form['#suffix'] = drupal_render($exam_spider_exam_results);
	  return $form;
  }
	/**
	 * Add/Update exam results validate callbacks.
	 */
  public function validateForm(array &$form, FormStateInterface $form_state) {

  }

  /**
   * Exam results submit callbacks.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {

  }
}
