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
	
  /**
   * Get exam results form.
   */
  public function getFormId() {
    return 'exam_results_form';
  }

  /**
   * Build exam results form.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
  	$examspider_service = new ExamSpider();
	  $form = [];
	  $form['#attached']['library'][] = 'exam_spider/exam_spider';
	  $exam_names = [];
	  $exams_data = $examspider_service->exam_spider_get_exam();
	  $examresults_url = Url::fromRoute('exam_spider.exam_spider_exam_results');
	  $link_options = [
		  'attributes' => [
		    'class' => [
		      'button',
		    ],
		  ],
		];
		$examresults_url->setOptions($link_options);
 		$examresults_link = Link::fromTextAndUrl($this->t('Reset'), $examresults_url)->toString();
	  $form['#method'] = 'get';
	  if ($exams_data) {
	    foreach ($exams_data as $exam_name) {
	      $exam_names[$exam_name->id] = $exam_name->exam_name;
	    }
	    $form['filter'] = [
	      '#type' => 'details',
	      '#title'       => $this->t('Filter option'),
	      '#attributes'  => ['class' => ['container-inline']],
	    ];
	    $form['filter']['exam_name'] = [
	      '#type'          => 'select',
	      '#title'         => $this->t('@examSpiderExamTitle Name', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]),
	      '#options'       => $exam_names,
	      '#default_value' => isset($_GET['exam_name']) ? $_GET['exam_name'] : NULL,
	    ];
	    $form['filter']['submit'] = [
	      '#type'  => 'submit',
	      '#value' => $this->t('Filter'),
	    ];
	    $form['filter']['reset_button'] = [
	     '#markup' => $examresults_link,
	    ];
	  }
    $exam_spider_exam_results = $examspider_service->exam_spider_exam_results();
	  $form['#suffix'] = drupal_render($exam_spider_exam_results);
	  return $form;
  }

}
