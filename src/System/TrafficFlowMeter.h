#ifndef TRAFFIC_FLOW_METER_H
#define TRAFFIC_FLOW_METER_H

#include <Arduino.h>

class TrafficFlowMeter
{
  private:
    uint8_t _bottom_led;
    uint8_t _middle_bottom_led;
    uint8_t _middle_top_led;
    uint8_t _top_led;

  public:
    TrafficFlowMeter(uint8_t bottom_led, uint8_t middle_bottom_led,
                     uint8_t middle_top_led, uint8_t top_led)
    {
      _bottom_led = bottom_led;
      _middle_bottom_led = middle_bottom_led;
      _middle_top_led = middle_top_led;
      _top_led = top_led;
    }

    // the ratio is between a possibly longer updated time and a minimum original setted time.
    // e.g. 20s / 10s = 2, 10s being the traffic lights initial time.
    void update(float traffic_light_orig_ratio)
    {
      int tlro = (int)(traffic_light_orig_ratio * 100);

      if (tlro <= 100)
      {
        digitalWrite(_bottom_led, LOW);
        digitalWrite(_middle_bottom_led, LOW);
        digitalWrite(_middle_top_led, LOW);
        digitalWrite(_top_led, LOW);
      }
      else if (tlro <= 150)
      {
        digitalWrite(_bottom_led, HIGH);
        digitalWrite(_middle_bottom_led, LOW);
        digitalWrite(_middle_top_led, LOW);
        digitalWrite(_top_led, LOW);
      }
      else if (tlro <= 200)
      {
        digitalWrite(_bottom_led, LOW);
        digitalWrite(_middle_bottom_led, HIGH);
        digitalWrite(_middle_top_led, LOW);
        digitalWrite(_top_led, LOW);
      }
      else if (tlro <= 250)
      {
        digitalWrite(_bottom_led, LOW);
        digitalWrite(_middle_bottom_led, LOW);
        digitalWrite(_middle_top_led, HIGH);
        digitalWrite(_top_led, LOW);
      }
      else if (tlro > 250)
      {
        digitalWrite(_bottom_led, LOW);
        digitalWrite(_middle_bottom_led, LOW);
        digitalWrite(_middle_top_led, LOW);
        digitalWrite(_top_led, HIGH);
      }
    }
};

#endif
