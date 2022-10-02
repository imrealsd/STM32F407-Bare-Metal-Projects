#include "sys_clock.h"
#include "seven_segment.h"
#include "buzzer.h"
#include "delay.h"
#include "button.h"

int main()
{
    setup_system_clk();
    setup_seven_segment_gpio();
    setup_buttons();
    buzzer_off();

    while (1){
        if ((is_mode_pressed()| is_alarm_pressed() | is_hour_pressed() | is_min_pressed())){
            buzzer_on();
        } else {
            buzzer_off();
        }
        // buzzer_on();
        // delay_ms(500);
        // buzzer_off();
        // delay_ms(500);
    }
}

