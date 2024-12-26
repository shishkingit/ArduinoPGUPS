#include <avr/io.h>
#define F_CPU 16000000UL //частота ATMEGA328P
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//пины дисплея
#define LCD_RS 12
#define LCD_EN 9
#define LCD_D4 5
#define LCD_D5 4
#define LCD_D6 3
#define LCD_D7 2

//пины управления MAX487
#define MAX487REDE_PIN 11

//отправление 4 бит на LCD
void lcd_write_4bit(unsigned char data, unsigned char rs) {
     if (rs) {
         if(LCD_RS >= 8) {
           PORTB |= (1 << (LCD_RS-8));
        } else {
            PORTD |= (1 << LCD_RS);
        }
    } else {
        if(LCD_RS >= 8) {
           PORTB &= ~(1 << (LCD_RS-8));
        } else {
            PORTD &= ~(1 << LCD_RS);
        }
    }

    if (data & 0x01) PORTD |= (1 << LCD_D4); else PORTD &= ~(1 << LCD_D4);
    if (data & 0x02) PORTD |= (1 << LCD_D5); else PORTD &= ~(1 << LCD_D5);
    if (data & 0x04) PORTD |= (1 << LCD_D6); else PORTD &= ~(1 << LCD_D6);
    if (data & 0x08) PORTD |= (1 << LCD_D7); else PORTD &= ~(1 << LCD_D7);

   if(LCD_EN >= 8) {
         PORTB |= (1 << (LCD_EN-8));
    } else {
        PORTD |= (1 << LCD_EN);
    }
    _delay_us(1);
   if(LCD_EN >= 8) {
        PORTB &= ~(1 << (LCD_EN-8));
    } else {
        PORTD &= ~(1 << LCD_EN);
    }
    _delay_us(100);
}

//отправка команд на LCD
void lcd_send_command(unsigned char command) {
    lcd_write_4bit(command >> 4, 0);
    lcd_write_4bit(command & 0x0F, 0);
}

//отправление данных на LCD
void lcd_send_data(unsigned char data) {
    lcd_write_4bit(data >> 4, 1);
    lcd_write_4bit(data & 0x0F, 1);
}

//инициализация дисплея
void lcd_init() {
     if (LCD_RS >= 8) {
      DDRB |= (1 << (LCD_RS - 8));
    } else {
      DDRD |= (1 << LCD_RS);
    }
    if (LCD_EN >= 8) {
      DDRB |= (1 << (LCD_EN - 8));
    } else {
      DDRD |= (1 << LCD_EN);
    }
    DDRD |= (1 << LCD_D4) | (1 << LCD_D5) | (1 << LCD_D6) | (1 << LCD_D7);

    _delay_ms(15); //для включения

    lcd_write_4bit(0x03, 0);
    _delay_ms(5);
    lcd_write_4bit(0x03, 0);  
    _delay_us(150);
     lcd_write_4bit(0x03, 0); 
    _delay_us(150);
    lcd_write_4bit(0x02, 0); //переключение на 4 бита

    lcd_send_command(0x28); //4 бита, 2 линии, 2х8
    lcd_send_command(0x0C); //включение дисплея, курсор выключен
    lcd_send_command(0x01); //очистка дисплея
    _delay_ms(2);
    lcd_send_command(0x06); //установка курсора
}

//конвертация переменной
void float_to_string(float f, char *str, int precision) {
     int int_part = (int)f;
    float frac_part = f - (float)int_part;
     if (frac_part < 0.0) {
         frac_part = -frac_part;
     }
    int pow10 = pow(10, precision);
    int frac_int = (int)(frac_part * pow10);
    
    char int_str[10];
    char frac_str[10];

    itoa(int_part, int_str, 10);
    itoa(frac_int, frac_str, 10);

     sprintf(str, "%s.", int_str);
     int len = strlen(str);
     int i = 0;
    for (;i < precision; ++i){
        str[len + i] = (frac_str[i] != 0) ? frac_str[i] : '0';
    }
    str[len + i] = 0;
}

//основной цикл, вывод на дисплей
int main(void) {
     //установка пина в режим вывода
     if(MAX487REDE_PIN >= 8){
        DDRB |= (1 << (MAX487REDE_PIN-8));
      } else {
        DDRD |= (1 << MAX487REDE_PIN);
      }
    //подача тока на пин
     if(MAX487REDE_PIN >= 8) {
        PORTB |= (1 << (MAX487REDE_PIN-8));
    } else {
        PORTD |= (1 << MAX487REDE_PIN);
    }
    _delay_ms(200);


    //инициализация
    UBRR0H = 0;
    UBRR0L = 103; // 9600
    UCSR0B = (1 << RXEN0);  //добавление ресивера

    lcd_init();
     float t;
      char buffer[20];
     char tempStr[10];
      int i;

    while (1) {
      //чтение температуры 
      if(MAX487REDE_PIN >= 8) {
                PORTB &= ~(1 << (MAX487REDE_PIN-8));
        } else {
           PORTD &= ~(1 << MAX487REDE_PIN);
        }
          for(i=0; i < sizeof(float); ++i) {
            while (!(UCSR0A & (1 << RXC0)));
            ((char*)&t)[i] = UDR0;
        }
       if(MAX487REDE_PIN >= 8) {
                PORTB |= (1 << (MAX487REDE_PIN-8));
        } else {
            PORTD |= (1 << MAX487REDE_PIN);
        }
	    lcd_send_command(0x01); 
    _delay_ms(2); // небольшая задержка

      //превращение температуры в строку
         float_to_string(t, tempStr, 2);
        snprintf(buffer, sizeof(buffer), "t = %s C", tempStr);


      //вывод на дисплей
       for ( i = 0; buffer[i] != '\0'; i++) {
            lcd_send_data(buffer[i]);
        }

    }

  return 0;
}
