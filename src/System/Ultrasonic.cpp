#include "include/Ultrasonic.h"

/**
 *  Constructor for Ultrasonic.
 *  @param trigger_pin The pin connected to the trigger of the sensor.
 *  @param echo_pin The pin connected to the echo of the sensor.
 */
Ultrasonic::Ultrasonic(uint8_t trigger_pin, uint8_t echo_pin, float max_distance = -1.0)
  : _trigger_pin(trigger_pin), _echo_pin(echo_pin)
{
  _pd.start_time = micros();
  _pd.end_time = micros();
  _pd.duration_available = false;

  _max_distance = max_distance;
}

/**
 *  Converts a duration to a distance.
 *  @param duration The duration in microseconds.
 *  @returns The distance in centimeters.
 */
float Ultrasonic::duration_to_distance_cm(unsigned long duration)
{
  return (duration * _speed_of_sound) / 2.0;
}

/**
 *  Converts a distance to a duration.
 *  @param distance The distance in centimeters.
 *  @returns The duration in microseconds.
 */
unsigned long Ultrasonic::distance_cm_to_duration(float distance)
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
void Ultrasonic::read()
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
void Ultrasonic::begin()
{
  pinMode(_trigger_pin, OUTPUT);
  pinMode(_echo_pin, INPUT);

  digitalWrite(_trigger_pin, LOW);
}

/**
 *  Sends a pulse to activate the sensor transmitter.
 *  @note Calling this produces an inevitable delay of 10 microseconds.
 */
void Ultrasonic::trigger()
{
  digitalWrite(_trigger_pin, HIGH);
  delayMicroseconds(10); // necessary 10us time of signal HIGH specified by HC-SR04
  digitalWrite(_trigger_pin, LOW);
}

/**
 * Sets the max distance to a new one provided.
 * @param max_distance The new distance limit.
 * @note Set to -1.0 to disable the limit.
 */
void Ultrasonic::set_max_distance(float max_distance)
{
  _max_distance = max_distance;
}

/**
 *  @returns A new duration if it's available, 0 otherwise.
 */
unsigned long Ultrasonic::new_duration()
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

/**
 *  @returns The distance in centimeters if it's lower or equal the distance limit or if the limit
 *  is unset, it returns the max distance otherwise.
 *  @note It uses new_duration(), a subsequent call to new_duration() may return 0.
 */
float Ultrasonic::new_distance_cm()
{
  float distance = duration_to_distance_cm(new_duration());

  if (distance <= _max_distance || _max_distance == -1.0)
    return distance;
  else
    return _max_distance;
}

/**
 *  @copydoc Sensor::ratio()
 */
float Ultrasonic::ratio()
{
  if (_max_distance == -1.0)
    return new_distance_cm();
  else
    return new_distance_cm() / _max_distance;
}
