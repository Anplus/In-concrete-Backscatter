#include <msp430.h>
#include "mpu6050.h"

int Temp;         //温度数据,单位摄氏度,该温度为芯片温度非环境温度

const int second_counter_max = 50;       //该值为n,则n ms采样转换一次.     修改此值即可修改采样率
int second_counter = 0;
const int numbit = 16;
int state = 0;
int already_sent = 0;
const int len = 50;
void send_data(){
    int data = Temp;
    int i = numbit-1;
    for(;i>=0;i--){
        if(state == 0){
            P1OUT |= BIT0; // swtich to high
            state = 1;
            __delay_cycles(50);
            if((data >> i) & 1U){
            // last 1 , --
                state = 1;
                //P1OUT |= BIT0;
            }else{
            // last 0, -_
                state = 0;
                P1OUT &= ~BIT0; // low
            }
        }else{
            P1OUT &= ~BIT0; // swtich to low
            state = 0;
            __delay_cycles(50);
            if((data >> i) & 1U){
            // last 1 , __
                state = 0;
                // P1OUT &= ~BIT0;
            }else{
            // last 0, _-
                state = 1;
                P1OUT |= BIT0; // high
            }
        }
        __delay_cycles(50);
    }
    state = 0;  // clear state
    P1OUT &= ~BIT0; // clear pin
}

// 50ms interrupt
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    // 50ms
    second_counter += 1;
    if(second_counter >= second_counter_max){
        second_counter = 0;
        Temp = MPU6050_GetData(TEMP_OUT_H);        //读取温度
        // start send data
        send_data();
    }
}

void TimerInit()
{
    BCSCTL1 = CALBC1_1MHZ; // Basic Clock System Control 1
    DCOCTL = CALDCO_1MHZ; // DCO Clock Frequency Control
    BCSCTL2 |=SELM_1+DIVM_1; // Basic Clock System Control 2, MCLK Source Select 1: DCOCLK, MCLK Divider 1: /2
    // timer 0 controlling the packet gap
    TACCR0 =  125; // 1 ms
    TACTL |= TASSEL_2 + ID_3 + MC_1; // SMCLK, ID_0: 1 divide, Up to CCR0
    //TACCR0 = 10e3; // 500Hz: 10e3, 1k:5e3, 2k: 2.5e3
    //TACTL |= TASSEL_2 + ID_0 + MC_1; // SMCLK, ID_0: 1 divide, Up to CCR0
    TACCTL0 |= CCIE;
}

/**
 * main.c
 */
int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    MPU6050_Init();
    TimerInit();
    //ADC_init();
    P1DIR |= 0x01;   // Set P1.0 to output direction
    _BIS_SR(GIE);
}





















