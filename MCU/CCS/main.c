#include <msp430.h>
// continue send 01 data at different date rate and measure the power consumption
void backscatter_init()
{
  //P1.0 as backscatter control output
  P1DIR |= BIT0; // output
  P1OUT &= ~BIT0; // low
}

void TimerInit()
{
    BCSCTL1 = CALBC1_1MHZ; // Basic Clock System Control 1
    DCOCTL = CALDCO_1MHZ; // DCO Clock Frequency Control
    BCSCTL2 |=SELM_1+DIVM_0; // Basic Clock System Control 2, MCLK Source Select 1: DCOCLK, MCLK Divider 1: /2
    // timer 0 controlling the data rate
    // 500Hz:125, 1k:63, 2k:32, 3k:21, 4k:16, 5k:10, 6k: 9, 7k: 8, 8k: 7, 10k: 6
    TACCR0 = 6; // 50 ms
    TACTL |= TASSEL_2 + ID_3 + MC_1; // SMCLK, ID_0: 1 divide, Up to CCR0
    //TACCR0 = 10e3; // 500Hz: 10e3, 1k:5e3, 2k: 2.5e3
    //TACTL |= TASSEL_2 + ID_0 + MC_1; // SMCLK, ID_0: 1 divide, Up to CCR0
    TACCTL0 |= CCIE;
}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    P1OUT^=(1<<0);
}

int main()
 {
   // Write your code here
   WDTCTL = WDTPW | WDTHOLD;    // stop watchdog timer
   backscatter_init();
   TimerInit();
   _BIS_SR(GIE);
   // LPM3; //enter the low-power mode 2
    return 0;
 }
