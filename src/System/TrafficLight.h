#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "Arduino.h"

typedef enum { RED_STAGE, YELLOW_STAGE, GREEN_STAGE } TL_Stage;

class TrafficLight
{
	private:
		uint8_t _red_pin;
		uint8_t _yellow_pin;
		uint8_t _green_pin;

    unsigned long _min_red_time;
    unsigned long _min_yellow_time;
    unsigned long _min_green_time;

		unsigned long _red_time;
		unsigned long _yellow_time;
		unsigned long _green_time;

		unsigned long _old_red_time;
		unsigned long _old_yellow_time;
		unsigned long _old_green_time;
    bool _still_old_time = false;

		unsigned long _last_update_time = millis();
		TL_Stage _stage;

	public:
    static unsigned long calculate_time_by_flow(unsigned long base_time, float flow_intensity)
    {
      return base_time + (base_time / 2) * flow_intensity;
    }

		TrafficLight(uint8_t red_pin, uint8_t yellow_pin, uint8_t green_pin,
								 unsigned long red_time, unsigned long yellow_time,
								 unsigned long green_time, TL_Stage initial_stage)
    {
      _red_pin = red_pin;
      _yellow_pin = yellow_pin;
      _green_pin = green_pin;

      _min_red_time = _red_time = red_time;
      _min_yellow_time = _yellow_time = yellow_time;
      _min_green_time = _green_time = green_time;

      _stage = initial_stage;
    }

		void begin()
    {
      pinMode(_red_pin, OUTPUT);
      pinMode(_yellow_pin, OUTPUT);
      pinMode(_green_pin, OUTPUT);

      digitalWrite(_red_pin, LOW);
      digitalWrite(_yellow_pin, LOW);
      digitalWrite(_green_pin, LOW);

      set_stage(_stage);
    }

    bool is_doable_red_time(unsigned long red_time)
    {
      return red_time >= _min_red_time;
    }

    void set_new_time_for(TL_Stage stage, unsigned long time)
    {
      switch (stage)
      {
        case RED_STAGE:
          _old_red_time = _red_time;
          _red_time = time;
          break;

        case YELLOW_STAGE:
          _old_yellow_time = _yellow_time;
          _yellow_time = time;
          break;

        case GREEN_STAGE:
          _old_green_time = _green_time;
          _green_time = time;
      }

      if (stage == _stage)
      {
        _still_old_time = true;
      }
    }

    unsigned long get_min_time_for(TL_Stage stage)
    {
      unsigned long time = 0;

      switch (stage)
      {
        case RED_STAGE:
          time = _min_red_time;
          break;

        case YELLOW_STAGE:
          time = _min_yellow_time;
          break;

        case GREEN_STAGE:
          time = _min_green_time;
      }

      return time;
    }

    unsigned long get_time_for(TL_Stage stage)
    {
      unsigned long time = 0;

      switch (stage)
      {
        case RED_STAGE:
          time = _red_time;
          break;

        case YELLOW_STAGE:
          time = _yellow_time;
          break;

        case GREEN_STAGE:
          time = _green_time;
      }

      return time;
    }

		void set_stage(TL_Stage stage)
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
      _last_update_time = millis();
      _stage = stage;
    }

    TL_Stage get_stage()
    {
      return _stage;
    }

    void next_stage()
    {
      switch (_stage)
      {
        case RED_STAGE:
          set_stage(GREEN_STAGE);
          break;
        case YELLOW_STAGE:
          set_stage(RED_STAGE);
          break;
        case GREEN_STAGE:
          set_stage(YELLOW_STAGE);
      }
    }

		bool tick()
    {
      bool has_changed = true;
      
      unsigned long time;

      switch (_stage)
      {
        case RED_STAGE:
          time = (_still_old_time) ? _old_red_time : _red_time;

          if (millis() - _last_update_time >= time)
          {
            set_stage(GREEN_STAGE);
          }
          else
          {
            has_changed = false;
          }
          break;

        case YELLOW_STAGE:
          time = (_still_old_time) ? _old_yellow_time : _yellow_time;

          if (millis() - _last_update_time >= time)
          {
            set_stage(RED_STAGE);
          }
          else
          {
            has_changed = false;
          }
          break;

        case GREEN_STAGE:
          time = (_still_old_time) ? _old_green_time : _green_time;

          if (millis() - _last_update_time >= time)
          {
            set_stage(YELLOW_STAGE);
          }
          else
          {
            has_changed = false;
          }
      }

      if (has_changed && _still_old_time)
      {
        _still_old_time = false;
      }

      return has_changed;
    }
};

#endif
