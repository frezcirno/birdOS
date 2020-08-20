; 
; boot0.asm
; 

%include "load.inc"

    org 0x7c00

    ; 关中断
    cli

    ; 重置CS:IP
    jmp 0:entry ; 保证cs:ip = 0:7cxx

entry:
    ; 初始化段寄存器 ds = es = 0
    mov ax, 0
    mov ds, ax
    mov es, ax

    ; 初始化栈
    mov ss, ax
    mov sp, 0x7c00

    ; 重置显示模式
    ; mov ax, 0x0002 ; 80x25x16色文本模式
    ; int 0x00

    ; 清屏
    mov ax, 0x0600 ; 向上滚动, 行数
    xor bx, bx
    xor cx, cx
    mov dx, 0x1950
    int 0x10

    ; 打印提示信息
    mov ax, msg_hello
    mov bp, ax     ; es:bp = msg
    mov cx, msg_hello_end - msg_hello
    mov dx, 0x0000  ; 行, 列
    call PrintLine
    
    ; 打开中断
    sti

    ; fdd复位
    xor ah, ah
    xor dl, dl
    int 0x13

    ; 将位于0-0-2到0-0-18的boot1加载到内存BOOT1_ADDR处
    mov ax, 0
    mov es, ax ; 目标地址
    mov bx, BOOT1_ADDR ; 目标地址
    mov dx, 0 ; 磁头, 驱动器号
    mov cx, 2 ; 柱面, 扇区
    mov al, 17; 扇区数量
    call ReadSector

    ; 跳转到boot1
    jmp 0:BOOT1_ADDR

%include "lib16.inc"

error:
    cli ; 再次关中断
    mov ax, ds
    mov es, ax     ; es = ds
    mov ax, msg_error
    mov bp, ax     ; bp = msg
    mov cx, msg_error_end - msg_error
    call PrintLine
error1:
    hlt
    jmp error1

msg_hello  db  "BootLoader ok!" 
msg_hello_end:
msg_error  db  "BootLoader Error!" 
msg_error_end:

; MBR flag
    times  510-($-$$) db 0 ; fill zeros to make it exactly 512 bytes
    dw  0xaa55    ; boot record signature