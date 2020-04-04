/*
 * UART.h
 *
 *  Created on: Mar 16, 2020
 *      Author: Hjalti
 */

#ifndef APPLICATION_UART_H_
#define APPLICATION_UART_H_

#define RXD        BIT1
#define TXD        BIT0
#define BUTTON    BIT5

extern void UART_TX(char * tx_data);            // Function Prototype for TX
extern void i2s(int i,char *s);
extern void UART(void);


#endif /* APPLICATION_UART_H_ */
