#ifndef GLOBAL_H

#ifdef GLOBAL_H
#define EXTERN extern
#else
#define EXTERN
#endif

#define GLOBAL_H

#include <M5StickC.h>

enum MyState
{
  STATE_WAIT_INIT,
  STATE_WAIT,
  STATE_ADVERTISE_INIT,
  STATE_ADVERTISE,
  STATE_WAIT_MEAS,
  STATE_MEAS,
  STATE_MAX,
};

enum MyEvent
{
  EVT_BTN_A_LONGPRESS,
  EVT_BTN_B_LONGPRESS,
  EVT_CONNECT_CENTRAL,
  EVT_DISCONNECT_CENTRAL,
  EVT_MEAS_START,
  EVT_MEAS_STOP,
  EVT_TIMEOUT,
  EVT_NOP,
};

extern bool IsConnected;
extern bool IsMeasStop;
extern uint8_t Heart_rate;
extern uint8_t state;
extern uint8_t event;
extern char device_id[64];
extern uint64_t timer_start_timeout;
extern uint64_t timer_stop_timeout;

#endif //GLOBAL_H