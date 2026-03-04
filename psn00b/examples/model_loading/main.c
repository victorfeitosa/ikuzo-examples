#include "controller.h"
#include "display.h"
#include "mdx.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define fix2int(f) ((f + 512) >> 12)

extern const uint32_t tim_texture[];

// Main function
int main()
{
    RenderContext context;
    context.active_buffer = 0;
    TIM_IMAGE tim;
    uint8_t mdx_buffer[320000];  // Buffer to hold the loaded model, adjust size as needed

    MDX_Info info;
    info.current_frame = 0;
    info.animation_speed = 8;

    // LoadMD2FromMem(md2, mummy);
    VECTOR position = {0, 0, 400};
    SVECTOR rotation = {0, 0, 0};
    // uint32_t scale = 2048;

    // Init graphics and controllers
    InitDisplay(&context, 1);
    InitControllers();

    /* Load .TIM file */
    GetTimInfo(tim_texture, &tim);
    if (tim.mode & 0x8)
        LoadImage(tim.crect, tim.caddr); /* Upload CLUT if present */
    LoadImage(tim.prect, tim.paddr);     /* Upload texture to VRAM */

    // Load MDX model
    MDX *mdx = loadMDX("mummy.mdx");

    printMDXInfo(mdx);
    // Main loop
    while (1)
    {
        // Check inputs
        if (pad_enabled(PAD1, PAD_ID_DIGITAL))
        {
            if (pad_pressed(PAD1, PAD_CROSS))
            {
                if (pad_pressed(PAD1, PAD_LEFT))
                {
                    rotation.vy += 8;
                }
                else if (pad_pressed(PAD1, PAD_RIGHT))
                {
                    rotation.vy -= 8;
                }

                if (pad_pressed(PAD1, PAD_UP))
                {
                    rotation.vx -= 8;
                }
                else if (pad_pressed(PAD1, PAD_DOWN))
                {
                    rotation.vx += 8;
                }

                if (pad_pressed(PAD1, PAD_R1))
                {
                    rotation.vz += 8;
                }
                else if (pad_pressed(PAD1, PAD_L1))
                {
                    rotation.vz -= 8;
                }
            }
            else
            {
                if (pad_pressed(PAD1, PAD_LEFT))
                {
                    position.vx -= 4;
                }
                else if (pad_pressed(PAD1, PAD_RIGHT))
                {
                    position.vx += 4;
                }

                if (pad_pressed(PAD1, PAD_UP))
                {
                    position.vy -= 4;
                }
                else if (pad_pressed(PAD1, PAD_DOWN))
                {
                    position.vy += 4;
                }
                if (pad_pressed(PAD1, PAD_R1))
                {
                    position.vz += 4;
                }
                else if (pad_pressed(PAD1, PAD_L1))
                {
                    position.vz -= 4;
                }
            }
        }
        if (advance_animation)
        {
            info.current_frame = (info.current_frame + 1) % 40;
            advance_animation = 0;
        }
        sortMDX(&context, mdx, position, rotation, scale, &info, &tim);

        // Display graphics
        DrawDisplay(&context);

        // Increase frame count
        // frames++;
    }

    return 0;
}