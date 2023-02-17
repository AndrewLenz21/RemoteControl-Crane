#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "TivaES.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "UART.h"


#define CR 13 // Retorno de carro



void delayms(int tiempo);

void delayms(int tiempo)
{
    int m,n;
    for (m=0 ; m < tiempo ; m++)
        for(n=0; n<3180 ; n++)
        {}
}


void ConfiguraPinB (void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

    GPIO_PORTB_DIR_R |= 0xC0;
    GPIO_PORTB_AMSEL_R &= ~(0xC0);

    GPIO_PORTB_AFSEL_R &= ~(0xC0);

    GPIO_PORTB_DR8R_R |= (0xC0);
    GPIO_PORTB_DEN_R |= 0xC0;
    GPIO_PORTB_DATA_R &= ~(0xC0);
}

void ConfiguraPinC (void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;

    GPIO_PORTC_DIR_R |= 0x30;
    GPIO_PORTC_AMSEL_R &= ~(0x30);

    GPIO_PORTC_AFSEL_R &= ~(0x30);

    GPIO_PORTC_DR8R_R |= (0x30);
    GPIO_PORTC_DEN_R |= 0x30;
    GPIO_PORTC_DATA_R &= ~(0x30);
}

void ConfiguraPinD (void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;

    GPIO_PORTD_DIR_R |= 0x0F;
    GPIO_PORTD_AMSEL_R &= ~(0x0F);

    GPIO_PORTD_AFSEL_R &= ~(0x0F);

    GPIO_PORTD_DR8R_R |= (0x0F);
    GPIO_PORTD_DEN_R |= 0x0F;
    GPIO_PORTD_DATA_R &= ~(0x0F);
}

void ConfiguraPinE (void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;

    GPIO_PORTE_DIR_R |= 0x3C;
    GPIO_PORTE_AMSEL_R &= ~(0x3C);

    GPIO_PORTE_AFSEL_R &= ~(0x3C);

    GPIO_PORTE_DR8R_R |= (0x3C);
    GPIO_PORTE_DEN_R |= 0x3C;
    GPIO_PORTE_DATA_R &= ~(0x3C);
}

void ConfiguraPinPWMb4(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

    GPIO_PORTB_DIR_R |= 0x10;
    GPIO_PORTB_AMSEL_R &= ~(0x10);

    GPIO_PORTB_AFSEL_R |= 0x10;

    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFFF0FFFF) | 0x70000;
    GPIO_PORTB_DEN_R |= 0x10;
}


void VelocidadPB4 (int vel){
    uint32_t MS,ms,veloPWM;
    veloPWM=((100-vel)*1598);
    MS=(veloPWM & 0xF0000)>>16;
    ms=(veloPWM & 0x0FFFF);

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    while (!(SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1));

    TIMER1_CTL_R &= ~(0x0101);
    TIMER1_CFG_R = 0x00000004;

    TIMER1_TAMR_R = 0x0000000A;

    TIMER1_TAPR_R = 0x2;
    TIMER1_TAILR_R = 0x70FF;

    TIMER1_TAPMR_R = MS;
    TIMER1_TAMATCHR_R = ms;

    TIMER1_CTL_R |= 0x0101;

}

void ConfiguraPinPWMb5(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;

    GPIO_PORTB_DIR_R |= 0x20;
    GPIO_PORTB_AMSEL_R &= ~(0x20);

    GPIO_PORTB_AFSEL_R |= 0x20;  //YA NO ES CONTROLADO POR GPIO

    GPIO_PORTB_PCTL_R = (GPIO_PORTB_PCTL_R & 0xFF0FFFFF) | 0x00700000; //TIPO --->  TICCP1
    GPIO_PORTB_DEN_R |= 0x20; //FUNCIONES DIGITALES ACTIVADAS
}

void VelocidadPB5 (int vel){

    uint32_t MS,ms,veloPWM;
    veloPWM=((100-vel)*1598);
    MS=(veloPWM & 0xF0000)>>16;
    ms=(veloPWM & 0x0FFFF);

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    while (!(SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1));

    TIMER1_CTL_R &= ~(0x0101);
    TIMER1_CFG_R = 0x00000004;

    TIMER1_TBMR_R = 0x0000050A;

    TIMER1_TBPR_R = 0x2;
    TIMER1_TBILR_R = 0x70FF;

    TIMER1_TBPMR_R = MS;
    TIMER1_TBMATCHR_R = ms;

    TIMER1_CTL_R |= 0x0101;
}



//CONFIGURACION BLUETOOH ESCLAVO
// cuyos pines a utilizar son el PE0(tx) Y PE1(rx).
void ConfiguraUART7(void){
    // Habilitamos reloj para el UART7
    SYSCTL_RCGCUART_R |=  SYSCTL_RCGCUART_R7;
    // Habilitamos reloj para GPIOE

    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;
    // Inhabilitamos el  UART7
    UART7_CTL_R &= ~UART_CTL_UARTEN;

    // Velocidad 9600bps (Fsysclk = 16MHz)
    UART7_IBRD_R = (UART7_IBRD_R & 0xFFFF0000) | 104;
    UART7_FBRD_R = (UART7_FBRD_R & 0xFFFFFFC0) | 11;
    // 8, N, 1, FIFOs habilitados
    UART7_LCRH_R = (UART7_LCRH_R & 0xFFFFFF00) | 0x70;
    // Habilitamos el UART7
    UART7_CTL_R |= UART_CTL_UARTEN;
    // Desactivamos modo anal�gico en PE1y PE0
    GPIO_PORTE_AMSEL_R &= ~(0x03);
    // Conectamos UART0 a PE0 y PE1
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFFF00)|0x00000011;
    // Activamos funciones alternas en PE1 y PE0
    GPIO_PORTE_AFSEL_R |= 0x03;
    // Activamos funciones digitales en PE0 y PE1
    GPIO_PORTE_DEN_R |= 0x03;
}

//FUNCION QUE LEE EL DATO DEL MAESTRO


uint8_t EsperaDato(void){
 uint8_t datoRx;
 while(UART7_FR_R & UART_FR_RXFE);
 datoRx = (uint8_t)UART7_DR_R;
 return datoRx;
}


//FUNCION QUE CREA CADENA DE DATOS
void EsperaCadena(uint8_t cadenaASCIIZ[]){
 uint8_t datoRx;
 uint8_t hay1CaracterFlag = FALSE;
 uint8_t finFlag = FALSE;
 uint8_t i = 0;
 do{
 // Esperamos recibir un dato por el puerto serial.
 datoRx = EsperaDato();
 // Si dato recibido es diferente de ENTER.
 if(datoRx != CR){
 if(!hay1CaracterFlag) hay1CaracterFlag = TRUE;
 //UART_TransmiteDato(datoRx);
 cadenaASCIIZ[i++] = datoRx;
 }
 // Si dato recibido es ENTER.
 else if(hay1CaracterFlag){
 cadenaASCIIZ[i] = 0;
 //UART_SiguienteLinea();
 finFlag = TRUE;
 }
 }while(!finFlag);
}



int main (void){

    UART_Inicializa();

    ConfiguraPinB();
    ConfiguraPinC();
    ConfiguraPinD();
    ConfiguraPinE();

    ConfiguraUART7();
    ConfiguraPinPWMb4();
    ConfiguraPinPWMb5();

    uint8_t CADENA[8];
    uint8_t derecha='0';
    uint8_t izquierda='0';
    uint8_t abajo='0';
    uint8_t cerrado='0';


    while(1){
        EsperaCadena(CADENA);

        if(     ((CADENA[1]>='0')&&(CADENA[1]<='3'))&&
                ((CADENA[2]>='0')&&(CADENA[2]<='6'))&&
                ((CADENA[3]>='0')&&(CADENA[1]<='2'))&&
                ((CADENA[4]>='0')&&(CADENA[4]<='1'))&&
                ((CADENA[5]>='0')&&(CADENA[5]<='1'))&&
                ((CADENA[6]=='1'))
                ){

            UART_TransmiteDato(CADENA[1]);
            UART_TransmiteDato(CADENA[2]);
            UART_TransmiteDato(CADENA[3]);
            UART_TransmiteDato(CADENA[4]);
            UART_TransmiteDato(CADENA[5]);




 //FUNCIONES CARRO

 if((CADENA[3]=='1')&&(CADENA[4]=='0')&&(CADENA[5]<'2')&&(CADENA[6]=='1')){

        if ((CADENA[1]=='0')&&(izquierda=='0')){
            VelocidadPB5(35);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTC_DATA_R &= ~(0x30);

            GPIO_PORTE_DATA_R &= ~(0x0C);
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTE_DATA_R |= 0x04;
            delayms(100);
            izquierda='1';

        }else if ((CADENA[1]=='1')&&(izquierda=='1')){
            VelocidadPB5(35);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTC_DATA_R &= ~(0x30);

            GPIO_PORTD_DATA_R &= ~(0x0C);
            GPIO_PORTE_DATA_R &= ~(0x0C);
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTE_DATA_R |= 0x08;
            delayms(100);


            izquierda='0';



        }else if((CADENA[1]=='1')&&(derecha=='1')){
            VelocidadPB5(35);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTC_DATA_R &= ~(0x30);

            GPIO_PORTD_DATA_R &= ~(0x0C);
            GPIO_PORTE_DATA_R &= ~(0x0C);
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTE_DATA_R |= 0x04;
            delayms(100);


            derecha='0';

        }else if ((CADENA[1]=='2')&&(derecha=='0')){
            VelocidadPB5(35);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTC_DATA_R &= ~(0x30);

            GPIO_PORTD_DATA_R &= ~(0x0C);
            GPIO_PORTE_DATA_R &= ~(0x0C);
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTE_DATA_R |= 0x08;
            delayms(100);

            derecha='1';


        }




        if(CADENA[2]=='6'){
            VelocidadPB4(85);

            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x10;
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x02;


        }else if(CADENA[2]=='5'){
            VelocidadPB4(54);

            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x10;
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x02;


        }else if(CADENA[2]=='4'){
            VelocidadPB4(27);

            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x10;

            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x02;


        }else if(CADENA[2]=='3'){
            {VelocidadPB4(00);
            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x20;
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x01;}



        }else if(CADENA[2]=='2'){
            {VelocidadPB4(27);
            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x20;
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x01;}


        }else if(CADENA[2]=='1'){
            {VelocidadPB4(54);
            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x20;
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x01;}


        }else if(CADENA[2]=='0'){
            VelocidadPB4(80);

            {GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTE_DATA_R |= 0x20;
            GPIO_PORTD_DATA_R &= ~(0x03);
            GPIO_PORTD_DATA_R |= 0x01;}

        }

     }


//FUCNCIONES GARRA


 if((CADENA[1]=='1')&&(CADENA[2]=='3')&&(CADENA[6]=='1')){






        if ((CADENA[4]=='1')&&(abajo=='0')){

            VelocidadPB5(30);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTC_DATA_R &= ~(0x30);
            GPIO_PORTD_DATA_R |= 0x08;

            delayms(150);

            GPIO_PORTD_DATA_R &= ~(0x0F);

            abajo='1';

        } else if((CADENA[4]=='0')&&(abajo=='1')){

            VelocidadPB5(30);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTE_DATA_R &= ~(0x3C);
            GPIO_PORTC_DATA_R &= ~(0x30);
            GPIO_PORTD_DATA_R |= 0x04;

            delayms(150);

            GPIO_PORTD_DATA_R &= ~(0x0F);

            abajo='0';
        }





        if((CADENA[5]=='1')&&(cerrado=='0')){

            VelocidadPB5(30);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTE_DATA_R &= ~(0x3C);

            GPIO_PORTC_DATA_R &= ~(0x30);
            GPIO_PORTC_DATA_R |= 0x10;
            delayms(130);

            GPIO_PORTC_DATA_R &= ~(0x30);

            cerrado='1';
        } else if((CADENA[5]=='0')&&(cerrado=='1')){

            VelocidadPB5(30);

            GPIO_PORTD_DATA_R &= ~(0x0F);
            GPIO_PORTE_DATA_R &= ~(0x3C);

            GPIO_PORTC_DATA_R &= ~(0x30);
            GPIO_PORTC_DATA_R |= 0x20;
            delayms(130);

            GPIO_PORTC_DATA_R &= ~(0x30);

            cerrado='0';
        }
     }


 UART_TransmiteDato(CADENA[6]);
 UART_SiguienteLinea();


    }

    }

}



