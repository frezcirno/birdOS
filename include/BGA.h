/**
 * Bochs emulated graphics hardware
 * (henceforth called BGA for Bochs Graphics Adaptor)
 */
// 读: 先向INDEX PORT写入INDEX, 然后去DATA PORT读取DATA
// 写: 先向INDEX PORT写入INDEX, 然后去DATA PORT写入新DATA
#pragma once

#define VBE_DISPI_IOPORT_INDEX 0x01CE
#define VBE_DISPI_IOPORT_DATA  0x01CF

#define VBE_DISPI_DISABLED    0x00
#define VBE_DISPI_ENABLED     0x01
#define VBE_DISPI_LFB_ENABLED 0x40 // 启动参数, 开启LFB, 如果开启VBE拓展时,
//不显示指定使用LFB, 则默认使用bank
#define VBE_DISPI_NOCLEARMEM 0x80 // 启动参数, 不重置显存区

#define VBE_DISPI_INDEX_ID          0x0 //
#define VBE_DISPI_INDEX_XRES        0x1 // X分辨率
#define VBE_DISPI_INDEX_YRES        0x2 // Y分辨率
#define VBE_DISPI_INDEX_BPP         0x3 // 位深度
#define VBE_DISPI_INDEX_ENABLE      0x4 // 是否开启VBE拓展 & 是否使用LFB
#define VBE_DISPI_INDEX_BANK        0x5 // 当前对应的显存bank号
#define VBE_DISPI_INDEX_VIRT_WIDTH  0x6 // 虚拟屏幕尺寸, 默认和实际尺寸一致
#define VBE_DISPI_INDEX_VIRT_HEIGHT 0x7 // 虚拟屏幕尺寸, 默认和实际尺寸一致
#define VBE_DISPI_INDEX_X_OFFSET    0x8 // 屏幕实际显示的左上角位置
#define VBE_DISPI_INDEX_Y_OFFSET    0x9 // 屏幕实际显示的左上角位置

#define VBE_DISPI_ID0 0xB0C0
// setting X and Y resolution and bit depth(8 BPP only), banked mode
#define VBE_DISPI_ID1 0xB0C1 // virtual width and height, X and Y offset
#define VBE_DISPI_ID2 0xB0C2
// 15, 16, 24 and 32 BPP modes, support for linear frame buffer,
// support for retaining memory contents on mode switching
#define VBE_DISPI_ID3 0xB0C3
// support for getting capabilities, support for using 8 bit DAC
#define VBE_DISPI_ID4 0xB0C4 // VRAM increased to 8 MB
#define VBE_DISPI_ID5 0xB0C5 // VRAM increased to 16 MB

#define VBE_DISPI_BPP_4  0x04
#define VBE_DISPI_BPP_8  0x08
#define VBE_DISPI_BPP_15 0x0F
#define VBE_DISPI_BPP_16 0x10
#define VBE_DISPI_BPP_24 0x18
#define VBE_DISPI_BPP_32 0x20

extern unsigned int cur_bank;
extern unsigned int bank_start, bank_end;

void bga_write_register(unsigned short index, unsigned short data);
unsigned short bga_read_register(unsigned short index);
void bga_set_video_mode(unsigned int Width, unsigned int Height,
                        unsigned int BitDepth, int use_lfb, int clear);

int bga_get_lfb_addr();