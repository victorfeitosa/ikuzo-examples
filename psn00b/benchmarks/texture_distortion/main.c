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
 * has to sort two primitives per face to be rendered. Even when trying to save GTE cycles when rendering
 * the triangles by not calculating the average Z or face colors twice, quads end up being more performant
 * and are generally recommended to render thing such as level geometry.
 *
 *
 */

#include "controller.h"
#include "planes.h"
#include "display.h"
#include <psxpad.h>

extern const uint32_t checker_tex[];
extern const uint32_t fence_tex[];
extern const uint32_t wall_tex[];

// Main function
int main()
{
    RenderContext context;
    context.active_buffer = 0;
    TIM_IMAGE tims[3];
    int curr_tim, select;
    const uint32_t *textures[] = {checker_tex, fence_tex, wall_tex};

    // Init graphics and controllers
    InitDisplay(&context, 1);
    InitControllers();

    MATRIX mtx;
    WORLD_SPACE = &mtx;

    /* Load .TIM files */
    for (size_t i = 0; i < 3; i++)
    {
        GetTimInfo(textures[i], &tims[i]);
        if (tims[i].mode & 0x8)
            LoadImage(tims[i].crect, tims[i].caddr); /* Upload CLUT if present */
        LoadImage(tims[i].prect, tims[i].paddr);     /* Upload texture to VRAM */
    }

    curr_tim = select = 0;

    // Main loop
    while (1)
    {
        FntPrint(-1, "FPS: %d\n", fps);

        // Check inputs
        if (pad_enabled(PAD1, PAD_ID_DIGITAL))
        {
            // Move cubes
            if (pad_pressed(PAD1, PAD_UP))
            {
                pos.vy -= 8;
            }
            else if (pad_pressed(PAD1, PAD_DOWN))
            {
                pos.vy += 8;
            }

            if (pad_pressed(PAD1, PAD_LEFT))
            {
                pos.vx -= 8;
            }
            else if (pad_pressed(PAD1, PAD_RIGHT))
            {
                pos.vx += 8;
            }

            if (pad_pressed(PAD1, PAD_R2))
            {
                pos.vz -= 4;
            }
            else if (pad_pressed(PAD1, PAD_L2))
            {
                pos.vz += 4;
            }

            // Rotate planes
            if (pad_pressed(PAD1, PAD_R1))
            {
                rot.vy += 4;
            }
            else if (pad_pressed(PAD1, PAD_L1))
            {
                rot.vy -= 4;
            }

            if (pad_pressed(PAD1, PAD_CROSS))
            {
                rot.vx += 4;
            }
            else if (pad_pressed(PAD1, PAD_TRIANGLE))
            {
                rot.vx -= 4;
            }

            if (pad_pressed(PAD1, PAD_CIRCLE))
            {
                rot.vz += 4;
            }
            else if (pad_pressed(PAD1, PAD_SQUARE))
            {
                rot.vz -= 4;
            }

            if (pad_pressed(PAD1, PAD_SELECT))
            {
                select = 1;
            }
            else if (select)
            {
                curr_tim = (curr_tim + 1) % 3;
                select = 0;
            }

            // Force exit
            if (pad_pressed(PAD1, PAD_SELECT) && pad_pressed(PAD1, PAD_START))
            {
                return 0;
            }
        }

        SortPlanes(&context, &tims[curr_tim], pos, rot);
        DrawDisplay(&context);

        frames++;
    }

    return 0;
}
