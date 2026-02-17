#include "controller.h"
#include "display.h"
#include "md2.h"
#include "mummy.h"

#define fix2int(f) ((f + 512) >> 12)

extern const uint32_t tim_texture[];

// Main function
int main()
{
    RenderContext context;
    context.active_buffer = 0;
    TIM_IMAGE tim;
    uint8_t mdx_buffer[320000];  // Buffer to hold the loaded model, adjust size as needed

    MDX *mdx = (MDX *)mdx_buffer;

    const size_t mdx_size = LoadMDX("./mummy.md2", mdx);

    // LoadMD2FromMem(md2, mummy);
    VECTOR position = {0, 0, 400};
    SVECTOR rotation = {0, 0, 0};
    // uint32_t scale = 2048;

    // Init graphics and controllers
    InitDisplay(&context, 1);
    InitControllers();

    //  /* Load .TIM file */
	// GetTimInfo(tim_texture, &tim);
	// if( tim.mode & 0x8 )
	// 	LoadImage( tim.crect, tim.caddr );	/* Upload CLUT if present */
	// LoadImage( tim.prect, tim.paddr );		/* Upload texture to VRAM */

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
        // if (advance_animation)
        // {
        //     md2->current_frame = (md2->current_frame + 1) % 40;
        //     advance_animation = 0;
        // }
        // SortMD2(&context, md2, &tim, position, rotation, scale);
        FntPrint(-1, "Pos: (%d, %d, %d) Rot: (%d, %d, %d)\n", position.vx, position.vy, position.vz, rotation.vx, rotation.vy, rotation.vz);
        FntPrint(-1, "MDX\n");
        FntPrint(-1, "\tVertices: %d\n", mdx->head.num_vertices);
        FntPrint(-1, "\tUVS: %d\n", mdx->head.num_uv);
        FntPrint(-1, "\tTris: %d\n", mdx->head.num_tris);
        FntPrint(-1, "\tFrames: %d\tMat: %d\n", mdx->head.num_frames, mdx->head.mat);

        // Display graphics
        DrawDisplay(&context);

        // Increase frame count
        // frames++;
    }

    return 0;
}