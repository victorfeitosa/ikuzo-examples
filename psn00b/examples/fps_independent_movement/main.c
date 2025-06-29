#include "display.h"
#include "controller.h"
#include "tile.h"
#include "timer.h"

#define MAX_DELAY 100000000
#define fix2int(f) ((f) >> 12)

void resetDelta(uint32_t *delta, uint8_t timerId)
{
  *delta = TIMER_VALUE(timerId);
}
void getDelta(uint32_t *delta, uint8_t timerId)
{
  *delta = TIMER_VALUE(timerId) - *delta;
  TIMER_VALUE(timerId) = 0;
}

typedef struct
{
  int32_t x, y;
} Pos;

// Main function
int main()
{
  RenderContext context;
  context.active_buffer = 0;

  uint16_t counter_mode = 0;
  uint32_t delta = 0;
  uint32_t delay = 1;

  uint8_t movement = 0;
  int32_t regular_spd = 3;
  int32_t independent_spd = 48;
  int32_t indepX = 0;     // Independent tile fixed point X pos

  // Set timer mode
  SetCounterFlag(&counter_mode, SYNC_MODE_1);
  SetCounterFlag(&counter_mode, C1_SRC_HBLANK);
  SetCounterFlag(&counter_mode, IRQ_DISABLED);
  TIMER_CTRL(1) = counter_mode;

  Tile tiles[] = {
      CreateTile(50, 50, 127, 15, 25), // FPS-bound tile
      CreateTile(50, 50, 15, 127, 25), // FPS independent tile
  };

  Pos tilePos[] = {
      {0, 60},
      {0, 160}};

  // Init graphics and controllers
  InitDisplay(&context, 1);
  InitControllers();

  // Main loop
  while (1)
  {
    // Update delta and speed
    getDelta(&delta, 1);

    FntPrint(-1, "FPS: %d\t\tVSync %s\n", fps, vsync_disabled ? "DISABLED" : "ENABLED");
    FntPrint(-1, "DELAY: %d\t\tDELTA: %d\n", delay, delta);

    // Check inputs
    if (pad_enabled(PAD1, PAD_ID_DIGITAL))
    {
      // Turn VSync ON/OFF
      if (pad_pressed(PAD1, PAD_CROSS))
      {
        vsync_disabled = 0; // VSYNC ENABLED
      }
      else if (pad_pressed(PAD1, PAD_TRIANGLE))
      {
        vsync_disabled = 1; // VSYNC DISABLED
      }

      // Increase/decrease artificial frame delay
      if (pad_pressed(PAD1, PAD_RIGHT) && delay < MAX_DELAY)
      {
        delay += 1;
      }
      else if (pad_pressed(PAD1, PAD_LEFT) && delay > 0)
      {
        delay -= 1;
      }
    }

    // Increase tile pos
    tilePos[0].x += regular_spd;

    // fps independent tile
    tilePos[1].x = fix2int(indepX);

    if (tilePos[0].x <= 0 || tilePos[0].x >= 280)
      regular_spd *= -1;
    if (tilePos[1].x <= 0 || tilePos[1].x >= 280)
      independent_spd *= -1;

    indepX += independent_spd * delta;
    // Reset delta
    resetDelta(&delta, 1);

    // Apply artificial delay
    for (int i = 0; i < delay; i++)
    {
      // draw calls take CPU cycles, good for making up artificial CPU load
      FntPrint(-1, "\0"); // draws an empty character
    }

    // Sort cube primitives
    SortTile(&context, &tiles[0], tilePos[0].x, tilePos[0].y);
    SortTile(&context, &tiles[1], tilePos[1].x, tilePos[1].y);

    // Display graphics
    DrawDisplay(&context);

    // Increase frame count
    frames++;
  }

  return 0;
}
