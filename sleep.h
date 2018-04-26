/*

    Idle – в этом режиме отключается CPU – процессор, наш обработчик команд – и, соответственно, flash-память. При этом остаются включенными последовательный интерфейс SPI, USART, аналоговый компаратор, аналого-цифровой преобразователь (ADC), интерфейс TWI – он же I2C, все таймеры, в том числе и сторжевой, ну и система прерываний. Если нам что-нибудь не нужно – компаратор, АЦП или сторжевой таймер, то до засыпания можно отключить это ручками, и сэкономим ещё больше.
    Этот режим буквально создан для работы с периферией, когда нужно быстро проснуться по команде от внешних интерфейсов и не тормозить. Получается даже не сон, а так... легкая дрема

    Power-Down - тут останавливается практически всё, кроме обработки внешних прерываний, интерфейса TWI и сторжевого таймера.
    Вызывать в таком состоянии прерывания могут только внешний сброс-reset, сброс по сторжевому таймеру, при провале напряжения. Также возможны прерывания при работе TWI и внешние прерывания.
    Большинство счётчиков и таймеров тут тоже останавливаются – таким образом, в таком режиме с микросхемой могут работать только асинхронные интерфейсы.
    В общем, тут чип засыпает намертво, проснуться самому ему достаточно проблематично, а уж если разбудили - просыпаться он будет мучительно долго, пропуская мимо ножек всю информацию от прерываний, например(

Idle и Power-Down встречаются наиболее часто.

    Power-save - этот режим похож на Power-down, но если таймер2 установлен в асинхронном режиме - бит ACCP.AS2 = 1 - то он будет работать; если таймер2 так не установлен, то даташит советует использовать Power-down - при пробуждении регистр этого таймера будет не определён.
    Здесь микросхема погружена в глубокий сон , но где-то глубоко в подсознании чип про себя отмечает, сколько времени он проспал. Этот режим хорош для устройств, требующих знания о частоте выхода из спящего режима, например, для часов. Но, к сожалению, он не подходит для точного измерения временных отрезков между событиями.
    Standby - он тоже похож на Power-down, но используется при работе от внешнего источника тактирования и оставляет включённым осциллятор, а включенный осцилятор означает меньше времени на раскачку - быстрее старт. Этот режим используется, если нужно поймать какое-либо событие - например, в клавиатуре чипу надо проснуться до того, как пользователь отпустит клавишу. Ещё один красивый пример - автоспуск пленочного фотоаппарата на молнию.
    Extended Standby - похож на Power-Save, но оставляет включённым осциллятор. Вот этот режим как раз для устройств, которым нужно точно знать, сколько времени прошло после входа в режим сна - например, для измерителя скорости вращения колеса велосипеда.

Наконец, режим, стоящий особняком:

    ADC Noise Reduction. Этот режим останавливает процессор, но оставляет работать АЦП, систему внешних прерываний, интерфейс TWI, сторжевой таймер и таймер/счётчик2; соответственно, останавливаются только часы I/O, CPU и FLASH. Все это предназначено для того, чтобы уменьшить внутренние помехи (возникающие при переключении транзисторов самой микросхемы) при работе АЦП - так получаются измерения более высокой точности.



MCUCR register
SM2		SM1		SM0		Mode
0		0		0		Idle
0		0		1		ADC Noise Reduction
0		1		0		Power-Down
0		1		1		Power-Save
1		0		0		Reserved
1		0		1		Reserved
1		1		0		Standby



*/

void sleep_init(void)
{
	// Init Power-Down mode
	setZero(MCUCR,SM2);
	setPlus(MCUCR,SM1);
	setZero(MCUCR,SM0);
}

void sleep_enable(void)
{
	MCUCR |= (1<<SE);
}

void sleep_disabled(void)
{
	MCUCR &=~ (1<<SE);
}

void sleep(void)
{
	asm("sleep");
}
