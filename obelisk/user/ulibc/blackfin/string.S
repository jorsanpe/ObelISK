/*
 * $FILE: string.S
 *
 * String functions optimized for blackfin architecture. This
 * functions have some alignment constraints
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
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

.global   _memclr;
.align 8;
_memclr:
   // Push registers needed
   [--sp] = r2;
   [--sp] = p0;
   [--sp] = p1;

   // Initialize pointers
   p0 = r0;
   p1 = r1;
   p1 = p1 >> 2;
   r2 = 0 (z);

   // Clear 4 bytes by 4 bytes
   LSETUP(clrloop,clrloop)  lc0 = p1;
clrloop: [p0++] = r2;

   // Check if there are any bytes left
   r2 = 0x3 (z);
   r2 = r1 & r2;
   cc = r2 == 0;
   if cc jump finishclr;

   // Clear remaining bytes
   p1 = r2;
   r2.l = 0x0;
   LSETUP(rclrloop,rclrloop)  lc0 = p1;
rclrloop: b[p0++] = r2;

finishclr:
   // Restore registers needed
   p1 = [sp++];
   p0 = [sp++];
   r2 = [sp++];

   rts;
.size _memclr, .-_memclr

.global   _memset;
.align 8;
_memset:
	P0 = R0 ;              /* P0 = address */
	P2 = R2 ;              /* P2 = count   */
	R3 = R0 + R2;          /* end          */
	CC = R2 <= 7(IU);
	IF CC JUMP  .Lset_too_small;
	R1 = R1.B (Z);         /* R1 = fill char */
	R2 =  3;
	R2 = R0 & R2;          /* addr bottom two bits */
	CC =  R2 == 0;             /* AZ set if zero.	*/
	IF !CC JUMP  .Lset_force_align ;  /* Jump if addr not aligned. */

.Lset_aligned:
	P1 = P2 >> 2;          /* count = n/4        */
	R2 = R1 <<  8;         /* create quad filler */
	R2.L = R2.L + R1.L(NS);
	R2.H = R2.L + R1.H(NS);
	P2 = R3;

	LSETUP (.Lset_quad_loop , .Lset_quad_loop) LC0=P1;
.Lset_quad_loop:
	[P0++] = R2;

	CC = P0 == P2;
	IF !CC JUMP .Lset_bytes_left;
	RTS;

.Lset_bytes_left:
	R2 = R3;                /* end point */
	R3 = P0;                /* current position */
	R2 = R2 - R3;           /* bytes left */
	P2 = R2;

.Lset_too_small:
	CC = P2 == 0;           /* Check zero count */
	IF CC JUMP .Lset_finished;    /* Unusual */

.Lset_bytes:
	LSETUP (.Lset_byte_loop , .Lset_byte_loop) LC0=P2;
.Lset_byte_loop:
	B[P0++] = R1;

.Lset_finished:
	RTS;

.Lset_force_align:
	CC = BITTST (R0, 0);  /* odd byte */
	R0 = 4;
	R0 = R0 - R2;
	P1 = R0;
	R0 = P0;		    /* Recover return address */
	IF !CC JUMP .Lset_skip1;
	B[P0++] = R1;
.Lset_skip1:
	CC = R2 <= 2;          /* 2 bytes */
	P2 -= P1;              /* reduce count */
	IF !CC JUMP .Lset_aligned;
	B[P0++] = R1;
	B[P0++] = R1;
	JUMP .Lset_aligned;
.size _memset, .-_memset

.global   _memcmp;
.align 8;
_memcmp:
	I1 = P3;
	P0 = R0;			/* P0 = s1 address */
	P3 = R1;			/* P3 = s2 Address  */
	P2 = R2 ;			/* P2 = count */
	CC = R2 <= 7(IU);
	IF CC JUMP .Lcmp_too_small;
	I0 = R1;			/* s2 */
	R1 = R1 | R0;		/* OR addresses together */
	R1 <<= 30;		/* check bottom two bits */
	CC =  AZ;			/* AZ set if zero. */
	IF !CC JUMP .Lcmp_bytes ;	/* Jump if addrs not aligned. */

	P1 = P2 >> 2;		/* count = n/4 */
	R3 =  3;
	R2 = R2 & R3;		/* remainder */
	P2 = R2;			/* set remainder */

	LSETUP (.Lcmp_quad_loop_s, .Lcmp_quad_loop_e) LC0=P1;
.Lcmp_quad_loop_s:
#if ANOMALY_05000202
	R0 = [P0++];
	R1 = [I0++];
#else
	MNOP || R0 = [P0++] || R1 = [I0++];
#endif
	CC = R0 == R1;
	IF !CC JUMP .Lcmp_quad_different;
.Lcmp_quad_loop_e:
	NOP;

	P3 = I0;			/* s2 */
.Lcmp_too_small:
	CC = P2 == 0;		/* Check zero count*/
	IF CC JUMP .Lcmp_finished;	/* very unlikely*/

.Lcmp_bytes:
	LSETUP (.Lcmp_byte_loop_s, .Lcmp_byte_loop_e) LC0=P2;
.Lcmp_byte_loop_s:
	R1 = B[P3++](Z);	/* *s2 */
	R0 = B[P0++](Z);	/* *s1 */
	CC = R0 == R1;
	IF !CC JUMP .Lcmp_different;
.Lcmp_byte_loop_e:
	NOP;

.Lcmp_different:
	R0 = R0 - R1;
	P3 = I1;
	RTS;

.Lcmp_quad_different:
	/* We've read two quads which don't match.
	 * Can't just compare them, because we're
	 * a little-endian machine, so the MSBs of
	 * the regs occur at later addresses in the
	 * string.
	 * Arrange to re-read those two quads again,
	 * byte-by-byte.
	 */
	P0 += -4;		/* back up to the start of the */
	P3 = I0;		/* quads, and increase the*/
	P2 += 4;		/* remainder count*/
	P3 += -4;
	JUMP .Lcmp_bytes;

.Lcmp_finished:
	R0 = 0;
	P3 = I1;
	RTS;
.size _memcmp, .-_memcmp

.global   _memcpy;
.align 8;
_memcpy:
	CC = R2 <=  0;	/* length not positive? */
	IF CC JUMP .Lcpy_badlength;	/* Nothing to do */

	P0 = R0 ;	/* dst*/
	P1 = R1 ;	/* src*/
	P2 = R2 ;	/* length */

	/* check for overlapping data */
	CC = R1 < R0;	/* src < dst */
	IF !CC JUMP .Lcpy_no_overlap;
	R3 = R1 + R2;
	CC = R0 < R3;	/* and dst < src+len */
	IF CC JUMP .Lcpy_has_overlap;

.Lcpy_no_overlap:
	/* Check for aligned data.*/

	R3 = R1 | R0;
	R1 = 0x3;
	R3 = R3 & R1;
	CC = R3;	/* low bits set on either address? */
	IF CC JUMP .Lcpy_not_aligned;

	/* Both addresses are word-aligned, so we can copy
	at least part of the data using word copies.*/
	P2 = P2 >> 2;
	CC = P2 <= 2;
	IF !CC JUMP .Lcpy_more_than_seven;
	/* less than eight bytes... */
	P2 = R2;
	LSETUP(.Lcpy_three_start, .Lcpy_three_end) LC0=P2;
.Lcpy_three_start:
	R3 = B[P1++] (X);
.Lcpy_three_end:
	B[P0++] = R3;

	RTS;

.Lcpy_more_than_seven:
	/* There's at least eight bytes to copy. */
	P2 += -1;	/* because we unroll one iteration */
	LSETUP(.Lcpy_word_loops, .Lcpy_word_loope) LC0=P2;
	I1 = P1;
	R3 = [I1++];
#if ANOMALY_05000202
.Lcpy_word_loops:
	[P0++] = R3;
.Lcpy_word_loope:
	R3 = [I1++];
#else
.Lcpy_word_loops:
.Lcpy_word_loope:
	MNOP || [P0++] = R3 || R3 = [I1++];
#endif
	[P0++] = R3;
	/* Any remaining bytes to copy? */
	R3 = 0x3;
	R3 = R2 & R3;
	CC = R3 == 0;
	P1 = I1;	/* in case there's something left, */
	IF !CC JUMP .Lcpy_bytes_left;
	RTS;
.Lcpy_bytes_left:	P2 = R3;
.Lcpy_not_aligned:
	/* From here, we're copying byte-by-byte. */
	LSETUP (.Lcpy_byte_start, .Lcpy_byte_end) LC0=P2;
.Lcpy_byte_start:
	R1 = B[P1++] (X);
.Lcpy_byte_end:
	B[P0++] = R1;

.Lcpy_badlength:
	RTS;

.Lcpy_has_overlap:
	/* Need to reverse the copying, because the
	 * dst would clobber the src.
	 * Don't bother to work out alignment for
	 * the reverse case.
	 */
	P0 = P0 + P2;
	P0 += -1;
	P1 = P1 + P2;
	P1 += -1;
	LSETUP(.Lcpy_over_start, .Lcpy_over_end) LC0=P2;
.Lcpy_over_start:
	R1 = B[P1--] (X);
.Lcpy_over_end:
	B[P0--] = R1;

	RTS;
.size _memcpy, .-_memcpy



