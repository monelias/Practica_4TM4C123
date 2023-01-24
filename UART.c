#include "lib/include.h"

extern void Configurar_UART2(void) //configuración uart2
{
    SYSCTL->RCGCUART  |= (1<<2);   //Paso 1 (RCGCUART) pag.344 UART modulo2 0->deshabilitar 1->habilitar
    SYSCTL->RCGCGPIO |= (1<<3);   //Paso 2 (RCGCGPIO) pag.340 /////// Aqui es para habilitar el puerto D
    GPIOD -> LOCK = 0x4C4F434B; // Este valor desbloquea el gpiocr, bloque valores fuera de la escritura que quiere
    GPIOD -> CR = (1<<6) | (1<<7); // 
    //(GPIOAFSEL) pag.671 Enable alternate function
    GPIOD->AFSEL = (1<<6) | (1<<7);
    //GPIO Port Control (GPIOPCTL) PD6-> U0Rx  PD7-> U0Tx pag.688
    GPIOD->PCTL = (GPIOD->PCTL&0x00FFFFFF) | 0x11000000;// (1<<0) | (1<<4);//0x00000011
    // GPIO Digital Enable (GPIODEN) pag.682
    GPIOD->DEN = (1<<6) | (1<<7);//PD7 PA6
    //UART2 UART Control (UARTCTL) pag.918 DISABLE!!
    UART2->CTL = (0<<9) | (0<<8) | (0<<0);
   // UART Integer Baud-Rate Divisor (UARTIBRD) pag.914
    /* calculos de acuerdo al inciso que me toco
    BRD = 40,000,000 / (16*115200) = 21.701
    UARTFBRD[DIVFRAC] = integer(.701 * 64 + 0.5)
    mi velocidad es de 115200    */
    UART2->IBRD = 21;
    // UART Fractional Baud-Rate Divisor (UARTFBRD) pag.915
    UART2->FBRD = 45; //numero mas cercano al 45.364
    //  UART Line Control (UARTLCRH) pag.916
    UART2->LCRH = (0x3<<5)|(1<<4);
    //  UART Clock Configuration(UARTCC) pag.939
    UART2->CC =(0<<0);
    //Disable UART0 UART Control (UARTCTL) pag.918
    UART2->CTL = (1<<0) | (1<<8) | (1<<9);
}

extern char readChar(void)
{
    //UART FR flag pag 911
    //UART DR data 906
    int v;
    char c;
    while((UART2->FR & (1<<4)) != 0 );
    v = UART2->DR & 0xFF;
    c = v;
    return c;
}
extern void printChar(char c)
{
    while((UART2->FR & (1<<5)) != 0 );
    UART2->DR = c;
}

extern void printString(char* string)
{
    int i = 0;
    while(string[i] > 0)
    {
        printChar(string[i]);
        i++;
    }
}

//___________________ configuracion práctica 3___________________/

extern void readString(char *string, char delimitador)
{
   int i=0;
   char c = readChar();
   while(c != delimitador)
   {
    string[i] = c;
    i++;
    c = readChar();
   }
   string[0] = (i-1);  //numero de caracteres, i-1 es la cantidad de letras o caracteres
                       // es menos uno pq en la primer posicion del arreglo esta la longitud.
   return;
}
//
////El envio es su nombre invertirlo y regresarlo con numeros consecutivos entre letras
extern void invertir(char *string) //Caracter se va hacia el apuntador string
{
int i = string[0];         // el numero de letras esta en la posición 0 y
                           // se le asigna a i 
int m = 0;
char auxiliar[i];
for(m = 0 ; m <= (i) ; m++ )  // m empieza en cero y cuenta de uno en uno 
                             // hasta que m vale lo mismo que i(i,totaldecaracteres)
{
 auxiliar[m] = string[(i)-m]; // m va recorriendo la cadena de abajo a arriba
                              // se guarda en auxiliar que empieza en 0
}
for(m = 0 ; m <= (100) ; m++ )  
{
if (m <= i) 
        string[m] = auxiliar[m];
        else 
        string[m] = 0;
}
return;
}

//_______Combinacion de numero y letra (Entero y caracter)

 
extern void numeroletra (char *string)
{
	// El resultado de ingresar Monse debería ser e01s02n03s04o05M
    int lectura = 0;
    // Ciclo para determinar la longitud
	while(string[lectura] != 0)
	{
		lectura++;
	}
    int tam,decimal,unidad,i = 0;
    int cont = 1;
    int flag = 0;
    int cont2 =0;
    tam = (lectura-1) * 3; // Se le resta uno a la longitud porque la cadena contenía el tamaño
    char cadenafinal[tam];  
    while(i<tam)
    {
		decimal = (int)(cont / 10);   //Se calcula el valor de los decimales
		unidad = cont % 10;         //Se calcula el valor de las unidades
		if (flag == 0)              		//Estructura "uno a uno" si la bandera vale cero....
			//INSTRUCCIONES DEL IF
			{
            flag = 1;             		//La bandera se pone en uno para que a la siguiente iteración, siga las instrucciones del ELSE
		    cadenafinal[i] = string[cont-1];  
            }
		else
        {
            //INSTRUCCIONES DEL ELSE:      
			flag = 0;                    // Aquí vuelve a poner la bandera en uno para que a la siguiente iteración, siga las instrucciones del IF
			cadenafinal[i + 1] = decimal + '0'; //Aquí el valor del índice se le suma 1 porque seguido de la letra va el valor decimal ej a01
			cadenafinal[i + 2] = unidad + '0'; //Aquí el valor del índice se le suman 2 porque seguido del decimal va el valor de la unidad ej a01
            cont++;
            i = i + 3;           //Cada que se añade una letra, se le suman 3 a la posición del k para dejar espacio 
						        // para los dos números, ej. si pone una a, la secuencia sería agregarle 01 así que queda a01, por eso se suman 3
        }
cont2++;
    }
// Ciclo para transferir los datos
    for(int g = 0;g<=tam-1;g++)
        string[g]=cadenafinal[g];
// Ciclo para borrar el resto de la cadena
    for(int n = i;n < 100;n++)
    string[n] = 0;
    return;
}

//_____Convertir numero entero en cadena de caracteres que contenga los digitos del ADC______/
extern void dig(uint16_t num) //Caracter se va hacia el apuntador string num= numero que se quiere convertir
{
char string[5]; 
int i = 4; //La longitud de cada lectura del ADC es de 4 dígitos 
int p = 1000; // la lectura mayor es miles
int valor;

for (int contador = 0; contador < i; contador++) //contador debe contar de 0 a 3
{ 
    valor=(int)(num/p); // 3590/1000=3.595 
    string[contador]= valor + '0'; // convertir valor a caracter
    num = num - (valor * p); // al numero original, se le resta el digito que se convirtio a caracter
    p= p/10; // se cambia p a la siguiente unidad
}
string[i] = '\0';
printString(string);
}

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



