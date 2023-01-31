#define F_CPU 7372800UL
#include <avr/io.h>
#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>

char keypad[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

char newPassword[5];
char password[5];
int position = 0;
int flagChangePass = 0; // 0 -> inicijalizacija i otvaranje vrata (A), 1 -> promjena sifre
int flagWritePass = 0;
int flagMaster = 0;
char pos[20];
char MASTER[5] = {'0', '0', '0', '0', '\0'};

int main(void) {
	DDRB = 0xF0; // Set PINA[7:4] as output and PINA[3:0] as input
	PORTB = 0xFF; // Enable pull-up resistors on PINA[3:0]
	DDRD = _BV(4);

	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 10;

	lcd_init(LCD_DISP_ON); // Initialize LCD

	lcd_clrscr();
	lcd_gotoxy(0, 0);
	lcd_puts("A - upis loznike\nD - nova lozinka");


	while (1) {
		for (int i = 0; i < 4; i++) { // Loop through the columns
			PORTB = ~(1 << (i + 4)); // Set the current column to 0 and others to 1
			for (int j = 0; j < 4; j++) { // Loop through the rows
				if (!(PINB & (1 << j))) { // Check if the current button is pressed
					
					if (keypad[j][i] == 'D' && flagMaster == 0) {
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Master lozinka:");
						flagMaster = 1;
					}
					
					if (keypad[j][i] != 'D' && flagMaster && flagChangePass == 0) {
						if (position < 4) {
							lcd_gotoxy(position, 1);
							lcd_putc('*');
							password[position++] = keypad[j][i];
							if (position == 4) {
								lcd_clrscr();
								lcd_gotoxy(0, 0);
								if (strcmp(MASTER, password) == 0) {
									lcd_puts("Dobar master\nUpis nove sifre");
									flagChangePass = 1;
									flagMaster = 0;
									_delay_ms(2000);
									lcd_clrscr();
									lcd_puts("Nova lozika:");
									position = 0;
									break;
								}
								else {
									lcd_puts("Ne valja master\nPokusajte opet");
									_delay_ms(2000);
									lcd_clrscr();
									lcd_puts("A - upis loznike\nD - nova lozinka");
									flagMaster = 0;
									flagChangePass = 0;
									position = 0;
									break;
								}
							}
						}
					}
					
					if (keypad[j][i] != 'D' && flagChangePass && flagMaster == 0) {
						lcd_gotoxy(position, 1);
						lcd_putc(keypad[j][i]);
						newPassword[position++] = keypad[j][i];
						if (position == 4) {
							position = 0;
							flagChangePass = 0;
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("Nova lozinka :");
							lcd_gotoxy(0, 1);
							lcd_puts(newPassword);
							
							_delay_ms(2000);
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("A - upis loznike\nD - nova lozinka");
						}
					}
					
 					if (keypad[j][i] == 'A') {
						 lcd_clrscr();
						 lcd_gotoxy(0, 0);
						 lcd_puts("Upisi lozinku:");
						 flagWritePass = 1;
					}
					if (keypad[j][i] != 'A' && flagWritePass) {
						if (position < 4) {
							lcd_gotoxy(position, 1);
							lcd_putc('*');
							password[position++] = keypad[j][i];
							if (position == 4) {
								lcd_clrscr();
								lcd_gotoxy(0, 0);
								if (strcmp(MASTER, password) == 0 || strcmp(password, newPassword) == 0) {
									lcd_puts("Dobra lozinka");
								}
								else {
									lcd_puts("Ne valja");
								}
								position = 0;
								flagWritePass = 0;
								_delay_ms(5000);
								lcd_clrscr();
								lcd_gotoxy(0, 0);
								lcd_puts("A - upis loznike\nD - nova lozinka");
							}
						}
					}
					
					_delay_ms(500);
				}
			}
		}
	}
	return 0;
}
