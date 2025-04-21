#ifndef SENSOR_H
#define SENSOR_H

/**
 *  This is a abstract class and has no purpose other than inheriting from it.
 *  It only has a begin() and ratio() method.
 *  @note What the ratio() return value means is up to the person making a subclass from it.
 */
class Sensor
{
  public:
    virtual void begin() = 0;
    /**
     *  Calculates a ratio between the current value and a reference/maximum value.
     *  @returns A float in the range [0.0, 1.0].
     */
    virtual float ratio() = 0;
};

#endif
