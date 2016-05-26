<?php
/**
 * @file callbacks.inc
 * Minor menu callbacks for quiz matrix helpers.
 */

/**
 * A central administrative page for quiz matrix.
 */
function exam_spider_dashboard() {
  $output = '';
  $output .= l(t('+ Create '.EXAM_SPIDER_EXAM_TITLE), 'admin/structure/'.EXAM_SPIDER_EXAM_URL.'/add', array('html' => TRUE));
  $header = array(
    array('data' =>  EXAM_SPIDER_EXAM_TITLE.' Id', 'field' => 'id', 'sort' => 'desc'),
    array('data' =>  EXAM_SPIDER_EXAM_TITLE.' Name', 'field' => 'exam_name'),
    array('data' =>  EXAM_SPIDER_EXAM_TITLE.' Description', 'field' => 'exam_description'),
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
      'status'
    ));
  $results = $query->execute();
  $rows = array();
  $i = 0;
  foreach ($results as $row) {
    if ($row->status == 0) $status = 'Closed';
    else $status = 'Open';
    $user_data = user_load($row->uid);
    $rows[] = array(
      'data' => array(
        EXAM_SPIDER_EXAM_TITLE . '-' . $row->id,
        l($row->exam_name,  EXAM_SPIDER_EXAM_URL. '/' . $row->id . '/start'),
        $row->exam_description,
        $user_data->name,
        $status,
        l(t('Questions'), 'admin/structure/question/add_question/' . $row->id) . ' | ' . l(t('Edit'), 'admin/structure/'.EXAM_SPIDER_EXAM_URL.'/' . $row->id . '/edit') . ' | ' . l(t('Delete'), 'admin/structure/'.EXAM_SPIDER_EXAM_URL.'/' . $row->id . '/delete'),
      )
    );
    $i++;
  }
  $output .= theme('table', array(
    'header' => $header,
    'rows'   => $rows,
    'empty'  => t('No Exam created yet. ') . l(t('+ Create Exam'), 'admin/structure/'.EXAM_SPIDER_EXAM_URL.'/add')
  ));
  $output .= theme('pager');
  return ($output);
}

function exam_spider_add_exam($form, &$form_state) {
  $form = array();
  $form['exam_name'] = array(
    '#title'     => t(EXAM_SPIDER_EXAM_TITLE.' Name'),
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
    '#title'       => t(EXAM_SPIDER_EXAM_TITLE.' settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,

  );
  $form['examsettings']['exam_duration'] = array(
    '#title'         => t(EXAM_SPIDER_EXAM_TITLE.' Duration'),
    '#description'   => t('Time duration in minutes.'), 
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
  $form['examsettings']['mark_per_quest'] = array(
    '#title'            => t('Marks Per Question'),
    '#type'             => 'textfield',
    '#maxlength'        => '10',
    '#size'             => 10,
    '#required'         => TRUE,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['examsettings']['negative_mark'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Negative Marking'),
  );
  $form['examsettings']['random_quest'] = array(
    '#type'  => 'checkbox',
    '#title' => t('Display Random Question'),
  );

  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function exam_spider_add_exam_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['exam_duration'])) {
    form_set_error('exam_duration', t('You must enter a positive integer value for '.EXAM_SPIDER_EXAM_TITLE.' duration.'));
  }

}


function exam_spider_add_exam_submit(&$form, &$form_state) {
  global $user;
  db_insert('exam_list')
    ->fields(array(
      'exam_name',
      'exam_description',
      'uid',
      'total_marks',
      'mark_per_quest',
      'negative_mark',
      'random_quest',
      'created',
      'changed'
    ))
    ->values(array(
      'exam_name'        => $form_state['values']['exam_name'],
      'exam_description' => $form_state['values']['exam_description'],
      'uid'              => $user->uid,
      'total_marks'      => $form_state['values']['total_marks'],
      'mark_per_quest'   => $form_state['values']['mark_per_quest'],
      'negative_mark'    => $form_state['values']['negative_mark'],
      'random_quest'     => $form_state['values']['random_quest'],
      'exam_duration'    => $form_state['values']['exam_duration'],
      'created'          => REQUEST_TIME,
      'changed'          => REQUEST_TIME,
    ))
    ->execute();
  drupal_set_message(t('You have successfully created '.EXAM_SPIDER_EXAM_TITLE.', Please add Questions for '.$form_state['values']['exam_name'].' '.EXAM_SPIDER_EXAM_TITLE.'.'));
  drupal_goto('admin/structure/'.EXAM_SPIDER_EXAM_URL);
  return $form;

}

function exam_spider_edit_exam($form, &$form_state, $exam_id) {
  $values = exam_spider_get_exam($exam_id);
  $form = array();
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $form['exam_name'] = array(
    '#title'         => t(EXAM_SPIDER_EXAM_TITLE.' Name'),
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
    '#title'       => t(EXAM_SPIDER_EXAM_TITLE.' settings'),
    '#collapsible' => TRUE,
    '#collapsed'   => FALSE,

  );
  $form['examsettings']['exam_duration'] = array(
    '#title'         => t(EXAM_SPIDER_EXAM_TITLE.' Duration'),
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
  $form['examsettings']['mark_per_quest'] = array(
    '#title'            => t('Marks Per Question'),
    '#type'             => 'textfield',
    '#maxlength'        => '10',
    '#size'             => 10,
    '#required'         => TRUE,
    '#default_value'    => isset($values['mark_per_quest']) ? $values['mark_per_quest'] : NULL,
    '#element_validate' => array('element_validate_integer_positive'),
  );
  $form['examsettings']['negative_mark'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Negative Marking'),
    '#default_value' => isset($values['negative_mark']) ? $values['negative_mark'] : NULL,
  );
  $form['examsettings']['random_quest'] = array(
    '#type'          => 'checkbox',
    '#title'         => t('Display Random Question'),
    '#default_value' => isset($values['random_quest']) ? $values['random_quest'] : NULL,
  );
  $form['submit'] = array(
    '#type'  => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

function exam_spider_edit_exam_validate($form, &$form_state) {
  if (!is_numeric($form_state['values']['exam_duration'])) {
    form_set_error('exam_duration', t('You must enter a integer value for '.EXAM_SPIDER_EXAM_TITLE.' duration.'));
  }
}

function exam_spider_edit_exam_submit(&$form, &$form_state) {
  global $user;
  db_update('exam_list')
    ->fields(array(
      'exam_name'        => $form_state['values']['exam_name'],
      'exam_description' => $form_state['values']['exam_description'],
      'uid'              => $user->uid,
      'total_marks'      => $form_state['values']['total_marks'],
      'exam_duration'    => $form_state['values']['exam_duration'],
      'mark_per_quest'   => $form_state['values']['mark_per_quest'],
      'negative_mark'    => $form_state['values']['negative_mark'],
      'random_quest'     => $form_state['values']['random_quest'],
      'changed'          => REQUEST_TIME,
    ))
    ->condition('id', $form_state['values']['exam_id'], '=')
    ->execute();
  drupal_set_message(t('You have successfully updated '.$form_state['values']['exam_name'].' '.EXAM_SPIDER_EXAM_TITLE.'.'));
  drupal_goto('admin/structure/'.EXAM_SPIDER_EXAM_URL);
  return $form;

}

function exam_spider_exam_delete_confirm($form, &$form_state, $exam_id) {
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $path = 'admin/structure/'.EXAM_SPIDER_EXAM_URL;
  return confirm_form($form,
    t('Are you sure you want to delete '.EXAM_SPIDER_EXAM_TITLE.'-%exam_id ?', array('%exam_id' => $exam_id)),
    $path,
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}

function exam_spider_exam_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    $exam_id = $form_state['values']['exam_id'];
    db_delete('exam_list')
      ->condition('id', $exam_id, '=')
      ->execute();
    db_delete('exam_questions')
      ->condition('examid', $exam_id, '=')
      ->execute();

    drupal_set_message(t('EXAM-%exam_id has been deleted.', array('%exam_id' => $exam_id)));
  }
  $form_state['redirect'] = 'admin/structure/exam';
}

function exam_spider_exam_start($form, &$form_state, $exam_id) {
  global $user;
  $form['exam_id'] = array('#type' => 'value', '#value' => $exam_id);
  $form['name'] = array(
    '#type'          => 'textfield',
    '#title'         => 'Name',
    '#default_value' => isset($user->name) ? $user->name : NULL,
    '#required'      => TRUE,
  );

  $form['mail'] = array(
    '#type'          => 'textfield',
    '#title'         => 'Email',
    '#default_value' => isset($user->mail) ? $user->mail : NULL,
    '#required'      => TRUE,
  );
  $form['next'] = array(
    '#type'  => 'submit',
    '#value' => t('Start')
  );

  return $form;
}

function exam_spider_exam_start_validate(&$form, &$form_state) {
  if (!valid_email_address($form_state['values']['mail'])) {
    return form_set_error('mail', t('The email address appears to be invalid.'));
  }
}

function exam_spider_exam_start_submit(&$form, &$form_state) {
  global $user;
  db_insert('exam_results')
    ->fields(array('examid', 'uid'))
    ->values(array(
      'examid' => $form_state['values']['exam_id'],
      'uid'    => $user->uid,
    ))
    ->execute();
  if (isset($form_state['values']['exam_id'])) {
    drupal_goto(EXAM_SPIDER_EXAM_URL . '/' . $form_state['values']['exam_id'] . '/continue');
  }
  else {
    drupal_goto(EXAM_SPIDER_EXAM_URL .'/' . $form_state['values']['exam_id'] . '/start');
  }


}

function exam_spider_exam_continue($form, &$form_state, $exam_id) {
  $output = '';
  $form['#prefix'] = '<div id="exam_timer"></div>';
  if (is_numeric($exam_id)) {
    exam_spider_clock('exam-spider-exam-continue');
  }
  drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/jquery.bxslider.min.js');
  drupal_add_js(drupal_get_path('module', 'exam_spider') . '/js/exam_spider.js');

  $query = db_select("exam_questions", "eq")
    ->fields("eq")
    ->condition('examid', $exam_id, '=')->orderRandom()->execute();
  $results = $query->fetchAll();
  if(empty($results)){
    $output .= t('No question created yet for this '.EXAM_SPIDER_EXAM_TITLE.'.');
  }
  else{
    $form['li_prefix'] = array(
      '#markup' => '<ul class="exam-slider">',
    );
    foreach ($results as $value) {
      $options[1] = $value->opt1;
      $options[2] = $value->opt2;
      $options[3] = $value->opt3;
      $options[4] = $value->opt4;
      if ($value->multiple == 1) {
        $form['question'][$value->id] = array(
          '#type'    => 'checkboxes',
          '#options' => drupal_map_assoc($options),
          '#title'   => t($value->question),
        );
      }
      else {
        $form['question'][$value->id] = array(
          '#type'    => 'radios',
          '#title'   => t($value->question),
          '#options' => $options,
          '#prefix'  => '<li>',
          '#suffix'  => '</li>',
        );
      }
    }
    $form['next'] = array(
      '#type'   => 'submit',
      '#prefix' => '<li>',
      '#suffix' => '</li>',
      '#value'  => t('Submit')
    );
    $form['li_suffix'] = array(
      '#markup' => '</ul>',
    );
    $output .= ' <p><span id="slider-next"></span></p>';
  }
  $form['#suffix'] = $output;
  return $form;
}

function exam_spider_exam_continue_submit(&$form, &$form_state) {
  print_r($form_state['values']);
   global $user;
/*  db_insert('exam_results')
    ->fields(array('examid', 'uid'))
    ->values(array(
      'examid' => $form_state['values']['exam_id'],
      'uid'    => $user->uid,
    ))*/
  die('miller');
}

function exam_spider_exam_results() {
  $output = '';
  $header = array(
    array('data' =>  'REG Id', 'field' => 'id', 'sort' => 'desc'),
    array('data' =>  'Exam Name', 'field' => 'examid'),
    array('data' =>  'Name', 'field' => 'uid'),
    array('data' =>  'Total Marks', 'field' => 'total'),
    array('data' => 'Obtain Marks', 'field' => 'obtain'),
    array('data' => 'Wrong', 'field' => 'wrong'),

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
      'wrong'
    ));
  $results = $query->execute();
  $rows = array();
  $i = 0;
  foreach ($results as $row) {
    $user_data = user_load($row->uid);
    $exam_data = exam_spider_get_exam( $row->examid);
    $rows[] = array(
      'data' => array(
        t('REG -') . $row->id,
        $exam_data['exam_name'],
        $user_data->name,
        $row->total,
        $row->obtain,
        $row->wrong,
      )
    );
    $i++;
  }
  $output .= theme('table', array(
    'header' => $header,
    'rows'   => $rows,
    'empty'  => t('No Exam attempt by any user.')
  ));
  $output .= theme('pager');
  return ($output);
}

function exam_spider_clock($formid) {
  require_once(drupal_get_path('module', 'exam_spider') . '/includes/ExamSpiderDigitalClock.php');
  $clock = new exam_spider_digital_clock();
  return $clock->_show_clock($formid);
}