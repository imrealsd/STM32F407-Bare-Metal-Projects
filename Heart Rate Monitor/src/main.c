#include "sys_clock.h"
#include "delay.h"
#include "ADC1.h"
#include "millis.h"
#include "lcd.h"
#include "onboard_button.h"
#include <stdlib.h>
#define SAMPLE_SIZE 20
#define ARR_SIZE 10
#define RATE_SAMPLE 300

int32_t arr[ARR_SIZE] = {0};
int32_t sample_val = 0, heart_rate = 0, pulse = 0;

int32_t get_arr_avg();
void include_in_array (int32_t);
int32_t get_20ms_sample_avg();
void send_to_lcd (int16_t);
int32_t get_heart_rate (int32_t);
void reset (void);
void initialize_lcd_with_msg (void);

int main()
{   
    setup_system_clk();
    setup_onboard_button();
    setup_adc1_single_channel();
    initialize_lcd_with_msg();

    while (1){

        /* Taking RATE_SAMPLE no. of heart rate samples and takig 
         * their mean value to avoid outliers
        */

        for (int16_t i = 0; i < RATE_SAMPLE; i++){
            
            /* press and hold button to reset values */
            if (button_is_pressed()){i = 0;reset();}  

            /* sample calculation code */
            sample_val = get_20ms_sample_avg();    
            include_in_array(sample_val);
            sample_val = get_arr_avg();
            heart_rate += get_heart_rate(sample_val);
        }
        heart_rate = heart_rate / RATE_SAMPLE;
        send_to_lcd(heart_rate);
        heart_rate = 0;
    }
}

void initialize_lcd_with_msg (void){

    start_lcd();
    set_cursor(0,0);
    lcd_string("  Welcome To");
    set_cursor(1,0);
    lcd_string("Heart Rate Meter");
    delay_ms(2000);
    clear_lcd();
    set_cursor(0,0);
    lcd_string("Rate:000");
    set_cursor(0,9);
    lcd_string("Per Min");
}

void reset (void)
{   
    set_cursor(1,0);
    lcd_string("Reseting . . .");
    delay_ms(700);
    set_cursor(1,0);
    lcd_string("               ");
    set_cursor(0,5);
    lcd_string("000");

    for (int8_t i = 0; i < ARR_SIZE; i++){
        arr[i] = 0;
    }
    sample_val = 0;
    heart_rate = 0;
    pulse = 0;
}

int32_t get_heart_rate (int32_t val)
{   
    static int32_t current = 0, prev = 0, falling_flag = 0;
    static int16_t period = 0, heart_rate = 0;
    float temp;
    current = val; 

    /* we will take the time period b/w two falling edges
     * that is the period of heart pulse in mili sec, then we will
     * calculate pulse per min from the period 
    */

    if ((current < prev) && falling_flag == 0){  

        pulse++;
        falling_flag = 1;

        if (pulse == 1){            // 1st falling edge
            start_millis();

        } else if (pulse == 2){     // 2nd falling edge

            period = millis();
            stop_millis();
            pulse = 0;
        }
    }
    if (current > prev){
        falling_flag = 0;
    }
    prev = current;
    temp = (float)60 * ((float)1000 / (float)period);
    heart_rate = (int16_t)temp;
    return heart_rate;
}

void send_to_lcd (int16_t data)
{   
    /* sending to lcd */
    int8_t d1 = (data % 10);
    int8_t d2 = (data /10) %10;
    int8_t d3 = (data /100) % 10;

    set_cursor(0,5);
    lcd_data(d3+48);
    lcd_data(d2+48);
    lcd_data(d1+48);
}


int32_t get_arr_avg (void)
{
    int32_t sum = 0;
    for (int8_t i = 0; i < ARR_SIZE; i++){
        sum += arr[i];
    }
    return sum/ARR_SIZE;
}

int32_t get_20ms_sample_avg (void)
{
    /* We will take samples for 20ms and take avg
     * to nullify the 50Hz electric light freq [noise]
    */
    int32_t val = 0;

    for (int8_t i = 0; i < SAMPLE_SIZE; i++){             
        val += read_adc1();
        delay_ms(1);
    }
    val = val / 20;  
    return val;
}

void include_in_array (int32_t sample_val)
{
    /* To make the curve smoother we are taking 10 values
     * in an array and make average
    */
    static int8_t ptr = 0;                                 
    arr[ptr] = sample_val;
    ptr++;
    if (ptr == ARR_SIZE){
        ptr = 0;
    }
}
