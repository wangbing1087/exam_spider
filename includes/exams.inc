<?php
/**
 * @file callbacks.inc
 * Minor menu callbacks for quiz matrix helpers.
 */

/**
 * A central administrative page for quiz matrix.
 */
function exam_dashboard() {
    $output = '';
    $output .= l(t('+ Create Exam'), "admin/structure/exam/add", array('html' => TRUE));
    $header = array(
            array('data' => 'Exam Id', 'field' => 'id', 'sort' => 'desc'),
            array('data' => 'Exam Name', 'field' => 'exam_name'),
            array('data' =>"Exam Description",'field' =>'exam_description' ),
            array('data' =>"Created By",'field' =>'uid'),
            array('data' =>"Status",'field' =>'status'),
            array('data' =>"Operations"),

      );
    $query = db_select("exam_list","a")
        ->extend('PagerDefault')
        ->limit(10) 
        ->extend('TableSort') 
        ->orderByHeader($header)
        ->fields("a", array('id','exam_name','exam_description','uid','status'));
    $results = $query->execute();
    $rows = array();
        $i = 0;
        foreach ($results as $row) {
          if ($row->status==0) 
                $status ='Closed';
          else
            $status ='Open';
          
        $user_data = user_load($row->uid);
        $rows[] =array('data' =>array(
              //'<input type = "checkbox" id = "main_check'.$i.'" value = "'.$row->id.'" name = "id[]">',
              'EXAM-'.$row->id,
              l($row->exam_name, 'exam/'.$row->id.'/start'),
              $row->exam_description,
              $user_data->name,
              $status,
              l('Questions', 'admin/structure/question/add_question/'.$row->id).' | '.l('Edit', 'admin/structure/exam/'.$row->id.'/edit').' | '.l('Delete', 'admin/structure/exam/'.$row->id.'/delete'),
            )
           );
        $i++;
        }
    //$rows[] = array("<input type = 'submit' name  = 'delete' value = 'Delete'>");
    $output .= theme('table', array('header' => $header, 'rows' => $rows, 'empty'=> t('No Exam created yet. ').l(t('+ Create Exam'), "admin/structure/exam/add")));
    $output .= theme('pager');
   return ($output);
}
function add_exam($form, &$form_state) {
    $form  = array();
    $form['exam_name']= array(
      '#title' => t('Exam Name'),
      '#type' => 'textfield',
      '#maxlength' => '170',
      '#required' => TRUE,
    );
    $form['exam_description']= array(
      '#title' => t('Description'),
      '#type' => 'textarea',
      '#maxlength' => '550',
    );
    $form['examsettings'] = array(
          '#type' => 'fieldset', 
          '#title' => t('Exam settings'), 
          '#collapsible' => TRUE, 
          '#collapsed' => FALSE,

    );
    $form['examsettings']['exam_duration'] = array(
          '#title' => t('Exam Duration'),
          '#type' => 'textfield',
          '#maxlength' => '10',
          '#size' => 10,
          '#required' => TRUE,
          '#default_value' =>0,
          
    );
    $form['examsettings']['total_marks'] = array(
            '#title' => t('Total Marks'),
          '#type' => 'textfield',
          '#maxlength' => '10',
          '#size' => 10,
          '#required' => TRUE,
          '#element_validate' => array('element_validate_integer_positive'), 
    );
    $form['examsettings']['mark_per_quest'] = array(
            '#title' => t('Marks Per Question'),
          '#type' => 'textfield',
          '#maxlength' => '10',
          '#size' => 10,
          '#required' => TRUE,
          '#element_validate' => array('element_validate_integer_positive'), 
    );
    $form['examsettings']['negative_mark'] = array(
            '#type' => 'checkbox',
            '#title' => t('Negative Marking'),
    );
    $form['examsettings']['random_quest'] = array(
            '#type' => 'checkbox',
            '#title' => t('Display Random Question'),
    );

    $form['submit'] = array(
       '#type' => 'submit',
       '#value' => t('Submit'),
    );
    return  $form;
}
function add_exam_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['total_marks']) &&  intval($form_state['values']['total_marks'])>0) {
    form_set_error('total_marks', t('You must enter a positive integer value for total marks.'));  
  }
  if (!is_numeric($form_state['values']['mark_per_quest'])) {
   form_set_error('mark_per_quest', t('You must enter a positive integer value for mark per question.'));    
  }
  if (!is_numeric($form_state['values']['exam_duration'])) {
    form_set_error('exam_duration', t('You must enter a positive integer value for Exam duration.'));    
  }
  
}



function add_exam_submit(&$form, &$form_state) {
  global $user;
  db_insert('exam_list')
    ->fields(array('exam_name','exam_description','uid', 'total_marks', 'mark_per_quest', 'negative_mark', 'random_quest', 'created', 'changed'))
    ->values(array(
      'exam_name'       => $form_state['values']['exam_name'],
      'exam_description'=> $form_state['values']['exam_description'],
      'uid'             => $user->uid,
      'total_marks'     => $form_state['values']['total_marks'],
      'mark_per_quest'  => $form_state['values']['mark_per_quest'],
      'negative_mark'   => $form_state['values']['negative_mark'],
      'random_quest'    => $form_state['values']['random_quest'],
      'exam_duration'   => $form_state['values']['exam_duration'],
      'created'         => REQUEST_TIME,
      'changed'         => REQUEST_TIME,
    ))
    ->execute();
     drupal_set_message(t('You have successfully created Exam, Please add Questions for this exam.'));

//$schoolid = $form_state['values']['exam_name']
$node = new stdClass();
$node->type = 'page';
$node->status = 1;
$node->language = LANGUAGE_NONE;
$node->title = 'Neeraj';
$node->body = 
node_save($node);


     drupal_goto('admin/structure/exam');
return $form;

}

function edit_exam($form, &$form_state, $exam_id) {
    $values = getExam ($exam_id);
    $form  = array();
    $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
    $form['exam_name']= array(
      '#title' => t('Exam Name'),
      '#type' => 'textfield',
      '#maxlength' => '170',
      '#required' => TRUE,
      '#default_value' => isset($values['exam_name']) ? $values['exam_name'] : NULL,
    );
    $form['exam_description']= array(
      '#title' => t('Description'),
      '#type' => 'textarea',
      '#maxlength' => '550',
      '#cols' => 20,
      '#rows' => 1,
      '#default_value' => isset($values['exam_description']) ? $values['exam_description'] : NULL,
    );
    $form['examsettings'] = array(
          '#type' => 'fieldset', 
          '#title' => t('Exam settings'), 
          '#collapsible' => TRUE, 
          '#collapsed' => FALSE,

    );
    $form['examsettings']['exam_duration'] = array(
            '#title' => t('Exam Duration'),
          '#type' => 'textfield',
          '#maxlength' => '10',
          '#size' => 10,
          '#required' => TRUE,
          '#default_value' => isset($values['exam_duration']) ? $values['exam_duration'] : NULL,
          
    );
    $form['examsettings']['total_marks'] = array(
            '#title' => t('Total Marks'),
          '#type' => 'textfield',
          '#maxlength' => '10',
          '#size' => 10,
          '#required' => TRUE,
          '#default_value' => isset($values['total_marks']) ? $values['total_marks'] : NULL,
          '#element_validate' => array('element_validate_integer_positive'), 
    );
    $form['examsettings']['mark_per_quest'] = array(
          '#title' => t('Marks Per Question'),
          '#type' => 'textfield',
          '#maxlength' => '10',
          '#size' => 10,
          '#required' => TRUE,
          '#default_value' => isset($values['mark_per_quest']) ? $values['mark_per_quest'] : NULL,
          '#element_validate' => array('element_validate_integer_positive'), 
    );
    $form['examsettings']['negative_mark'] = array(
            '#type' => 'checkbox',
            '#title' => t('Negative Marking'),
            '#default_value' => isset($values['negative_mark']) ? $values['negative_mark'] : NULL,
    );
    $form['examsettings']['random_quest'] = array(
            '#type' => 'checkbox',
            '#title' => t('Display Random Question'),
            '#default_value' => isset($values['random_quest']) ? $values['random_quest'] : NULL,
    );
    $form['submit'] = array(
       '#type' => 'submit',
       '#value' => t('Submit'),
    );
    return  $form;
}
function edit_exam_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['total_marks']) ) {
    form_set_error('total_marks', t('You must enter a integer value for total marks.'));  
  }
  if (!is_numeric($form_state['values']['mark_per_quest'])) {
   form_set_error('mark_per_quest', t('You must enter a integer value for mark per question.'));    
  }
  if (!is_numeric($form_state['values']['exam_duration'])) {
    form_set_error('exam_duration', t('You must enter a integer value for Exam duration.'));    
  }
}
function edit_exam_submit(&$form, &$form_state) {
  global $user;
   db_update('exam_list')
    ->fields(array('exam_name'=>$form_state['values']['exam_name'],'exam_description'=>$form_state['values']['exam_description'],'uid'=>$user->uid,'total_marks' =>$form_state['values']['total_marks'], 'exam_duration'=>$form_state['values']['exam_duration'], 'mark_per_quest'=>$form_state['values']['mark_per_quest'],'negative_mark'=>$form_state['values']['negative_mark'],'random_quest'=>$form_state['values']['random_quest'], 'changed'=>REQUEST_TIME,))
    ->condition('id', $form_state['values']['exam_id'],'=')
    ->execute();
    drupal_set_message(t('You have successfully updated exam'));
    drupal_goto('admin/structure/exam');
return $form;

}

function exam_delete_confirm($form, &$form_state, $exam_id) {  
  // Always provide entity id in the same form key as in the entity edit form.
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $path = 'admin/structure/exam'; 
  return confirm_form($form,
    t('Are you sure you want to delete EXAM-%exam_id ?', array('%exam_id' => $exam_id)),
    $path,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function exam_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $exam_id = $form_state['values']['exam_id'];
    db_delete('exam_list')
      ->condition('id', $exam_id,'=')
      ->execute();
    db_delete('exam_questions')
      ->condition('examid', $exam_id,'=')
      ->execute();

    drupal_set_message(t('EXAM-%exam_id has been deleted.', array('%exam_id' => $exam_id)));
  }

  $form_state['redirect'] = 'admin/structure/exam';
}
function exam_start($form, &$form_state, $exam_id) {  
  global $user;
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $form['name'] = array(
    '#type' => 'textfield',
    '#title' => 'Name',
    '#default_value' => isset($user->name) ? $user->name : NULL,
    '#required' => TRUE,
  );
  
  $form['mail'] = array(
    '#type' => 'textfield',
    '#title' => 'Email',
    '#default_value' => isset($user->mail) ? $user->mail : NULL,
    '#required' => TRUE,
  );
   $form['next'] = array(
      '#type' => 'submit',
      '#value' => t('Start')
    );
  
  return  $form;
}
function exam_start_validate(&$form, &$form_state) {
  if (!valid_email_address($form_state['values']['mail'])) {
   return form_set_error('mail', t('The email address appears to be invalid.'));
  }
}
function exam_start_submit(&$form, &$form_state) {
  global $user;
  db_insert('exam_results')
    ->fields(array('examid','uid'))
    ->values(array(
      'examid'        => $form_state['values']['exam_id'],
      'uid'              => $user->uid,
    ))
    ->execute();
  if (isset($form_state['values']['exam_id'])) {
    drupal_goto('exam/'.$form_state['values']['exam_id'].'/continue');
  }
  else{
     drupal_goto('exam/'.$form_state['values']['exam_id'].'/start');
  }
  

}
function DigiClock($frmCls){
   module_load_include('inc', 'exam_spider', 'DigitalClock');
    require_once( drupal_get_path('module', 'exam_spider').'/includes/ExamSpiderDigitalClock.php');
      $clock = new ExamSpiderDigitalClock();
      return $clock->_showClock($frmCls);
  }
function exam_continue($form, &$form_state, $exam_id) {  

    global $user;
    $output ='';
    $form['#prefix'] = '<div id="exTimer"></div>';
    if(is_numeric($exam_id)){
          DigiClock('exam-continue');
    }
   // drupal_add_css(drupal_get_path('module', 'exam_spider') . '/css/jquery.bxslider.css');
    drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/jquery.bxslider.min.js');
    drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/exam_spider.js');

    $query = db_select("exam_questions","eq")
        ->fields("eq")
        ->condition('examid', $exam_id,'=')->orderRandom()->execute();
    $results = $query->fetchAll();
   // print_r($results);
    //$form['#prefix'] = '<ul class="bxslider">';
    $form['li_prefix'] = array(
      '#markup' => '<ul class="exam-slider">',
    );
    foreach ($results as $key => $value) {
      $options[1] = $value->opt1;
      $options[2] = $value->opt2;
      $options[3] = $value->opt3;
      $options[4] = $value->opt4;
      if ($value->multiple==1) {
        $form['question'][$key] = array(
          '#type' => 'checkboxes',
          '#options' => drupal_map_assoc($options),
          '#title' => t($value->question),
        );
      }
      else{
        $form['question'][$key] = array(
          '#type' => 'radios',
          '#title' => t($value->question),
          '#options' => $options,
          '#prefix' => '<li>',
          '#suffix' => '</li>',
        );
      }
    }
    $form['next'] = array(
      '#type' => 'submit',
      '#prefix' => '<li>',
      '#suffix' => '</li>',
      '#value' => t('Submit')
    );
    $form['li_suffix'] = array(
      '#markup' => '</ul>',
    );
    $output .= ' <p><span id="slider-next"></span></p>';
    $form['#suffix']  = $output;  
  return  $form;
  //DigiClock('exam-continue');
}
function exam_continue_submit(&$form, &$form_state) {
  print_r($form_state['values']);
  die('miller');
}

function exam_results(){
  return "Coming soon!";
}

