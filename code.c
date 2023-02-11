#define F_CPU 7372800UL  // CPU clock for delay
#include <avr/io.h>
#include "lcd.h"
#include <string.h>		// included all necessary libraries for normal function of system
#include <stdio.h>
#include <util/delay.h>

#define MOTION_SENZOR PINC	// defined port for motion sensor
#define SERVO_PIN PA4		// defined port for servo and buzzer
#define BUZZER_PIN PA3

char keypad[4][4] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},	// 2d array equal to values on keypad
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};

char newPassword[5];	//array of characters for storing new password
char password[5];		// array of characters used for checking if the password matches to the one saved in the system or master password
int position = 0;		//storing values of keys form keypad on designated position in an array of characters
int flagChangePass = 0;		//used for creating new password after validating master password 
int flagWritePass = 0;		//used for validating entered password in order to open ramp after pushing 'A' key on keypad
int flagMaster = 0;		//used for validating master password before creating a new one, it is set on 1 after pushing key 'D' on keypad
char MASTER[5] = {'0', '0', '0', '0', '\0'}; // predefined value of master password


int tryCounter = 3;		//counter of invalid password inputs

/*
Function to create buzzing sound
*/

void call_buzzer(int time){
	if(!(PORTA & _BV(BUZZER_PIN))) return;
	
	PORTA &= ~_BV(BUZZER_PIN);
	
	while (time > 0){
		_delay_ms(1);
		time--;
	}
	
	PORTA |= _BV(BUZZER_PIN);
}

/*
Function that generates specific delay based on value received as an argument with which a delay is created to move the ramp to a certain position.
*/

void setServoPosition(int position) {
	int delay = (position * 20) + 900;
	DDRA |= (1 << SERVO_PIN);
	PORTA |= (1 << SERVO_PIN);
	_delay_loop_2(delay);
	PORTA &= ~(1 << SERVO_PIN);
	_delay_ms(1000);
}

int main(void) {

	DDRB = 0xF0;
	PORTB = 0xFF;
	DDRC = 0x00;
	DDRA |= (1 << BUZZER_PIN);
	DDRD = _BV(4);
	
	PORTA |= (1 << BUZZER_PIN);

	TCCR1A = _BV(COM1B1) | _BV(WGM10);
	TCCR1B = _BV(WGM12) | _BV(CS11);
	OCR1B = 40;

	lcd_init(LCD_DISP_ON);

	lcd_clrscr();
	lcd_gotoxy(0, 0);
	lcd_puts("A - upis loznike\nD - nova lozinka");


	while (1) {
		for (int i = 0; i < 4; i++) {   // go trough rows and collumns with 2 while loops
			PORTB = ~(1 << (i + 4)); // Set the current column to 0 and others to 1
			for (int j = 0; j < 4; j++) { // Loop through the rows
				if (!(PINB & (1 << j))) { // Check if the current button is pressed
					call_buzzer(50); //call buzzer for every pressed key
					if (keypad[j][i] == 'D' && flagMaster == 0) { //enter master pass
						lcd_clrscr();
						lcd_clrscr();
						lcd_gotoxy(0, 0);
						lcd_puts("Master lozinka:");
						flagMaster = 1;
					}
					
					if (keypad[j][i] != 'D' && flagMaster && flagChangePass == 0) { //when master is good, you can change current password
						if (position < 4) {
							lcd_gotoxy(position, 1);
							lcd_putc('*');
							password[position++] = keypad[j][i];
							if (position == 4) {
								lcd_clrscr();
								lcd_clrscr();
								lcd_gotoxy(0, 0);
								if (strcmp(MASTER, password) == 0) {
									tryCounter = 3;
									lcd_puts("Dobar master\nUpis nove sifre");
									PORTA |= _BV(BUZZER_PIN);
									flagChangePass = 1;
									flagMaster = 0;
									_delay_ms(2000);
									lcd_clrscr();
									lcd_clrscr();
									lcd_puts("Nova lozika:");
									position = 0;
									break;
								}
								else { //if password is not correct on "change passwod"
									tryCounter--; // we use try counter to keep track of trys that were used to change the password
									
									if(tryCounter == 0){
										PORTA &= ~_BV(BUZZER_PIN);
										
									}
									
									lcd_puts("Ne valja master\nPokusajte opet");
									_delay_ms(2000);
									lcd_clrscr();
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
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("Nova lozinka :");
							lcd_gotoxy(0, 1);
							lcd_puts(newPassword);
							
							_delay_ms(2000);
							lcd_clrscr();
							lcd_clrscr();
							lcd_gotoxy(0, 0);
							lcd_puts("A - upis loznike\nD - nova lozinka");
						}
					}
					
					if (keypad[j][i] == 'A') {
						lcd_clrscr();
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
								lcd_clrscr();
								lcd_gotoxy(0, 0);
								if (strcmp(MASTER, password) == 0 || strcmp(password, newPassword) == 0) {
									tryCounter = 3;
									PORTA |= _BV(BUZZER_PIN);
									lcd_puts("Ispravna lozinka");
									_delay_ms(100);
									call_buzzer(100);
									_delay_ms(100);
									call_buzzer(100);
									_delay_ms(100);
									call_buzzer(100);
									_delay_ms(2000);
									setServoPosition(90);
									_delay_ms(5000);
									setServoPosition(0);
								}
								else {
									lcd_puts("Kriva lozinka\nProbajte opet");
									
									tryCounter--;
									
									if(tryCounter == 0){
										PORTA &= ~_BV(BUZZER_PIN);
										_delay_ms(2000);
										lcd_clrscr();
										lcd_clrscr();
										lcd_puts("3 kriva unosa\nupisi lozinku");
									}
								}
								position = 0;
								flagWritePass = 0;
								_delay_ms(5000);
								lcd_clrscr();
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
		
		if (MOTION_SENZOR & (1<<0)) {
			_delay_ms(2000); //wait 2 seconds before pulling the ramp up
			setServoPosition(90);//pull ramp up
			_delay_ms(5000);//wait while ramp is up and car passes through
			setServoPosition(0); //return ramp to initial state
		}
		
	}
	return 0;
}
