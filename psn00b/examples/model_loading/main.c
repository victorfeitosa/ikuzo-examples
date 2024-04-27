#include "display.h"
#include "controller.h"

#define fix2int(f) ((f + 512) >> 12)

// Main function
int main()
{
  RenderContext context;
  context.active_buffer = 0;

  // Init graphics and controllers
  InitDisplay(&context, 1);
  InitControllers();

  // Main loop
  while (1)
  {
    // Check inputs
    if (pad_enabled(PAD1, PAD_ID_DIGITAL))
    {
      // Turn VSync ON/OFF
      if (pad_pressed(PAD1, PAD_LEFT))
      {
      }
      else if (pad_pressed(PAD1, PAD_RIGHT))
      {
      }

      // Increase/decrease artificial frame delay
      if (pad_pressed(PAD1, PAD_UP))
      {
      }
      else if (pad_pressed(PAD1, PAD_DOWN))
      {
      }
    }

    // Display graphics
    DrawDisplay(&context);

    // Increase frame count
    frames++;
  }

  return 0;
}
