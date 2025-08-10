#include "controller.h"
#include "display.h"
#include "psxsn.h"

void SortSprite(RenderContext *context, uint32_t x, uint32_t y, uint32_t w, uint32_t h, TIM_IMAGE *texture);
void SortTPage(RenderContext *context, TIM_IMAGE *texture);

int main()
{
    RenderContext context;
    context.active_buffer = 0;
    uint32_t fd;
    int32_t error = 0;
    TIM_IMAGE texture;
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
#endif
    uint8_t tex[img_size];

#ifdef USE_PCDRV
    init = PCread(fd, tex, img_size);
#endif

    GetTimInfo((uint32_t *)tex, &texture);
    LoadImage(texture.crect, texture.caddr);
    LoadImage(texture.prect, texture.paddr);

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
        FntPrint(-1, "TIM SIZE: %db\n", img_size);
        FntPrint(-1, "Tim: %d:%d - %dx%d\n", texture.prect->x, texture.prect->y, texture.prect->w, texture.prect->h);

        SortSprite(&context, 80, 80, 16, 16, &texture);
        SortTPage(&context, &texture);
        DrawDisplay(&context);
    }

#ifdef USE_PCDRV
    PCclose(fd);
#endif

    return 0;
}

void SortSprite(RenderContext *context, uint32_t x, uint32_t y, uint32_t w, uint32_t h, TIM_IMAGE *texture)
{
    DrawEnv *buff = active_buffer(context);
    SPRT *spr = (SPRT *)context->nextpri;

    setSprt(spr);
    setWH(spr, w, h);
    setXY0(spr, x, y);
    setRGB0(spr, 32 + rand() % 224, 32 + rand() % 224, 32 + rand() % 224);
    setUV0(spr, 0, 0);
    setClut(spr, texture->crect->x, texture->crect->y);

    addPrim(buff->ot, spr);
    context->nextpri += sizeof(SPRT);
}

void SortTPage(RenderContext *context, TIM_IMAGE *texture)
{
    DrawEnv *buff = active_buffer(context);
    DR_TPAGE *tp = (DR_TPAGE *)context->nextpri;
    setDrawTPage(tp, 0, 0, getTPage(0, 0, texture->prect->x, texture->prect->y));

    addPrim(buff->ot + (OT_LEN - 1), tp);
    context->nextpri += sizeof(DR_TPAGE);
}