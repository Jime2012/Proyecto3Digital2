//**************************************************************************************************************
/**
 * main.c
 * Código ejemplo de uso de Interrupciones y de timer
 */
//**************************************************************************************************************
// Librerias
//**************************************************************************************************************
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "driverlib/adc.h"
#include "utils/uartstdio.h"
//**************************************************************************************************************
// Variables Globales
//**************************************************************************************************************
uint32_t ui32Period;
uint8_t ds1;
uint8_t charIn = 'a';
uint8_t newChar;
uint8_t modo;
uint8_t cont = 0;
uint8_t valanterior;
uint8_t val_temporal;
uint8_t valor1;
uint8_t valor;
uint8_t led1 = 0;
uint8_t led2 = 0;
uint8_t led3 = 0;
uint8_t led4 = 0;
uint8_t flag = 0;
uint8_t flag1 = 0;
uint8_t flag2 = 0;
uint8_t flag3 = 0;
uint8_t flag4 = 0;
uint8_t count_down1 = 2;
uint8_t numero = 0;
uint32_t operacion;
uint8_t decenas;
uint8_t unidades;
uint8_t count_down2 = 2;
uint8_t count_down3 = 3;
uint8_t numero1 = 0;
uint8_t numero2 = 0;
uint8_t numero3 = 0;
void InitUART(void);
void UART0ReadIntHandler(void);


//**************************************************************************************************************
// Código Principal
//**************************************************************************************************************



int main(void)
                                                       {

    uint32_t pui32ADC0Value[1];
    // Configuración del oscilador externo, usando PLL y teniendo una frecuencia de 40MHz
        SysCtlClockSet( SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_XTAL_16MHZ
                        | SYSCTL_OSC_MAIN);

        valanterior = cont;
        // Se habilita el reloj para el puerto F
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
        SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
        // Se habilita el reloj para el temporizador
        SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
        //Configuracion del ADC0
        SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

        // Se establecen como salidas el puerto F

         GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0 |GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3|GPIO_PIN_4);
         GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2| GPIO_PIN_3|GPIO_PIN_6|GPIO_PIN_7);
         GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_1 | GPIO_PIN_2);
         GPIOPinTypeGPIOInput(GPIO_PORTA_BASE, GPIO_PIN_5);
         GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_7);
         GPIOPinTypeGPIOInput(GPIO_PORTE_BASE, GPIO_PIN_2);
         GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_7);
         GPIOPinTypeADC(GPIO_PORTE_BASE, GPIO_PIN_3);

//Configuracion del ADC
         ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

         ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH0 | ADC_CTL_IE |
                                  ADC_CTL_END);

         // Since sample sequence 3 is now configured, it must be enabled.
         ADCSequenceEnable(ADC0_BASE, 3);

         // Clear the interrupt status flag.  This is done to make sure the
         // interrupt flag is cleared before we sample.
         ADCIntClear(ADC0_BASE, 3);

        // Configuración del Timer 0 como temporizador perodico
        TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

        // Se calcula el período para el temporizador (1 seg)
        ui32Period = (SysCtlClockGet()) / 2;
        // Establecer el periodo del temporizador
        TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);
        InitUART();
        // Se habilita la interrupción por el TIMER0A
        IntEnable(INT_TIMER0A);
        // Se establece que exista la interrupción por Timeout
        TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
        // Se habilitan las interrupciones Globales
        IntMasterEnable();
        // Se habilita el Timer
        TimerEnable(TIMER0_BASE, TIMER_A);


        // Habilitar interrupciones
        IntEnable(INT_UART0);
        // Habilitar el FIFO en 1/8 datos recibidos
        UARTFIFOLevelSet(UART0_BASE, UART_FIFO_TX1_8, UART_FIFO_RX1_8);
        //UARTFIFODisable(UART0_BASE);
        // Habilitar interrupción de recepción de UART0
        UARTIntEnable(UART0_BASE, UART_INT_RX);
        // Habilitar interrupciones globales
        IntMasterEnable();

    //**********************************************************************************************************
    // Loop Principal
    //**********************************************************************************************************
    while (1)
        {

        uint32_t pui32ADC0Value[1];
        //Configuracion de los modos con los dip switches
        if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_2)){

            if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1)){
                flag1 = 0;
                flag2 = 0;
                flag3 = 0;
                if(flag4 == 0){
                    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                    GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
                    flag4 = 1;
                }
                                  modo = 4;}
           else{
               flag1 = 0;
               flag2 = 0;
               flag4 = 0;
               if(flag3 == 0){
                   GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                   GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                   GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                   GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                   GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                   GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                   GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                   GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
                   flag3 = 1;
               }
                               modo = 3; }

                }
         else
                {
             if (GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_1)){
                 flag1 = 0;
                 flag4 = 0;
                 flag3 = 0;
                 if(flag2 == 0){
                     numero = 0;
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
                     flag2 = 1;
                 }
                                               modo = 2;}
             else {
                 flag3 = 0;
                 flag2 = 0;
                 flag4 = 0;
                 if(flag1 == 0){
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, 0);
                     flag1 = 1;
                 }
                                            modo = 1;}


                            }

      switch(modo){

          case 1:
              //Mostrar lo estados de los leds
                            if (GPIOPinRead(GPIO_PORTA_BASE,GPIO_PIN_5)){
                                while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5));
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '1');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led1 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '2');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led2 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '3');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led3 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '4');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led4 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                }
                            else if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)){
                                while(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2));
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '1');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led1 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '2');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led2 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '3');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led3 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                UARTCharPut(UART0_BASE, 'L');
                                UARTCharPut(UART0_BASE, 'E');
                                UARTCharPut(UART0_BASE, 'D');
                                UARTCharPut(UART0_BASE, '4');
                                UARTCharPut(UART0_BASE, ':');
                                UARTCharPut(UART0_BASE, led4 + 48);
                                UARTCharPut(UART0_BASE, '\n');
                                }
            break;
          case 2:
              //Contador de los pushbuttons
              val_temporal = valanterior;
                            if (GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5)){
                                while(GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_5)){
                                if (val_temporal==cont){
                                    if(cont<15){
                                        cont++;}
                                    else{
                                        cont = 0;
                                    }
                                }
                                else{
                                    cont = cont;
                                }
                                }
                                valanterior = cont;
                                            }
                            else if (GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)){
                                while(GPIOPinRead(GPIO_PORTE_BASE, GPIO_PIN_2)){
                                if (val_temporal==cont){
                                   if (cont>0) {
                                    cont--;}
                                   else{
                                     cont = 15;
                                   }
                                }
                                else{
                                     cont = cont;
                                }
                                }
                                valanterior = cont;
                                            }
                            GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, cont);
            break;
          case 3:
              //Animaciones
              switch(numero1){
                                     case 0:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 1:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 2:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                          break;
                                     case 3:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                                         break;
                                     case 4:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 5:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                                         break;
                                     default:
                                         break;
                                     }
              switch(numero2){
                                     case 0:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 1:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 2:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                          break;
                                     case 3:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                                         break;
                                     case 4:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 5:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                                         break;
                                     default:
                                         break;
                                     }
              switch(numero3){
                                     case 0:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 1:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 2:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                          break;
                                     case 3:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                                         break;
                                     case 4:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                                         break;
                                     case 5:
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                                         break;
                                     default:
                                         break;
                                     }
              break;
          case 4:
              // Trigger the ADC conversion.
                     ADCProcessorTrigger(ADC0_BASE, 3);

                     // Wait for conversion to be completed.
                     while(!ADCIntStatus(ADC0_BASE, 3, false))
                     {
                     }

                     // Clear the ADC interrupt flag.
                     ADCIntClear(ADC0_BASE, 3);

                     // Read ADC Value.
                     ADCSequenceDataGet(ADC0_BASE, 3, pui32ADC0Value);


                     valor = (uint8_t)((pui32ADC0Value[0]*99)/4095);
                     decenas = valor/10;
                     unidades = valor%10;
                     if (valor<10){
                         unidades = 0;
                     }

                     //Valores del ADC
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3, decenas);
                     switch(unidades){
                     case 0:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     case 1:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                         break;
                     case 2:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                          break;
                     case 3:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                         break;
                     case 4:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     case 5:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     case 6:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     case 7:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                         break;
                     case 8:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     case 9:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     default:
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                         GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                         GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                         GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                         break;
                     }
              break;
          default:
              break;
        }



                }
}

//**************************************************************************************************************
// Handler de la interrupción del TIMER 0 - Recordar modificar el archivo tm4c123ght6pm_startup_css.c
//**************************************************************************************************************
void Timer0IntHandler(void)
{
    // Clear the timer interrupt
    TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    // Read the current state of the GPIO pin and
    // write back the opposite state

    switch(modo) {
             case 1:

               break;
            //Conteo del display con el Timer
             case 2:
                 if (count_down1 > 0){
                     count_down1--;
                 }
                 else{
                     count_down1 = 2;
                     if (numero < 15){
                         numero++;

                     }
                     else{
                         numero = 0;

                     }
                     switch(numero){
                                            case 0:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 1:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 2:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                 break;
                                            case 3:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 4:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 5:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 6:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 7:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 8:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 9:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, numero + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 10:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, 'A');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 11:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, 'B');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 12:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, 'C');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 13:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 14:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, 'E');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 15:
                                                UARTCharPut(UART0_BASE, 'D');
                                                UARTCharPut(UART0_BASE, 'i');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, 'p');
                                                UARTCharPut(UART0_BASE, 'l');
                                                UARTCharPut(UART0_BASE, 'a');
                                                UARTCharPut(UART0_BASE, 'y');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, 'F');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            default:
                                                break;
                                            }

                                          switch(cont){
                                            case 0:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 1:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 2:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                 break;
                                            case 3:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 4:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 5:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 6:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 7:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 8:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 9:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, cont + 48);
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 10:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '0');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 11:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 12:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '2');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 13:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '3');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 14:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '4');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            case 15:
                                                UARTCharPut(UART0_BASE, 'L');
                                                UARTCharPut(UART0_BASE, 'e');
                                                UARTCharPut(UART0_BASE, 'd');
                                                UARTCharPut(UART0_BASE, 's');
                                                UARTCharPut(UART0_BASE, ':');
                                                UARTCharPut(UART0_BASE, '1');
                                                UARTCharPut(UART0_BASE, '5');
                                                UARTCharPut(UART0_BASE, '\n');
                                                break;
                                            default:
                                                break;
                                            }
                 }
                 switch(numero){
                 case 0:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 1:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 2:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                      break;
                 case 3:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 4:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 5:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 6:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 7:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 8:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 9:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 10:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 11:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 12:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 13:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 14:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 15:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 default:
                     break;
                 }

               break;

             //Timers de las animaciones
             case 3:
             if (flag == 0){
                 if (numero1 < 5){
                     numero1++;
                         }
                 else{
                      flag = 1;
                      numero1 = 0; }
                         }
             else if(flag == 1){
                 if (count_down2 > 0){
                         count_down2--;
                                  }
                 else{
                       count_down2 = 2;
                       if (numero2 < 5){
                           numero2++;
                               }
                       else{
                           flag = 2;
                            numero2 = 0; }
                 }
             }
             else if(flag == 2){
                 if (count_down3 > 0){
                         count_down3--;
                                  }
                 else{
                       count_down3 = 3;
                       if (numero3 < 5){
                           numero3++;
                               }
                       else{
                           flag = 0;
                            numero3 = 0; }
                 }
             }
                 break;
             case 4:

                 break;
             default:
                 break;
           }
}

void InitUART(void)
{
    /*Enable the GPIO Port A*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    /*Enable the peripheral UART Module 0*/
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    /* Make the UART pins be peripheral controlled. */
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    /* Sets the configuration of a UART. */
    UARTConfigSetExpClk(
            UART0_BASE, SysCtlClockGet(), 115200,
            (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

    //UARTIntEnable(UART0_BASE, UART_INT_RX | UART_INT_RT);
}

void UART0ReadIntHandler(void)
{
    UARTIntClear(UART0_BASE, UART_INT_RX);

    valor = UARTCharGet(UART0_BASE);
if(valor){
    charIn = UARTCharGet(UART0_BASE);}

    switch(modo) {
    //Coleccion de datos del UART
             case 1:
                 switch(charIn){
                 case 48:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 49:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 50:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                      break;
                 case 51:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 52:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 53:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 54:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 55:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 56:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 57:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 97:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 98:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 99:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 100:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 0);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 64);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 0);
                     break;
                 case 101:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 4);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 102:
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_4, 16);
                     GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_7, 128);
                     GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_6, 0);
                     GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_2, 0);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_3, 8);
                     GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_7, 128);
                     break;
                 case 103:
                     if (led1 == 0){

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 1);
                         led1 = 1;
                     }
                     else {

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0, 0);
                         led1 = 0;
                     }
                     break;
                 case 104:
                     if (led2 == 0){

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 2);
                         led2 = 1;
                     }
                     else {

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
                         led2 = 0;
                     }
                     break;
                 case 106:
                     if (led3 == 0){

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 4);
                         led3 = 1;
                     }
                     else {

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2, 0);
                         led3 = 0;
                     }
                     break;
                 case 107:
                     if (led4 == 0){

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 8);
                         led4 = 1;
                     }
                     else {

                         GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3, 0);
                         led4 = 0;
                     }
                     break;
                 default:
                     break;
                 }

               break;
             case 2:

               break;
             case 3:

                 break;
             case 4:


                 break;
             default:
                 break;
           }
}
