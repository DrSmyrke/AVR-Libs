#define BAUDRATE 9600 // Скорость обмена данными
#include <string.h>
#include <avr/interrupt.h> 
void uart_init(){
// Параметры соединения: 8 бит данные, 1 стоповый бит, нет контроля четности
// USART Приемник: Включен
// USART Передатчик: Включен
// USART Режим: Асинхронный
// USART Скорость обмена: 9600
    UCSRA=0x00;
    UCSRB=0x18;
	UCSRC=0x86;
	//UBRR = ( F_CPU /( BAUDRATE * 16 ) ) - 1
	UBRRH=0x00;
    UBRRL=0x33;
	
	//разрешить прием, передачу данных и прерывание по приёму байта
	UCSRB=( 1 << TXEN ) | ( 1 << RXEN ) | (1 << RXCIE ) | (1 << TXCIE );
	UCSRC |= (1 << URSEL)| // Для доступа к регистру UCSRC выставляем бит URSEL
	(1 << UCSZ1)|(1 << UCSZ0); // Размер посылки в кадре 8 бит
}
// Функция передачи данных по USART
void uart_send(unsigned char data){
	while(!( UCSRA & (1 << UDRE)));   // Ожидаем когда очистится буфер передачи
	UDR = data; // Помещаем данные в буфер, начинаем передачу
}
 // Функция передачи строки по USART
void str_uart_send(char *string){
	while(*string != '\0'){
		uart_send(*string);
		string++;
	}
}
unsigned char uart_receive(void) {
    while(!(UCSRA & (1<<RXC)) );
    return UDR;
}
// Прерывание по окончанию приема данных по USART
ISR(USART_RXC_vect){	// прием данных
	uart_send(uart_receive());
	//ibi(PORTD,3);
}
ISR(USART_TXC_vect){	// Конец отправки данных

}
