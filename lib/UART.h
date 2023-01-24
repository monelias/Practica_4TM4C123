#ifndef UART_UART_H_
#define UART_UART_H_

extern void Configurar_UART2(void);
extern void Configurar_UART0(void);
extern char readChar(void);
extern void printChar(char c);
extern void printString(char* string);
extern void readString(char *string, char delimitador);
extern void invertir(char *string);
extern void numeroletra (char *string);
extern void dig(uint16_t num);
#endif /* UART_UART_H_ */