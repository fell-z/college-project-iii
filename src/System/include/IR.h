#ifndef IR_H
#define IR_H

#include "Arduino.h"
#include "Sensor.h"

class IR : public Sensor
{
	private:
		const uint8_t _data_pin;

	public:
		IR(uint8_t data_pin);

		void begin();
		uint8_t read();
    bool detect_object();

    float ratio();
};

#endif
