#ifndef DISPLAY_H
#define DISPLAY_H

#include <inline_c.h>
#include <psxapi.h>
#include <psxetc.h>
#include <psxgpu.h>
#include <psxgte.h>
#include <stdint.h>

// OT and Packet Buffer sizes
#define OT_LEN 16384
#define PACKET_LEN 120000

// Screen resolution
#define SCREEN_XRES 320
#define SCREEN_YRES 256

// Screen center position
#define CENTERX SCREEN_XRES >> 1
#define CENTERY SCREEN_YRES >> 1

// Draw env structure
typedef struct
{
    DISPENV disp;        // Display environment
    DRAWENV draw;        // Drawing environment
    uint32_t ot[OT_LEN]; // Ordering table
    char p[PACKET_LEN];  // Packet buffer
} DrawEnv;

// Render Context struct
typedef struct
{
    DrawEnv draw_buffer[2];
    uint8_t active_buffer;
    char *nextpri;
} RenderContext;

// Gets the pointer to the currently active buffer
inline DrawEnv *active_buffer(RenderContext *ctx)
{
    return &(ctx->draw_buffer[ctx->active_buffer]);
}

static volatile uint32_t fps = 0;
static volatile uint32_t frames = 0;
static uint8_t vsync_disabled = 0;

// FPS update callback
static void vsync_callback()
{
    int refresh_rate = (GetVideoMode() == MODE_PAL) ? 50 : 60;

    if (VSync(-1) % refresh_rate)
        return;

    fps = frames;
    frames = 0;
}

DrawEnv *swap_buffers(RenderContext *ctx)
{
    ctx->active_buffer ^= 1;
    DrawEnv *active_buff = active_buffer(ctx);
    ctx->nextpri = active_buff->p;

    return active_buff;
}

// Initializes render context, 3d, pads and VSync callback
void InitDisplay(RenderContext *ctx, uint8_t initGTE)
{
    ResetGraph(0);

    // First Draw and Display environment
    SetDefDispEnv(&ctx->draw_buffer[0].disp, 0, 0, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&ctx->draw_buffer[0].draw, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES);

    setRGB0(&ctx->draw_buffer[0].draw, 30, 60, 100);
    ctx->draw_buffer[0].draw.isbg = 1;
    ctx->draw_buffer[0].draw.dtd = 1;

    // Second Draw and Display environment
    SetDefDispEnv(&ctx->draw_buffer[1].disp, SCREEN_XRES, 0, SCREEN_XRES, SCREEN_YRES);
    SetDefDrawEnv(&ctx->draw_buffer[1].draw, 0, 0, SCREEN_XRES, SCREEN_YRES);

    setRGB0(&ctx->draw_buffer[1].draw, 30, 60, 100);
    ctx->draw_buffer[1].draw.isbg = 1;
    ctx->draw_buffer[1].draw.dtd = 1;

    // Set the first Draw buffer as current drawing environment
    PutDrawEnv(&ctx->draw_buffer[0].draw);

    ClearOTagR(ctx->draw_buffer[0].ot, OT_LEN);
    ClearOTagR(ctx->draw_buffer[1].ot, OT_LEN);

    // Set primitive pointer address
    ctx->nextpri = ctx->draw_buffer[0].p;

    if (initGTE)
    {
        // Initialize the GTE
        InitGeom();

        // Set GTE offset
        gte_SetGeomOffset(CENTERX, CENTERY);

        // Set screen depth (basically FOV control, W/2 works best)
        gte_SetGeomScreen(CENTERX);

        // Set light ambient color
        gte_SetBackColor(10, 15, 30);
    }

    // Load debug font
    FntLoad(960, 0);
    FntOpen(8, 8, 320, 200, 0, 200);

    // Sets up VSync callback to count FPS
    EnterCriticalSection();
    VSyncCallback(&vsync_callback);
    ExitCriticalSection();

    // Timeout and reset VSync callback so controller won't block irqs
    VSync(0);
}

void DrawDisplay(RenderContext *ctx)
{
    // Put font in display buffer
    FntFlush(-1);

    // Wait for GPU to finish drawing
    DrawSync(0);
    VSync(vsync_disabled);

    DrawEnv *active_buff = swap_buffers(ctx);

    // Clear the OT of the next frame
    ClearOTagR(active_buff->ot, OT_LEN);

    // Apply display/drawing environments
    PutDrawEnv(&active_buff->draw);
    PutDispEnv(&active_buff->disp);

    // Enable display
    SetDispMask(1);

    // Start drawing the OT of the last buffer
    DrawOTag(ctx->draw_buffer[1 - ctx->active_buffer].ot + (OT_LEN - 1));
}

#endif // DISPLAY_H