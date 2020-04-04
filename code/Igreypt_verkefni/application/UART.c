//UART test 1 - Polling

#include  <msp430.h>
#include "UART.h"



void UART(void)
{
  WDTCTL = WDTPW + WDTHOLD;         // Stop Watch dog timer

  // Disable the GPIO power-on default high-impedance mode to activate previously configured port settings
  PM5CTL0 &= ~LOCKLPM5;

  // Clock setup
  CSCTL0_H = CSKEY >> 8;                                                                      // Unlock CS registers
  CSCTL1 = DCOFSEL_3 | DCORSEL;                                                       //Set DCO to 8MHz
  CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;     // Set ACLK = VLOCLK; MCLK = SMCLK = DCO
  CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;                                              // Set all dividers to 1

  // Lock CS registers
  CSCTL0_H = 0;

  P5DIR &=~BUTTON;                          // Ensure button is input

  P1OUT &= ~BIT0;                               // Clear P1.0 output latch
  P1DIR |= BIT0;                                    // For LED

  P5OUT |=  BUTTON;                         // Enables pullup resistor on button
  P5REN |=  BUTTON;

  P6SEL1 &= ~(BIT0 | BIT1);
  P6SEL0 |= (BIT0 | BIT1);                     // USCI_A3 UART operation

  UCA3CTLW0 = UCSWRST;                             // Put eUSCI in reset
  UCA3CTLW0 |= UCSSEL__SMCLK;               // CLK = SMCLK
  // Baud Rate calculation
  // 8000000/(16*9600) = 52.083
  // Fractional portion = 0.083
  // User's Guide Table 21-4: UCBRSx = 0x04
  // UCBRFx = int ( (52.083-52)*16) = 1
  UCA3BRW = 52;                                                           // 8000000/16/9600
  UCA3MCTLW |= UCOS16 | UCBRF_1 | 0x0400;       //
  UCA3CTLW0 &= ~UCSWRST;                                  // Initialize eUSCI



}

void UART_TX(char * tx_data)                               // Define a function which accepts a character pointer to an array
{
    unsigned int i=0;
    while(tx_data[i])                                                   // Increment through array, look for null pointer (0) at end of string
    { 
        while ((UCA3STATW & UCBUSY));                 // Wait if line TX/RX module is busy with data
        UCA3TXBUF = tx_data[i];                                // Send out element i of tx_data array on UART bus
        i++;                                                                   // Increment variable for array address
    }
}
// Convert Integer to String
void i2s(int i,char *s)
{
char sign;short len;char *p;
sign='+';len=0;
if(i<0){sign='-';i=-i;}
do{*s=(i%10)+'0';s++;len++;i/=10;}while(i!=0);
if(sign=='-'){*s='-';s++;len++;}
for(i=0;i<len/2;i++){p[len]=p[i];p[i]=p[len-1-i];p[len-1-i]=p[len];}
p[len]=0;
}
// Convert String to Integer
