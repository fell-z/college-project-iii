#include <string.h>
#define MATCH 0

// better to receive a 1 or 0 instead of string 'true' or 'false'.
#define TRUE_I  1
#define FALSE_I 0

#include "Comm.h"
#include "CircularAverage.h"
#include "TrafficLightPair.h"
#include "TrafficFlowMeter.h"

// all timeouts are in milliseconds

/*====== Communication settings ======*/
constexpr unsigned long BAUD_RATE = 9600;
/*====================================*/

/*====== Traffic light settings ======*/

// TL1 and TL3 are synced and always opposite of TL2 and TL4, and vice-versa.
// e.g. TL1 and TL3 are in the green stage, then TL2 and TL4 are in the red stage.

// value in cm/ms scaled down using a scale of 1/64.
// the value was gathered randomly across the internet, so it may not be really accurate.
constexpr float HUMAN_WALKING_SPEED = (0.125 * (1.0/64));

constexpr unsigned long TLP_YELLOW_TIME = 1000;

// cm / (cm/ms) == ms
constexpr unsigned long TLP1_RED_TIME = 14 / HUMAN_WALKING_SPEED;
constexpr unsigned long TLP2_RED_TIME = 9 / HUMAN_WALKING_SPEED;

constexpr unsigned long TLP1_GREEN_TIME = TLP2_RED_TIME - TLP_YELLOW_TIME;
constexpr unsigned long TLP2_GREEN_TIME = TLP1_RED_TIME - TLP_YELLOW_TIME;
/*====================================*/

/*====== Infrared settings ======*/
constexpr unsigned long IR_READ_TIMEOUT = 100;
/*===============================*/

/*====== LDR settings ======*/
constexpr unsigned long LDR_READ_TIMEOUT = 100;
constexpr unsigned long LDR_UPDATE_TIMEOUT = 60000 * 5; // five minutes in ms
float ldr_detection_factor = 1.25;
/*==========================*/

/*====== Ultrasonic settings ======*/
typedef void (*isr)();
isr US1_update();
isr US2_update();
isr US3_update();

constexpr float SPEED_OF_SOUND = 0.0343; // in cm/us with dry air at 20°C
constexpr unsigned long US_TRIGGER_TIMEOUT = 100;
constexpr unsigned long US_READ_TIMEOUT = 100;
float ultrasonic_distance_threshold = 9.0; // in centimeters
/*=================================*/

/*====== Region settings ======*/
constexpr unsigned long MEAN_TIMEOUT = 10000;
constexpr unsigned int SAMPLES_PER_MEAN = 120;
constexpr unsigned long SAMPLE_UPDATE_TIMEOUT = MEAN_TIMEOUT / SAMPLES_PER_MEAN;
constexpr unsigned long REF_MEAN_UPDATE_TIMEOUT = 10000; // 60000 * 5;
/*=============================*/

typedef enum
{
  TL1_RED = 22,
  TL1_YELLOW = 24,
  TL1_GREEN = 26,

  TL2_RED = 23,
  TL2_YELLOW = 25,
  TL2_GREEN = 27,

  TL3_RED = 28,
  TL3_YELLOW = 30,
  TL3_GREEN = 32,

  TL4_RED = 29,
  TL4_YELLOW = 31,
  TL4_GREEN = 33,

  TFM1_BOTTOM = 34,
  TFM1_MIDDLE_BOTTOM = 36,
  TFM1_MIDDLE_TOP = 38,
  TFM1_TOP = 40,

  TFM2_BOTTOM = 35,
  TFM2_MIDDLE_BOTTOM = 37,
  TFM2_MIDDLE_TOP = 39,
  TFM2_TOP = 41,

  TFM3_BOTTOM = 42,
  TFM3_MIDDLE_BOTTOM = 44,
  TFM3_MIDDLE_TOP = 46,
  TFM3_TOP = 48,

  TFM4_BOTTOM = 43,
  TFM4_MIDDLE_BOTTOM = 45,
  TFM4_MIDDLE_TOP = 47,
  TFM4_TOP = 49,

  IR1 = 8,
  IR2 = 9,
  IR3 = 10,

  US1_ECHO = 2,
  US1_TRIGGER = 5,

  US2_ECHO = 3,
  US2_TRIGGER = 6,

  US3_ECHO = 18,
  US3_TRIGGER = 7,

  LDR1 = A0,
  LDR2 = A1,
  LDR3 = A2,
  LDR4 = A3,
  LDR5 = A4,
} Pin;

struct IR
{
  Pin pin;
  unsigned long last_read_time = millis();
} ir1, ir2, ir3;

struct LDR
{
  Pin pin;
  unsigned long last_update_time = millis();
  unsigned long last_read_time = millis();
  int threshold;
  int ref;
  int value; 
} ldr1, ldr2, ldr3, ldr4, ldr5;

volatile struct Ultrasonic
{
  Pin trigger;
  Pin echo;
  unsigned long last_trigger_time = millis();
  unsigned long last_read_time = millis();
  unsigned long start_time = micros();
  unsigned long end_time = micros();
  bool duration_available = false;
} us1, us2, us3;

struct Region
{
  bool running_auto = true;
  float no_auto_mean = 0.0; // if 'running_auto' is set to false, use this mean or a mean from serial input
  float ref_mean = -1.0; 
  unsigned long last_ref_mean_update_time = millis();
  unsigned int n_sensors;
  unsigned int detected_objects = 0;
  unsigned int samples = 0;
  unsigned long last_sample_update_time = millis();
  bool new_mean = false;
  CircularAverage<unsigned int> detected_mean = CircularAverage<unsigned int>(SAMPLES_PER_MEAN);
} r1, r2, r3;

TrafficLightPair tlp1(TL1_RED, TL1_YELLOW, TL1_GREEN,
                     TL3_RED, TL3_YELLOW, TL3_GREEN,
                     TLP1_RED_TIME, TLP_YELLOW_TIME, TLP1_GREEN_TIME, RED_STAGE);

TrafficLightPair tlp2(TL2_RED, TL2_YELLOW, TL2_GREEN,
                     TL4_RED, TL4_YELLOW, TL4_GREEN,
                     TLP2_RED_TIME, TLP_YELLOW_TIME, TLP2_GREEN_TIME, GREEN_STAGE);

TrafficFlowMeter tfm1(TFM1_BOTTOM, TFM1_MIDDLE_BOTTOM, TFM1_MIDDLE_TOP, TFM1_TOP);
TrafficFlowMeter tfm2(TFM2_BOTTOM, TFM2_MIDDLE_BOTTOM, TFM2_MIDDLE_TOP, TFM2_TOP);
TrafficFlowMeter tfm3(TFM3_BOTTOM, TFM3_MIDDLE_BOTTOM, TFM3_MIDDLE_TOP, TFM3_TOP);
TrafficFlowMeter tfm4(TFM4_BOTTOM, TFM4_MIDDLE_BOTTOM, TFM4_MIDDLE_TOP, TFM4_TOP);

void setup()
{
  Serial.begin(BAUD_RATE);
  while (!Serial);

  IR_begin(ir1, IR1);
  IR_begin(ir2, IR2);
  IR_begin(ir3, IR3);

  LDR_begin(ldr1, LDR1);
  LDR_begin(ldr2, LDR2);
  LDR_begin(ldr3, LDR3);
  LDR_begin(ldr4, LDR4);
  LDR_begin(ldr5, LDR5);

  US_begin(us1, US1_TRIGGER, US1_ECHO, US1_update);
  US_begin(us2, US2_TRIGGER, US2_ECHO, US2_update);
  US_begin(us3, US3_TRIGGER, US3_ECHO, US3_update);

  r1.n_sensors = 4;
  r2.n_sensors = 4;
  r3.n_sensors = 3;

  tlp1.begin();
  tlp2.begin();
}

struct SerialData recv;
float ultsnd, ldr_df; // to hold received sensor settings

struct SerialData to_send;
char buf[256];

TL_Stage stage;
float mean;
bool tlp1_has_changed = false, tlp2_has_changed = false;

unsigned long r1_possible_green_time = 0, r2_possible_green_time = 0, r3_possible_green_time = 0;
unsigned long longest_possible_green_time = 0;

unsigned long last_mean_time = millis();
unsigned long last_debug_time = millis();

void loop()
{
  /*====== Receive data ======*/
  recv_data();

  if (new_data_to_get() == true)
  {
    recv = get_recv_data();

    if (strcmp(recv.identifier, "tl1") == MATCH || strcmp(recv.identifier, "tl3") == MATCH)
    {
      stage = (TL_Stage)strtol(recv.options[0].value, (char**)NULL, 10);

      tlp1.set_stage(stage);

      if (stage == RED_STAGE)
      {
        tlp2.set_stage(GREEN_STAGE);
      }
      else
      {
        tlp2.set_stage(RED_STAGE);
      }
    }
    else if (strcmp(recv.identifier, "tl2") == MATCH || strcmp(recv.identifier, "tl4") == MATCH)
    {
      stage = (TL_Stage)strtol(recv.options[0].value, (char**)NULL, 10);

      tlp2.set_stage(stage);

      if (stage == RED_STAGE)
      {
        tlp1.set_stage(GREEN_STAGE);
      }
      else
      {
        tlp1.set_stage(RED_STAGE);
      }
    }
    else if (strcmp(recv.identifier, "sensors") == MATCH)
    {
      for (int i = 0; i < 2; i++)
      {
        if (strcmp(recv.options[i].key, "ldr") == MATCH)
        {
          ldr_df = strtod(recv.options[i].value, NULL);
        }
        else if (strcmp(recv.options[i].key, "ultsnd") == MATCH)
        {
          ultsnd = strtod(recv.options[i].value, NULL);
        }
      }

      set_sensors_settings(ldr_df, ultsnd);
      // updates all ldr's thresholds as the factor has changed;
      LDR_update_threshold(ldr1);
      LDR_update_threshold(ldr2);
      LDR_update_threshold(ldr3);
      LDR_update_threshold(ldr4);
      LDR_update_threshold(ldr5);
    }
    else if (strcmp(recv.identifier, "r1") == MATCH)
    {
      if (strcmp(recv.options[0].key, "auto") == MATCH)
      {
        r1.running_auto = strtol(recv.options[0].value, (char**)NULL, 10) == TRUE_I;
      }
      else if (strcmp(recv.options[0].key, "flow_mean") == MATCH)
      {
        r1.no_auto_mean = strtod(recv.options[0].value, (char**)NULL);
      }
    }
    else if (strcmp(recv.identifier, "r2") == MATCH)
    {
      if (strcmp(recv.options[0].key, "auto") == MATCH)
      {
        r2.running_auto = strtol(recv.options[0].value, (char**)NULL, 10) == TRUE_I;
      }
      else if (strcmp(recv.options[0].key, "flow_mean") == MATCH)
      {
        r2.no_auto_mean = strtod(recv.options[0].value, (char**)NULL);
      }
    }
    else if (strcmp(recv.identifier, "r3") == MATCH)
    {
      if (strcmp(recv.options[0].key, "auto") == MATCH)
      {
        r3.running_auto = strtol(recv.options[0].value, (char**)NULL, 10) == TRUE_I;
      }
      else if (strcmp(recv.options[0].key, "flow_mean") == MATCH)
      {
        r3.no_auto_mean = strtod(recv.options[0].value, (char**)NULL);
      }
    }
  }
  /*==========================*/

  /*====== Update traffic lights ======*/
  tlp1_has_changed = tlp1.tick();
  tlp2_has_changed = tlp2.tick();

  if (tlp1_has_changed)
  {
    stage = tlp1.get_stage();

    tlp1.next_stage();

    if (stage != GREEN_STAGE)
    {
      tlp2.next_stage();

      tlp1_has_changed = tlp2_has_changed = true;
    }
  }
  else if (tlp2_has_changed)
  {
    stage = tlp2.get_stage();

    tlp2.next_stage();

    if (stage != GREEN_STAGE)
    {
      tlp1.next_stage();

      tlp1_has_changed = tlp2_has_changed = true;
    }
  }

  if (tlp1_has_changed == true)
  {
    TLP_send_stage(tlp1, "tl1", "tl3");
  }

  if (tlp2_has_changed == true)
  {
    TLP_send_stage(tlp2, "tl2", "tl4");
  }

  /*
  if (tlp1_has_changed == true && tlp2_has_changed == false)
  {
    stage = tlp1.get_stage();

    tlp1.set_stage(stage); // reset to prevent further desync

    switch (stage)
    {
      case RED_STAGE:
        tlp2.set_stage(GREEN_STAGE);
        tlp1_has_changed = tlp2_has_changed = true;
        break;
      case GREEN_STAGE:
        tlp2.set_stage(RED_STAGE);
        tlp1_has_changed = tlp2_has_changed = true;
    }
  }

  else if (tlp1_has_changed == false && tlp2_has_changed == true)
  {
    stage = tlp2.get_stage();

    tlp2.set_stage(stage); // reset to prevent further desync

    switch (stage)
    {
      case RED_STAGE:
        tlp1.set_stage(GREEN_STAGE);
        tlp1_has_changed = tlp2_has_changed = true;
        break;
      case GREEN_STAGE:
        tlp1.set_stage(RED_STAGE);
        tlp1_has_changed = tlp2_has_changed = true;
    }
  }

  if (tlp1_has_changed == true)
  {
    TLP_send_stage(tlp1, "tl1", "tl3");
  }

  if (tlp2_has_changed == true)
  {
    TLP_send_stage(tlp2, "tl2", "tl4");
  }
  */

  tlp1_has_changed = tlp2_has_changed = false;
  /*===================================*/


  /*====== Update sensors ======*/
  LDR_update(ldr1);
  LDR_update(ldr2);
  LDR_update(ldr3);
  LDR_update(ldr4);
  LDR_update(ldr5);
  US_trigger(us1);
  US_trigger(us2);
  US_trigger(us3);
  /*============================*/


  /*====== Detect objects ======*/
  if (r1.new_mean == false)
  {
    if (millis() - ir1.last_read_time >= IR_READ_TIMEOUT)
    {
      ir1.last_read_time = millis();
      if (IR_detect_object(ir1))
      {
        // r1.detected_objects++;
        r1.detected_objects += 20;
      }
    }

    if (millis() - ldr1.last_read_time >= LDR_READ_TIMEOUT)
    {
      ldr1.last_read_time = millis();
      if (LDR_detect_object(ldr1))
      {
        // r1.detected_objects++;
        r1.detected_objects += 20;
      }
    }

    if (millis() - ldr2.last_read_time >= LDR_READ_TIMEOUT)
    {
      ldr2.last_read_time = millis();
      if (LDR_detect_object(ldr2))
      {
        // r1.detected_objects++;
        r1.detected_objects += 20;
      }
    }

    if (millis() - us1.last_read_time >= US_READ_TIMEOUT)
    {
      us1.last_read_time = millis();
      if (us1.duration_available == true && US_detect_object(us1))
      {
        // r1.detected_objects++;
        r1.detected_objects += 20;
      }
    }
  }

  if (r2.new_mean == false)
  {
    if (millis() - ir2.last_read_time >= IR_READ_TIMEOUT)
    {
      ir2.last_read_time = millis();
      if (IR_detect_object(ir2))
      {
        // r2.detected_objects++;
        r2.detected_objects += 20;
      }
    }

    if (millis() - ldr3.last_read_time >= LDR_READ_TIMEOUT)
    {
      ldr3.last_read_time = millis();
      if (LDR_detect_object(ldr3))
      {
        // r2.detected_objects++;
        r2.detected_objects += 20;
      }
    }

    if (millis() - ldr4.last_read_time >= LDR_READ_TIMEOUT)
    {
      ldr4.last_read_time = millis();
      if (LDR_detect_object(ldr4))
      {
        // r2.detected_objects++;
        r2.detected_objects += 20;
      }
    }

    if (millis() - us2.last_read_time >= US_READ_TIMEOUT)
    {
      us2.last_read_time = millis();
      if (us2.duration_available == true && US_detect_object(us2))
      {
        // r2.detected_objects++;
        r2.detected_objects += 20;
      }
    }
  }

  if (r3.new_mean == false)
  {
    if (millis() - ir3.last_read_time >= IR_READ_TIMEOUT)
    {
      ir3.last_read_time = millis();
      if (IR_detect_object(ir3))
      {
        // r3.detected_objects++;
        r3.detected_objects += 20;
      }
    }

    if (millis() - ldr5.last_read_time >= LDR_READ_TIMEOUT)
    {
      ldr5.last_read_time = millis();
      if (LDR_detect_object(ldr5))
      {
        // r3.detected_objects++;
        r3.detected_objects += 20;
      }
    }

    if (millis() - us3.last_read_time >= US_READ_TIMEOUT)
    {
      us3.last_read_time = millis();
      if (us3.duration_available == true && US_detect_object(us3))
      {
        // r3.detected_objects++;
        r3.detected_objects += 20;
      }
    }
  }
  /*============================*/


  /*====== Update region mean ======*/
  Region_update(r1);
  Region_update(r2);
  Region_update(r3);
  /*================================*/

  /*====== Update region ref's and related traffic lights ======*/
  if (millis() - r3.last_ref_mean_update_time >= REF_MEAN_UPDATE_TIMEOUT)
  {
    r3.last_ref_mean_update_time = millis();

    r3_possible_green_time = TLP_possible_time(r3, tlp2, tlp1);

    TLP_update_times(r3, tlp2, tlp1);

    tfm3.update(tlp2.get_orig_time_ratio());
    tfm4.update(tlp2.get_orig_time_ratio());
  }

  if (millis() - r2.last_ref_mean_update_time >= REF_MEAN_UPDATE_TIMEOUT)
  {
    r2.last_ref_mean_update_time = millis();

    r2_possible_green_time = TLP_possible_time(r2, tlp1, tlp2);

    TLP_update_times(r2, tlp1, tlp2);
  }

  if (millis() - r1.last_ref_mean_update_time >= REF_MEAN_UPDATE_TIMEOUT)
  {
    r1.last_ref_mean_update_time = millis();

    r1_possible_green_time = TLP_possible_time(r1, tlp1, tlp2);

    TLP_update_times(r1, tlp1, tlp2);

    tfm1.update(tlp1.get_orig_time_ratio());
    tfm2.update(tlp1.get_orig_time_ratio());
  }

  longest_possible_green_time = max(r1_possible_green_time, r2_possible_green_time);
  longest_possible_green_time = max(longest_possible_green_time, r3_possible_green_time);

  if (longest_possible_green_time == r1_possible_green_time)
  {
    TLP_update_times(r1, tlp1, tlp2);
  }
  else if (longest_possible_green_time == r2_possible_green_time)
  {
    TLP_update_times(r2, tlp1, tlp2);
  }
  else if (longest_possible_green_time == r3_possible_green_time)
  {
    TLP_update_times(r3, tlp2, tlp1);
  }
  /*============================================================*/

  /*====== Send mean ======*/
  if (millis() - last_mean_time >= MEAN_TIMEOUT)
  {
    last_mean_time = millis();

    if (r1.new_mean == true)
    {
      Region_send_mean(r1, "r1");
    }

    if (r2.new_mean == true)
    {
      Region_send_mean(r2, "r2");
    }

    if (r3.new_mean == true)
    {
      Region_send_mean(r3, "r3");
    }
  }
  /*=======================*/

  if (millis() - last_debug_time >= 500)
  {
    last_debug_time = millis();

    /*
    Serial.println("");
    Serial.print("Time 1: ");
    Serial.print("Red -> ");
    Serial.print(tlp1.get_time_for(RED_STAGE));
    Serial.print(" Green -> ");
    Serial.println(tlp1.get_time_for(GREEN_STAGE));
    Serial.print("Time 2: ");
    Serial.print("Red -> ");
    Serial.print(tlp2.get_time_for(RED_STAGE));
    Serial.print(" Green -> ");
    Serial.println(tlp2.get_time_for(GREEN_STAGE));

    Serial.print("Mean: ");
    Serial.print(r1.detected_mean.quick_average());
    Serial.print(" Detected objects: ");
    Serial.print(r1.detected_objects);
    Serial.print(" Samples: ");
    Serial.print(r1.samples);
    Serial.print(" New mean?: ");
    Serial.println(r1.new_mean);
    */
  }
}

/*=============================================================*/
/*===================== Helper Functions ======================*/
/*=============================================================*/

/*====== Sensors in general ======*/
void set_sensors_settings(float new_ldr_detection_factor, float new_ultsnd_distance_threshold)
{
  ldr_detection_factor = new_ldr_detection_factor;
  ultrasonic_distance_threshold = new_ultsnd_distance_threshold;
}
/*================================*/


/*====== IR ======*/
void IR_begin(IR& ir, Pin pin)
{
  ir.pin = pin;
  pinMode(ir.pin, INPUT_PULLUP);
}

bool IR_detect_object(IR& ir)
{
  return digitalRead(ir.pin) == LOW;
}
/*================*/


/*====== LDR ======*/
void LDR_begin(LDR& ldr, Pin pin)
{
  ldr.pin = pin;
  ldr.ref = analogRead(ldr.pin);
  ldr.threshold = ldr.ref * ldr_detection_factor;
  pinMode(ldr.pin, INPUT);
}

void LDR_update_threshold(LDR& ldr)
{
  ldr.ref = analogRead(ldr.pin);
  ldr.threshold = ldr.ref * ldr_detection_factor;
}

void LDR_update(LDR& ldr)
{
  if (millis() - ldr.last_update_time >= LDR_UPDATE_TIMEOUT)
  {
    ldr.last_update_time = millis();
    ldr.ref = analogRead(ldr.pin);
    ldr.threshold = round(ldr.ref * ldr_detection_factor);
  }

  ldr.value = analogRead(ldr.pin);
}

bool LDR_detect_object(LDR& ldr)
{
  return ldr.value >= ldr.threshold;
}
/*=================*/


/*====== Ultrasonic ======*/
typedef void (*isr)();

isr US1_update()
{
  if (us1.duration_available == false)
  {
    if (digitalRead(us1.echo) == HIGH)
    {
      us1.start_time = micros();
    }
    else
    {
      us1.end_time = micros();
      us1.duration_available = true;
    }
  }
}

isr US2_update()
{
  if (us2.duration_available == false)
  {
    if (digitalRead(us2.echo) == HIGH)
    {
      us2.start_time = micros();
    }
    else
    {
      us2.end_time = micros();
      us2.duration_available = true;
    }
  }
}

isr US3_update()
{
  if (us3.duration_available == false)
  {
    if (digitalRead(us3.echo) == HIGH)
    {
      us3.start_time = micros();
    }
    else
    {
      us3.end_time = micros();
      us3.duration_available = true;
    }
  }
}

void US_begin(volatile Ultrasonic& us, Pin trigger_pin, Pin echo_pin, isr update_func)
{
  us.trigger = trigger_pin;
  us.echo = echo_pin;
  pinMode(us.trigger, OUTPUT);
  pinMode(us.echo, INPUT);
  digitalWrite(us.trigger, LOW);

  attachInterrupt(digitalPinToInterrupt(us.echo), update_func, CHANGE);
}

void US_trigger(volatile Ultrasonic& us)
{
  if (us.duration_available == false)
  {
    if (millis() - us.last_trigger_time >= US_TRIGGER_TIMEOUT)
    {
      us.last_trigger_time = millis();

      digitalWrite(us.trigger, HIGH);
      delayMicroseconds(10); // necessary 10us time of signal HIGH specified by HC-SR04
      digitalWrite(us.trigger, LOW);
    }
  }
}

bool US_detect_object(volatile Ultrasonic& us)
{
  us.duration_available = false;

  float distance = ((us.end_time - us.start_time) * SPEED_OF_SOUND) / 2.0;

  return distance <= ultrasonic_distance_threshold;
}
/*========================*/


/*====== Region ======*/
void Region_update(Region& r)
{
  if (r.new_mean == false)
  {
    if (millis() - r.last_sample_update_time >= SAMPLE_UPDATE_TIMEOUT)
    {
      r.last_sample_update_time = millis();

      // divide by n sensors as more than one sensor may detect the same object.
      float d_obj_mean = r.detected_objects / (r.n_sensors * 1.0);

      r.detected_mean.add((int) round(d_obj_mean));
      r.detected_objects = 0;
      r.samples++;

      if (r.samples == SAMPLES_PER_MEAN)
      {
        r.new_mean = true;
      }
    }
  }
}

void Region_reset(Region& r)
{
  r.new_mean = false;
  r.detected_mean.clear();
  r.samples = 0;
}

float Region_mean(Region& r)
{
  if (r.running_auto == false)
  {
    return r.no_auto_mean;
  }
  else
  {
    return r.detected_mean.quick_average();
  }
}

void Region_send_mean(Region& r, char identifier[])
{
  float m;

  if (r.new_mean == true)
  {
    m = Region_mean(r);

    Region_reset(r);

    strcpy(to_send.identifier, identifier);
    strcpy(to_send.options[0].key, "flow_mean");
    // arduino's sprintf doesn't support float conversions, so make it into an int
    sprintf(to_send.options[0].value, "%d", (int)(m * 1000));

    data_to_send(to_send);
    send_data();
  }
}
/*====================*/


/*====== Traffic Light Pairs ======*/
unsigned long TLP_possible_time(Region& r, TrafficLightPair& green_tlp, TrafficLightPair& red_tlp)
{
  float m;
  unsigned long t;

  m = Region_mean(r);

  t = TrafficLightPair::calculate_time_by_flow(green_tlp.get_min_time_for(GREEN_STAGE), m);

  if (red_tlp.is_doable_red_time(t))
  {
    return t;
  }
  else
  {
    return 0;
  }
}

void TLP_update_times(Region& r, TrafficLightPair& green_tlp, TrafficLightPair& red_tlp)
{
  float m;
  unsigned long t;

  m = Region_mean(r);

  t = TrafficLightPair::calculate_time_by_flow(green_tlp.get_min_time_for(GREEN_STAGE), m);

  if (red_tlp.is_doable_red_time(t))
  {
    green_tlp.set_new_time_for(GREEN_STAGE, t - TLP_YELLOW_TIME);
    red_tlp.set_new_time_for(RED_STAGE, t);
  }
  else
  {
    green_tlp.set_new_time_for(GREEN_STAGE, green_tlp.get_min_time_for(GREEN_STAGE));
    red_tlp.set_new_time_for(RED_STAGE, red_tlp.get_min_time_for(RED_STAGE));
  }

  r.ref_mean = m;
}

void TLP_send_stage(TrafficLightPair& tlp, char tl1_id[], char tl2_id[])
{
  strcpy(to_send.identifier, tl1_id);
  strcpy(to_send.options[0].key, "stage");
  sprintf(to_send.options[0].value, "%d", tlp.get_stage());

  data_to_send(to_send);
  send_data();

  strcpy(to_send.identifier, tl2_id);
  strcpy(to_send.options[0].key, "stage");
  sprintf(to_send.options[0].value, "%d", tlp.get_stage());

  data_to_send(to_send);
  send_data();
}
/*============================*/
