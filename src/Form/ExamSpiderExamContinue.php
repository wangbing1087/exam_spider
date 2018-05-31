<?php

namespace Drupal\exam_spider\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\exam_spider\Controller\ExamSpider;
use Drupal\Core\Link;
use Drupal\Core\Url;
use Drupal\Component\Utility\Xss;
//use Drupal\Component\HttpKernel\Exception;

/**
 * Form builder for the exam continue form.
 *
 * @package Drupal\exam_spider\Form
 */
class ExamSpiderExamContinue extends FormBase {

  public function getFormId() {
    return 'exam_continue_form';
  }
  /**
   * Exam continue form.
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
  	if (!empty($_SESSION['exam_result_data'])) {
  		$form['exam_result_data'] = [
	      '#markup' => $_SESSION['exam_result_data'],
	    ];
	    $_SESSION['exam_result_data'] = '';
  	}
  	else{
	  	$current_path = \Drupal::service('path.current')->getPath();
	    $path_args = explode('/', $current_path);
	    $exam_id = $path_args[2];
	  	$examspider_service = new ExamSpider();
		  $form['exam_id'] = ['#type' => 'value', '#value' => $exam_id];
		  $exam_data = $examspider_service->exam_spider_get_exam($exam_id);
		  $re_attempt = $exam_data['re_attempt'];
		  $user_last_result = $examspider_service->exam_spider_any_exam_last_result($exam_id);
		  $user_last_attempt_timestamp = $user_last_result['created'];
		  $re_attempt_timestamp = strtotime('+' . $re_attempt . ' day', $user_last_attempt_timestamp);
		  if ($re_attempt_timestamp > REQUEST_TIME) {
			  $re_exam_warning = $this->t('You have already attempt this @examSpiderExamTitle, You will be eligible again after @re_attempt days from previus @examSpiderExamTitle attempt day.', [
			      '@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE,
			      '@re_attempt' => $re_attempt,
			    ]);
		    $form['re_exam_warning'] = [
		      '#markup' => $re_exam_warning,
		    ];
		  }
			else {
		    $output = NULL;
		    $form['#prefix'] = '<div id="exam_timer"></div>';
		    $form['#attached']['library'][] = 'exam_spider/exam_spider';
		    if ($exam_data['status'] == 0) {
		      // throw new AccessDeniedHttpException();
		    }
		    if ($exam_data['random_quest'] == 1) {
		      $query = db_select("exam_questions", "eq")
		        ->fields("eq")
		        ->condition('examid', $exam_id)->orderRandom()->execute();

		    }
		    else {
		      $query = db_select("exam_questions", "eq")
		        ->fields("eq")
		        ->condition('examid', $exam_id)->execute();
		    }
		    $results = $query->fetchAll();
		    $form['#title'] = $this->t($exam_data['exam_name']);
		    if (empty($results)) {
		      $output .= $this->t('No question created yet for this @examSpiderExamTitle.', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]);
		    }
		    else {
		      if ($exam_data['exam_duration'] > 0) {
		        // exam_spider_clock('exam-spider-exam-continue');
		      }
		      $form['li_prefix'] = [
		        '#markup' => ' <ul class="exam_spider_slider_exam">',
		      ];
		      $total_slides = count($results);
		      foreach ($results as $key => $value) {
		        $options[1] = Xss::filter($value->opt1);
		        $options[2] = Xss::filter($value->opt2);
		        $options[3] = Xss::filter($value->opt3);
		        $options[4] = Xss::filter($value->opt4);

		        if ($value->multiple == 1) {
		          $form['question'][$value->id] = [
		            '#type'    => 'checkboxes',
		            '#options' => $options,
		            '#title'   => $this->t('@question', ['@question' => Xss::filter($value->question)]),
		            '#prefix'  => '<li id="examslide_' . $key . '" class="exam_spider_slider">',
		            '#suffix'  => ' <a class="exam_spider_slide_next button" href="#next">' . $this->t('Next') . '</a></li>',
		          ];
		        }
		        else {
		          $form['question'][$value->id] = [
		            '#type'    => 'radios',
		            '#title'   => $this->t('@question', ['@question' => Xss::filter($value->question)]),
		            '#options' => $options,
		            '#prefix'  => '<li id="examslide_' . $key . '" class="exam_spider_slider">',
		            '#suffix'  => ' <a class="exam_spider_slide_next button" href="#next">' . $this->t('Next') . '</a></li>',
		          ];
		        }
		      }
		      $form['next'] = [
		        '#type'   => 'submit',
		        '#prefix' => '<li id="examslide_' . $total_slides . '" class="exam_spider_slider">' . $this->t('<h2>I am done.</h2><br />'),
		        '#suffix' => '</li>',
		        '#value'  => $this->t('Submit'),
		      ];
		      $form['#tree'] = TRUE;
		      $form['li_suffix'] = [
		        '#markup' => '</ul>',
		      ];
		    }
		    $form['#suffix'] = $output;
		  }
  	}
	  return $form;
  }

  /**
   * Exam continue page form submit callbacks.
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
  	$examspider_service = new ExamSpider();
	  $score_obtain = $total_marks = $wrong_quest = 0;
	  $exam_data = $examspider_service->exam_spider_get_exam($form_state->getValue('exam_id'));
	  $total_marks = $exam_data['total_marks'];
	  $negative_mark = $exam_data['negative_mark'];
	  $negative_mark_per = $exam_data['negative_mark_per'];
	  $total_quest = count($form_state->getValue('question'));
	  $mark_per_quest = ($total_marks / $total_quest);
    $negative_marking_number = (($mark_per_quest * $negative_mark_per) / 100);
    foreach ($form_state->getValue('question') as $key => $answervalues) {
      $question_data = $examspider_service->exam_spider_get_question($key);
      if (is_array($answervalues)) {
        $answer_combine = '';
        foreach ($answervalues as $key => $answervalue) {
          if ($answervalue != 0) {
            $answer_combine .= 'opt' . $answervalue . '-';
          }
        }
        $checkanswer = rtrim($answer_combine, "-");
        if ($checkanswer == $question_data['answer']) {
          $score_obtain += $mark_per_quest;
        }
        else {
          if ($negative_mark == 1) {
            $score_obtain -= $negative_marking_number;
          }
          $wrong_quest += 1;
        }
      }
      else {
        $checkanswer = 'opt' . $answervalues;
        if ($checkanswer == $question_data['answer']) {
          $score_obtain += $mark_per_quest;
        }
        else {
          if ($negative_mark == 1) {
            $score_obtain -= $negative_marking_number;
          }
          $wrong_quest += 1;
        }
      }
    }
    $correct_answers = $total_quest - $wrong_quest;
    $reg_id = db_insert('exam_results')
      ->fields(['examid', 'uid', 'total', 'obtain', 'wrong', 'created'])
      ->values([
        'examid'  => $form_state->getValue('exam_id'),
        'uid'     => \Drupal::currentUser()->id(),
        'total'   => $total_marks,
        'obtain'  => $score_obtain,
        'wrong'   => $wrong_quest,
        'created' => REQUEST_TIME,
      ])
      ->execute();
    drupal_set_message($this->t('Your @examSpiderExamTitle has submitted successfully and your REG id is REG-@reg_id.', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE, '@reg_id' => $reg_id]));
    $exam_result_data = $this->t('<b>You have got @score_obtain marks out of @total_marks<br/>Correct Answer(s) @correctAnswers <br/>Wrong Answer(s) @wrong_quest<b>', [
      '@score_obtain' => $score_obtain,
      '@total_marks' => $total_marks,
      '@correctAnswers' => $correct_answers,
      '@wrong_quest' => $wrong_quest,
    ]);
    $_SESSION['exam_result_data'] = $exam_result_data;
  }

}
