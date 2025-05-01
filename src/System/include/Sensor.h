#ifndef SENSOR_H
#define SENSOR_H

/**
 *  This is a abstract class and has no purpose other than inheriting from it.
 */
class Sensor
{
  public:
    virtual void begin() = 0;

    virtual bool detect_object() = 0;
};

#endif
