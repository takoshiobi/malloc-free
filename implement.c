#include <stdio.h>
#include <errno.h>

/* ===== MEMORY_TYPE =====
* After this type definition, the identifier MEMORY_TYPE can be used as an  * abbreviation for the type unsigned int */
typedef unsigned int MEMORY_TYPE;

/* ===== *mem, memSize, firstFree ===== 
* *mem : memory pointer, memSize : size of used memory, 
firstFree : index of the first free range */
MEMORY_TYPE *mem;
MEMORY_TYPE memSize;
MEMORY_TYPE firstFree; 

// declarations used for sbrk
extern unsigned int _kernel_end; 
extern uint32_t rpi_mmio_base;  

MEMORY_TYPE blockSize(MEMORY_TYPE x) {
  return mem[x];
}

MEMORY_TYPE next(MEMORY_TYPE x) {
  return mem[x + mem[x]];
}

MEMORY_TYPE linkToNext(MEMORY_TYPE x) {
  return x + mem[x];
}

void init(void *ptr, unsigned size) {
  mem = (MEMORY_TYPE *) ptr;
  memSize = size / sizeof(MEMORY_TYPE);
  mem[0] = memSize - 1;
  mem[memSize - 1] = memSize;
  firstFree = 0;
}

/* =============== *MALLOC =============== */

void *malloc(unsigned size) {

  if (size == 0) {  
    return NULL;
  }

  MEMTYPE num = size / sizeof(MEMORY_TYPE);
  if (size % sizeof(MEMORY_TYPE) > 0) num++;
  MEMORY_TYPE cur, prev;  
  MEMORY_TYPE allocateFirst = 1;  

  prev = cur = firstFree;

  test:
  if (firstFree == memSize) {  
    return NULL;
  }

  if (blockSize(cur) < num) {  
    allocateFirst = 0;
    prev = cur;

    if (next(cur) == memSize) {  
      return NULL;
    }
    else
      cur = next(cur);
    goto test;
  }

  if (blockSize(cur) == num) {  

    if (allocateFirst)
      firstFree = next(cur);
    else
      mem[linkToNext(prev)] = next(cur);
  }

  else {  

    if (allocateFirst) {
      if ((blockSize(cur) - num) == 1)  
        firstFree = next(cur);
      else
        firstFree = cur + num + 1;
    }
    else {
      if ((blockSize(cur) - num) == 1)  
        mem[linkToNext(prev)] = next(cur);
      else
        mem[linkToNext(prev)] = cur + num + 1;
    }

    if ((blockSize(cur) - num) == 1)  
      mem[cur] = num + 1;
    else {
      mem[cur + num + 1] = blockSize(cur) - num - 1;
      mem[cur] = num;
    }
  }

  return (void *) &(mem[cur+1]);
}


/* =============== FREE =============== */

void free(void *ptr) {

  MEMORY_TYPE toFree;  
  MEMORY_TYPE cur, prev;

  toFree = ((MEMORY_TYPE *)ptr - (mem + 1));


  if (toFree < firstFree) {  

    if (((linkToNext(toFree) + 1) == firstFree) && firstFree < memSize)  
      mem[toFree] += (mem[firstFree] + 1);  
    else
      mem[linkToNext(toFree)] = firstFree;

    firstFree = toFree;
  }

  else {  

    prev = cur = firstFree;

    while (cur < toFree) {
      prev = cur;
      cur = next(cur);
    }

    if ((linkToNext(prev) + 1) == toFree) { 

      mem[prev] += (mem[toFree] + 1);  

      if (((linkToNext(toFree) + 1) == cur) && cur < memSize)  
        mem[prev] += (mem[cur] + 1);  
      else
        mem[linkToNext(toFree)] = cur;
    }
    else {
      mem[linkToNext(prev)] = toFree;
      mem[linkToNext(toFree)] = cur;
    }

  }

/* =============== *SBRK =============== */

void *sbrk (int incr){
  static char *heap_end = (char *)(&_kernel_end);
  char *heap_max = (char *)(rpi_mmio_base - 1);

  char *ptr;

  if (heap_end + incr <= heap_max)
  {
    ptr = heap_end;
    heap_end += incr;
    return (void *)ptr;
  } else {
    errno = ENOMEM;
    return (void *)(-1);
  }
}
