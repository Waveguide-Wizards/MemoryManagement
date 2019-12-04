/*
 * flash.c
 *
 *  Created on: Dec 1, 2019
 *      Author: hadji
 */


#include "flash.h"
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/ssi.h"
#include "driverlib/sysctl.h"


void FLASHInit(){
    //
    // The SSI0 peripheral must be enabled for use.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_SSI0);
    //
    // For this example SSI0 is used with PortA[5:2].  The actual port and pins
    // used may be different on your part, consult the data sheet for more
    // information.  GPIO port A needs to be enabled so these pins can be used.
    // TODO: change this to whichever GPIO port you are using.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    //
    // Configure the pin muxing for SSI0 functions on port A2, A3, A4, and A5.
    // This step is not necessary if your part does not support pin muxing.
    // TODO: change this to select the port/pin you are using.
    //
    GPIOPinConfigure(GPIO_PA2_SSI0CLK);
    //GPIOPinConfigure(GPIO_PA3_SSI0FSS);
    GPIOPinConfigure(GPIO_PA4_SSI0RX);
    GPIOPinConfigure(GPIO_PA5_SSI0TX);

    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_3);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x8);
    //
    // Configure the GPIO settings for the SSI pins.  This function also gives
    // control of these pins to the SSI hardware.  Consult the data sheet to
    // see which functions are allocated per pin.
    // The pins are assigned as follows:
    //      PA5 - SSI0Tx
    //      PA4 - SSI0Rx
    //      PA3 - SSI0Fss
    //      PA2 - SSI0CLK
    // TODO: change this to select the port/pin you are using.
    //
    //GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_3 |
    //               GPIO_PIN_2);

    GPIOPinTypeSSI(GPIO_PORTA_BASE, GPIO_PIN_5 | GPIO_PIN_4 | GPIO_PIN_2);

    //
    // Configure and enable the SSI port for SPI master mode.  Use SSI0,
    // system clock supply, idle clock level low and active low clock in
    // freescale SPI mode, master mode, 1MHz SSI frequency, and 8-bit data.
    // For SPI mode, you can set the polarity of the SSI clock when the SSI
    // unit is idle.  You can also configure what clock edge you want to
    // capture data on.  Please reference the datasheet for more information on
    // the different SPI modes.
    //
    SSIConfigSetExpClk(SSI0_BASE, SysCtlClockGet(), SSI_FRF_MOTO_MODE_0,
                       SSI_MODE_MASTER, 1000000, 8);
    //
    // Enable the SSI0 module.
    //
    SSIEnable(SSI0_BASE);

    //
    // Read any residual data from the SSI port.  This makes sure the receive
    // FIFOs are empty, so we don't read any unwanted junk.  This is done here
    // because the SPI SSI mode is full-duplex, which allows you to send and
    // receive at the same time.  The SSIDataGetNonBlocking function returns
    // "true" when data was returned, and "false" when no data was returned.
    // The "non-blocking" function checks if there is any data in the receive
    // FIFO and does not "hang" if there isn't.
    //
    /*
    while(SSIDataGetNonBlocking(SSI0_BASE, &pui32DataRx[0]))
    {
    }
    */
}

void FLASHSendCommand(uint32_t * data, uint32_t size){
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);
    volatile uint32_t index = 0;
    for(index = 0; index < size; index++)
    {
        //
        // Send the data using the "blocking" put function.  This function
        // will wait until there is room in the send FIFO before returning.
        // This allows you to assure that all the data you send makes it into
        // the send FIFO.
        //
        SSIDataPut(SSI0_BASE, data[index]);
        while(SSIBusy(SSI0_BASE))
        {
        }

    }

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x8);
    //
    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
    //
    while(SSIBusy(SSI0_BASE))
    {
    }
}

void FLASHSendCommandNoCS(uint32_t * data, uint32_t size){
    volatile uint32_t index = 0;
    for(index = 0; index < size; index++)
    {
        //
        // Send the data using the "blocking" put function.  This function
        // will wait until there is room in the send FIFO before returning.
        // This allows you to assure that all the data you send makes it into
        // the send FIFO.
        //
        SSIDataPut(SSI0_BASE, data[index]);
        while(SSIBusy(SSI0_BASE))
        {
        }

    }
    //
    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
    //
    while(SSIBusy(SSI0_BASE))
    {
    }
}

void FLASHClockOut(uint32_t size){
    volatile uint32_t index = 0;
    volatile uint32_t dummy = 0x00;
    for(index = 0; index < size; index++)
    {
        //
        // Send the data using the "blocking" put function.  This function
        // will wait until there is room in the send FIFO before returning.
        // This allows you to assure that all the data you send makes it into
        // the send FIFO.
        //
        SSIDataPut(SSI0_BASE, 0x00);
        while(SSIBusy(SSI0_BASE))
        {
        }

    }
    //
    // Wait until SSI0 is done transferring all the data in the transmit FIFO.
    //
    while(SSIBusy(SSI0_BASE))
    {
    }
}

void FLASHReadResponse(uint32_t * data, uint32_t size){
    volatile uint32_t index = 0;
    for(index = 0; index < size; index++)
    {
        //
        // Receive the data using the "blocking" Get function. This function
        // will wait until there is data in the receive FIFO before returning.
        //
        SSIDataGet(SSI0_BASE, &data[index]);

        //
        // Since we are using 8-bit data, mask off the MSB.
        //
        data[index] &= 0x00FF;
    }
}

void FLASHClockIn(uint32_t size){
    volatile uint32_t index = 0;
    uint32_t dummy[1];
    for(index = 0; index < size; index++)
    {
        //
        // Receive the data using the "blocking" Get function. This function
        // will wait until there is data in the receive FIFO before returning.
        //
        SSIDataGet(SSI0_BASE, &dummy[1]);
    }
}


void FLASHWriteEnable(){
    uint32_t dataTx[1];
    uint32_t dataRx[1];
    dataTx[0] = 0x06;
    FLASHSendCommand(dataTx,1);
    FLASHReadResponse(dataRx,1);
}

void FLASHWriteAddress(uint32_t * address, uint32_t * data, uint32_t data_width){
    uint32_t command[1];
    command[0] = 0x02;

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);
    FLASHSendCommandNoCS(command,1);
    FLASHSendCommandNoCS(address,3);
    FLASHSendCommandNoCS(data,data_width);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x8);

    FLASHClockIn(data_width+4);

}

void FLASHReadAddress(uint32_t * address, uint32_t * data, uint32_t data_width){
    uint32_t command[1];
    command[0] = 0x03;
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);
    FLASHSendCommandNoCS(command,1);
    FLASHSendCommandNoCS(address,3);
    FLASHClockOut(data_width);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x8);
    FLASHReadResponse(data,data_width+4);

}

void FLASHReadId(uint32_t * id){
    uint32_t dataTx[4] = {0x9f,0x00,0x00,0x00};
    FLASHSendCommand(dataTx,4);
    FLASHReadResponse(id,4);

}

void FLASHEraseSector(uint32_t * address){
    uint32_t command[1];
    command[0] = 0x20;

    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x0);
    FLASHSendCommandNoCS(command,1);
    FLASHSendCommandNoCS(address,3);
    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0x8);

    FLASHClockIn(4);
}
