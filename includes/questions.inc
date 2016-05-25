<?php
/**
 * @file callbacks.inc
 * Minor menu callbacks for exam spider helpers.
 */

function add_question($form, &$form_state, $default_sel) {
    drupal_add_css(drupal_get_path('module', 'exam_spider') . '/css/exam_spider.css');
    drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/exam_spider.js');
    $form  = $exam_options = array();
    $allExam = getExam ();
    foreach ($allExam as $option) {
      $exam_options[$option->id] = $option->exam_name;
    }
    $form['#attributes'] = array('class' => array('questions-action'));
    $form['selected_exam'] = array(
      '#type' => 'select',
      '#title' => 'Select Exam',
      '#options' => $exam_options,
      '#default_value' => $default_sel,  
      '#required' => TRUE,
      //'#attributes' => array('onchange' => "form.submit('selected_exam')"),
    );
    $form['question_name']= array(
      '#title' => t('Question Name'),
      '#type' => 'textfield',
      '#maxlength' => '170',
      '#required' => TRUE,
    );
    $form['options'] = array(
      '#type' => 'fieldset', 
      '#title' => t('Option settings'), 
      '#collapsible' => TRUE, 
      '#collapsed' => TRUE,

    );
    $form['options']['multi_answer'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable Multiple Answers'),
    );
    for ($i=1; $i <=4 ; $i++) { 
      $form['options']['opt'.$i]= array(
        '#title' => t('Option ').$i,
        '#type' => 'textarea',
        '#maxlength' => '550',
        '#cols' => 20,
        '#rows' => 1,
        '#required' => TRUE,
        /*'#resizable' => FALSE,*/
        '#prefix' => '<div class="option_set">',
      );
      $form['options']['answer'.$i] = array(
        '#type' => 'checkbox',
        '#title' => t('Correct Option'),
        '#attributes' => array('class' => array('answer')),
        '#suffix' => '</div>',
      );
    }
    
    $form['submit'] = array(
       '#type' => 'submit',
       '#value' => t('Submit'),
    );
    $form['#suffix'] = getQuestions($default_sel);
    return  $form;
}
function add_question_validate(&$form, &$form_state) {
  if ($form_state['values']['answer1']==0&&$form_state['values']['answer2']==0&&$form_state['values']['answer3']==0&&$form_state['values']['answer4']==0) {
    return form_set_error('time', t('Please tic correct answer.'));
  }
}
function add_question_submit(&$form, &$form_state) {
  $answer ='';
  for ($i=1; $i <=4 ; $i++) { 
     if($form_state['values']['answer'.$i]==1){
         $answer .= 'opt'.$i.'-';
     }
  }
  $answer = rtrim($answer, "-");
  db_insert('exam_questions')
    ->fields(array('examid', 'question','opt1','opt2', 'opt3', 'opt4', 'answer', 'multiple', 'created', 'changed'))
    ->values(array(
      'examid'   => $form_state['values']['selected_exam'],
      'question' => $form_state['values']['question_name'],
      'opt1'   =>  $form_state['values']['opt1'],
      'opt2'   =>  $form_state['values']['opt2'],
      'opt3'   =>  $form_state['values']['opt3'],
      'opt4'   =>  $form_state['values']['opt4'],
      'answer' =>  $answer,
      'multiple' => $form_state['values']['multi_answer'],
      'created'  => REQUEST_TIME,
      'changed'  =>REQUEST_TIME,
    ))
    ->execute();
  drupal_set_message(t('You have successfully created question for this exam.'));
  drupal_goto('admin/structure/question/add_question/'.$form_state['values']['selected_exam']);
  return $form;
}
function edit_question($form, &$form_state, $question_id) {
    drupal_add_css(drupal_get_path('module', 'exam_spider') . '/css/exam_spider.css');
    drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/exam_spider.js');
    $form  = $exam_options = array();
    $allExam = getExam ();
    foreach ($allExam as $option) {
      $exam_options[$option->id] = $option->exam_name;
    }
    
    $values = getQuestion ($question_id);
    $answer = array_flip(explode('-', $values['answer']));

    $form['#attributes'] = array('class' => array('questions-action'));
    $form['question_id'] = array('#type' => 'value', '#value' => $question_id);
    $form['selected_exam'] = array(
      '#type' => 'select',
      '#title' => 'Select Exam',
      '#options' => $exam_options,
      '#default_value' =>isset($values['examid']) ? $values['examid'] : NULL,
      '#required' => TRUE,
      //'#attributes' => array('onchange' => "form.submit('selected_exam')"),
    );
    $form['question_name']= array(
      '#title' => t('Question Name'),
      '#type' => 'textfield',
      '#maxlength' => '170',
      '#required' => TRUE,
      '#default_value' => isset($values['question']) ? $values['question'] : NULL,
    );
    $form['options'] = array(
      '#type' => 'fieldset', 
      '#title' => t('Option settings'), 
      '#collapsible' => TRUE, 
      '#collapsed' => FALSE,

    );
    $form['options']['multi_answer'] = array(
        '#type' => 'checkbox',
        '#title' => t('Enable Multiple Answers'),
        '#default_value' => isset($values['multiple']) ? $values['multiple'] : NULL,
    );
    for ($i=1; $i <=4 ; $i++) { 
      $form['options']['opt'.$i]= array(
        '#title' => t('Option ').$i,
        '#type' => 'textarea',
        '#maxlength' => '550',
        '#cols' => 20,
        '#rows' => 1,
        '#required' => TRUE,
        '#default_value' =>isset($values['opt'.$i]) ? $values['opt'.$i] : NULL,
        /*'#resizable' => FALSE,*/
        '#prefix' => '<div class="option_set">',
      );
      $form['options']['answer'.$i] = array(
        '#type' => 'checkbox',
        '#title' => t('Correct Option'),
        '#attributes' => array('class' => array('answer')),
        '#default_value' =>isset($answer['opt'.$i]) ? 1 : NULL,
        '#suffix' => '</div>',
      );
    }
    
    $form['submit'] = array(
       '#type' => 'submit',
       '#value' => t('Submit'),
    );
    return $form;
}
function edit_question_validate(&$form, &$form_state) {
  if ($form_state['values']['answer1']==0&&$form_state['values']['answer2']==0&&$form_state['values']['answer3']==0&&$form_state['values']['answer4']==0) {
     return form_set_error('time', t('Please tic correct answer.'));
  }
}
function edit_question_submit(&$form, &$form_state) {
  $answer ='';
  for ($i=1; $i <=4 ; $i++) { 
     if($form_state['values']['answer'.$i]==1){
         $answer .= 'opt'.$i.'-';
     }
  }
  $answer = rtrim($answer, "-");
   db_update('exam_questions')
    ->fields(array('examid'=>$form_state['values']['selected_exam'],'question'=>$form_state['values']['question_name'],'opt1'=>$form_state['values']['opt1'],'opt2'=>$form_state['values']['opt2'],'opt3'=>$form_state['values']['opt3'],'opt4'=>$form_state['values']['opt4'],'answer'=>$answer,'multiple'=>$form_state['values']['multi_answer'],'changed'=>REQUEST_TIME,))
     ->condition('id', $form_state['values']['question_id'],'=')
    ->execute();
  drupal_set_message(t('You have successfully updated question for this exam.'));
  //drupal_goto('admin/structure/exam');
  return $form;
}
function question_delete_confirm($form, &$form_state, $question_id) {  
  // Always provide entity id in the same form key as in the entity edit form.
  $form['question_id'] = array('#type' => 'value', '#value' => $question_id);
  $path = 'admin/structure/exam'; 
  return confirm_form($form,
    t('Are you sure you want to delete %question_name ?', array('%question_name' => $question_id)),
    $path,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function question_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $question_id = $form_state['values']['question_id'];
    db_delete('exam_questions')
      ->condition('id', $question_id,'=')
      ->execute();

    drupal_set_message(t('%question_name question has been deleted.', array('%question_name' => $question_id)));
  }
  $form_state['redirect'] = 'admin/structure/question/add_question/'.arg(4);
}

function getQuestions($exam_id){
  $output = '';
  if (is_numeric($exam_id)) {
   // $output .= l('Add Question', 'admin/structure/question/add_question/'.$exam_id);
    $header = array(
            array('data' =>'Question', 'field' => 'question'),
            array('data' =>"Status",'field' =>'status'),
            array('data' =>"Operations"),

      );
    $query = db_select("exam_questions","eq")
        ->extend('PagerDefault')
        ->limit(10) 
        ->extend('TableSort') 
        ->orderByHeader($header)
        ->fields("eq", array('id','question','status'))
        ->condition('examid', $exam_id,'=');
    $results = $query->execute();
    $rows = array();
        $i = 0;
        foreach ($results as $row) {
          if ($row->status==0) 
                $status ='Closed';
          else
            $status ='Open';
          
        $rows[] =array('data' =>array(
            $row->question,
            $status,
            l('Edit', 'admin/structure/question/'.$row->id.'/edit').' | '.l('Delete', 'admin/structure/question/'.$row->id.'/delete'),
            )
           );
        $i++;
        }
    //$rows[] = array("<input type = 'submit' name  = 'delete' value = 'Delete'>");
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'empty'=> t('No question created yet for this exam. ').l(t('Add Question'), 'admin/structure/question/add_question/'.$exam_id)));
    $output .= theme('pager');
  }
  return $output;
}