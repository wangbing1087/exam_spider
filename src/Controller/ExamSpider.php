<?php

namespace Drupal\exam_spider\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Link;
use Drupal\Core\Url;

/**
 * A class for muliple ExamSpider functions.
 */
class ExamSpider extends ControllerBase {

  /**
   * Displays a listing of Exams list.
   */
  public function exam_spider_dashboard() {
    $createexam_url = Url::fromRoute('exam_spider.exam_spider_add_exam');
    $createexam_link = Link::fromTextAndUrl($this->t('+ Create @examSpiderExamTitle', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]), $createexam_url)->toString();
    $output['add_exams_link'] = [
      '#markup' => $createexam_link,
    ];
    $header = [
      [
        'data' => EXAM_SPIDER_EXAM_TITLE . ' Id',
        'field' => 'el.id',
        'sort' => 'desc',
      ],
      [
        'data' => EXAM_SPIDER_EXAM_TITLE . ' Name',
        'field' => 'el.exam_name',
      ],
      [
        'data' => EXAM_SPIDER_EXAM_TITLE . ' Description',
        'field' => 'exam_description',
      ],
      [
        'data' => 'Created By',
        'field' => 'el.uid',
      ],
      [
        'data' => 'Status',
        'field' => 'el.status',
      ],
      [
        'data' => 'Operations',
      ],
    ];
    $query = \Drupal::database()->select('exam_list', 'el')
      ->extend('\Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('\Drupal\Core\Database\Query\TableSortExtender');
    $query->fields('el',
      ['id', 'exam_name', 'exam_description', 'uid', 'status']
    );
    $results = $query
      ->limit(10)
      ->orderByHeader($header)
      ->execute()
      ->fetchAll();
    $rows = [];
    foreach ($results as $row) {
      if ($row->status == 0) {
        $status = 'Closed';
      } 
      else {
        $status = 'Open';
      }
      $addquestion_url = Url::fromRoute('exam_spider.exam_spider_add_question', ['examid' => $row->id]);
      $addquestion_link = Link::fromTextAndUrl($this->t('Questions'), $addquestion_url)->toString();
      $editexam_url = Url::fromRoute('exam_spider.exam_spider_edit_exam', ['examid' => $row->id]);
      $editexam_link = Link::fromTextAndUrl($this->t('Edit'), $editexam_url)->toString();
      $deleteexam_url = Url::fromRoute('exam_spider.exam_spider_delete_exam', ['examid' => $row->id]);
      $deleteexam_link = Link::fromTextAndUrl($this->t('Delete'), $deleteexam_url)->toString();
      $examcontinue_url = Url::fromRoute('exam_spider.exam_spider_exam_continue', ['examid' => $row->id]);
      $examcontinue_link = Link::fromTextAndUrl($this->t($row->exam_name), $examcontinue_url)->toString();
      $operations = t('@addquestion_link | @editexam_link | @deleteexam_link', ['@addquestion_link' => $addquestion_link, '@editexam_link' => $editexam_link, '@deleteexam_link' => $deleteexam_link]);
      $user = \Drupal\user\Entity\User::load($row->uid);
      $rows[] = [
        'data' => [
          EXAM_SPIDER_EXAM_TITLE . '-' . $row->id,
          $examcontinue_link,
          $row->exam_description,
          $user->get('name')->value,
          $status,
          $operations,
        ]
      ];
    }
    $output['exams_list'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => $this->t('No Exams available.@create_exam_link', ['@create_exam_link' => $createexam_link]),
      '#attributes' => ['class' => 'exams-list-table'],
    ];
    $output['exams_pager'] = ['#type' => 'pager'];
    return $output;
  }

  /**
   * Get Question list using exam id function.
   */
  function exam_spider_get_questions($exam_id) {
    $output = NULL;
    if (is_numeric($exam_id)) {
      $header = [
        [
          'data' => 'Question',
          'field' => 'eq.question',
        ],
        [
          'data' => 'Status',
          'field' => 'eq.status',
        ],
        [
          'data' => 'Operations',
        ],
      ];
      $query = \Drupal::database()->select("exam_questions", "eq")
        ->extend('\Drupal\Core\Database\Query\PagerSelectExtender')
        ->extend('\Drupal\Core\Database\Query\TableSortExtender');
      $query->fields('eq', ['id', 'question', 'status']);
      $query->condition('examid', $exam_id);
      $results = $query
        ->limit(10)
        ->orderByHeader($header)
        ->execute()
        ->fetchAll();
      $rows = [];
      foreach ($results as $row) {
        $editquestion_url = Url::fromRoute('exam_spider.exam_spider_edit_question', ['questionid' => $row->id]);
        $editquestion_link = Link::fromTextAndUrl($this->t('Edit'), $editquestion_url)->toString();
        $deletequestion_url = Url::fromRoute('exam_spider.exam_spider_delete_question', ['questionid' => $row->id]);
        $deletequestion_link = Link::fromTextAndUrl($this->t('Delete'), $deletequestion_url)->toString();
        $operations = $this->t('@editquestion_link | @deletequestion_link ', ['@editquestion_link' => $editquestion_link, '@deletequestion_link' => $deletequestion_link]);
        if ($row->status == 0) {
          $status = 'Closed';
        } 
        else {
          $status = 'Open';
        }
        $rows[] = [
          'data' => [
            $row->question,
            $status,
            $operations,
          ],
        ];
      }
      $output['questions_list'] = [
        '#theme' => 'table',
        '#header' => $header,
        '#rows' => $rows,
        '#empty' => $this->t('No question created yet for this @examSpiderExamTitle', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]),
        '#attributes' => ['class' => 'questions-list-table'],
      ];
      $output['questions_pager'] = ['#type' => 'pager'];
    }
    return $output;
  }

  /**
   * Get exam results function.
   */
  public function exam_spider_exam_results() {
    $header = [
      [
        'data' => 'REG Id',
        'field' => 'er.id',
        'sort' => 'desc',
      ],
      [
        'data' => EXAM_SPIDER_EXAM_TITLE . ' Name',
        'field' => 'er.examid',
      ],
      [
        'data' => 'Name',
        'field' => 'er.uid',
      ],
      [
        'data' => 'Total Marks',
        'field' => 'er.total',
      ],
      [
        'data' => 'Obtain Marks',
        'field' => 'er.obtain',
      ],
      [
        'data' => 'Wrong',
        'field' => 'er.wrong',
      ],
      [
        'data' => 'Date',
        'field' => 'er.created',
      ],
      [
        'data' => 'Operations',
      ],
    ];
    $query = \Drupal::database()->select('exam_results', 'er')
      ->extend('\Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('\Drupal\Core\Database\Query\TableSortExtender');
    $query->fields('er', ['id', 'examid', 'uid', 'total', 'obtain', 'wrong', 'created']);
    if (isset($_GET['exam_name'])) {
      $query->condition('examid', $_GET['exam_name']);
    }
    $results = $query
      ->limit(10)
      ->orderByHeader($header)
      ->execute()
      ->fetchAll();
    $rows = [];
    foreach ($results as $row) {
      $deleteresult_url = Url::fromRoute('exam_spider.exam_spider_delete_result', ['resultid' => $row->id]);
      $deleteresult_link = Link::fromTextAndUrl($this->t('Delete'), $deleteresult_url)->toString();
      $exam_data = $this->exam_spider_get_exam($row->examid);
      $user = \Drupal\user\Entity\User::load($row->uid);
      $rows[] = [
        'data' => [
          t('REG -') . $row->id,
          $exam_data['exam_name'],
          $user->get('name')->value,
          $row->total,
          $row->obtain,
          $row->wrong,
          format_date($row->created, 'short'),
          $deleteresult_link,
        ],
      ];
    }
    $output['exams_result_list'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => $this->t('No @examSpiderExamTitle result found.', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]),
      '#attributes' => ['class' => 'exams-result-table'],
    ];
    $output['exams_result_pager'] = ['#type' => 'pager'];
    return $output;
  }

  /**
   * Get exam list using exam id and without exam id complete exam list.
   */
  public function exam_spider_get_exam($exam_id = NULL) {
    if (is_numeric($exam_id)) {
      $query = db_select("exam_list", "el")
        ->fields("el")
        ->condition('id', $exam_id);
      $query = $query->execute();
      return $query->fetchAssoc();
    } 
    else {
      $query = db_select("exam_list", "el")
        ->fields("el");
      $query = $query->execute();
      return $query->fetchAll();
    }
  }

  /**
   * Get questions using question id and without question id questions list.
   */
  public function exam_spider_get_question($question_id = NULL) {
    if (is_numeric($question_id)) {
      $query = db_select("exam_questions", "eq")
        ->fields("eq")
        ->condition('id', $question_id);
      $query = $query->execute();
      return $query->fetchAssoc();
    } 
    else {
      $query = db_select("exam_questions", "eq")
        ->fields("eq");
      $query = $query->execute();
      return $query->fetchAll();
    }
  }

  /**
   * Get any user last result for any exam.
   */
  public function exam_spider_any_exam_last_result($exam_id = NULL, $uid = NULL) {
    if ($uid === NULL) {
      $uid = \Drupal::currentUser()->id();
    }
    if (is_numeric($exam_id)) {
      $query = db_select("exam_results", "er")
        ->fields("er")
        ->condition('examid', $exam_id)
        ->orderBy('id', 'DESC')
        ->condition('uid', $uid);
      $query = $query->execute();
      return $query->fetchAssoc();
    } 
    else {
      return FALSE;
    }
  }

}
