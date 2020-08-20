#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "fs.h"
#include "tty.h"
#include "console.h"
#include "proc.h"
#include "global.h"
#include "proto.h"
#include "glib.h"
#include "BGA.h"
#include "font.h"
#include "string.h"
#include "memory.h"

void bga_write_register(unsigned short index, unsigned short data)
{
    out16(VBE_DISPI_IOPORT_INDEX, index);
    out16(VBE_DISPI_IOPORT_DATA, data);
}

unsigned short bga_read_register(unsigned short index)
{
    out16(VBE_DISPI_IOPORT_INDEX, index);
    return in16(VBE_DISPI_IOPORT_DATA);
}

void bga_set_video_mode(unsigned int Width, unsigned int Height,
                        unsigned int BitDepth, int use_lfb, int clear)
{
    bga_write_register(VBE_DISPI_INDEX_ENABLE, VBE_DISPI_DISABLED);
    bga_write_register(VBE_DISPI_INDEX_XRES, Width);
    bga_write_register(VBE_DISPI_INDEX_YRES, Height);
    bga_write_register(VBE_DISPI_INDEX_BPP, BitDepth);
    bga_write_register(VBE_DISPI_INDEX_ENABLE,
                       VBE_DISPI_ENABLED | (use_lfb ? VBE_DISPI_LFB_ENABLED : 0)
                           | (clear ? 0 : VBE_DISPI_NOCLEARMEM));
}

int bga_get_lfb_addr()
{
    for (int bus = 0; bus < 5; bus++)
    {
        for (int device = 0; device < 32; device++)
        {
            int paddr = (0x80000000 | (bus << 16) | (device << 11));
            out32(0xcf8, paddr);
            int in = in32(0xcfc);
            if (in == 0x11111234)
            {
                out32(0xcf8, paddr | 0x10); // BAR0
                return in32(0xcfc) & 0xfffffff0;
            }
        }
    }
    return 0;
}
