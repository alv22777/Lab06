/*

 */ 
#define  F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void initUART9600(void);
void initADC(void);
void writeChar(char);
uint8_t ASCII = 0;

int main(void)
{
	cli();
    initUART9600();
	initADC();
	sei();
    while (1) 
    {
		PORTD=(ASCII<<6);
		PORTB=(ASCII>>2); 
		_delay_ms(100);
		writeChar(ASCII);
		ADCSRA |= (1<<ADSC);
		
    }
}


void initUART9600(void){
	//Rx as input, Tx as output
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD0);
	
	UCSR0A = 0;	UCSR0B = 0; UCSR0C = 0;
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
	ADCSRA |= (1<<ADSC);
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADCSRA |= (1<<ADSC);
}

void writeChar(char c){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = c;
}

ISR(USART_RX_vect){
	PORTB = UDR0;
	PORTD = (UDR0);
	_delay_ms(2000);
}
ISR(ADC_vect){
	ASCII = ADCH; 
	
	ADCSRA |= (1<<ADIF); //Clear interrupt flag.
	
}