#include <avr/io.h>
#include <avr/interrupt.h>
//TIFR0 &= ~(1 << TOV0);	timer start
//Вектор по совпадению
ISR( TIMER0_COMP_vect ){
		
}
//Вектор при переполнении счетчика
ISR( TIMER0_OVF_vect ){
	
}
void t0_init(){
	//TCNT0=0x83; //выставляем начальное значение TCNT0
	//TCCR0B|=(1<<CS00);//предделитель/1
	//TCCR0B|=(1<<CS01);//предделитель/8
	TCCR0B|=(1<<CS01)|(1<<CS00);//предделитель/64
	//TCCR0B|=(1<<CS02)|(1<<CS00);//предделитель/1024
//Нормальный режим
//TCCR0A &=~ (1<<WGM00);	//0
//TCCR0A &=~ (1<<WGM01);	//0
//TCCR0B &=~ (1<<WGM02);	//0
//TIMSK0|=(1<<TOIE0);		// Разрешаем прерывание по переполнению Т0
//При совпадении с OCR0
TCCR0A &=~ (1<<WGM00);	//0 Срабатывание при переполнении
TCCR0A |= (1<<WGM01);	//1	Срабатывание при совпадении
TIMSK0|=(1<<OCIE0);		// Разрешаем прерывание по совпадению

	//TCCR0B &=~ (1<<WGM02);
	//SREG|=(1<<I);//глобальное разрешение прерывний
	// Global enable interrupts
	sei();
}