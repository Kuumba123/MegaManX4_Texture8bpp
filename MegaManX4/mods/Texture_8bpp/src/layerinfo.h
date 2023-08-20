#include <gpu.h>
typedef struct /*4 per Layer*/
{
    DR_TPAGE tpages[8 + 4]; //8 * 4bpp , 4 * 8bpp
} drawLayer;
/*Needs 0xB40 bytes*/

#define layerInfo ((drawLayer*)0x8000A000) //unused region of kernal RAM