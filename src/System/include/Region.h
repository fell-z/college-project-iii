#ifndef REGION_H
#define REGION_H

#include "Arduino.h"
#include "Sensor.h"
#include "CircularAverage.h"

class Region
{
	private:
		Sensor** _sensors;
		size_t _n;
    int16_t _detected_objects = 0;

    CircularAverage<int> _detected_objects_mean;
    size_t _samples_ndx = 0;
    size_t _samples_per_min;
    bool _new_mean = false;
    unsigned long _last_mean_update_timeout = millis();
    unsigned long _mean_update_timeout;
	
	public:
    Region(Sensor** sensors, size_t n, size_t samples_per_min) : _detected_objects_mean(samples_per_min)
    {
      _sensors = sensors;
      _n = n;
      _samples_per_min = samples_per_min;

      _mean_update_timeout = 60000 / samples_per_min;
    }

    void begin()
    {
      for (int i = 0; i < _n; ++i)
      {
        _sensors[i]->begin();
      }
    }

    void reset()
    {
      _detected_objects_mean.clear();
      _new_mean = false;
    }

    void update()
    {
      if (_new_mean == false)
      {
        if (millis() - _last_mean_update_timeout >= _mean_update_timeout)
        {
          _last_mean_update_timeout = millis();
          _detected_objects_mean.add(round(_detected_objects / (float)_n));
          _detected_objects = 0;
          _samples_ndx++;
          if (_samples_ndx == _samples_per_min)
          {
            _samples_ndx = 0;
            _new_mean = true;
          }
        }

        for (int i = 0; i < _n; ++i)
        {
          if (_sensors[i]->detect_object())
            _detected_objects++; 
        }
      }
    }

    bool new_mean()
    {
      return _new_mean;
    }

    float detected_objects_mean()
    {
      if (_new_mean == false)
        return -1.0;

      float mean = _detected_objects_mean.quick_average<float>();
      reset();
      return mean;
    }
};

#endif
