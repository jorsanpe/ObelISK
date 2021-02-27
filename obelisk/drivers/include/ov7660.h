/*
 * $FILE: ov7660.h
 *
 * ov7660 camera driver header
 *
 * Author: Jordi Sánchez, <jorsanp3@upvnet.upv.es>
 *
 * $LICENSE:
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 */
#ifndef _OV7660_H_
#define _OV7660_H_

#define OV7660                            0x7660
#define OV7660_ADDRESS                    0x42

#define OV7660_COLOR                      1
#define OV7660_GRAY                       0

//#define OV7660_CCIR656_QVGA_RGB_15FPS     1
//#define OV7660_CCIR656_QVGA_YUV_15FPS     2
//#define OV7660_CCIR656_VGA_YUV_15FPS      3

#define OV7660_RIGHT_ON                   1
#define OV7660_RIGHT_OFF                  0
#define OV7660_LEFT_ON                    1
#define OV7660_LEFT_OFF                   0

// IMPLEMENTATION DEPENDENT
#define OV7660_SIOC_PIN                   PF15
#define OV7660_SIOD_PIN                   PF12
#define OV7660_PWDN_PIN                   PF10

#endif //_OV7660_H_
