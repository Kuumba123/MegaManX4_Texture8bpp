/*
* Written by PogChampGuy AKA Kuumba (https://www.twitch.tv/kuumba_)
* 
* This mod adds in support for 8bpp Textures for Background Layers 1-3.
* This was later added in MegaMan X5/X6 however MegaMan X4 only supports
* 4bpp Textures , this mod is mainly designed to be used with my Editor.
*/

#include <common.h>
#include <layer.h>
#include <gpu.h>
#include <misc.h>
#include "layerinfo.h"
#include <libcd.h>
void NewResetLayer(int layer);
void NewDrawLayer(int layer);

void NewDrawBackground()
{
    CdSearchFile(0,"TEST");
    rectCount = 0;
    tempPrimP = &rectBuffer[buffer];

    for (size_t i = 0; i < 3; i++)
    {
        if (bgLayers[i].update)
        {
            DumpLayerScreens(i);
        }
    }
    DumpActiveScreens(); // BG1-3

    NewIntroEffect();

    for (size_t i = 0; i < 3; i++)
    {
        if (bgLayers[i].display)
        {
            NewResetLayer(i);
            NewDrawLayer(i);
        }
    }
}

void NewIntroEffect() // TODO: add an option for disabling this I guess...
{
    if (game.stageid == 0)
    {
        TILE *monoRect = (TILE *)0x8013b7b0;
        AddPrim(&drawP->ot[0xB], &monoRect[buffer]);
    }
}

void NewResetLayer(int layer)
{
    for (size_t i = 0; i < 2; i++)
    {
        int ot;
        if (i == 0)
        {
            ot = bgLayers[layer].normalOT;
        }
        else
        {
            ot = bgLayers[layer].priorityOT;
        }
        for (size_t o = 0; o < (4 + 8); o++)
        {
            CatPrim(&layerInfo[i + layer * 2 + buffer * 6].tpages[o], &layerInfo[i + layer * 2 + buffer * 6].tpages[o + 1]);
        }
        AddPrims(&drawP->ot[ot], &layerInfo[i + layer * 2 + buffer * 6].tpages[0], &layerInfo[i + layer * 2 + buffer * 6].tpages[4 + 8 - 1]);
    }
}

#define tileInfoP (((u_char *)*(int *)0x1f80000c))
#define activeScreenBuffer 0x801441c8
void NewDrawLayer(int layer)
{
    int layerX = bgLayers[layer].x;
    int layerY = bgLayers[layer].y;

    for (size_t y = 0; y < 16; y++)
    {
        for (size_t x = 0; x < 21; x++)
        {
            int x16 = (layerX + x * 16) >> 4;
            int y16 = (layerY + y * 16) >> 4;

            ushort tileVal = *(u_short *)(activeScreenBuffer + (x16 & 0x1F) * 2 + (y16 & 0x1F) * 0x40 + layer * 0x800);
            if (tileVal != 0)
            {
                if (rectCount > 999)
                {
                    return;
                }
                ushort tileId = tileVal & 0x3FFF;

                /*Get Various Tile Id Info*/
                u_char clut = tileInfoP[tileId * 4 + 1];
                u_char cord = tileInfoP[tileId * 4 + 2];
                u_char tpage = tileInfoP[tileId * 4 + 3];

                if(tpage == 0xFF || tpage > 0xB) //just in case someone imports a X5 Level
                {
                    continue;
                }
                SPRT_16 *rect = (SPRT_16 *)tempPrimP;
                
                /*RAW Textured Rectangle*/
                *(u_char *)((int)&rect->tag + 3) = 3;
                rect->code = 0x7D;

                if ((tileVal & 0x4000) != 0) // Semi-Trans flag
                {
                    SetSemiTrans(rect, 1);
                }

                if ((tileVal & 0x8000) != 0) // Priority
                {
                    AddPrim(&layerInfo[buffer * 6 + layer * 2 + 1].tpages[tpage], rect);
                }
                else // Normal
                {
                    AddPrim(&layerInfo[buffer * 6 + layer * 2].tpages[tpage], rect);
                }
                /********/

                rect->clut = GetClutCord(clut + 64);
                setUV0(rect, (cord & 0xF) * 16, cord & 0xF0);

                /*Calculate Draw Location*/
                int innerX = layerX & 0xF;
                int innerY = layerY & 0xF;
                setXY0(rect, x * 16 - innerX, y * 16 - innerY);

                rectCount += 1;
                tempPrimP += sizeof(SPRT_16);
            }
        }
    }
}

#undef activeScreenBuffer
#undef tileInfoP