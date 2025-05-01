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
    float _detection_distance;
    volatile PulseData _pd;

    const uint8_t _trigger_pin;
    const volatile uint8_t _echo_pin;

  public:
    /**
     *  Constructor for Ultrasonic.
     *  @param trigger_pin The pin connected to the trigger of the sensor.
     *  @param echo_pin The pin connected to the echo of the sensor.
     */
    Ultrasonic(uint8_t trigger_pin, uint8_t echo_pin, float detection_distance = 400.0)
      : _trigger_pin(trigger_pin), _echo_pin(echo_pin)
    {
      _pd.start_time = micros();
      _pd.end_time = micros();
      _pd.duration_available = false;

      _detection_distance = detection_distance;
    }

    /**
     *  Converts a duration to a distance.
     *  @param duration The duration in microseconds.
     *  @returns The distance in centimeters.
     */
    float duration_to_distance_cm(unsigned long duration)
    {
      return (duration * _speed_of_sound) / 2.0;
    }

    /**
     *  Converts a distance to a duration.
     *  @param distance The distance in centimeters.
     *  @returns The duration in microseconds.
     */
    unsigned long distance_cm_to_duration(float distance)
    {
      return (unsigned long)((distance * 2.0) / _speed_of_sound);
    }

    /**
     *  @note This is special method that should be called in a function passed to an attachInterrupt()
     *  call, lambdas can also be used.
     *  @see https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/
     *  for more information.
     *  @warning It doesn't make sense to use this outside of an ISR.
     */
    void read()
    {
      if (digitalRead(_echo_pin) == HIGH)
        _pd.start_time = micros();
      else
      {
        _pd.end_time = micros();
        _pd.duration_available = true;
      }
    }

    /**
     *  Initializes the pins to the appropriate mode and sets the max distance.
     *  @param max_distance The max value for methods like new_distance_cm().
     *  @note Leave @a max_distance unsetted for no distance limit.
     */
    void begin()
    {
      pinMode(_trigger_pin, OUTPUT);
      pinMode(_echo_pin, INPUT);

      digitalWrite(_trigger_pin, LOW);
    }

    /**
     *  Sends a pulse to activate the sensor transmitter.
     *  @note Calling this produces an inevitable delay of 10 microseconds.
     */
    void trigger()
    {
      digitalWrite(_trigger_pin, HIGH);
      delayMicroseconds(10); // necessary 10us time of signal HIGH specified by HC-SR04
      digitalWrite(_trigger_pin, LOW);
    }

    void set_detection_distance(float detection_distance)
    {
      _detection_distance = detection_distance;
    }

    /**
     *  @returns A new duration if it's available, 0 otherwise.
     */
    unsigned long new_duration()
    {
      if (_pd.duration_available)
      {
        _pd.duration_available = false;
        return _pd.end_time - _pd.start_time;
      }
      else
      {
        return 0;
      }
    }

    float new_distance_cm()
    {
      return duration_to_distance_cm(new_duration());
    }

    bool detect_object()
    {
      return new_distance_cm() <= _detection_distance;
    }
};

#endif
