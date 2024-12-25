#include <avr/io.h>
#include <stdint.h> // Для uint16_t

#define F_CPU 16000000UL // Clock frequency for Arduino NANO

#include <util/delay.h>
// MAX6675 pins
#define THERMO_CLK 6
#define THERMO_CS  5
#define THERMO_SO  4
// MAX487 REDE pin
#define MAX487REDE_PIN 2
// Optocoupler pin
#define OPTO_PIN 7

// Function to read temperature from MAX6675
float read_temperature() {
    uint16_t value = 0;
    int i;

    // Enable MAX6675
    PORTD &= ~(1 << THERMO_CS);
    _delay_us(1);

    // Read data (16 bits)
    for (i = 0; i < 16; i++) {
        PORTD |= (1 << THERMO_CLK);
        value <<= 1;
        if (PIND & (1 << THERMO_SO)) {
            value |= 1;
        }
        PORTD &= ~(1 << THERMO_CLK);
    }

    // Disable MAX6675
    PORTD |= (1 << THERMO_CS);

    // Convert to Celsius (with offset and coefficient)
    return (0.09 * (value - 793));
}

int main(void) {
     // Setup pins as output
    DDRD |= (1 << MAX487REDE_PIN) | (1 << OPTO_PIN) | (1 << THERMO_CS) | (1 << THERMO_CLK);

    // Basic UART initialization
    UBRR0H = 0;
    UBRR0L = 103;  // 9600 baud
    UCSR0B = (1 << TXEN0);  // Enable transmitter


    float t;
    while (1) {
       // Read temperature
        t = read_temperature();

      // Send temperature via UART
        PORTD &= ~(1 << MAX487REDE_PIN);
       _delay_us(1);
        int i;
      for(i=0; i < sizeof(float); ++i) {
            while (!(UCSR0A & (1 << UDRE0)));
            UDR0 = ((char*)&t)[i];
        }
        PORTD |= (1 << MAX487REDE_PIN);


        // Control the optocoupler based on temperature
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