# ProiectSDTR TUREAC BETINA
Sistem de control si monitorizare al unui ceas multifunctional

Ceasul multifunctional este conceput pe o placa FPGA-Nexys3, cu procesor MicroBlaze, la care se conecteaza la un ecran VMOD TFT, cu touchscreen si un senzor de temperatura MCP9808. FPGA-ul va fi conectat printr-o interfata seriala la un Rasberry Pi 3, cu sistem de operare Linux cu 2 aplicatii, una pentru managementul comunicatiei cu FPGA-ul si cea de-a 2-a, pentru comunicatia cu mediul exterior Rasberry Pi. Ambele aplicatii includ sistem de testare de tip BIST. 
1.Fisierul FPGAsystem.c reprezinta elementul software, incarcat pe microprocesorul FPGA-ului si actioneaza ca o interfata intre perifericele de temperatura si ceas, si comunicarea UART, cu exteriorul.
2.Fisierul UART.c reprezinta mediul de verificare al comunicatiei pe UART, intre Rasberry Pi si FPGA. In acesta se transmite comanda de citire a temperaturii(caracterul k) si se asteapta raspuns.
