#include <stdio.h>
#include <unistd.h>			//Used for UART
#include <fcntl.h>			//Used for UART
#include <termios.h>		//Used for UART

int uart0_filestream = -1;

int main(void){
	uart0_filestream = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);		//Open in non blocking read/write mode
	if (uart0_filestream == -1)
	{
		//ERROR - CAN'T OPEN SERIAL PORT
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}
	else{
		printf("USB 0 opened succsesfully!\n");
	}
	
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		//<Set baud rate
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);
	
	unsigned char tx_buffer[256];
	tx_buffer[0] = 'k';
	if (uart0_filestream != -1){
		write(uart0_filestream, tx_buffer, 1);		
	}
		
		
	if (uart0_filestream != -1)
	{
	printf("Reading\n");
		// Read up to 255 characters from the port if they are there
		unsigned char rx_buffer[256];
		int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		//Filestream, buffer to store in, number of bytes to read (max)
		if (rx_length < 0)
		{
			printf("Error1");//An error occured (will occur if there are no bytes)
		}
		else if (rx_length == 0)
		{
			//No data waitingplevnei
						printf("Error2");
		}
		else
		{
			//Bytes receivedemaew
			rx_buffer[rx_length] = '\0';
			printf("%i bytes read : %u\n", rx_length, rx_buffer[0]);
		}
		
	}
		
	close(uart0_filestream);
	return 0;
		
	}
	
	



