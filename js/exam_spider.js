(function($) {
  $('document').ready(function(){
    removedisable();
    $('#edit-multi-answer').click(function(){
       $('.answer.form-checkbox').removeAttr('checked');
       $('.answer.form-checkbox').removeAttr('disabled');
    });
    $('.answer.form-checkbox').click(function(){
      if(!$('#edit-multi-answer').is(':checked')){
        if($(this).is(':checked')){
          $('.answer.form-checkbox').attr('disabled','disabled');
          $(this).removeAttr('disabled');
        }
      }  
    });
    function removedisable(){
      if(!$('#edit-multi-answer').is(':checked')){
        $('.answer.form-checkbox').each(function(i) {
          if($('#edit-answer'+i).is(':checked')){
            $('.answer.form-checkbox').attr('disabled','disabled');
            $('#edit-answer'+i).removeAttr('disabled');
          }
        }); 
      }
    }
   var examslider = $('.exam-slider').bxSlider({
      infiniteLoop: false,
      adaptiveHeight: true,
      pager:false,
      auto: false, 
      mode: 'fade',
       controls: true,
      hideControlOnEnd: true,
      nextSelector: '#slider-next',
      nextText: 'Next',
     onSlideNext: function($slideElement, oldIndex, newIndex) {
        totalslider = (examslider.getSlideCount()-1);
        if (newIndex==totalslider) { $('#slider-next').hide('fast'); }
    }
    });
  
  }); 
})(jQuery);