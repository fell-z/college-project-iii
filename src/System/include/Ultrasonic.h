#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "Arduino.h"
#include "Sensor.h"

struct PulseData
{
  unsigned long start_time;
  unsigned long end_time;
  bool duration_available;
};

class Ultrasonic : public Sensor
{
  private:
    const float _speed_of_sound = 0.0343; // in cm/us with air at 20°C
    float _max_distance;
    volatile PulseData _pd;

    const uint8_t _trigger_pin;
    const volatile uint8_t _echo_pin;

  public:
    Ultrasonic(uint8_t trigger_pin, uint8_t echo_pin, float max_distance = -1.0);

    float duration_to_distance_cm(unsigned long duration);
    unsigned long distance_cm_to_duration(float distance);

    // @note must be used in an ISR function, which is passed to an attachInterrupt() call
    void read();

    void begin();
    void trigger();
    void set_max_distance(float max_distance);
    unsigned long new_duration();
    float new_distance_cm();

    float ratio();
};

#endif
