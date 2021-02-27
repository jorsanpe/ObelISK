/*
 * $FILE: camera.h
 *
 * ObelISK Camera Device Driver header
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

#ifndef _CAMERA_H_
#define _CAMERA_H_

#define GRAY   0
#define COLOR  1

#define OV7660_CCIR656_QVGA_YUV_15FPS  0
#define OV7660_CCIR656_VGA_YUV_15FPS   1

extern int open_camera(int mode, int col);
extern void read_frame(unsigned char *pframe);

#endif //_CAMERA_H_
