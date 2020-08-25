/*************************************************************************//**
 *****************************************************************************
 * @file   sl.c
 * @brief  sl
 * @author BirdOS小组
 * @date   2015
 *****************************************************************************
 *****************************************************************************/

#include "type.h"
#include "stdio.h"
#include "const.h"
#include "protect.h"
#include "string.h"
#include "fs.h"
#include "proc.h"
#include "tty.h"
#include "console.h"
#include "global.h"
#include "proto.h"

/*****************************************************************************
 *                                sl
 *****************************************************************************/


PUBLIC void sl(){
  char str[15]="loading...";
  int delay_time = 600;
  clear();
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                              **************                                                    \n");
  printf("                            **  **********                                                      \n");
  printf("              ******     ***  *********                                                         \n");
  printf("             ** @  ***  ***  ********           xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx               \n");
  printf("         ****       ****   *******              xx                             xx               \n");
  printf("              ********   *******                xx  Bird-OS                    xx               \n");                      
  printf("              *********  ****                   xx  Based on Orange's Kernal   xx               \n");
  printf("               ******** ****                    xx  version 1.0.0              xx               \n");
  printf("                *************                   xx                             xx               \n");
  printf("                  ************                  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx               \n");
  printf("                    ****************                                                            \n");
  printf("                          *******                                                               \n");
  printf("                            ***                                                                 \n");
  printf("                             *                                                                  \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("        ==================================================================================      \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("              #######   ######  ########   #####                ######      ######              \n");
  printf("              ##    ##    ##    ##     ##  ##   ##             ##    ##   ##      #             \n");
  printf("              ######      ##    ########   ##    ##  #######  ##      ##    #####               \n");
  printf("              ##    ##    ##    ##  ##     ##   ##             ##    ##   #      ##             \n");
  printf("              #######   ######  ##    ###  #####                ######     ######               \n");
  printf("                                                                                                \n"); 
  printf("                                                                                                \n");
  printf("        ==================================================================================      \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  drawRect(100,500,700,550,0);
  fillRect(100,500,200,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time);
  //clear();

  drawRect(100,500,700,550,0);
  fillRect(100,500,300,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time);
  //clear();

  drawRect(100,500,700,550,0);
  fillRect(100,500,350,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time);
  //clear();

  drawRect(100,500,700,550,0);
  fillRect(100,500,450,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time);
  //clear();

  drawRect(100,500,700,550,0);
  fillRect(100,500,550,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time);
  //clear();
 
  drawRect(100,500,700,550,0);
  fillRect(100,500,625,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time);
  //clear();

  drawRect(100,500,700,550,0);
  fillRect(100,500,675,550,0);
  drawText(705,530,str,1);
  milli_delay(delay_time); 
  //clear();

  drawRect(100,500,700,550,0);
  fillRect(100,500,700,550,0);
  drawText(705,530,str,1);

  clear();
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                              **************                                                    \n");
  printf("                            **  **********                                                      \n");
  printf("              ******     ***  *********                                                         \n");
  printf("             ** @  ***  ***  ********           xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx               \n");
  printf("         ****       ****   *******              xx                             xx               \n");
  printf("              ********   *******                xx  Bird-OS                    xx               \n");                      
  printf("              *********  ****                   xx  Based on Orange's Kernal   xx               \n");
  printf("               ******** ****                    xx  version 1.0.0              xx               \n");
  printf("                *************                   xx                             xx               \n");
  printf("                  ************                  xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx               \n");
  printf("                    ****************                                                            \n");
  printf("                          *******                                                               \n");
  printf("                            ***                                                                 \n");
  printf("                             *                                                                  \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("        ==================================================================================      \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("            ##   ##   ##  #######  ##        #####     #####       ##  ##     #######           \n");
  printf("            ##   ##   ##  ##       ##       ##    #   ##   ##     ###  ###    ##                \n");
  printf("             ##  ##  ##   #######  ##      ##        ##     ##   ##  ##  ##   #######           \n");
  printf("              ###  ###    ##       ##       ##    #   ##   ##   ##   ##   ##  ##                \n");
  printf("               ##  ##     #######  #######   #####     #####    ##   ##   ##  #######           \n");
  printf("                                                                                                \n"); 
  printf("                                                                                                \n");
  printf("        ==================================================================================      \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("                                                                                                \n");
  printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
  delay_time=2000;
  milli_delay(delay_time);
  return;
}
