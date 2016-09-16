<?php

/**
 * @file
 * Administrative page callbacks for the questions of Exam Spider module.
 */

/**
 * Add question page callbacks.
 */
function exam_spider_add_question($form, &$form_state, $default_sel) {
  drupal_add_css(drupal_get_path('module', EXAM_SPIDER_EXAM_SPIDER) . '/css/exam_spider.css');
  drupal_add_js(drupal_get_path('module', EXAM_SPIDER_EXAM_SPIDER) . '/js/exam_spider.js');
  $form = $exam_options = array();
  $form['#prefix'] = l(t('Back To @examSpiderExamTitle Dashboard', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)), 'admin/structure/' . EXAM_SPIDER_EXAM_URL);

  $all_exam = exam_spider_get_exam();
  foreach ($all_exam as $option) {
    $exam_options[$option->id] = $option->exam_name;
  }
  $form['#attributes'] = array('class' => array('questions-action'));
  $form['selected_exam'] = array(
    '#type'          => 'select',
    '#title'         => 'Select Exam',
    '#options'       => $exam_options,
    '#default_value' => $default_sel,
    '#required'      => TRUE,
  );
  $form['question_name'] = array(
    '#title'     => t('Question Name'),
    '#type'      => 'textfield',
    '#maxlength' => '170',
    '#required'  => TRUE,
  );
  $form['options'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Option settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => TRUE,

  );
  $form['options']['multi_answer'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Enable Multiple Answers'),
  );
  for ($i = 1; $i <= 4; $i++) {
    $form['options']['opt' . $i] = array(
      '#title'     => t('Option @i', array('@i' => $i)),
      '#type'      => 'textarea',
      '#maxlength' => '550',
      '#cols'      => 20,
      '#rows'      => 1,
      '#required'  => TRUE,
      /*'#resizable' => FALSE,*/
      '#prefix'    => '<div class="option_set">',
    );
    $form['options']['answer' . $i] = array(
      '#type'       => 'checkbox',
      '#title'      => t('Correct Option'),
      '#attributes' => array('class' => array('answer')),
      '#suffix'     => '</div>',
    );
  }

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit'),
  );
  $form['#suffix'] = exam_spider_get_questions($default_sel);
  return $form;
}

/**
 * Add question validate page callbacks.
 */
function exam_spider_add_question_validate(&$form, &$form_state) {
  if ($form_state['values']['answer1'] == 0 && $form_state['values']['answer2'] == 0 && $form_state['values']['answer3'] == 0 && $form_state['values']['answer4'] == 0) {
    return form_set_error('time', t('Please choose at least one answer.'));
  }
}

/**
 * Add question submit page callbacks.
 */
function exam_spider_add_question_submit(&$form, &$form_state) {
  $answer = NULL;
  for ($i = 1; $i <= 4; $i++) {
    if ($form_state['values']['answer' . $i] == 1) {
      $answer .= 'opt' . $i . '-';
    }
  }
  $answer = rtrim($answer, "-");
  db_insert('exam_questions')
    ->fields(array(
      'examid',
      'question',
      'opt1',
      'opt2',
      'opt3',
      'opt4',
      'answer',
      'multiple',
      'created',
      'changed',
    ))
    ->values(array(
      'examid'   => $form_state['values']['selected_exam'],
      'question' => $form_state['values']['question_name'],
      'opt1'     => $form_state['values']['opt1'],
      'opt2'     => $form_state['values']['opt2'],
      'opt3'     => $form_state['values']['opt3'],
      'opt4'     => $form_state['values']['opt4'],
      'answer'   => $answer,
      'multiple' => $form_state['values']['multi_answer'],
      'created'  => REQUEST_TIME,
      'changed'  => REQUEST_TIME,
    ))
    ->execute();
  drupal_set_message(t('You have successfully created question for this @examSpiderExamTitle', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)));
  drupal_goto('admin/structure/question/add/' . $form_state['values']['selected_exam']);
  return $form;
}

/**
 * Edit question page callbacks.
 */
function exam_spider_edit_question($form, &$form_state, $question_id) {
  drupal_add_css(drupal_get_path('module', EXAM_SPIDER_EXAM_SPIDER) . '/css/exam_spider.css');
  drupal_add_js(drupal_get_path('module', EXAM_SPIDER_EXAM_SPIDER) . '/js/exam_spider.js');
  $form = $exam_options = array();
  $all_exam = exam_spider_get_exam();
  foreach ($all_exam as $option) {
    $exam_options[$option->id] = $option->exam_name;
  }

  $values = exam_spider_get_question($question_id);
  $answer = array_flip(explode('-', $values['answer']));

  $form['#attributes'] = array('class' => array('questions-action'));
  $form['question_id'] = array('#type' => 'value', '#value' => $question_id);
  $form['selected_exam'] = array(
    '#type'          => 'select',
    '#title'         => 'Select ' . EXAM_SPIDER_EXAM_TITLE,
    '#options'       => $exam_options,
    '#default_value' => isset($values['examid']) ? $values['examid'] : NULL,
    '#required'      => TRUE,
  );
  $form['question_name'] = array(
    '#title'         => t('Question Name'),
    '#type'          => 'textfield',
    '#maxlength'     => '170',
    '#required'      => TRUE,
    '#default_value' => isset($values['question']) ? $values['question'] : NULL,
  );
  $form['options'] = array(
    '#type'        => 'fieldset',
    '#title'       => t('Option settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,

  );
  $form['options']['multi_answer'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Enable Multiple Answers'),
    '#default_value' => isset($values['multiple']) ? $values['multiple'] : NULL,
  );
  for ($i = 1; $i <= 4; $i++) {

    $form['options']['opt' . $i] = array(
      '#title'         => t('Option @i', array('@i' => $i)),
      '#type'          => 'textarea',
      '#maxlength'     => '550',
      '#cols'          => 20,
      '#rows'          => 1,
      '#required'      => TRUE,
      '#default_value' => isset($values['opt' . $i]) ? $values['opt' . $i] : NULL,
      '#prefix'        => '<div class="option_set">',
    );
    $form['options']['answer' . $i] = array(
      '#type'          => 'checkbox',
      '#title'         => t('Correct Option'),
      '#attributes'    => array('class' => array('answer')),
      '#default_value' => isset($answer['opt' . $i]) ? 1 : NULL,
      '#suffix'        => '</div>',
    );
  }

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Edit question validate page callbacks.
 */
function exam_spider_edit_question_validate(&$form, &$form_state) {
  if ($form_state['values']['answer1'] == 0 && $form_state['values']['answer2'] == 0 && $form_state['values']['answer3'] == 0 && $form_state['values']['answer4'] == 0) {
    return form_set_error('time', t('Please choose at least one answer.'));
  }
}

/**
 * Edit question submit page callbacks.
 */
function exam_spider_edit_question_submit(&$form, &$form_state) {
  $answer = NULL;
  for ($i = 1; $i <= 4; $i++) {
    if ($form_state['values']['answer' . $i] == 1) {
      $answer .= 'opt' . $i . '-';
    }
  }
  $answer = rtrim($answer, "-");
  db_update('exam_questions')
    ->fields(array(
      'examid'   => $form_state['values']['selected_exam'],
      'question' => $form_state['values']['question_name'],
      'opt1'     => $form_state['values']['opt1'],
      'opt2'     => $form_state['values']['opt2'],
      'opt3'     => $form_state['values']['opt3'],
      'opt4'     => $form_state['values']['opt4'],
      'answer'   => $answer,
      'multiple' => $form_state['values']['multi_answer'],
      'changed'  => REQUEST_TIME,
    ))
    ->condition('id', $form_state['values']['question_id'], '=')
    ->execute();
  drupal_set_message(t('You have successfully updated question.'));
  return $form;
}

/**
 * Question delete confirm page callbacks.
 */
function exam_spider_question_delete_confirm($form, &$form_state, $question_id) {
  $form['question_id'] = array('#type' => 'value', '#value' => $question_id);
  $question_data = exam_spider_get_question($question_id);

  $path = 'admin/structure/' . EXAM_SPIDER_EXAM_URL;
  return confirm_form($form,
    t('Are you sure you want to delete %question_name ?', array('%question_name' => $question_data['question'])),
    $path,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Question delete confirm submit page callbacks.
 */
function exam_spider_question_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $question_id = $form_state['values']['question_id'];
    $form['question_id'] = array('#type' => 'value', '#value' => $question_id);
    $question_data = exam_spider_get_question($question_id);
    db_delete('exam_questions')
      ->condition('id', $question_id, '=')
      ->execute();

    drupal_set_message(t('%question_name question has been deleted.', array('%question_name' => $question_data['question'])));
  }
}

/**
 * Get Question list using exam id function.
 */
function exam_spider_get_questions($exam_id) {
  $output = NULL;
  if (is_numeric($exam_id)) {
    $header = array(
      array('data' => 'Question', 'field' => 'question'),
      array('data' => "Status", 'field' => 'status'),
      array('data' => "Operations"),

    );
    $query = db_select("exam_questions", "eq")
      ->extend('PagerDefault')
      ->limit(10)
      ->extend('TableSort')
      ->orderByHeader($header)
      ->fields("eq", array('id', 'question', 'status'))
      ->orderBy('id', 'DESC')
      ->condition('examid', $exam_id, '=');
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
      $destination = drupal_get_destination();
      $rows[] = array(
        'data' => array(
          $row->question,
          $status,
          l(t('Edit'), 'admin/structure/question/' . $row->id . '/edit', array('query' => array($destination))) . ' | ' . l(t('Delete'), 'admin/structure/question/' . $row->id . '/delete', array('query' => array($destination))),
        ),
      );
      $i++;
    }
    $output .= theme('table', array(
      'header' => $header,
      'rows'   => $rows,
      'empty'  => t('No question created yet for this @examSpiderExamTitle', array('@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE)) . l(t('+Add Question'), 'admin/structure/question/add/' . $exam_id),
    ));
    $output .= theme('pager');
  }
  return $output;
}
