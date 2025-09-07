#include "controller.h"
#include "display.h"
#include "psxcd.h"
#include "psxsn.h"

void initSprite(SPRT *sprite, uint8_t *tex_data, TIM_IMAGE *tim, int x, int y, int w, int h);
void sortSprite(RenderContext *context, SPRT *sprite);
void sortTPage(RenderContext *context, TIM_IMAGE *tim);

int main()
{
    RenderContext context;
    context.active_buffer = 0;
    uint32_t fd;
    int32_t error = 0;
    TIM_IMAGE ball_tim;
    uint32_t img_size = 0;
    SPRT ball_sprite;

    // Init graphics and controllers
    InitDisplay(&context, 0);

#ifdef USE_PCDRV
    uint8_t init = PCinit();
    if (init != 0)
    {
        error++;
    }
    fd = PCopen("ball.tim", PCDRV_MODE_READ);

    if (fd < 0)
    {
        error++;
        FntPrint(-1, "Error!\n");
    }

    img_size = PClseek(fd, 0, PCDRV_SEEK_END);
    PClseek(fd, 0, PCDRV_SEEK_SET);
#else
    CdlDIR directory = CdOpenDir("./");
    if (!directory)
    {
        error++;
        FntPrint(-1, "Error!\n");
    }

    uint8_t command;
    uint8_t result;
    command = CdControlB(CdlSeekL, &directory, &result);

    if (!command)
    {
        error++;
        FntPrint(-1, "Error!\n");
    }
    else
    {
        FntPrint("Result: %d\n", result);
    }

#endif
    uint8_t tex[img_size];

#ifdef USE_PCDRV
    init = PCread(fd, tex, img_size);
    PCclose(fd);
#else
    CdCloseDir(directory);
#endif

    // initSprite(&ball_sprite, tex, &ball_tim, 160, 120, 16, 16);

    while (1)
    {
        if (error)
        {
            FntPrint(-1, "ERROR:%d\n", error);
        }
#ifdef USE_PCDRV
        FntPrint(-1, "USING PCDRV TO LOAD DATA\n", error);
        if (init != img_size)
        {
            FntPrint(-1, "ERROR READING TIM HEADER\n");
        }
#else
        FntPrint(-1, "LOADING DATA FROM CD\n");
#endif
        // FntPrint(-1, "TIM SIZE: %db\n", img_size);
        // FntPrint(-1, "Tim: %d:%d - %dx%d\n", ball_tim.prect->x, ball_tim.prect->y, ball_tim.prect->w,
        //          ball_tim.prect->h);

        // sortSprite(&context, &ball_sprite);
        // sortTPage(&context, &ball_tim);
        DrawDisplay(&context);
    }

    return 0;
}

void initSprite(SPRT *sprite, uint8_t *tex_data, TIM_IMAGE *tim, int x, int y, int w, int h)
{
    // Load tim info, CLUT and Data
    GetTimInfo((uint32_t *)tex_data, tim);
    LoadImage(tim->crect, tim->caddr);
    LoadImage(tim->prect, tim->paddr);

    // Set sprite attributes
    setSprt(sprite);
    setWH(sprite, w, h);
    setXY0(sprite, x, y);
    setUV0(sprite, 0, 0);
    setClut(sprite, tim->crect->x, tim->crect->y);
}

void sortSprite(RenderContext *context, SPRT *sprite)
{
    DrawEnv *buff = active_buffer(context);

    setRGB0(sprite, 32 + rand() % 224, 32 + rand() % 224, 32 + rand() % 224);
    addPrim(buff->ot, sprite);
    context->nextpri += sizeof(SPRT);
}

void sortTPage(RenderContext *context, TIM_IMAGE *tim)
{
    DrawEnv *buff = active_buffer(context);
    DR_TPAGE *tp = (DR_TPAGE *)context->nextpri;
    setDrawTPage(tp, 0, 0, getTPage(0, 0, tim->prect->x, tim->prect->y));

    addPrim(buff->ot + (OT_LEN - 1), tp);
    context->nextpri += sizeof(DR_TPAGE);
}