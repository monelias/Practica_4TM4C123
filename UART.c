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




