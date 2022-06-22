#include <msp430.h>

const int NUM = 100;
int temp = 0;

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
	// timer 0 controlling the packet gap
    TACCR0 = 62500; // 50 ms
    TACTL |= TASSEL_2 + ID_3 + MC_1; // SMCLK, ID_0: 1 divide, Up to CCR0
    //TACCR0 = 10e3; // 500Hz: 10e3, 1k:5e3, 2k: 2.5e3
    //TACTL |= TASSEL_2 + ID_0 + MC_1; // SMCLK, ID_0: 1 divide, Up to CCR0
    TACCTL0 |= CCIE;
    // timer 1 controlling the data rate
	// 500Hz:125, 1k:63, 2k:32, 3k:21, 4k:16, 5k:10, 6k: 9, 7k: 8, 8k: 7, 10k: 6
    TACCR1 = 125; // time second
    //TACCTL1 |= CCIE;

}

#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
	// start sending packet
	TACCR1 = 125;
	TACCTL1 |= CCIE;
}

#pragma vector=TIMER0_A1_VECTOR
__interrupt void Timer_A1(void)
{
	switch(TAIV)
	{
      case 2:
        {
		    P1OUT^=(1<<0);
			TACCR1 += 125;
			if(temp <= NUM){
				temp++;

			}else{
				TACCTL1 &= ~CCIE;
				temp = 0;
			}
			break;
        }
      default:break;
	}
}

int main()
 {
   // Write your code here
   WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	backscatter_init();
	TimerInit();
	_BIS_SR(GIE);
	return 0;
 }