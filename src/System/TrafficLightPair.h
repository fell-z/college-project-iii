#ifndef TRAFFIC_LIGHT_PAIR_H
#define TRAFFIC_LIGHT_PAIR_H

#include "TrafficLight.h"

class TrafficLightPair
{
  private:
    TrafficLight _tl1;
    TrafficLight _tl2;

  public:
    static unsigned long calculate_time_by_flow(unsigned long base_time, float flow_intensity)
    {
      return (unsigned long)(base_time + (base_time / 2.0) * flow_intensity);
    }

    TrafficLightPair(uint8_t tl1_red_pin, uint8_t tl1_yellow_pin,
                     uint8_t tl1_green_pin, uint8_t tl2_red_pin,
                     uint8_t tl2_yellow_pin, uint8_t tl2_green_pin,
                     unsigned long red_time, unsigned long yellow_time,
                     unsigned long green_time, TL_Stage initial_stage)
        : _tl1(tl1_red_pin, tl1_yellow_pin, tl1_green_pin, red_time, yellow_time,
              green_time, initial_stage),
          _tl2(tl2_red_pin, tl2_yellow_pin, tl2_green_pin, red_time, yellow_time,
              green_time, initial_stage) {}

    void begin()
    {
      _tl1.begin();
      _tl2.begin();
    }

    float get_orig_time_ratio()
    {
      return get_time_for(GREEN_STAGE) / (get_min_time_for(GREEN_STAGE) * 1.0);
    }

    bool is_doable_red_time(unsigned long red_time)
    {
      return _tl1.is_doable_red_time(red_time) && _tl2.is_doable_red_time(red_time);
    }

    void set_new_time_for(TL_Stage stage, unsigned long time)
    {
      _tl1.set_new_time_for(stage, time);
      _tl2.set_new_time_for(stage, time);
    }

    unsigned long get_min_time_for(TL_Stage stage)
    {
      return _tl1.get_min_time_for(stage);
    }

    unsigned long get_time_for(TL_Stage stage)
    {
      return _tl1.get_time_for(stage);
    }

    void set_stage(TL_Stage stage)
    {
      _tl1.set_stage(stage);
      _tl2.set_stage(stage);
    }

    TL_Stage get_stage()
    {
      return _tl1.get_stage();
    }

    void next_stage()
    {
      _tl1.next_stage();
      _tl2.next_stage();
    }

    bool tick()
    {
      bool has_changed;

      // although they have the same timeouts, due to how millis() works one may return true after one returns false.
      has_changed = _tl1.tick() || _tl2.tick();

      return has_changed;
    }
};

#endif
