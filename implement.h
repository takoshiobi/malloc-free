#include <stdio.h>

/* ===== blockSize ===== 
* Returns size of memory block */
MEMORY_TYPE blockSize(MEMORY_TYPE x);

/* ===== next =====
* Returns next free block of memory */
MEMORY_TYPE next(MEMORY_TYPE x);

/* ===== linkToNext =====
* Returns index of pointer to next free block */
MEMORY_TYPE linkToNext(MEMORY_TYPE x);

/* ===== init =====
* Memory initialization */
void init(void *ptr, unsigned size);

/* ========== MEMORY ALLOCATOR ========== */

/* ===== malloc =====
* Custom malloc function implementation */
void *malloc(unsigned size);

/* ========== FREE MEMORY ========== */

/* ===== free =====
* Free memory block */
void free(void *ptr);

/* ===== sbrk =====
* ----- */
void *sbrk (int incr);
