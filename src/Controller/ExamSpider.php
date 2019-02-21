<?php

namespace Drupal\exam_spider\Controller;

use Drupal\Core\Controller\ControllerBase;
use Drupal\Core\Database\Connection;
use Drupal\Core\Datetime\DateFormatterInterface;
use Drupal\Core\Form\FormBuilderInterface;
use Drupal\user\Entity\User;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Drupal\Core\Link;
use Drupal\Core\Url;


/**
 * A class for muliple ExamSpider functions.
 */
class ExamSpider extends ControllerBase {

  /**
   * The database service.
   *
   * @var \Drupal\Core\Database\Connection
   */
  protected $database;

  /**
   * The date formatter service.
   *
   * @var \Drupal\Core\Datetime\DateFormatterInterface
   */
  protected $dateFormatter;

  /**
   * The form builder service.
   *
   * @var \Drupal\Core\Form\FormBuilderInterface
   */
  protected $formBuilder;

  /**
   * The user storage.
   *
   * @var \Drupal\user\UserStorageInterface
   */
  protected $userStorage;

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('database'),
      $container->get('date.formatter'),
      $container->get('form_builder')
    );
  }

  /**
   * Constructs a DbLogController object.
   *
   * @param \Drupal\Core\Database\Connection $database
   *   A database connection.
   * @param \Drupal\Core\Datetime\DateFormatterInterface $date_formatter
   *   The date formatter service.
   * @param \Drupal\Core\Form\FormBuilderInterface $form_builder
   *   The form builder service.
   */
  public function __construct(Connection $database, DateFormatterInterface $date_formatter, FormBuilderInterface $form_builder) {
    $this->database = $database;
    $this->dateFormatter = $date_formatter;
    $this->formBuilder = $form_builder;
    $this->userStorage = $this->entityManager()->getStorage('user');
  }

  /**
   * Get time limit function.
   */
  public function examSpidergetTimeLimit($exam_duration) {
    $timer = time() + intval($exam_duration * 60);
    return date('r', $timer);
  }

  /**
   * Displays a listing of Exams list.
   */
  public function examSpiderDashboard() {
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
    $query = $this->database->select('exam_list', 'el')
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
      $examcontinue_link = Link::fromTextAndUrl($row->exam_name, $examcontinue_url)->toString();
      $operations = $this->t(
        '@addquestion_link | @editexam_link | @deleteexam_link', [
          '@addquestion_link' => $addquestion_link,
          '@editexam_link' => $editexam_link,
          '@deleteexam_link' => $deleteexam_link,
        ]
      );
      $user = $this->userStorage->load($row->uid);
      $rows[] = [
        'data' => [
          EXAM_SPIDER_EXAM_TITLE . '-' . $row->id,
          $examcontinue_link,
          $row->exam_description,
          $user->getUsername(),
          $status,
          $operations,
        ],
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
  public function examSpiderGetQuestionsList($exam_id) {
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
      $query = $this->database->select("exam_questions", "eq")
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
        $operations = $this->t('@editquestion_link | @deletequestion_link', ['@editquestion_link' => $editquestion_link, '@deletequestion_link' => $deletequestion_link]);
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
  public function examSpiderExamResults() {
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
    $query = $this->database->select('exam_results', 'er')
      ->extend('\Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('\Drupal\Core\Database\Query\TableSortExtender');
    $query->fields(
      'er', ['id', 'examid', 'uid', 'total', 'obtain', 'wrong', 'created']
    );
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
      $sendmail_url = Url::fromRoute('exam_spider.exam_spider_exam_result_mail', ['resultid' => $row->id, 'uid' => $row->uid]);
      $sendmail_link = Link::fromTextAndUrl($this->t('Send Mail'), $sendmail_url)->toString();
      $operations = $this->t('@deleteresult_link | @sendmail_link', ['@deleteresult_link' => $deleteresult_link, '@sendmail_link' => $sendmail_link]);

      $exam_data = $this->examSpiderGetExam($row->examid);
      $user = $this->userStorage->load($row->uid);
      $rows[] = [
        'data' => [
          $this->t('REG -') . $row->id,
          $exam_data['exam_name'],
          $user->get('name')->value,
          $row->total,
          $row->obtain,
          $row->wrong,
          $this->dateFormatter->format($row->created, 'short'),
          $operations,
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
  public function examSpiderGetExam($exam_id = NULL) {
    if (is_numeric($exam_id)) {
      $query = $this->database->select("exam_list", "el")
        ->fields("el")
        ->condition('id', $exam_id);
      $query = $query->execute();
      return $query->fetchAssoc();
    }
    else {
      $query = $this->database->select("exam_list", "el")
        ->fields("el");
      $query = $query->execute();
      return $query->fetchAll();
    }
  }

  /**
   * Get questions using question id and without question id questions list.
   */
  public function examSpiderGetQuestion($question_id = NULL) {
    if (is_numeric($question_id)) {
      $query = $this->database->select("exam_questions", "eq")
        ->fields("eq")
        ->condition('id', $question_id);
      $query = $query->execute();
      return $query->fetchAssoc();
    }
    else {
      $query = $this->database->select("exam_questions", "eq")
        ->fields("eq");
      $query = $query->execute();
      return $query->fetchAll();
    }
  }

  /**
   * Get any user last result for any exam.
   */
  public function examSpiderAnyExamLastResult($exam_id = NULL, $uid = NULL) {
    if ($uid === NULL) {
      $uid = \Drupal::currentUser()->id();
    }
    if (is_numeric($exam_id)) {
      $query = $this->database->select("exam_results", "er")
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

  /**
   * All exam listed page to start exam page callbacks.
   */
  public function examSpiderExamStart() {
    $output = NULL;
    $header = [
      [
        'data' => EXAM_SPIDER_EXAM_TITLE . ' Name',
        'field' => 'el.exam_name',
      ],
      [
        'data' => EXAM_SPIDER_EXAM_TITLE . ' Description',
        'field' => 'el.exam_description',
      ],
      [
        'data' => 'Operations',
      ],
    ];
    $query = $this->database->select('exam_list', 'el')
      ->extend('\Drupal\Core\Database\Query\PagerSelectExtender')
      ->extend('\Drupal\Core\Database\Query\TableSortExtender');
    $query->fields(
      'el', ['id', 'exam_name', 'exam_description', 'status']
    );
    $results = $query
      ->limit(10)
      ->orderByHeader($header)
      ->execute()
      ->fetchAll();
    $rows = [];
    foreach ($results as $row) {
      if ($row->status == 1) {

      }
      $examcontinue_url = Url::fromRoute('exam_spider.exam_spider_exam_continue', ['examid' => $row->id]);
      $examcontinue_link = Link::fromTextAndUrl($this->t('Start @examSpiderExamTitle', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]), $examcontinue_url)->toString();
      $examcontinue__name_link = Link::fromTextAndUrl($this->t('@examName', ['@examName' => $row->exam_name]), $examcontinue_url)->toString();
      $rows[] = [
        'data' => [
          $examcontinue__name_link,
          $row->exam_description,
          $examcontinue_link,
        ],
      ];
    }
    $output['exams_start_list'] = [
      '#theme' => 'table',
      '#header' => $header,
      '#rows' => $rows,
      '#empty' => $this->t('No @examSpiderExamTitle created yet.', ['@examSpiderExamTitle' => EXAM_SPIDER_EXAM_TITLE]),
      '#attributes' => ['class' => 'exams-start-table'],
    ];
    $output['exams_start_pager'] = ['#type' => 'pager'];
    return $output;
  }

  /**
   * Send result score card via mail.
   */
  public function examSpiderExamResultMail($resultid) {
    if (is_numeric($resultid)) {
      $query = $this->database->select("exam_results", "er")
        ->fields("er")
        ->condition('id', $resultid);
      $exam_result_data = $query->execute()->fetchAssoc();
      $user_data = User::load($exam_result_data['uid']);
      $exam_data = $this->examSpiderGetExam($exam_result_data['examid']);
      $mailManager = \Drupal::service('plugin.manager.mail');
      $body = $this->t('Hi @tomail,

      You have got @score_obtain marks out of @total_marks.
      Wrong Answer(s) @wrong_quest.

      Many Thanks,
      @sitename', [
        '@score_obtain'   => $exam_result_data['obtain'],
        '@total_marks'    => $exam_result_data['total'],
        '@wrong_quest'    => $exam_result_data['wrong'],
        '@sitename'       => \Drupal::config('system.site')->get('name'),
        '@tomail'         => @$user_data->get('name')->value,

      ]);
      $module = 'exam_spider';
      $key = 'exam_spider_result';
      $to = $user_data->get('mail')->value;
      $params['message'] = $body;
      $params['subject'] = 'Eaxam Result for ' . $exam_data['exam_name'];
      $langcode = \Drupal::currentUser()->getPreferredLangcode();
      $send = TRUE;
      $result = $mailManager->mail($module, $key, $to, $langcode, $params, NULL, $send);
      if ($result['result'] !== TRUE) {
        return drupal_set_message($this->t('There was a problem sending your message and it was not sent.'), 'error');
      }
      else {
        return drupal_set_message($this->t('Your message has been sent.'));
      }
      // Commented return $this->redirect('exam_spider.exam_spider_delete_result');.
    }
  }

}
