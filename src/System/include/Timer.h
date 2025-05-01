#ifndef TIMER_H
#define TIMER_H

#include "Arduino.h"

typedef void (*fn)();

struct Task
{
  char *name;
  fn callback;
  unsigned long timeout;
  unsigned long last_timeout;
};

class Timer
{
  private:
    const uint8_t _max_tasks;
    uint8_t _n_tasks = 0;
    Task *_tasks; 

    int8_t find_task(const char* name)
    {
      for (int i = 0; i < _max_tasks; ++i)
      {
        if (_tasks[i].name == name)
        {
          return i;
        }
      }

      return -1;
    }

  public:
    Timer(uint8_t n_tasks) : _max_tasks(n_tasks)
    {
      _tasks = new Task[n_tasks] { { NULL } };
    }

    ~Timer()
    {
      delete [] _tasks;
    }

    void every(const char* name, unsigned long timeout, fn callback)
    {
      if (_n_tasks == _max_tasks) return;

      // search a empty task to fill
      int8_t index = find_task(NULL);

      _tasks[index] = (Task){
        .name = name,
        .callback = callback,
        .timeout = timeout,
        .last_timeout = millis()
      };

      _n_tasks++;
    }

    void tick()
    {
      for (int i = 0; i < _max_tasks; ++i)
      {
        if (_tasks[i].name == NULL)
          continue;

        if (millis() - _tasks[i].last_timeout >= _tasks[i].timeout)
        {
          _tasks[i].last_timeout = millis();
          _tasks[i].callback();
        }
      }
    }
};

#endif
