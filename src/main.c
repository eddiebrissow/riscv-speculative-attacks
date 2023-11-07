// #include <emmintrin.h>
// #include <x86intrin.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "time.h"
#include <sys/cachectl.h>
#include "d1.h"
#include "cache.h"


#define CACHE_HIT_THRESHOLD (2000)
#define SIZE_2 (1024)
#define SIZE_1 (10)


uint8_t array[SIZE_1*SIZE_2];




int main(int argc, const char **argv) {
  int junk=0;
  unsigned long time1, time2;
  volatile uint8_t *addr;
  int i;

  uint8_t *end = &array[i*SIZE_2];
  uint8_t *start = &array[0];

  // Initialize the array
  for(i=0; i<SIZE_1; i++) array[i*SIZE_2]=100;

  // FLUSH the array from the CPU cache
  
  // Flush by waiting
  // wait(10); // // Doesn't work


  // Flush by allocating more than L1
  // flush_allocate(); // // Doesn't work


  // Linux flush
  // for(i=0; i<SIZE_1; i++) __riscv_flush_icache(start, end, 0); // // Doesn't work


  // Boom flush
  for(i=0; i<SIZE_1; i++) flushCache(&array, SIZE_1*SIZE_2); // // Doesn't work

  // Flush by fence instruction cache
  // fence(); // Doesn't work

  // Flush by sfence
  // sfence(); // Illegal

  // Flush using custom instruction
  // writel(IOMMU + IOMMU_TLB_FLUSH_ENABLE_REG, FLUSH_TBD); // Not allowed

  // CLWB test
  // for(i=0; i<SIZE_1; i++) clwb(&array[i*SIZE_2]); // Doesn't support

  // Access some of the array items
  array[3*SIZE_2] = 100;
  array[7*SIZE_2] = 200;

  for(i=0; i<SIZE_1; i++) {
    addr = &array[i*SIZE_2];
    time1 = rdcycle();
    junk = *addr;
    time2 = rdcycle() - time1;  
    if (time2 <= CACHE_HIT_THRESHOLD)
    printf("Access time for array[%d*%d]: %d CPU cycles\n",i,SIZE_2, (int)time2);
  }
  return 0; 
}