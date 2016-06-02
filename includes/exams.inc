<?php

/**
 * @file
 * Administrative page callbacks for the Exam Spider module.
 */

/**
 * Exam Spider dashboard page callbacks.
 */
function exam_spider_dashboard() {
  $output = '';
  $output .= l(t('+ Create @examSpiderExamTitle', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)), 'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/add', array('html' => TRUE));
  $header = array(
    array(
      'data'  => EXAM_SPIDER_EXAM_TITLE . ' Id',
      'field' => 'id',
      'sort'  => 'desc',
    ),
    array('data' => EXAM_SPIDER_EXAM_TITLE . ' Name', 'field' => 'exam_name'),
    array(
      'data'  => EXAM_SPIDER_EXAM_TITLE . ' Description',
      'field' => 'exam_description',
    ),
    array('data' => "Created By", 'field' => 'uid'),
    array('data' => "Status", 'field' => 'status'),
    array('data' => "Operations"),

  );
  $query = db_select("exam_list", "a")
    ->extend('PagerDefault')
    ->limit(10)
    ->extend('TableSort')
    ->orderByHeader($header)
    ->fields("a", array(
      'id',
      'exam_name',
      'exam_description',
      'uid',
      'status',
    ));
  $results = $query->execute();
  $rows = array();
  $i = 0;
  foreach ($results as $row) {
    if ($row->status == 0) {
      $status = 'Closed';
    }
    else {
      $status = 'Open';
    }
    $user_data = user_load($row->uid);
    $destination = drupal_get_destination();
    $rows[] = array(
      'data' => array(
        EXAM_SPIDER_EXAM_TITLE . '-' . $row->id,
        l($row->exam_name, EXAM_SPIDER_EXAM_URL . '/' . $row->id . '/continue'),
        $row->exam_description,
        $user_data->name,
        $status,
        l(t('Questions'), 'admin/structure/question/add/' . $row->id) . ' | ' . l(t('Edit'), 'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/' . $row->id . '/edit', array('query' => array($destination))) . ' | ' . l(t('Delete'), 'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/' . $row->id . '/delete', array('query' => array($destination))),
      ),
    );
    $i++;
  }
  $output .= theme('table', array(
    'header' => $header,
    'rows'   => $rows,
    'empty'  => t('No @examSpiderExamTitle created yet.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)) . l(t('+ Create @examSpiderExamTitle', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)), 'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/add'),
  ));
  $output .= theme('pager');
  return ($output);
}

/**
 * Add exam page callbacks.
 */
function exam_spider_add_exam($form, &$form_state) {
  $form = array();
  $form['exam_name'] = array(
    '#title'     => t('@examSpiderExamTitle Name', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'      => 'textfield',
    '#maxlength' => '170',
    '#required'  => TRUE,
  );
  $form['exam_description'] = array(
    '#title'     => t('Description'),
    '#type'      => 'textarea',
    '#maxlength' => '550',
  );
  $form['examsettings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('@examSpiderExamTitle settings', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,

  );
  $form['examsettings']['exam_duration'] = array(
    '#title'         => t('@examSpiderExamTitle Duration', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#description'   => t('@examSpiderExamTitle time duration in minutes.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '10',
    '#size'          => 10,
    '#required'      => TRUE,
    '#default_value' => 0,

  );
  $form['examsettings']['total_marks'] = array(
    '#title'            => t('Total Marks'),
    '#type'             => 'textfield',
    '#maxlength'        => '10',
    '#size'             => 10,
    '#required'         => TRUE,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['examsettings']['random_quest'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Display Random Question'),
  );
  $form['examsettings']['status'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Published'),
    '#default_value' => 1,
  );
  $form['examsettings']['negative_mark'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Negative Marking'),
  );
  $form['examsettings']['negative_mark_per'] = array(
    '#title'         => t('Negative mark %'),
    '#description'   => t('@examSpiderExamTitle negative marking in %.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '10',
    '#size'          => 10,
    '#default_value' => 0,
  );
  $form['examsettings']['re_attempt'] = array(
    '#title'         => t('Re-attempt @examSpiderExamTitle time', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#description'   => t('Re-attempt @examSpiderExamTitle time in days.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '10',
    '#size'          => 10,
    '#default_value' => 0,
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Add exam page validate callbacks.
 */
function exam_spider_add_exam_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['exam_duration'])) {
    form_set_error('exam_duration', t('You must enter a positive integer value for @examSpiderExamTitle duration.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  }
  if (!is_numeric($form_state['values']['negative_mark_per'])) {
    form_set_error('negative_mark_per', t('You must enter a positive integer value for @examSpiderExamTitle negative marking.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  }
  if (!is_numeric($form_state['values']['re_attempt'])) {
    form_set_error('re_attempt', t('You must enter a positive integer value for Re-attempt @examSpiderExamTitle time.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  }
  
  

}

/**
 * Add exam page submit callbacks.
 */
function exam_spider_add_exam_submit(&$form, &$form_state) {
  global $user;
  db_insert('exam_list')
    ->fields(array(
      'exam_name',
      'exam_description',
      'uid',
      'exam_duration',
      'total_marks',
      'random_quest',
      'negative_mark',
      'negative_mark_per',
      're_attempt',
      'status',
      'created',
      'changed',
    ))
    ->values(array(
      'exam_name'         => $form_state['values']['exam_name'],
      'exam_description'  => $form_state['values']['exam_description'],
      'uid'               => $user->uid,
      'exam_duration'     => $form_state['values']['exam_duration'],
      'total_marks'       => $form_state['values']['total_marks'],
      'random_quest'      => $form_state['values']['random_quest'],
      'negative_mark'     => $form_state['values']['negative_mark'],
      'negative_mark_per' => $form_state['values']['negative_mark_per'],
      're_attempt'        => $form_state['values']['re_attempt'],
      'status'            => $form_state['values']['status'],
      'created'           => REQUEST_TIME,
      'changed'           => REQUEST_TIME,
    ))
    ->execute();

  drupal_set_message(t('You have successfully created @examSpiderExamTitle, Please add Questions for @examName @examSpiderExamTitle.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE, '@examName' => $form_state['values']['exam_name'])));
  drupal_goto('admin/structure/' . EXAM_SPIDER_EXAM_URL);
  return $form;

}

/**
 * Edit exam page callbacks.
 */
function exam_spider_edit_exam($form, &$form_state, $exam_id) {
  $values = exam_spider_get_exam($exam_id);
  $form = array();
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $form['exam_name'] = array(
    '#title'         => t('@examSpiderExamTitle Name', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '170',
    '#required'      => TRUE,
    '#default_value' => isset($values['exam_name']) ? $values['exam_name'] : NULL,
  );
  $form['exam_description'] = array(
    '#title'         => t('Description'),
    '#type'          => 'textarea',
    '#maxlength'     => '550',
    '#cols'          => 20,
    '#rows'          => 1,
    '#default_value' => isset($values['exam_description']) ? $values['exam_description'] : NULL,
  );
  $form['examsettings'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('@examSpiderExamTitle settings', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,

  );
  $form['examsettings']['exam_duration'] = array(
    '#title'         => t('@examSpiderExamTitle Duration', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '10',
    '#size'          => 10,
    '#required'      => TRUE,
    '#default_value' => isset($values['exam_duration']) ? $values['exam_duration'] : NULL,
  );
  $form['examsettings']['total_marks'] = array(
    '#title'            => t('Total Marks'),
    '#type'             => 'textfield',
    '#maxlength'        => '10',
    '#size'             => 10,
    '#required'         => TRUE,
    '#default_value'    => isset($values['total_marks']) ? $values['total_marks'] : NULL,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['examsettings']['random_quest'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Display Random Question'),
    '#default_value' => isset($values['random_quest']) ? $values['random_quest'] : NULL,
  );
  $form['examsettings']['status'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Published'),
    '#default_value' => isset($values['status']) ? $values['status'] : NULL,
  );
  $form['examsettings']['negative_mark'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Negative Marking'),
    '#default_value' => isset($values['negative_mark']) ? $values['negative_mark'] : NULL,
  );
  $form['examsettings']['negative_mark_per'] = array(
    '#title'         => t('Negative mark %'),
    '#description'   => t('@examSpiderExamTitle negative marking in %.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '10',
    '#size'          => 10,
    '#default_value' => isset($values['negative_mark_per']) ? $values['negative_mark_per'] : NULL,
  );
  $form['examsettings']['re_attempt'] = array(
    '#title'         => t('Re-attempt @examSpiderExamTitle time', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#description'   => t('Re-attempt @examSpiderExamTitle time in days.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
    '#type'          => 'textfield',
    '#maxlength'     => '10',
    '#size'          => 10,
    '#default_value' => isset($values['re_attempt']) ? $values['re_attempt'] : NULL,
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Edit exam validate page callbacks.
 */
function exam_spider_edit_exam_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['exam_duration'])) {
    form_set_error('exam_duration', t('You must enter a integer value for @examSpiderExamTitle duration.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  }
  if (!is_numeric($form_state['values']['negative_mark_per'])) {
    form_set_error('negative_mark_per', t('You must enter a positive integer value for @examSpiderExamTitle negative marking.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  }
  if (!is_numeric($form_state['values']['re_attempt'])) {
    form_set_error('re_attempt', t('You must enter a positive integer value for Re-attempt @examSpiderExamTitle time.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  }
}

/**
 * Edit exam submit page callbacks.
 */
function exam_spider_edit_exam_submit(&$form, &$form_state) {
  global $user;
  db_update('exam_list')
    ->fields(array(
      'exam_name'        => $form_state['values']['exam_name'],
      'exam_description' => $form_state['values']['exam_description'],
      'uid'              => $user->uid,
      'exam_duration'    => $form_state['values']['exam_duration'],
      'total_marks'      => $form_state['values']['total_marks'],
      'random_quest'     => $form_state['values']['random_quest'],
      'negative_mark'    => $form_state['values']['negative_mark'],
      'negative_mark_per'=> $form_state['values']['negative_mark_per'],
      're_attempt'       => $form_state['values']['re_attempt'],
      'status'           => $form_state['values']['status'],
      'changed'          => REQUEST_TIME,
    ))
    ->condition('id', $form_state['values']['exam_id'], '=')
    ->execute();
  drupal_set_message(t('You have successfully updated @examName @examSpiderExamTitle.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE, '@examName' => $form_state['values']['exam_name'])));
  return $form;

}

/**
 * Delete exam confirm page callbacks.
 */
function exam_spider_exam_delete_confirm($form, &$form_state, $exam_id) {
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $exam_data = exam_spider_get_exam($exam_id);

  $path = 'admin/structure/' . EXAM_SPIDER_EXAM_URL;
  return confirm_form($form,
    t('Are you sure you want to delete @exam_name ?', array('@exam_name' => $exam_data['exam_name'])),
    $path,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Delete exam confirm submit page callbacks.
 */
function exam_spider_exam_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $exam_id = $form_state['values']['exam_id'];
    $exam_data = exam_spider_get_exam($exam_id);
    db_delete('exam_list')
      ->condition('id', $exam_id, '=')
      ->execute();
    db_delete('exam_questions')
      ->condition('examid', $exam_id, '=')
      ->execute();

    drupal_set_message(t('@exam_name has been deleted successfully.', array('@exam_name' => $exam_data['exam_name'])));
  }
}

/**
 * All exam listed page to start exam page callbacks.
 */
function exam_spider_exam_start() {
  $output = '';
  $header = array(
    array('data' => EXAM_SPIDER_EXAM_TITLE . ' Name', 'field' => 'exam_name'),
    array(
      'data'  => EXAM_SPIDER_EXAM_TITLE . ' Description',
      'field' => 'exam_description',
    ),
    array('data' => "Operations"),

  );
  $query = db_select("exam_list", "a")
    ->extend('PagerDefault')
    ->limit(10)
    ->extend('TableSort')
    ->orderByHeader($header)
    ->fields("a", array(
      'id',
      'exam_name',
      'exam_description',
      'status',
    ));
  $results = $query->execute();
  $rows = array();
  $i = 0;
  foreach ($results as $row) {
    if ($row->status == 1) {
      $rows[] = array(
        'data' => array(
          l(t('@examName', array('@examName' => $row->exam_name)), EXAM_SPIDER_EXAM_URL . '/' . $row->id . '/continue'),
          $row->exam_description,
          l(t('Start @examSpiderExamTitle', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)), EXAM_SPIDER_EXAM_URL . '/' . $row->id . '/continue'),
        ),
      );

    }
    $i++;
  }
  $output .= theme('table', array(
    'header' => $header,
    'rows'   => $rows,
    'empty'  => t('No @examSpiderExamTitle created yet.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
  ));
  $output .= theme('pager');
  return ($output);
}

/**
 * Exam continue page callbacks.
 */
function exam_spider_exam_continue($form, &$form_state, $exam_id) {
  if (isset($form_state['storage']['exam_result'])) {
    drupal_set_title(t('@examSpiderExamTitle Score', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
    $form['exam_result_score'] = array(
      '#markup' => $form_state['storage']['exam_result'],
    );
  }
  elseif (isset($form_state['storage']['re_exam_warning'])) {
    drupal_set_title(t('Re-attempt @examSpiderExamTitle warning', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
    $form['exam_result_score'] = array(
      '#markup' => $form_state['storage']['re_exam_warning'],
    );
  }
  else {
    $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
    $output = '';
    $form['#prefix'] = '<div id="exam_timer"></div>';
    drupal_add_css(drupal_get_path('module', 'exam_spider') . '/css/exam_spider.css');
    drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/exam_spider.js');
    $exam_data = exam_spider_get_exam($exam_id);
    if ($exam_data['status'] == 0) {
      return drupal_access_denied();
    }

    if ($exam_data['random_quest'] == 1) {
      $query = db_select("exam_questions", "eq")
        ->fields("eq")
        ->condition('examid', $exam_id, '=')->orderRandom()->execute();

    }
    else {
      $query = db_select("exam_questions", "eq")
        ->fields("eq")
        ->condition('examid', $exam_id, '=')->execute();
    }
    $results = $query->fetchAll();
    if (empty($results)) {
      $output .= t('No question created yet for this @examSpiderExamTitle.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE));
    }
    else {
      if ($exam_data['exam_duration'] > 0) {
        exam_spider_clock('exam-spider-exam-continue');
      }
      $form['li_prefix'] = array(
        '#markup' => ' <ul class="exam_spider_slider_exam">',
      );
      $total_slides = count($results);
      foreach ($results as $key => $value) {
        $options[1] = $value->opt1;
        $options[2] = $value->opt2;
        $options[3] = $value->opt3;
        $options[4] = $value->opt4;
        if ($value->multiple == 1) {
          $form['question'][$value->id] = array(
            '#type'    => 'checkboxes',
            '#options' => $options,
            '#title'   => t('@question', array('@question' => $value->question)),
            '#prefix'  => '<li id="examslide_' . $key . '" class="exam_spider_slider">',
            '#suffix'  => ' <a class="exam_spider_slide_next button" href="javascript:void(0);">Next</a></li>',
          );
        }
        else {
          $form['question'][$value->id] = array(
            '#type'    => 'radios',
            '#title'   => t('@question', array('@question' => $value->question)),
            '#options' => $options,
            '#prefix'  => '<li id="examslide_' . $key . '" class="exam_spider_slider">',
            '#suffix'  => ' <a class="exam_spider_slide_next button" href="javascript:void(0);">Next</a></li>',
          );
        }
      }
      $form['next'] = array(
        '#type'   => 'submit',
        '#prefix' => '<li id="examslide_' . $total_slides . '" class="exam_spider_slider">' . t('<h2>I am done.</h2><br />'),
        '#suffix' => '</li>',
        '#value'  => t('Submit'),
      );
      $form['#tree'] = TRUE;
      $form['li_suffix'] = array(
        '#markup' => '</ul>',
      );
    }
    $form['#suffix'] = $output;
  }
  $form['#redirect'] = FALSE;
  return $form;
}

/**
 * Exam continue submit page callbacks.
 */
function exam_spider_exam_continue_submit(&$form, &$form_state) {
  global $user;
  $score_obtain = $total_marks = $wrong_quest = 0;
  $exam_data = exam_spider_get_exam($form_state['values']['exam_id']);
  $total_marks = $exam_data['total_marks'];
  $negative_mark = $exam_data['negative_mark'];
  $negative_mark_per = $exam_data['negative_mark_per'];
  $total_quest = count($form_state['values']['question']);
  $mark_per_quest = ($total_marks / $total_quest);
  $re_attempt =  $exam_data['re_attempt'];
  $user_last_result = any_exam_last_result($form_state['values']['exam_id']);
  $user_last_attempt_timestamp = $user_last_result['created'];
  $re_attempt_timestamp =  strtotime('+'.$re_attempt.' day',  $user_last_attempt_timestamp);
  if($re_attempt_timestamp > REQUEST_TIME){
    $form_state['storage']['re_exam_warning'] = t('You have already attempt this @examSpiderExamTitle, You will be eligible again after @re_attempt days from previus @examSpiderExamTitle attempt day.',  array(
    '@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE, '@re_attempt' => $re_attempt));
  }
  else{
    $negative_marking_number = (($mark_per_quest * $negative_mark_per) / 100);
    foreach ($form_state['values']['question'] as $key => $answervalues) {
      $question_data = exam_spider_get_question($key);
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
          if($negative_mark == 1) {
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
          if($negative_mark == 1) {
            $score_obtain -= $negative_marking_number;
          }
          $wrong_quest += 1;
        }
      }
    }
    $correct_answers = $total_quest - $wrong_quest;
    $form_state['storage']['exam_result'] = t('<b>You have got @score_obtain marks out of @total_marks<br/>Correct Answer(s) @correctAnswers <br/>Wrong Answer(s) @wrong_quest<b>', array(
      '@score_obtain' => $score_obtain,
      '@total_marks' => $total_marks,
      '@correctAnswers' => $correct_answers,
      '@wrong_quest' => $wrong_quest,
    ));
    $reg_id = db_insert('exam_results')
      ->fields(array('examid', 'uid', 'total', 'obtain', 'wrong', 'created'))
      ->values(array(
        'examid'  => $form_state['values']['exam_id'],
        'uid'     => $user->uid,
        'total'   => $total_marks,
        'obtain'  => $score_obtain,
        'wrong'   => $wrong_quest,
        'created' => REQUEST_TIME,
      ))
      ->execute();
    drupal_set_message(t('Your @examSpiderExamTitle has submitted successfully and your REG id is REG-@reg_id.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE, '@reg_id' => $reg_id))); 
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * Exam results page callbacks.
 */
function exam_spider_exam_results() {
  $output = '';
  $header = array(
    array('data' => 'REG Id', 'field' => 'id', 'sort' => 'desc'),
    array('data' => EXAM_SPIDER_EXAM_TITLE . ' Name', 'field' => 'examid'),
    array('data' => 'Name', 'field' => 'uid'),
    array('data' => 'Total Marks', 'field' => 'total'),
    array('data' => 'Obtain Marks', 'field' => 'obtain'),
    array('data' => 'Wrong', 'field' => 'wrong'),
    array('data' => 'Date', 'field' => 'created'),
  );
  $query = db_select("exam_results", "er")
    ->extend('PagerDefault')
    ->limit(10)
    ->extend('TableSort')
    ->orderByHeader($header)
    ->fields("er", array(
      'id',
      'examid',
      'uid',
      'total',
      'obtain',
      'wrong',
      'created',
    ));
  $results = $query->execute();
  $rows = array();
  $i = 0;
  foreach ($results as $row) {
    $user_data = user_load($row->uid);
    $exam_data = exam_spider_get_exam($row->examid);
    $rows[] = array(
      'data' => array(
        t('REG -') . $row->id,
        $exam_data['exam_name'],
        $user_data->name,
        $row->total,
        $row->obtain,
        $row->wrong,
        format_date($row->created, 'short'),
      ),
    );
    $i++;
  }
  $output .= theme('table', array(
    'header' => $header,
    'rows'   => $rows,
    'empty'  => t('No @examSpiderExamTitle attempt by any user.', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)),
  ));
  $output .= theme('pager');
  return ($output);
}

/**
 * Exam clock function.
 */
function exam_spider_clock($formid) {
  require_once drupal_get_path('module', 'exam_spider') . '/includes/ExamSpiderDigitalClock.php';
  $clock = new ExamSpiderDigitalClock();
  return $clock->showClock($formid);
}
