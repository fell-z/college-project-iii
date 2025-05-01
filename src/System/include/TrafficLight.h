#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "Arduino.h"

enum STAGE { RED_STAGE, YELLOW_STAGE, GREEN_STAGE };

class TrafficLight
{
	private:
		uint8_t _red_pin;
		uint8_t _yellow_pin;
		uint8_t _green_pin;

		unsigned long _red_time;
		unsigned long _yellow_time;
		unsigned long _green_time;

		unsigned long _last_timeout = millis();
    enum STAGE _last_stage;
		enum STAGE _stage = RED_STAGE;

	public:
		TrafficLight(uint8_t red_pin, uint8_t yellow_pin, uint8_t green_pin,
								 unsigned long red_time, unsigned long yellow_time,
								 unsigned long green_time)
    {
      _red_pin = red_pin;
      _yellow_pin = yellow_pin;
      _green_pin = green_pin;

      _red_time = red_time;
      _yellow_time = yellow_time;
      _green_time = green_time;
    }

		void begin()
    {
      pinMode(_red_pin, OUTPUT);
      pinMode(_yellow_pin, OUTPUT);
      pinMode(_green_pin, OUTPUT);

      digitalWrite(_red_pin, HIGH);
      digitalWrite(_yellow_pin, LOW);
      digitalWrite(_green_pin, LOW);
    }

		void set_new_green_time(unsigned long green_time)
    {
      _green_time = green_time;
    }

		void set_stage(enum STAGE stage)
    {
      switch (stage)
      {
        case RED_STAGE:
          digitalWrite(_red_pin, HIGH);
          digitalWrite(_yellow_pin, LOW);
          digitalWrite(_green_pin, LOW);
          break;

        case YELLOW_STAGE:
          digitalWrite(_red_pin, LOW);
          digitalWrite(_yellow_pin, HIGH);
          digitalWrite(_green_pin, LOW);
          break;

        case GREEN_STAGE:
          digitalWrite(_red_pin, LOW);
          digitalWrite(_yellow_pin, LOW);
          digitalWrite(_green_pin, HIGH);
      }
      _last_timeout = millis();
      _last_stage = _stage;
      _stage = stage;
    }

    enum STAGE get_last_stage()
    {
      return _stage;
    }

    enum STAGE get_stage()
    {
      return _stage;
    }

		void tick()
    {
      switch (_stage)
      {
        case RED_STAGE:
          if (millis() - _last_timeout >= _red_time)
          {
            set_stage(GREEN_STAGE);
          }
          break;

        case YELLOW_STAGE:
          if (millis() - _last_timeout >= _yellow_time)
          {
            set_stage(RED_STAGE);
          }
          break;

        case GREEN_STAGE:
          if (millis() - _last_timeout >= _green_time)
          {
            set_stage(YELLOW_STAGE);
          }
      }
    }
};

#endif
