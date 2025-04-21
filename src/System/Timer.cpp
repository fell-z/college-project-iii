#include "include/Timer.h"

Timer::Timer(uint8_t n_tasks) : _max_tasks(n_tasks)
{
  _tasks = new Task[n_tasks] { { NULL } };
}

Timer::~Timer()
{
  delete [] _tasks;
}

int8_t Timer::find(const char* name)
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

enum STATUS Timer::add(const char* name, callback fn, unsigned long timeout)
{
  if (_n_tasks == _max_tasks)
    return FULL;

  // search a empty task to fill
  int8_t index = find(NULL);

  _tasks[index] = (Task){ .name = name, .fn = fn, .timeout = timeout, .last_timeout = millis() };
  _n_tasks++;

  return OK;
}

enum STATUS Timer::remove(const char* name)
{
  int8_t index = find(name);

  if (index == -1)
    return ERR;

  _tasks[index] = { NULL };
  _n_tasks--;

  return OK;
}

enum STATUS Timer::set_timeout(const char* name, unsigned long timeout)
{
  int8_t index = find(name);

  if (index == -1)
    return ERR;

  _tasks[index].timeout = timeout;
  return OK;
}

enum STATUS Timer::set_callback(const char* name, callback fn)
{
  int8_t index = find(name);

  if (index == -1)
    return ERR;

  _tasks[index].fn = fn;
  return OK;
}

void Timer::tick()
{
  for (int i = 0; i < _max_tasks; ++i)
  {
    if (_tasks[i].name == NULL)
      continue;

    if (millis() - _tasks[i].last_timeout >= _tasks[i].timeout)
    {
      _tasks[i].last_timeout = millis();
      _tasks[i].fn();
    }
  }
}
