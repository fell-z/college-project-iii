#ifndef IR_H
#define IR_H

#include "Arduino.h"
#include "Sensor.h"

class IR : public Sensor
{
	private:
		const uint8_t _data_pin;

	public:
    /**
     *  Constructor for IR.
     *  @param data_pin The pin connected to the @a out of the sensor.
     */
    IR(uint8_t data_pin) : _data_pin(data_pin) {}

    /**
     *  Initializes the pin to the appropriate mode.
     */
    void begin()
    {
      pinMode(_data_pin, INPUT);
    }

    /**
     *  @returns HIGH or LOW.
     */
    uint8_t read()
    {
      return digitalRead(_data_pin);
    }

    bool detect_object();
    {
      return read() == HIGH;
    }
};

#endif
