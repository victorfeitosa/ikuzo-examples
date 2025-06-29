#ifndef _CONTROLLER_H_
#define _CONTROLLER_H_

#include <stdint.h>
#include <psxapi.h>
#include <psxpad.h>

// Controller buffer
#define PAD_BUFF_LEN 34
// Pad routines
#define pad_enabled(pad, padType) (pad->stat == 0 && pad->type == padType)
#define pad_pressed(pad, button) (!(pad->btn & button))

// Globals
static uint8_t pad_buffer[2][PAD_BUFF_LEN];
static PADTYPE *PAD1, *PAD2;

void InitControllers()
{
  // Initialize Port 1 Controller
  InitPAD(&pad_buffer[0][0], PAD_BUFF_LEN, &pad_buffer[1][0], PAD_BUFF_LEN);

  // Don't make pad driver acknowledge V-Blank IRQ (recommended)
  ChangeClearPAD(0);

  // Start pad
  StartPAD();

  PAD1 = (PADTYPE*)&pad_buffer[0];
  PAD2 = (PADTYPE*)&pad_buffer[1];
}

#endif // _CONTROLLER_H_
