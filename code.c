#define F_CPU 7372800UL
#include <avr/io.h>
#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include <util/delay.h>
#include <string.h>
#define PIR_Input PINC

#define SERVO_PIN PA4

char keypad[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

char password[5];
char inputPassword[5];
int position = 0;
char MASTER[5] = {'0', '0', '0', '0', '\0'};

/*
	States:
	0 - Show menu (default state
	1 - Password change - start
	2 - Password change - end
	3 - Enter password to open ramp

*/
int state = 0;

int passwordCheck(){
	if (strcmp(MASTER, inputPassword) == 0 || (strcmp(password, inputPassword) == 0)) {
		return 1;
	}
	else {
		return 0;
	}
}

/*
	Unos sifre
	
	1. Unos 4 znaka
	Nakon unosa 4 znaka:
	2.1 Ako je checkPassword 1, provjerava se ispravnost i vraca 1/0 ovisno o tocnosti
	2.2 Ako je checkPassword 0, vraca 1 (zavrsen unos)
*/
int passwordInput(int i, int j, int checkPassword){
	if (position < 4)
	{
		lcd_gotoxy(position, 1);
    	lcd_putc('*');
    	inputPassword[position++] = keypad[j][i];
		return 2;
	}
	else if (checkPassword == 1)
	{
		position = 0;
		lcd_clrscr();
		return passwordCheck();
	}
	else
	{
		position = 0;
		lcd_clrscr();
		return 1;
	}
}

void showMenu(){
    lcd_clrscr();
    lcd_puts("A - upis loznike\nD - nova lozinka");
}

void newPasswordSuccess(){
    lcd_puts("Dobar master\nUpis nove sifre");
    _delay_ms(2000);
    lcd_clrscr();
	lcd_puts("Nova lozika:");
	
	state = 2;
}

void newPasswordFail(){
    lcd_puts("Ne valja master\nPokusajte opet");
    _delay_ms(2000);

	state = 0;
}

void newPasswordSet(){
	lcd_puts("Nova lozinka :");
	lcd_gotoxy(0, 1);
    strcpy(password, inputPassword);
	lcd_puts(password);
	_delay_ms(2000);

	state = 0;
}

void setServoPosition(int position) {
	// Calculate delay time based on position
	int delay = (position * 10) + 600;
	// Set servo pin as output
	DDRA |= (1 << SERVO_PIN);
	// Send servo pulse
	PORTA |= (1 << SERVO_PIN);
	_delay_loop_2(delay);
	PORTA &= ~(1 << SERVO_PIN);
	_delay_ms(1000);
}

void passwordSuccess(){
	lcd_puts("Ispravna lozinka");
	_delay_ms(2000);
	setServoPosition(180);
	// Set servo to 135 degrees
	_delay_ms(5000);
	setServoPosition(30);
	_delay_ms(5000);

	state = 0;
}

void passwordFail(){
	lcd_puts("Ne valja");
	_delay_ms(5000);

	state = 0;
}

int main(void) {
	DDRB = 0xF0; // Set PINA[7:4] as output and PINA[3:0] as input
	PORTB = 0xFF; // Enable pull-up resistors on PINA[3:0]
	
	DDRC = 0x00;   /* Set the PIR port as input port */
	
	DDRD = _BV(4);

	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 10;

	lcd_init(LCD_DISP_ON); // Initialize LCD

	showMenu();

	while (1) {
		for (int i = 0; i < 4; i++) { // Loop through the columns
			PORTB = ~(1 << (i + 4)); // Set the current column to 0 and others to 1
			for (int j = 0; j < 4; j++) { // Loop through the rows
				if (!(PINB & (1 << j))) { // Check if the current button is pressed
					
					if (keypad[j][i] == 'D') {
						lcd_clrscr();
						lcd_puts("Master lozinka:");
						state = 1;
					}

					if (keypad[j][i] == 'A') {
						lcd_clrscr();
						lcd_puts("Upisi lozinku:");
						state = 3;
					}
					
					int passCorrect;
					switch (state)
					{
						
						//Password change - start
						case 1:
							passCorrect = passwordInput(i, j, 1);
							if (passCorrect == 1)
							{
								newPasswordSuccess();
							} 
							else if (passCorrect == 0)
							{
								newPasswordFail();
							}
							break;

						//Password change - end
						case 2:
							passCorrect = passwordInput(i, j, 0);
							if (passCorrect == 1)
							{
								newPasswordSet();
							}
							break;

						//Enter password to open ramp
						case 3:
							passCorrect = passwordInput(i, j, 1);
							if (passCorrect == 1)
							{
								passwordSuccess();
							} 
							else if (passCorrect == 0)
							{
								passwordFail();
							}
							break;

						default:
							showMenu();
							break;
					}
				
					_delay_ms(500);
				}
			}
		}
		
		if (PIR_Input & (1<<0)) {
			_delay_ms(2000);
			setServoPosition(180);
			// Set servo to 135 degrees
			_delay_ms(5000);
			setServoPosition(30);
		}
		
	}
	return 0;
}
