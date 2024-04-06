/**
 * @file main.c
 * @author VictorF (victor.klngo@gmail.com)
 * @brief Triangle VS Quad rendering benchmark
 * @version 0.1
 * @date 2024-04-06
 * 
 * This demo tests the rendering speed of triangles Vquads by sorting rotating cubes onto the screen
 * Controls:
 *   UP/DOWN/LEFT/RIGHT - Move the cubes around
 *   X/O - Change between rendering methods [Triangles/Quads]
 *   R1/L1 - Increase or decrease the number of cubes being sorted
 *   R2/L2 - Move the cubes towards/away from the camera
 * 
 * I found that quads are quicker to render. They not only save a few GTE cycles but are also lighter
 * on the CPU due to being a single primitive the GPU has to render while a cube rendered with triangles
 * has to send two primitives per face to be rendered. Even when trying to save GTE cycles when rendering
 * the triangles by not calculating the average Z or face colors twice, quads end up being more performant
 * and are generally recommended to render thing such as level geometry.
 * 
 * 
 */

#include "display.h"
#include "cubes.h"
#include <psxpad.h>

#define pad_pressed(pad, button) !(pad->btn & button)

// Main function
int main()
{
  RenderContext context;
  context.active_buffer = 0;
  PADTYPE *pad;

  // Init graphics
  init(&context);

  MATRIX mtx, lmtx; // Rotation matrices for geometry and lighting
  WORLD_SPACE = &mtx;
  LIGHT_SPACE = &lmtx;

  // Main loop
  while (1)
  {
    FntPrint(-1, "FPS: %d\n", fps);
    FntPrint(-1, "#CUBES RENDERED: %d\n", num_cubes);
    FntPrint(-1, "#POLYS RENDERED: %d\n", num_faces);
    FntPrint(-1, "METHOD: %s\n", method == 0 ? "Tris" : "Quads");

    // Constantly rotate cubes
    rot.vx += 16;
    rot.vy += 24;
    pad = (PADTYPE *)&pad_buffer[0];

    // Check inputs
    if (pad->stat == 0 && pad->type == PAD_ID_DIGITAL)
    {
      // Move cubes
      if (pad_pressed(pad, PAD_UP))
      {
        pos.vy += 8;
      }
      else if (pad_pressed(pad, PAD_DOWN))
      {
        pos.vy -= 8;
      }

      if (pad_pressed(pad, PAD_LEFT))
      {
        pos.vx -= 8;
      }
      else if (pad_pressed(pad, PAD_RIGHT))
      {
        pos.vx += 8;
      }

      if (pad_pressed(pad, PAD_R2))
      {
        pos.vz += 50;
      }
      else if (pad_pressed(pad, PAD_L2))
      {
        pos.vz -= 50;
      }

      // Increase?decrease  number of cubes
      if (pad_pressed(pad, PAD_R1) && num_cubes < 800)
      {
        num_cubes += 2;
      }
      else if (pad_pressed(pad, PAD_L1) && num_cubes > 0)
      {
        num_cubes -= 2;
      }

      // Swap rendering method
      if (pad_pressed(pad, PAD_CROSS))
      {
        method = 0;
      }
      else if (pad_pressed(pad, PAD_CIRCLE))
      {
        method = 1;
      }
    }

    draw_method = method == 0 ? &sort_tris_cube : &sort_quad_cube;

    // Sort cube primitives
    sort_cubes(&context);

    // Display graphics
    display(&context);

    // Increase frame count
    frames++;
  }

  return 0;
}
