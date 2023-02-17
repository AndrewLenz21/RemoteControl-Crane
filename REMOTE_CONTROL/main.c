#include <stdint.h>
#include <tm4c123gh6pm.h>
#include <stdio.h>
#include <math.h>
#include "stdint.h"
#include "TivaES.h"
#include "UART.h"


//CONFIGURACION DE PINES DE ENTRADA Y SALIDA
void ConfiguraPortF (void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x01;
    GPIO_PORTF_DIR_R &= ~(0x11);
    GPIO_PORTF_AMSEL_R &= ~(0x13);

    GPIO_PORTF_AFSEL_R &= ~(0x13);

    GPIO_PORTF_DR8R_R |= 0x13;
    GPIO_PORTF_DEN_R |= 0x13;
    GPIO_PORTF_PUR_R |= 0x11;
    GPIO_PORTF_DATA_R |= 0x11;
    GPIO_PORTF_DATA_R &= ~(0x02);
}
void ConfiguraPortC(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOC;

    GPIO_PORTC_DIR_R &= ~(0x30);

    GPIO_PORTC_PUR_R |= 0x30;

    GPIO_PORTC_AMSEL_R &= ~(0x30);
    GPIO_PORTC_AFSEL_R &= ~(0x30);

    GPIO_PORTC_DEN_R |= 0x30;
    GPIO_PORTC_DATA_R &= ~(0x30);
}
void ConfiguraPortD(void){
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD;

    GPIO_PORTD_DIR_R |= 0x01;

    GPIO_PORTD_PUR_R |= 0x01;

    GPIO_PORTD_AMSEL_R &= ~(0x01);
    GPIO_PORTD_AFSEL_R &= ~(0x01);

    GPIO_PORTD_DEN_R |= 0x01;
    GPIO_PORTD_DATA_R &= ~(0x01);
}


//CONFIGURACION DEL PUERTOS ESPECIALES

void ConfiguraUART5(void){
    // Habilitamos reloj para el UART5
    SYSCTL_RCGCUART_R |=SYSCTL_RCGCUART_R5 ;
    // Habilitamos reloj para GPIOE
    SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOE;

    // Inhabilitamos el UART5
    UART5_CTL_R &= ~UART_CTL_UARTEN;

    // Velocidad 9600bps (Fsysclk = 16MHz)
    UART5_IBRD_R = (UART5_IBRD_R & 0xFFFF0000) | 104;
    UART5_FBRD_R = (UART5_FBRD_R & 0xFFFFFFC0) | 11;
    // 8, N, 1, FIFOs habilitados
    UART5_LCRH_R = (UART5_LCRH_R & 0xFFFFFF00) | 0x70;
    // Habilitamos el UART5
    UART5_CTL_R |= UART_CTL_UARTEN;

    // Desactivamos modo analógico en PE4 y PE5
    GPIO_PORTE_AMSEL_R &= ~(0x30);
    // Conectamos UART5 a PE4 y PE5
    GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFF00FFFF)|0x00110000;
    // Activamos funciones alternas en PE4 y PE5
    GPIO_PORTE_AFSEL_R |= 0x30;
    // Activamos funciones digitales en PE4 Y  PE5
    GPIO_PORTE_DEN_R |= 0x30;
}

void TransmiteBluetooh(uint8_t datoTx){
 while(UART5_FR_R & UART_FR_TXFF);
 UART5_DR_R = (UART5_DR_R & 0xFFFFFF00) | datoTx;
}

void ConfiguracionADC(void){

    SYSCTL_RCGCADC_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;

    while(!(SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4));

    GPIO_PORTE_DIR_R &= ~(0x06);
    GPIO_PORTE_AFSEL_R |= 0x06;
    GPIO_PORTE_DEN_R &= ~(0x06);
    GPIO_PORTE_AMSEL_R |= 0x06;


    ADC0_PC_R |= 0X1;

    ADC0_SSPRI_R = 0x0123;
    ADC0_ACTSS_R &= ~0x0002;

    ADC0_EMUX_R &= ~0x00F0;  //USARE EL SS1
    ADC0_SSMUX1_R = 0x0021;
    ADC0_SSCTL1_R = 0x0064;  //Guardo del 0:3 y del 4:7

    ADC0_ACTSS_R |= 0x0002;
}

void capturaADCdeXeY(uint32_t data[2]){

    ADC0_PSSI_R = 0x0002;  //selecciono secuenciador 1

    while(!(ADC0_RIS_R & 0x02)){};

    data[1] = ADC0_SSFIFO1_R & 0xFFF;
    data[0] = ADC0_SSFIFO1_R & 0xFFF;

    ADC0_ISC_R = 0x0002;


}






int main (void){

    TivaES_Inicializa();
    UART_Inicializa();
    ConfiguraUART5();

    ConfiguraPortC();
    ConfiguraPortD();
    ConfiguraPortF();
    ConfiguracionADC();

    uint64_t i;
    uint32_t cadena[2];
    uint16_t VALOR[7];
    uint8_t AnguloBrazo=0;
    uint8_t EstadoGarra=0;



    while(1){
//CAPTURAMOS VALOR DE XeY DEL JOYSTICK
        capturaADCdeXeY(cadena);

        if((cadena[0]>=0)&&(cadena[0]<=1000)){
            VALOR[0]='0';
        }else if((cadena[0]>1000)&&(cadena[0]<=3098)){
            VALOR[0]='1';
        }else if((cadena[0]>3098)&&(cadena[0]<=4098)){
            VALOR[0]='2';
        }
        //VALOR[0] DEFINE LA DIRECCION DEL CARRO
        UART_TransmiteDato(VALOR[0]);
        TransmiteBluetooh(VALOR[0]);


//CAPTURAMOS DE NUEVO XeY DEL JOYSTICK
        capturaADCdeXeY(cadena);
        if ((cadena[1]>0)&&(cadena[1]<=585)){
            VALOR[1]='0';
        } else if((cadena[1]>585)&&(cadena[1]<=1071)){
            VALOR[1]='1';
        } else if ((cadena[1]>1071)&&(cadena[1]<=1756)){
            VALOR[1]='2';
        } else if ((cadena[1]>1756)&&(cadena[1]<=2341)){
            VALOR[1]='3';
        } else if ((cadena[1]>2341)&&(cadena[1]<=2927)){
            VALOR[1]='4';
        } else if ((cadena[1]>2927)&&(cadena[1]<=3512)){
            VALOR[1]='5';
        } else if ((cadena[1]>3512)&&(cadena[1]<=4098)){
            VALOR[1]='6';
        };
        //VALOR[1] DEFINE LA VELOCIDAD DEL CARRO
        UART_TransmiteDato(VALOR[1]);
        TransmiteBluetooh(VALOR[1]);


//CON VALOR[2] DEFINIMOS EL GIRO DEL BRAZO
        if(!(GPIO_PORTF_DATA_R & 0x01)){
            VALOR[2]='2';
        } else if (!(GPIO_PORTF_DATA_R & 0x10)){
            VALOR[2]='0';
        } else if (GPIO_PORTF_DATA_R &= ~(0x02)){
            VALOR[2]='1';
        };

        UART_TransmiteDato(VALOR[2]);
        TransmiteBluetooh(VALOR[2]);




//CON VALOR[3] DEFINIMOS EL ANGULO DEL BRAZO
        if(!(GPIO_PORTC_DATA_R & 0x10)){
            for(i=0;i<200000;i++);

            if(!(GPIO_PORTC_DATA_R & 0x00)){
                if(AnguloBrazo==0){
                    AnguloBrazo=1;
                } else if(AnguloBrazo==1){
                    AnguloBrazo=0;
                }
            }
        }

        if (AnguloBrazo==0){
            VALOR[3]='0';
        } else if(AnguloBrazo==1){
            VALOR[3]='1';
        }
        UART_TransmiteDato(VALOR[3]);
        TransmiteBluetooh(VALOR[3]);



//CON VALOR[4] DEFINIMOS EL ESTADO DE LA GARRA
        if(!(GPIO_PORTC_DATA_R & 0x20)){
            for(i=0;i<200000;i++);

            if(!(GPIO_PORTC_DATA_R & 0x00)){
                if(EstadoGarra==0){
                    EstadoGarra=1;
                } else if(EstadoGarra==1){
                    EstadoGarra=0;
                }
            }
        }

        if (EstadoGarra==0){
            VALOR[4]='0';
        } else if(EstadoGarra==1){
            VALOR[4]='1';
        }
        UART_TransmiteDato(VALOR[4]);
        TransmiteBluetooh(VALOR[4]);


//USANDO VALOR[5] TENEMOS UNA BOCINA PARA EL TRAFICO
        if(!(GPIO_PORTD_DATA_R & 0x01)){
            VALOR[5]='1';
        } else {
            VALOR[5]='0';
        }

        UART_TransmiteDato(VALOR[5]);
        TransmiteBluetooh(VALOR[5]);







        UART_TransmiteDato(13);
        UART_TransmiteDato(10);

        TransmiteBluetooh(13);
        TransmiteBluetooh(10);
    }
}
