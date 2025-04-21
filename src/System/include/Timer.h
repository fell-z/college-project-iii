#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

enum STATUS
{
  OK,
  ERR,
  FULL
};

typedef void (*callback)();

struct Task
{
  char *name;
  callback fn;
  unsigned long timeout;
  unsigned long last_timeout;
};

class Timer
{
  private:
    const uint8_t _max_tasks;
    uint8_t _n_tasks = 0;
    Task *_tasks; 

    int8_t find(const char* name);

  public:
    Timer(uint8_t n_tasks);
    ~Timer();

    enum STATUS add(const char* name, callback fn, unsigned long timeout);
    enum STATUS remove(const char* name);
    enum STATUS set_timeout(const char* name, unsigned long timeout);
    enum STATUS set_callback(const char* name, callback fn);
    void tick();
};

#endif
