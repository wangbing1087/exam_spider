<?php

class DigitalClock {
    private $noLimit;
    public function __construct(){
        $this->noLimit = FALSE;

            //$this->noLimit = TRUE; 
    }
    public function _showClock($for){ ?>
        <!-- <div id="exTimer"></div>  <div id="testing">fsdffsdf</div> -->
        <script type="text/javascript">
            var TimeLimit = new Date('<?php echo $this->_getTimeLimit(); ?>');
            var forForm = '<?php echo $for; ?>';
            function countdownto() {
              var date = Math.round((TimeLimit-new Date())/1000);
              var hours = Math.floor(date/3600);
              date = date - (hours*3600);
              var mins = Math.floor(date/60);
              date = date - (mins*60);
              var secs = date;
              if (hours<10) hours = '0'+hours;
              if (mins<10) mins = '0'+mins;
              if (secs<10) secs = '0'+secs;
             var elem = document.getElementById('exTimer')
              if(typeof elem !== 'undefined' && elem !== null && parseInt(hours+mins+secs)!=0) {
                document.getElementById('exTimer').innerHTML = hours+':'+mins+':'+secs;
              }
              if(hours=='00' && mins == '00' && secs == '00'){
                document.getElementById('slider-next').disabled=true
                document.getElementById(forForm).submit();
              }
              setTimeout("countdownto()",1000);
            }
            countdownto();
        </script> <?php
    }
    private function _getTimeLimit(){
        $values = getExam (arg(1));
        $timer = time() + intval($values['exam_duration']*60);
        return date('r', $timer);
    }
}
