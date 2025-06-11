#ifndef COMM_H
#define COMM_H

#include <Arduino.h>
#include <string.h>

static const unsigned int buf_size = 256;
static char send[buf_size];
static char receive[buf_size];
static bool new_recv_data = false;
static bool new_send_data = false;

static const byte amount_of_opts = 6;

struct SerialData
{
  char identifier[16];
  struct
  {
    char key[16];
    char value[24];
  } options[amount_of_opts];
};

bool new_data_to_get()
{
  return new_recv_data;
}

struct SerialData get_recv_data()
{
  struct SerialData d;
  char *delim = ",", *subdelim = "=";
  char *parse_str, *option_str, *token, *subtoken;
  char *saveptr1, *saveptr2;
  byte i, j, opt_ndx = 0;

  for (parse_str = receive, i = 0;; parse_str = NULL, i++)
  {
    token = strtok_r(parse_str, delim, &saveptr1);

    if (token == NULL)
      break;
    else if (i == 0)
      strcpy(d.identifier, token);

    for (option_str = token, j = 0;; option_str = NULL, j++)
    {
      subtoken = strtok_r(option_str, subdelim, &saveptr2);

      if (subtoken == NULL)
        break;
      else if (opt_ndx >= amount_of_opts)
        break;

      if (j == 0)
      {
        strcpy(d.options[opt_ndx].key, subtoken);
      }
      else if (j == 1)
      {
        strcpy(d.options[opt_ndx].value, subtoken); 
        opt_ndx++;
      }
    }
  }

  new_recv_data = false;

  return d;
}

void data_to_send(struct SerialData data)
{
  strcpy(send, "");

  strcat(send, data.identifier);

  for (int i = 0; i < amount_of_opts; i++)
  {
    if (strcmp(data.options[i].key, "") == 0)
        continue; 
    strcat(send, ",");
    strcat(send, data.options[i].key);
    strcat(send, "=");
    strcat(send, data.options[i].value);
  }

  new_send_data = true;
}

void recv_data()
{
  static bool recv_in_progress = false;
  static unsigned int ndx = 0;

  char start_marker = '<';
  char end_marker = '>';
  char rc;

  while (Serial.available() > 0 && new_recv_data == false)
  {
    rc = Serial.read();

    if (recv_in_progress)
    {
      if (rc != end_marker)
      {
        receive[ndx] = rc;
        ndx++;
        // overflow check
        if (ndx >= buf_size)
          ndx = buf_size - 1;
      }
      else
      {
        receive[ndx] = '\0';
        recv_in_progress = false;
        ndx = 0;
        new_recv_data = true;
      }
    }
    else if (rc == start_marker)
    {
      recv_in_progress = true;
    }
  }
}

void send_data()
{
  if (new_send_data == false)
    return;

  unsigned int n = strlen(send);
	char end_marker = ';';

	if (Serial.availableForWrite() >= n + 1)
	{
		Serial.print(send);
		Serial.print(end_marker);
    new_send_data = false;
	}
}

#endif
