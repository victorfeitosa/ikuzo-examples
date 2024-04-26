#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

enum COUNTER_FLAG
{
  // Sync enabled
  SYNC_ENABLE_FREE = 0x0000,
  SYNC_ENABLE_SYNC = 0x0001,

  // Sync mode
  SYNC_MODE_0 = 0x0000, // C0-Pause on HBlank;                    C1-Pause on VBlank;                     C2-Stop at current value
  SYNC_MODE_1 = 0x0002, // C0-Reset on HBlank;                    C1-Reset on VBlank;                     C2-Free run
  SYNC_MODE_2 = 0x0004, // C0-Reset on HBlank and pause;          C1-Reset on VBlank and pause;           C2-Free run
  SYNC_MODE_3 = 0x0006, // C0-Pause until HBlank then free run;   C1-Pause until VBlank then free run;    C2-Stop at current value

  // Counter reset
  RESET_AFTER_MAX = 0x0000,
  RESET_AFTER_TARGET = 0x0008,

  // IRQ on target
  IRQ_TARGET_DISABLED = 0x0000,
  IRQ_TARGET_ENABLED = 0x0010,

  // IRQ on Max
  IRQ_MAX_DISABLED = 0x0000,
  IRQ_MAX_ENABLED = 0x0020,

  // IRQ repeat
  IRQ_REPEAT_ONCE = 0x0000,
  IRQ_REPEAT_REPEAT = 0x0040,

  // IRQ mode
  IRQ_PULSE = 0x0000,
  IRQ_TOGGLE = 0x0080,

  // Clock source
  SRC_CLK = 0x0000,
  C0_SRC_DOT = 0x0100,
  C1_SRC_HBLANK = 0x0100,
  C2_SRC_CLK_8 = 0x0200,

  // IRQ enabled state
  IRQ_ENABLED = 0x0000,
  IRQ_DISABLED = 0x0400,
};


inline void SetCounterFlag(uint16_t *mode, enum COUNTER_FLAG flag)
{
  *mode |= flag;
}

#endif // TIMER_H
