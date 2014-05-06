#include "FL7Dl.h"

FL7Dl::FL7Dl()
{
    pinMode(14, SINK);
    pinMode(15, SINK);
    pinMode(16, SINK);
    pinMode(17, SINK);
	pinMode(3, SOURCE);
	pinMode(4, SOURCE);
	pinMode(5, SOURCE);
	pinMode(6, SOURCE);
	pinMode(7, SOURCE);
	pinMode(8, SOURCE);
	pinMode(9, SOURCE);
	pinMode(10, SOURCE);
	PORTC |= B00001111;
	PORTD &= B00000111;
	PORTB &= B11111000;
    _on = ON;
	_ticker = OFF;
	_alarm = OFF;
	_green = OFF;
	_yellow = OFF;
    delay = DEFAULT_DELAY;
}

void FL7Dl::setBrightness(int brite) {
	delay = brite;
}

void FL7Dl::showTime(int time) {
	if (_on) {
		for (int di = 3; di >= 0; di--) {
			show(di, time % 10);
			time /= 10;
		}
	}
}

void FL7Dl::off() {
	_on = OFF;
}

void FL7Dl::on() {
	_on = ON;
}

void FL7Dl::allOff() {
	PORTC |= B00001111;
	PORTD &= B00000111;
	PORTB &= B11111000;
}

void FL7Dl::showMinutes(int minutes) {
	if (_on) {
		for (int di=3; di>=2; di--) {
			show(di, minutes % 10);
			minutes /= 10;
		}
	}
}


void FL7Dl::showHour(int hour) {
	if (_on) {
		for (int di=1; di>=0; di--) {
			show(di, hour % 10);
			hour /= 10;
		}
	}
}

void FL7Dl::test() {
}

void FL7Dl::tick() {
	_ticker = !_ticker;
}

void FL7Dl::tick(boolean to) {
	_ticker = to;
}

void FL7Dl::alarm(boolean to) {
	_alarm = to;
}

void FL7Dl::yellow(boolean to) {
	_yellow = to;
}

void FL7Dl::green(boolean to) {
	_green = to;
}

void FL7Dl::show(const int dig, int num) {
	if (_on) {
	PORTC &= ~(1<<dig);
	int n = 0;
	switch (dig) {
		case 3:
		PORTB |= _yellow << 2;
		n+=(_yellow)?250/delay:0;
		break;
		case 2:
		PORTB |= _green << 2;
		n+=(_green)? 250/delay :0;
		break;
		case 1:
		PORTB |= _ticker << 2;
		n+=(_ticker)? 250/delay :0;
		break;
		case 0:
		PORTB |= _alarm << 2;
		n+=(_alarm) ? 250/delay :0;
		break;
	}
	switch (num) {
		case 0:
		PORTD |= B11111000;
		PORTB |= B00000001;
		n += 6 * 250/delay;
		break;
		case 1:
		PORTD |= B00110000;
		n += 1 * 250/delay;
		break;
		case 2:
		PORTD |= B11011000;
		PORTB |= B00000010;
		n += 5 * 250/delay;
		break;
		case 3:
		PORTD |= B01111000;
		PORTB |= B00000010;
		n += 5 * 250/delay;
		break;
		case 4:
		PORTD |= B00110000;
		PORTB |= B00000011;
		n += 4 * 250/delay;
		break;
		case 5:
		PORTD |= B01101000;
		PORTB |= B00000011;
		n += 5 * 250/delay;
		break;
		case 6:
		PORTD |= B11101000;
		PORTB |= B00000011;
		n += 7 * 250/delay;
		break;
		case 7:
		PORTD |= B00111000;
		n += 3 * 250/delay;
		break;
		case 8:
		PORTD |= B11111000;
		PORTB |= B00000011;
		n += 7 * 250/delay;
		break;
		case 9:
		PORTD |= B01111000;
		PORTB |= B00000011;
		n += 6 * 250/delay;
		break;
		default:
		PORTB |= B00000010;
		n += 1 * 250/delay;
		break;
	}
	delayMicroseconds(n*delay/(5*500/delay));
	allOff();
	}
}

void FL7Dl::showString(char * str) {
	char str2[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	switch (strlen(str)) {
		case 4:
		str2[0] = str[3];
		case 3:
		str2[1] = str[2];
		case 2:
		str2[2] = str[1];
		case 1:
		str2[3] = str[0];
		break;
	}
	for (int i = 3; i >= 0; i--) {
		PORTC &= ~(1<<i);
		switch (str2[3-i]) {
			case 'a':
			PORTD |= B10111000;
			PORTB |= B00000011;
			break;
			case 'b':
			PORTD |= B11100000;
			PORTB |= B00000011;
			break;
			case 'c':
			PORTD |= B11000000;
			PORTB |= B00000010;
			break;
			case 'd':
			PORTD |= B11110000;
			PORTB |= B00000010;
			break;
			case 'e':
			PORTD |= B11001000;
			PORTB |= B00000011;
			break;
			case 'f':
			PORTD |= B10001000;
			PORTB |= B00000011;
			break;
			case 'g':
			PORTD |= B01111000;
			PORTB |= B00000011;
			break;
			case 'h':
			PORTD |= B10100000;
			PORTB |= B00000011;
			break;
			case 'i':
			PORTD |= B00100000;
			break;
			case 'j':
			PORTD |= B11110000;
			break;
			case 'l':
			PORTD |= B11000000;
			PORTB |= B00000001;
			break;
			case 'n':
			PORTD |= B10100000;
			PORTB |= B00000010;
			break;
			case 'o':
			PORTD |= B11100000;
			PORTB |= B00000010;
			break;
			case 'p':
			PORTD |= B10011000;
			PORTB |= B00000011;
			break;
			case 'q':
			PORTD |= B00111000;
			PORTB |= B00000011;
			break;
			case 'r':
			PORTD |= B10000000;
			PORTB |= B00000010;
			break;
			case 's':
			PORTD |= B01101000;
			PORTB |= B00000011;
			break;
			case 't':
			PORTD |= B11000000;
			PORTB |= B00000011;
			break;
			case 'u':
			PORTD |= B11110000;
			PORTB |= B00000001;
			break;
			case 'v':
			PORTD |= B11100000;
			break;
			case 'y':
			PORTD |= B01110000;
			PORTB |= B00000011;
			break;
			case 'z':
			PORTD |= B11011000;
			PORTB |= B00000010;
			break;
			case 0xFF:
			allOff();
			default:
			PORTB |= B00000010;
			break;
		}
		delayMicroseconds(delay);
		allOff();
	}
}
