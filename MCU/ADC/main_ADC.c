#include <MSP430.h>

const int second_counter_max = 1;
int second_counter = 0;
const int numbit = 16;
int state = 0;
int send = 100;
int already_sent = 0;
int preamble = 53248; //110100

void send_data(){
	int data = send;
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
	state = 0;	// clear state
	P1OUT &= ~BIT0; // clear pin
}

// 1s interrupt
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A(void)
{
    // 1s
	second_counter += 1;
    if(second_counter >= second_counter_max){
        second_counter = 0;
		ADC10CTL0 |= ENC + ADC10SC;                // Sampling and conversion start
        // start ADC collecting
		TACCTL0 &= ~CCIE; // stop timer
    }
}

// ADC10 interrupt ** routine
#pragma vector=ADC10_VECTOR
__interrupt void ADC10_ISR (void)
{
    send = preamble+ADC10MEM;
	send_data();
	TACCTL0 |= CCIE; // start timer
}

void ADC_init(){
    // Set Ref as internal 2.5V, ADC10ON, enable interrupt
    ADC10CTL0 = SREF_1 + ADC10SHT_0 + REFON + REF2_5V + ADC10ON + ADC10IE;
    ADC10CTL1 = INCH_1;
    ADC10AE0 |= 0x02;             // Set P1.1 as A1 input
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
}

/**
 * main.c
 */
int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	TimerInit();
	ADC_init();
	P1DIR |= 0x01;   // Set P1.0 to output direction
	return 0;
}