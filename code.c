#define F_CPU 7372800UL
#include <avr/io.h>
#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>

char keypad[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

char password[20];
int position = 0;
int flagInput = 0;
char pos[20];

int main(void) {
	DDRB = 0xF0; // Set PINA[7:4] as output and PINA[3:0] as input
	PORTB = 0xFF; // Enable pull-up resistors on PINA[3:0]
	DDRD = _BV(4);

	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 10;

	lcd_init(LCD_DISP_ON); // Initialize LCD

	while (1) {
		for (int i = 0; i < 4; i++) { // Loop through the columns
			PORTB = ~(1 << (i + 4)); // Set the current column to 0 and others to 1
			for (int j = 0; j < 4; j++) { // Loop through the rows
				if (!(PINB & (1 << j))) { // Check if the current button is pressed
					
					if (flagInput && position < 4)
					{
						password[position++] = keypad[j][i];
					}
					if (flagInput && position == 4)
					{
						lcd_clrscr(); // Clear LCD
						lcd_gotoxy(0, 0);
						sprintf(pos, "Password set to:\n%s", password);
						lcd_puts(pos); // Print the pressed key on LCD
						flagInput = 0;
						position = 0;
					}

					
					if (keypad[j][i] == 'D' && flagInput == 0) { // Check if "D" key is pressed
						lcd_clrscr();
						lcd_puts("Enter pass");
						flagInput = 1;
					}
					 
					_delay_ms(500);
				}
			}
		}
	}
	return 0;
}
