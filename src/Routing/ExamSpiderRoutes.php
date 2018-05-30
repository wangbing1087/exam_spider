<?php

namespace Drupal\exam_spider\Routing;

use Symfony\Component\Routing\Route;

/**
 * Defines a route subscriber to register a url for serving image styles.
 */
class ExamSpiderRoutes {
  /**
   * Returns an array of route objects.
   *
   * @return \Symfony\Component\Routing\Route[]
   *   An array of route objects.
   */

  public function routes() {
    $routes = [];
    $routes['exam_spider.exam_spider_dashboard'] = new Route(
      '/admin/structure/' . EXAM_SPIDER_EXAM_URL,
      array(
        '_controller' => '\Drupal\exam_spider\Controller\ExamSpider::exam_spider_dashboard',
        '_title'      => EXAM_SPIDER_EXAM_TITLE . 'Dashboard',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_exam_results'] = new Route(
      '/admin/structure/' . EXAM_SPIDER_EXAM_URL . '/results',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderResultsForm',
        '_title'      => EXAM_SPIDER_EXAM_TITLE . 'Results',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_delete_result'] = new Route(
      'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/result/{resultid}/delete',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderResultsDelete',
        '_title'      => 'Delete Result',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_exam_settings'] = new Route(
      '/admin/structure/' . EXAM_SPIDER_EXAM_URL . '/config',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderSettingsForm',
        '_title'      => EXAM_SPIDER_EXAM_TITLE . 'Settings',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_add_exam'] = new Route(
      '/admin/structure/' . EXAM_SPIDER_EXAM_URL . '/add',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderExamForm',
        '_title'      => 'Create ' . EXAM_SPIDER_EXAM_TITLE,
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_edit_exam'] = new Route(
      'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/{examid}/edit',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderExamForm',
        '_title'      => 'Edit ' . EXAM_SPIDER_EXAM_TITLE,
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_delete_exam'] = new Route(
      'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/{examid}/delete',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderExamDelete',
        '_title'      => 'Delete ' . EXAM_SPIDER_EXAM_TITLE,
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_add_question'] = new Route(
      'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/question/{examid}/add',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderQuestionForm',
        '_title'      => 'Add Question',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_edit_question'] = new Route(
      'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/question/{questionid}/edit',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderQuestionForm',
        '_title'      => 'Edit Question',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_delete_question'] = new Route(
      'admin/structure/' . EXAM_SPIDER_EXAM_URL . '/question/{questionid}/delete',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderQuestionDelete',
        '_title'      => 'Delete Question',
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    $routes['exam_spider.exam_spider_exam_continue'] = new Route(
      '/' . EXAM_SPIDER_EXAM_URL . '/{examid}/continue',
       array(
        '_form' => '\Drupal\exam_spider\Form\ExamSpiderExamContinue',
        '_title'      => 'Continue ' . EXAM_SPIDER_EXAM_TITLE,
      ),
      array(
        '_permission' => 'exam spider dashboard',
      )
    );
    return $routes;
  }

}