# UNO_as_UPDI_programmer
In newer ATtiny devices the MOSI/MISO programming interface has been replace with a UPDI (Universal, programming and debug interface).  
This is a 1 wire interface that uses a UART connected to the reset pin. 
Following a reset the UPDI is gived access to the reset pin for a short interval of time.
If a programmer is connected it can take control of the reset pin and program the device, releasing the pin when programming is complete.

Programs have already been written for the UNO that enable it it program AVR devices.
The intention is to replace the MOSI/MISO hardware routines with UPDI routines.

The first stage will involve simply reading out the signature bytes of ATtiny 806/1606 devices.
