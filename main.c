/*;UNIVERSIDAD DEL VALLE DE GUATEMALA
;IE2023 PROGRAMACIÓN DE MICROCONTROLADORES
;LABORATORIO 06
;main.c
;AUTOR: GABRIEL ANTONIO ALVAREZ DE LEÓN
;PROYECTO: Laboratorio 06, comunicación serial
;HARDWARE: ATMEGA328P
;CREADO: 22/04/2024
*/
#define  F_CPU 16000000
#define  Zero 48
#define  One 49
#define  Two 50

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
void initUART9600(void);
void initADC(void);
void writeChar(char);
void writeLine(char*);
void ASCII2Dec(uint8_t);	

char *string;

uint8_t ASCII = 0;
volatile uint8_t bufferTx = 0;
volatile uint8_t input = 0;
volatile uint8_t flag = 1; //Set flag for menu
int main(void)
{
	cli();
    initUART9600();
	initADC();
	sei();
    while (1) 
    {
			if(flag == 1){
				input = 0;
				//Display menu
				writeLine("1. Leer potenciómetro.");
				writeLine("2. Enviar ASCII.");
				//Wait for valid input
				while(!(input == One || input == Two));
				//Check for each of the two valid inputs
				switch(input){
					case One: //Input is (ASCII) One
						ADCSRA |= (1<<ADSC); //Start ADC Conversion
						flag = 0;
						_delay_ms(20); //Wait for it to finish
						break;
					case Two: //Input is (ASCII) Two
						writeLine("Ingrese ASCII."); //Input request
						flag = 0; //Set flag to 0
						while(flag == 0); //Wait for input 
						break;
				}
			}
			flag = 1;
			writeLine("Exiting loop.");
			
		}
}


void initUART9600(void){
	//Rx as input, Tx as output
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD0);
	
	UCSR0A = 0;	UCSR0B = 0; UCSR0C = 0; //Initial values
	UCSR0A |= (0<<U2X0); //UCSR0A Config 
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0)|(0<<UCSZ02); //UCSR0B Config. 
	UCSR0C |= (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00); //UCSR0C Config.
	UBRR0 = 103;
	
}
void initADC(void){
	ADMUX = 0;
	ADMUX |= (1<<REFS0)|(0<<REFS1)|(1<<ADLAR);
	ADCSRA = 0;
	ADCSRA |= (1<<ADEN);
	ADCSRA |= (1<<ADIE); 
	
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}
void writeChar(char c){ while(!(UCSR0A & (1<<UDRE0))); UDR0 = c;}
void writeLine(char* str){for(uint8_t i=0; str[i]!='\0';i++){writeChar(str[i]);} writeChar('\n');writeChar('\r');}
void ASCII2Dec(uint8_t encoded){
	uint8_t units, tens, hundreds;
	//Using modulus operator, we can get all positions, like this. input is restricted from 000 to 255, so we'll always do it 3 times.	
	units = encoded % 10;
	encoded = (encoded-units)/10;
	tens = encoded %10;
	encoded = (encoded-tens)/10;
	hundreds = encoded % 10;

	writeChar(Zero+hundreds);	
	writeChar(Zero+tens);
	writeChar(Zero+units);
	writeChar('\n');
	writeChar('\r');
}

ISR(USART_RX_vect){
	bufferTx = UDR0; input = bufferTx;	
	if(flag == 1){	
		if(input == One){writeLine("Leyendo potenciómetro...");}
		else if(input == Two){writeLine("Leyendo ASCII desde terminal...");}
		else{writeLine("Entrada inválida. Ingrese 1 o 2.");}
	}
	
	if(flag == 0){PORTD=(bufferTx<<6); PORTB=(bufferTx>>2); flag = 1;}
}
ISR(ADC_vect){
	ASCII = ADCH; 
	ASCII2Dec(ASCII);
	PORTD=(ASCII<<6);
	PORTB=(ASCII>>2);
	ADCSRA |= (1<<ADIF); //Clear interrupt flag.	
}
