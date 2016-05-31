(function ($) {
  'use strict';
  $('document').ready(function () {
    removedisable();
    $('#edit-multi-answer').click(function () {
      $('.answer.form-checkbox').removeAttr('checked');
      $('.answer.form-checkbox').removeAttr('disabled');
    });
    $('.answer.form-checkbox').click(function () {
      if (!$('#edit-multi-answer').is(':checked')) {
        if ($(this).is(':checked')) {
          $('.answer.form-checkbox').attr('disabled', 'disabled');
          $(this).removeAttr('disabled');
        }
      }
    });
    function removedisable() {
      if (!$('#edit-multi-answer').is(':checked')) {
        $('.answer.form-checkbox').each(function (i) {
          if ($('#edit-answer' + i).is(':checked')) {
            $('.answer.form-checkbox').attr('disabled', 'disabled');
            $('#edit-answer' + i).removeAttr('disabled');
          }
        });
      }
    }

    function exam_spider_slider() {
      if ($('.exam_spider_slider_exam').find('.show')) {
        var current_id = $('.exam_spider_slider_exam').find('.show').attr('id');
        var numeric_id = current_id.split('_');
        var counter_next = parseInt(numeric_id[1]) + parseInt(1);
        var total_slides = $('.exam_spider_slider_exam li').length;
        if (counter_next < total_slides) {
          $('.exam_spider_slider_exam .exam_spider_slider').removeClass('show');
          $('#examslide_' + counter_next).addClass('show');
        }
        else if (counter_next == total_slides) {
          $('.exam_spider_slider_exam .exam_spider_slider').removeClass('show');
          $('#examslide_' + counter_next).addClass('show');
          $('.exam_spider_slide_next').hide();
        }
      }
    }

    $('.exam_spider_slider_exam .exam_spider_slider:first-child').addClass('show');
    $('.exam_spider_slide_next').click(function () {
      exam_spider_slider();
    });
  });
})(jQuery);
