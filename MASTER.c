#include <avr/io.h>
#include <stdint.h>
#define F_CPU 16000000UL //частота ATMEGA328P
#include <util/delay.h>

//пины MAX6675
#define THERMO_CLK 6
#define THERMO_CS  5
#define THERMO_SO  4

//пины управления MAX487
#define MAX487REDE_PIN 2

//пин оптопары / лампочки
#define OPTO_PIN 7


//функция чтения температуры с термопары
float read_temperature() {
    uint16_t value = 0;
    int i;

    PORTD &= ~(1 << THERMO_CS);
    _delay_us(1);
  
    for (i = 0; i < 16; i++) {
        PORTD |= (1 << THERMO_CLK);
        value <<= 1;
        if (PIND & (1 << THERMO_SO)) {
            value |= 1;
        }
        PORTD &= ~(1 << THERMO_CLK);
    }

    //выкл MAX6675
    PORTD |= (1 << THERMO_CS);

    //пересчёт в градусы
    //!!!ЦИФРЫ НЕТОЧНЫЕ И ПОЛУЧЕНЫ ОПЫТНЫМ ПУТЁМ!!!
    return (0.09 * (value - 793));
}

//основная функция с циклом
int main(void) {
     //установка пиов в режим вывода
    DDRD |= (1 << MAX487REDE_PIN) | (1 << OPTO_PIN) | (1 << THERMO_CS) | (1 << THERMO_CLK);

    //базовая инициализация
    UBRR0H = 0;
    UBRR0L = 103;  // 9600
    UCSR0B = (1 << TXEN0);  //добавление трансмиттера

    float t;
    while (1) {
        t = read_temperature();

      //отправление температуры через MAX487
        PORTD &= ~(1 << MAX487REDE_PIN);
       _delay_us(1);
        int i;
      for(i=0; i < sizeof(float); ++i) {
            while (!(UCSR0A & (1 << UDRE0)));
            UDR0 = ((char*)&t)[i];
        }
        PORTD |= (1 << MAX487REDE_PIN);

        //включение оптопары/лампочки
        if (t > 20) {
            PORTD |= (1 << OPTO_PIN);
            _delay_ms(500);
            PORTD &= ~(1 << OPTO_PIN);
        } else {
            PORTD &= ~(1 << OPTO_PIN);
        }
      _delay_ms(200);
    }
    return 0;
}
