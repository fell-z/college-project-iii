#include "include/Region.h"

Region::Region(Sensor** sensors, size_t n)
{
  _sensors = sensors;
  _n = n;
}

void Region::begin()
{
  for (int i = 0; i < _n; ++i)
  {
    _sensors[i]->begin();
  }
}

void Region::update()
{
  float sum = 0;

  for (int i = 0; i < _n; ++i)
  {
    sum += _sensors[i]->ratio();
  }

  _ratio = sum / _n;
}

float Region::ratio()
{
  return _ratio;
}
