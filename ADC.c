#include "lib/include.h"
//____________________________________ Configuración ADC ____________________________________________________/

/* Usando el modulo 0 y 1 , configurar la tarjeta a la frecuencia 55 Mhz y velocidad de 1 millon de muestras
Para el modulo 0, se asigna secuenciador 2 y los canales 5 9 11 con velocidad de 115200
Para el modulo 1 se asigna secuenciador 1 y los canales 1 3 4
*/
extern void Configurar_ADC(void)
{
// Pag 352 RCGCADC cuando esta habilitado activa modulos de ADC proporcionando un reloj
    SYSCTL->RCGCADC = (1 << 0) | (1 << 1); // inicializar modulo 0 y 1

// Pag 801 Puertos para  habilitar señales de acuerdo a los canales 
//canal 1 puerto PE2, canal 3 puerto PE0, canal 4 puerto PD3, canal 5 puerto PD2, canal 9 puerto PE4, canal 11 puerto PB5//
    SYSCTL->RCGCGPIO |= (1 << 1) | (1 << 3) | (1 << 4); // B,D,E

 // Pag 663 GPIODIR habilira los pines como entradas o salidas, 0 para input, 1 para output
      GPIOE->DIR = (0 << 0) | (0 << 2)| (0 << 4); // Puerto E (PE0, PE2 Y PE4)
      GPIOD->DIR = (0 << 2) | (0 << 3); // puerto D (PD2 y PD3)
      GPIOB->DIR = (0 << 5); // puerto b (PB5)

    // Pag 671 GPIOAFSEL funciónes alternativas (analogico)
        GPIOE->AFSEL = (1 << 0) | (1 << 2) | (1 << 4); // PE0, PE2 Y PE4
        GPIOD->AFSEL = (1 << 2) | (1 << 3); // PD2 y PD3
        GPIOB->AFSEL = (1 << 5); // PB5

    // Pag 683 GPIODEN Habilita/deshabilita registro digital
        GPIOE->DEN = (0 << 0) | (0 << 2)| (0 << 2); // PE0, PE2 Y PE4
        GPIOD->DEN = (0 << 2) | (0 << 3); // PD2 y PD3
        GPIOB->DEN = (0 << 5); // PB5

    /*Pag 688 GPIOPCTL señal periférica para cada pin GPIO cuando se utiliza el modo de función alternativa,
    las funciones alternativas vienen en Pag 1351*/
         GPIOE->PCTL = GPIOE->PCTL & (0xFFF0F0F0); // PE0, PE2 Y PE4
         GPIOD->PCTL = GPIOD->PCTL & (0xFFFF00FF); // PD2 y PD3
         GPIOB->PCTL = GPIOB->PCTL & (0xFF0FFFFF); // PB5

    // P ag.687 GPIOAMSEL habilitar analogico
         GPIOE->AMSEL = (1 << 0) | (1 << 2) |(1 << 4); // PE0, PE2 Y PE4
         GPIOD->AMSEL = (1 << 2) | (1 << 3); // PD2 y PD3
         GPIOB->AMSEL = (1 << 5); // PB5

    // Pag 891 El registro ADCPC  velocidad por segundo (0111) sete para 1Ms
            ADC0->PC = ((1 << 2) | (1 << 1) | (1 << 0)); // 1 Msps modulo 0
            ADC1->PC = ((1 << 2) | (1 << 1) | (1 << 0)); // 1 Msps modulo 1

    // Pag 841 ADCSSPRI configuracción de prioridad 0 para la mas alta y 3 a la más baja
            ADC0->SSPRI = 0x3021; // prioridad de secuenciador 2 = 0
            ADC1->SSPRI = 0x3102; // prioridad de secuenciador 1 = 0

    // Pag 821 ADCACTSS Este registro controla la desactivación de los secuenciadores de muestras
            ADC0->ACTSS = (0 << 3) | (0 << 2) | (0 << 1) | (0 << 0);
            ADC1->ACTSS = (0 << 3) | (0 << 2) | (0 << 1) | (0 << 0);

    // Pag 833 ADCEMUX selecciona el evento (trigger)
            ADC0->EMUX = (0x0000);//m0
            ADC1->EMUX = (0x0000);//m1

    // Pag 867 ADCSSMUX2 define la configuracion de entradas analógicas para cada muestra y secuencia (orden en que se da la lectura)
            ADC0->SSMUX2 = 0x0B95; // Lectura 3 canal 11  Lectura 2 Canal 9 Lectura 1 Canal 5
            ADC1->SSMUX1 = 0x0431; // Lectura 3 Canal 4  Lectura 2 Canal 3 Lectura 1 Canal 1

    // pag 868 ADCSSCTL2 configura el ancho de bit de control de muestreo y de interrupción
            ADC0->SSCTL2 = (1 << 9) | (1 << 10); 
            ADC1->SSCTL1 = (1 << 9) | (1 << 10); 
    //Pag 825 Registro muy importante* controla/activa la interrupción cuando le llega un dato
            ADC0->IM |= (1 << 2); //sec 2
            ADC1->IM |= (1 << 1); //sec1

    // Pag 821 ADCACTSS activación de secuenciadores
            ADC0->ACTSS = (0 << 3) | (1 << 2) | (0 << 1) | (0 << 0); // Activa sec 2 m0
            ADC0->PSSI |= (1 << 2);                                  
            ADC1->ACTSS = (0 << 3) | (0 << 2) | (1 << 1) | (0 << 0); // Activa sec 1 m1
            ADC1->PSSI |= (1 << 1);                                  
}

extern void Lectura_ADC(uint16_t *Result) //**pendiente
{

    // ADC Processor Sample Sequence Initiate (ADCPSSI)
    //  módulo 0
    ADC0->PSSI = 0x00000004; // empezar a leer datos
    while ((ADC0->RIS & 0x04) == 0) // si no hay datos, esperar
    {
    };                                 
    Result[0] = ADC0->SSFIFO2 & 0xFFF; // ya hay datos, por lo tanto, hacer lectura de la pila
    Result[1] = ADC0->SSFIFO2 & 0xFFF; // Esto es para el  secuenciador 2 modulo 0
    Result[2] = ADC0->SSFIFO2 & 0xFFF; // 
    ADC0->ISC = 0x0004;                // 

    ADC1->PSSI = 0x00000002;           // Esto es para el  secuenciador 1 modulo 2
    while ((ADC1->RIS & 0x02) == 0)
    {
    };                                 
    Result[3] = ADC1->SSFIFO1 & 0xFFF; // Llegada de datos
    Result[4] = ADC1->SSFIFO1 & 0xFFF;  
    Result[5] = ADC1->SSFIFO1 & 0xFFF;
    ADC1->ISC = 0x0002;                
}
