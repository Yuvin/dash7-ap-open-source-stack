/*!
 *
 * \copyright (C) Copyright 2013 University of Antwerp (http://www.cosys-lab.be) and others.\n
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.\n
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * Contributors:
 * 		maarten.weyn@uantwerpen.be
 *     	glenn.ergeerts@uantwerpen.be
 *
 */

#include "../uart.h"
#include "platforms/platform.h"
#include "../system.h"

#include <msp430.h>
#include "driverlib/5xx_6xx/gpio.h"
#include "driverlib/5xx_6xx/uart.h"

void uart_init()
{
#ifdef PLATFORM_WIZZIMOTE // TODO ugly solution for now, UART needs to be extracted from oss-7 and implemented by platform specific HAL library
    PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
    P2MAP0 = PM_UCA0RXD;                      // Map UCA0RXD output to P2.6
    P2MAP1 = PM_UCA0TXD;                      // Map UCA0TXD output to P2.7
    PMAPPWD = 0;                              // Lock port mapping registers
	P2DIR |= BIT1;                            // Set P2.7 as TX output
    P2SEL |= BIT0 + BIT1;
#elif defined PLATFORM_ARTESIS
    PMAPPWD = 0x02D52;                        // Get write-access to port mapping regs
    P1MAP6 = PM_UCA0RXD;                      // Map UCA0RXD output to P2.6
    P1MAP5 = PM_UCA0TXD;                      // Map UCA0TXD output to P2.7
    PMAPPWD = 0;                              // Lock port mapping registers
    P1DIR |= BIT5;                            // Set P1.6 as TX output
    P1SEL |= BIT5 + BIT6;                     // Select P1.5 & P1.6 to UART function
#endif

    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
//    UCA0CTL1 |= UCSSEL_1;                     // ACLK
//    double c_speed = 32768.0;
//
//    double n = c_speed / BAUDRATE;
//    uint8_t ucos16 = (n >= 16);
//
//    if (ucos16)
//    {
//    	n = n / 16.0;
//    	UCA0MCTL |= UCOS16;
//
//    	//UCA0BR1 = (uint8_t) (c_speed / (BAUDRATE*256.0));
//    	//UCA0BR0 = (uint8_t) ((c_speed / BAUDRATE) - (256*UCA0BR1));
//    	UCA0BRW = (uint16_t) n;
//
//    	uint8_t ucbrf = (uint8_t) ((n - UCA0BRW) * 16 + 0.5);
//
//    	UCA0MCTL |= ucbrf << 4;
//    } else {
//    	//UCA0BR1 = (uint8_t) (c_speed / (BAUDRATE*256.0));
//    	//UCA0BR0 = (uint8_t) ((c_speed / BAUDRATE) - (256*UCA0BR1));
//    	UCA0BRW = (uint16_t) n;
//
//    	uint8_t ucbrs = (uint8_t) ((n - UCA0BRW) * 8 + 0.5);
//
//    	UCA0MCTL |= ucbrs << 1;
//    }
//
////    UCA0BRW = 0x01b4;
////    UCA0MCTL |= 0x07 << 1;
////

//    UCA0CTL1 |= UCSWRST;                      // **Put state machine in reset**
//    UCA0CTL1 |= UCSSEL_1;                     // CLK = ACLK
//    UCA0BR0 = 0x0D;                           // 2400 (see User's Guide)
//    UCA0BR1 = 0x00;                           //
//    UCA0MCTL |= UCBRS_6+UCBRF_0;              // Modulation UCBRSx=6, UCBRFx=0
//    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**



    UCA0CTL1 |= UCSSEL_2;
    double n = 1.0 * clock_speed / BAUDRATE;
    UCA0BRW = (uint16_t) n;
    uint8_t ucbrs = (uint8_t) ((n - UCA0BRW) * 8 + 0.5);
    UCA0MCTL |= ucbrs << 1;

    UCA0CTL1 &= ~UCSWRST;                     // **Initialize USCI state machine**

}

void uart_enable_interrupt()
{    
    UCA0IE |= UCRXIE;                         // Enable USCI_A0 RX interrupt
}

#pragma NO_HOOKS(uart_transmit_data)
void uart_transmit_data(unsigned char data)
{
	while(!uart_tx_ready());
    UCA0TXBUF = data;
}

#pragma NO_HOOKS(uart_transmit_message)
void uart_transmit_message(unsigned char *data, unsigned char length)
{
    unsigned char i=0;
    for (; i<length; i++)
    {
        uart_transmit_data(data[i]);
    }

}

#pragma NO_HOOKS(uart_tx_ready)
unsigned char uart_tx_ready()
{
    return UCA0IFG&UCTXIFG;
}

unsigned char uart_receive_data()
{
    return UCA0RXBUF;
}
