#include "lib/include.h"

int main(void)
{

    char terminator = 'A';
    uint16_t Result[6];
   
    char c = 0;
    Configurar_PLL(_40MHZ);      //Confiuracion de velocidad de reloj a 40 MHz
    Configurar_ADC();
    Configurar_UART2();
    Configurar_GPIO();
    c = readChar();
    if(c > 0)
    {
    while(1)
    {
        Lectura_ADC(Result); //conversion de datos
            dig(Result[0]);
            dig(Result[1]);
            dig(Result[2]);
            dig(Result[3]);
            dig(Result[4]);
            dig(Result[5]);
        printChar(terminator);
    }
    }
}