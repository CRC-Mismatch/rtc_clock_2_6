#ifndef FL7Dl_H
#define FL7Dl_H

#include <Arduino.h>

#define DEFAULT_DELAY 500

#define SINK OUTPUT
#define SOURCE OUTPUT
#define VCC true
#define GND false
#define ON true
#define OFF false

class FL7Dl
{
    public:
        FL7Dl();
        void showTime(int time);
        void showHour(int hour);
        void showMinutes(int minutes);
        void show(const int dig, int num);
        void tick();
		void tick(boolean to);
        void test();
        void on();
        void off();
        void allOff();
        void setBrightness(int brite);
        void showString(char * str);
	void alarm(boolean to);
	void green(boolean to);
	void yellow(boolean to);
    protected:
    private:
        boolean _on;
        boolean _ticker;
	boolean _alarm;
	boolean _green;
	boolean _yellow;
        int delay;
};

#endif // FL7Dl_H
