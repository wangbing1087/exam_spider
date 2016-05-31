<?php

class ExamSpiderDigitalClock {
    private $noLimit;

    public function __construct() {
        $this->noLimit = FALSE;
    }

    public function show_clock($for) { ?>
        <script type="text/javascript">
            var TimeLimit = new Date('<?php echo $this->_get_time_limit(); ?>');
            var forForm = '<?php echo $for; ?>';
            function countdownto() {
                var date = Math.round((TimeLimit - new Date()) / 1000);
                var hours = Math.floor(date / 3600);
                date = date - (hours * 3600);
                var mins = Math.floor(date / 60);
                date = date - (mins * 60);
                var secs = date;
                if (hours < 10) hours = '0' + hours;
                if (mins < 10) mins = '0' + mins;
                if (secs < 10) secs = '0' + secs;
                var elem = document.getElementById('exam_timer')
                if (typeof elem !== 'undefined' && elem !== null && parseInt(hours + mins + secs) != 0) {
                    document.getElementById('exam_timer').innerHTML = hours + ':' + mins + ':' + secs;
                }
                if (hours == '00' && mins == '00' && secs == '00') {
                    document.getElementById('slider-next').disabled = true
                    document.getElementById(forForm).submit();
                }
                setTimeout("countdownto()", 1000);
            }
            countdownto();
        </script> <?php
    }

    private function _get_time_limit() {
        $values = exam_spider_get_exam(arg(1));
        $timer = time() + intval($values['exam_duration'] * 60);
        return date('r', $timer);
    }
}
