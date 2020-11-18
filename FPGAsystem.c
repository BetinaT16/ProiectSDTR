/*
 * Copyright (c) 2009-2012 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */
volatile unsigned *ceas = (unsigned *)0xCE400000;
volatile unsigned *serial = (unsigned *)0x84000000;

volatile unsigned *led = (unsigned *)0x81400000;

#include "xparameters.h"
#include "mb_interface.h"
#include "string.h"
#include "stdlib.h"
#include <stdio.h>
#include "platform.h"

void print(char *str);

unsigned read_uart_lock()
{
unsigned x;
while((serial[0x8/4] & 0x01) ==0);
x=serial[0x0/4];
return x;
}

void write_uart(char x)
{
serial[0x4/4]=x;
while((serial[0x8/4] & 0x04) ==0);
}

void write_uart_u(unsigned x)
{
serial[0x4/4]=x;
while((serial[2] & 0x04) ==0);
}


int main()
{
    init_platform();
    unsigned temp = 0;
    char read;
    int var;


    for (var = 0; var < 9000000; var++) {}

    print("Hello World\n\r");
    unsigned a = 11;
    unsigned b = 00;
    unsigned c = 9;
    unsigned d = 00;
    unsigned e = 25;
    unsigned f = 10;
    unsigned g = 2020;

    ceas[0] = a;
    ceas[1] = b;
    ceas[2] = c;
    ceas[3] = d;
    ceas[4] = e;
    ceas[5] = f;
    ceas[6] = g;

    while (1){
    	read = read_uart_lock();
    	if (read == 'a') {
    		a = read_uart_lock();
    		ceas[0] = a;
    	}
    	if (read == 'z') {
    		a = ceas[0];
    		write_uart_u(a);
    	}
    	if (read == 's') {
    		b = read_uart_lock();
    		ceas[1] = b;
    	}
    	if (read == 'x') {
    		b = ceas[1];
    		write_uart_u(b);
    	}
    	if (read == 'd') {
    		c = read_uart_lock();
    		ceas[2] = c;
    	}
    	if (read == 'c') {
    		c = ceas[2];
    		write_uart_u(c);
    	}
    	if (read == 'f') {
    		d = read_uart_lock();
    		ceas[3] = d;
    	}
    	if (read == 'v') {
    		d = ceas[3];
    		write_uart_u(d);
    	}
    	if (read == 'g') {
    		e = read_uart_lock();
    		ceas[4] = e;
    	}
    	if (read == 'b') {
    		e = ceas[4];
    		write_uart_u(e);
    	}
    	if (read == 'h') {
    		f = read_uart_lock();
    		ceas[5] = f;
    	}
    	if (read == 'n') {
    		f = ceas[5];
    		write_uart_u(f);
    	}
    	if (read == 'j') {
    		g = read_uart_lock();
    		g = g*100;
    		g = g + read_uart_lock();
    		ceas[6] = g;
    	}
    	if (read == 'm') {
    		g = ceas[6];
    		g = g / 100;
    		write_uart_u(g);
    		read = ceas[6];
    		write_uart_u(read);
    	}
    	if (read == 'k') {
        	temp =  ceas[8];
        	write_uart_u(temp);
    	}
    	if (read == 'l') {
        	ceas[7] = 0x3;
        	print("ok\n\r");
    	}
    	if (read == 'p') {
    		ceas[7] = 0x1;
    		for (var = 0; var < 1000000; ++var) {

			}
    		ceas[7] = 0x0;
        	print("ok\n\r");
    	}
    }

    return 0;
}
