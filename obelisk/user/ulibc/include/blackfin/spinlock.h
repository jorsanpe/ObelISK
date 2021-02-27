/*
 * $FILE: spinlock.h
 *
 * Job dispatcher
 *
 * Author: Jordi Sánchez <jorsanp3@upvnet.upv.es>
 */
#ifndef _SPINLOCK_H_
#define _SPINLOCK_H_

#define SPIN_LOCK(lock) __asm__ __volatile__("p5 = %0;\n\t" \
                        "TESTSET p5;\n\t" : : "r"(lock));

#define SPIN_UNLOCK(lock) __asm__ __volatile__("%0 = 0;\n\t" : "=r"(lock) : );

typedef unsigned short spinlock_t;

#endif //_SPINLOCK_H_
