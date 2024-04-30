#include "display.h"
#include "controller.h"
#include "md2.h"
#include "mummy.h"

#define fix2int(f) ((f + 512) >> 12)

extern const unsigned char mummy[];

// Main function
int main()
{
    RenderContext context;
    context.active_buffer = 0;

    PSX_MD2 *md2;
    LoadMD2Mem(md2, mummy);

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

            // FntPrint(-1, "MD2 File\n");
            // FntPrint(-1, "Skin size: %dx%d\n", md2->head.skinwidth, md2->head.skinheight);
            // FntPrint(-1, "Frame size: %d\n", md2->head.framesize);
            // FntPrint(-1, "Number of vertices: %d\n", md2->head.num_vertices);
            // FntPrint(-1, "Number of triangles: %d\n", md2->head.num_tris);
            // FntPrint(-1, "Number of frames: %d\n", md2->head.num_frames);
        }

        // Display graphics
        DrawDisplay(&context);

        // Increase frame count
        frames++;
    }

    return 0;
}