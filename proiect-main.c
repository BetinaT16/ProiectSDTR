 
   #include <stdlib.h>
   #include <stdio.h>
   #include <sys/mman.h>	
   #include <unistd.h>		
   #include <malloc.h>
   #include <sys/time.h>	
   #include <sys/resource.h>	
   #include <pthread.h>
   #include <limits.h>
   #include <unistd.h>		
   #include <fcntl.h>		
   #include <termios.h>		
   #include <wiringPi.h>
   
   #define MY_STACK_SIZE       (100*1024)      

   int uart0_filestream = -1;
   unsigned char tx_buffer[256];


typedef struct Queue
{
        int capacity;
        int size;
        int front;
        int rear;
        int *elements;
}Queue;

Queue * createQueue(int maxElements) 	//Functie de creare Queue
{
    
        Queue *Q;
        Q = (Queue *)malloc(sizeof(Queue));

        Q->elements = (int *)malloc(sizeof(int)*maxElements);
        Q->size = 0;
        Q->capacity = maxElements;
        Q->front = 0;
        Q->rear = -1;
        
        return Q;
}

unsigned Dequeue(Queue *Q) //	//Functie de sctergere din Queue
{
    
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                return 0;
        }
      
        else
        {
                Q->size--;
                Q->front++;
             
                if(Q->front==Q->capacity)
                {
                        Q->front=0;
                }
        }
        return front(Q);
}
int front(Queue *Q)	//Functie de citire din Queue
{
        if(Q->size==0)
        {
                printf("Queue is Empty\n");
                return 0;
        }
        /
        return Q->elements[Q->front];
}
void Enqueue(Queue *Q,int element)	//Functie de scriere in Queue
{
        
        if(Q->size == Q->capacity)
        {
                printf("Queue is Full\n");
        }
        else
        {
                Q->size++;
                Q->rear = Q->rear + 1;
                
                if(Q->rear == Q->capacity)
                {
                        Q->rear = 0;
                }
                
                Q->elements[Q->rear] = element;
        }
        return;
}

   static void setprio(int prio, int sched)
   {
   	struct sched_param param;

   	param.sched_priority = prio;
   	if (sched_setscheduler(0, sched, &param) < 0)
   		perror("sched_setscheduler");
   }
 
   static void send_for_temp(void) {	// Functie predefinita pentru cererea temperaturii
 	tx_buffer[0] = 'k';
	if (uart0_filestream != -1){
		write(uart0_filestream, tx_buffer, 1);		
	}
   }
   static void send_unsigned(unsigned int x) {	// Functie pentru transmiterea unei valori int (Folosit la transmiterea de valori de setare ceas/calendar/alarma)
 	tx_buffer[0] = (char) x;
	if (uart0_filestream != -1){
		write(uart0_filestream, tx_buffer, 1);		
	}
   }   
   static void send_char(char x) {	// Functie pentru transmiterea unui caracter (Folosit la initierea procesului de setare ceas/calendar/alarma sau procesul de cerere) 
 	tx_buffer[0] = x;
	if (uart0_filestream != -1){
		write(uart0_filestream, tx_buffer, 1);		
	}
   }      
   
   static unsigned receive_temp(void) {	// Functie predefinita pentru citirea temperaturii
	unsigned char rx_buffer[256];
	int rx_length = read(uart0_filestream, (void*)rx_buffer, 255);		
	if (rx_length > 0)
	{
		rx_buffer[rx_length] = '\0';
		return rx_buffer[0];
	}
  }
      
   static void error(int at)	
   {
   	/* Just exit on error */
   	fprintf(stderr, "Some error occured at %d", at);
   	exit(1);
   }
   
   
   // Thread 3 in mod normal se activeaza de fiecare data cand se citeste o valoare mai mare decat pragul setat.
   // Dupa 2.5 secunde, threadul se termina timp in care mai este executata o citire a temperaturii. 
   // In mod normal, nu ar trebui sa se vada nici o intrerupere a ledului atata timp cat se citeste o valoare mare a temperaturii
   static void *my_rt_thread3(void *args)	// Thread 3- Blink. Aprinde si stinge un led la un interval de 0.5 s de 5 ori
   {
   	struct timespec ts, tsr;
   	ts.tv_sec = 0;
   	ts.tv_nsec = 500000000L;
   
   	setprio(sched_get_priority_max(SCHED_RR), SCHED_RR); // Setarea prioritatii thread-ului Round Robin
  
   	for (int i = 0; i < 5; i ++)
  	{
  		printf ("blink\n") ;
    		digitalWrite (0, 1) ;       // Aprinde led
    		if(nanosleep(&ts , &tsr) < 0 )   
   			{
     			return NULL;
   			}
    		digitalWrite (0, 0) ;       // Stinge led
    		if(nanosleep(&ts , &tsr) < 0 )   
   			{
     			return NULL;
   			}
  	}
		
   	printf ("Thread 3 has been killed");
   	return NULL;
   }   
   
   // 
   static void *my_rt_thread1(void *args)  // Thread 1- Temp Read- Thread ce administreaza comunicarea cu FPGA-ul prin UART
   {
	Queue *Q = args;	// Preluarea pointerului catre Queue pentru transmiterea valorii temperaturii intre thread-urile 1 si 2
	printf("Starting thread 1 to read the temperature.\n");
   	struct timespec ts, tsr;
   	unsigned temp;
   	ts.tv_sec = 5;
   	ts.tv_nsec = 0;
   
   	setprio(sched_get_priority_max(SCHED_RR), SCHED_RR);// Setarea prioritatii thread-ului Round Robin
      
	// Incearca sa deschida comunicarea UART
   	uart0_filestream = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);		
	if (uart0_filestream == -1)
	{
		
		printf("Error - Unable to open UART.  Ensure it is not in use by another application\n");
	}
	else {
		printf("USB 0 opened succsesfully!\n");
	}
	
	// Setarea UART 9600
	struct termios options;
	tcgetattr(uart0_filestream, &options);
	options.c_cflag = B9600 | CS8 | CLOCAL | CREAD;		
	options.c_iflag = IGNPAR;
	options.c_oflag = 0;
	options.c_lflag = 0;
	tcflush(uart0_filestream, TCIFLUSH);
	tcsetattr(uart0_filestream, TCSANOW, &options);


//  Citirea informatiilor de timp de la RaspberryPI (Obtinute prin internet la pornire)

	char TimeString[128];
	struct timeval curTime;
	gettimeofday(&curTime, NULL);
	
	strftime(TimeString, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
	
	char temp_bist[3];
	strftime(temp_bist, 3, "%H", localtime(&curTime.tv_sec));
	unsigned int vOut = strtoul(temp_bist,NULL,10);
	
	// Conversia lor in valori utilizabile de catre FPGA si transmiterea pe URAT
	// Setare ora
	send_char('a');
	send_unsigned(vOut);
	
	strftime(temp_bist, 3, "%M", localtime(&curTime.tv_sec));
	vOut = strtoul(temp_bist,NULL,10);	
	
	// Setare minut
	send_char('s');
	send_unsigned(vOut);
	
	strftime(temp_bist, 3, "%d", localtime(&curTime.tv_sec));
	vOut = strtoul(temp_bist,NULL,10);	
	// Setare zi
	send_char('g');
	send_unsigned(vOut);

	strftime(temp_bist, 3, "%m", localtime(&curTime.tv_sec));
	vOut = strtoul(temp_bist,NULL,10);	
	// Setare luna
	send_char('h');
	send_unsigned(vOut);


	// Bucla infinita de citire a temperaturii odata la 5 secunde si transmiterea ei in Queue
	while (1){
	   send_for_temp();
   	   temp = receive_temp();
   	   printf("TEMP read : %u\n", temp);
   	   Enqueue(Q,temp);
   	   if(nanosleep(&ts , &tsr) < 0 )   
	   {
      		printf("Nano sleep system call failed \n");
     		return NULL;
           }
		printf("Nano sleep successfull on thread 1 \n");
	}
		
   	printf ("Thread 1 has been killed \n\n");
   	return NULL;
   }
   
   
   // Procesarea valorilor de temperatura, odata la 2 secunde
   static void *my_rt_thread2(void *args)
   {
        Queue *Q = args;// Preluarea pointerului catre Queue pentru transmiterea valorii temperaturii intre thread-urile 1 si 2
        unsigned temp;
   	struct timespec ts, tsr;
   	ts.tv_sec = 2;
   	ts.tv_nsec = 0;
   
   	setprio(sched_get_priority_max(SCHED_RR), SCHED_RR);// Setarea prioritatii thread-ului Round Robin
    // Bucla infinita de citire a temperaturii din queue 
   	while (1){
   		temp = front(Q);
   		if ( temp == 0){
   			if(nanosleep(&ts , &tsr) < 0 )   
   			{
      			printf("Nano sleep system call failed \n");
     			return NULL;
   			}
		  
		}
		else{
			printf("Temp received : %u\n", temp);  // Odata ce a fost primita o temperatura diferita de 0
			Dequeue(Q);								// Se elimina din queue si se compara cu un prag setat
			if (temp > 30){
			   	pthread_t thread3;					// Daca pragul este depasit, se porneste tread-ul 3 (Blink)
				pthread_attr_t attr;
			   	if (pthread_attr_init(&attr))
					error(1);
			   	if (pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + MY_STACK_SIZE))
			   		error(2);
			   	pthread_create(&thread3, &attr, my_rt_thread3, NULL);	
			}
		}
	}
		
   	printf ("Thread has been killed");
   	return NULL;
   }   
   /*************************************************************/

	// Functia de pornire a thread-urilor
   static void start_rt_thread(Queue *Q)
   {
   	pthread_t thread1;
   	pthread_t thread2;
   	pthread_t thread_bist3;   	
   	pthread_attr_t attr;
   
   	if (pthread_attr_init(&attr))
   		error(1);
   
   	if (pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + MY_STACK_SIZE))
   		error(2);
  
   	pthread_create(&thread1, &attr, my_rt_thread1, Q);		// Functia de UART cu BIST pentru verificarea conexiunii
   	pthread_create(&thread2, &attr, my_rt_thread2, Q);		// Functia de verificare a temperaturii (Nu contine BIST)
   	pthread_create(&thread_bist3, &attr, my_rt_thread3, Q);   // Functia BIST pentru verificarea aprinderii si stingerii ledului (Se executa doar o singura data)
   }
   
   static void configure_malloc_behavior(void)
   {
   	
   	if (mlockall(MCL_CURRENT | MCL_FUTURE))
   		perror("mlockall failed:");
   	mallopt(M_TRIM_THRESHOLD, -1);
   	mallopt(M_MMAP_MAX, 0);
   }
   
   
   
   int main(int argc, char *argv[])
   {
        if (wiringPiSetup () == -1) // Se verifica initializarea accesului la pinii RaspberryPI-ului
            return 1 ;

        pinMode (0, OUTPUT) ;    
        digitalWrite (0, 0) ;  // Se seteaza pinul de led LOW pentru evitarea starilor de pornire accidentala      
   	configure_malloc_behavior(); 
  	Queue *Q = createQueue(5);		// Se initializeaza coada de comunicare intre thread-uri (In mod normal nu ar trebui sa fie mai mult de o pozitie ocupata)
   	start_rt_thread(Q);			// Se pornesc thread-urile.
   	
	// Dupa acest punct, programul va functiona pe baza thread-urilor, functia main actionand doar pentru a oprii programul
	printf("Press <ENTER> to exit\n");
   	getchar();
   
   	return 0;
   }
   
   
   