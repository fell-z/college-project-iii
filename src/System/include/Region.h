#ifndef REGION_H
#define REGION_H

#include "Arduino.h"
#include "Sensor.h"

class Region
{
	private:
		Sensor** _sensors;
		size_t _n;

		float _ratio;
	
	public:
		Region(Sensor** sensors, size_t n);

		void begin();
		void update();
		float ratio();
};

#endif
