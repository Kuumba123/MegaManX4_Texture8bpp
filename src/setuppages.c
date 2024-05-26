#include <common.h>
#include <gpu.h>
#include "layerinfo.h"

void SetupPages(uint tpageB, uint transT) // also sets up a few other things
{
    for (size_t b = 0; b < 2; b++)
    {
        for (size_t p = 0; p < 8; p++) // 4bpp
        {
            for (size_t l = 0; l < 3; l++)
            {
                SetDrawTPage(&layerInfo[b * 6 + l * 2].tpages[p], 0, 0, GetTPageValue(0, transT, 5 + p, 1));
                SetDrawTPage(&layerInfo[b * 6 + l * 2 + 1].tpages[p], 0, 0, GetTPageValue(0, transT, 5 + p, 1));
            }
        }
        for (size_t p = 0; p < 4; p++) // 8bpp
        {
            for (size_t l = 0; l < 3 * 4; l++)
            {
                SetDrawTPage(&layerInfo[b * 6 + l * 2].tpages[p + 8], 0, 0, GetTPageValue(1, transT, 5 + p * 2, 1));
                SetDrawTPage(&layerInfo[b * 6 + l * 2 + 1].tpages[p + 8], 0, 0, GetTPageValue(1, transT, 5 + p * 2, 1));
            }
        }
    }
    /*Setup SPRT_16 Buffer (mainly for pause menu)*/
    SPRT_16 *rect = &rectBuffer[0];
    for (size_t i = 0; i < 0x800; i++)
    {
        /*RAW Textured Rectangle*/
        *(u_char *)((int)&rect->tag + 3) = 3;
        rect->code = 0x7D;
        rect += 1;
    }

    /*For Intro Effects*/
    TILE *monoRect = (TILE *)0x8013b7b0;

    for (size_t i = 0; i < 2; i++)
    {
        SetTile(&monoRect[i]);
        setRGB0(&monoRect[i], 8, 0x18, 0x31);
        monoRect[i].w = 0x140;
        monoRect[i].x0 = 0;
        monoRect[i].y0 = 0;
        monoRect[i].h = 0x60;
    }

    /*Layout Pointer*/
    ((int *)0x1f800004)[0] = 0x80141be8;
}