#include "time.h"
#include <stdlib.h>


#define IOMMU 0x02010000
#define IOMMU_TLB_FLUSH_ENABLE_REG 0x0010
#define FLUSH_TBD 0x0003003f



unsigned int readl(unsigned int address){
    unsigned int reg_value;
    asm volatile ("lw %0, 0(%1)" : "=r" (reg_value) : "r" (address));
    return reg_value;
}

void writel(unsigned int address, unsigned int value){
    asm volatile (
        "sw %0, 0(%1)"  // Stores a new value on register
        :
        : "r" (value), "r" (address)
    );
}


void fence(){
     asm volatile ("fence.i");
}

void sfence(){
    asm volatile ("sfence.vma");
}

void clwb(void *address) {
    // asm volatile ("clwb %0" : "+m" (*(char *)address)); // doesn't exists
}


unsigned long rdcycle(void)
{
    unsigned long dst;
    asm volatile ("csrrs %0, 0xc00, x0" : "=r" (dst) );
    return dst;
}


void wait(int seconds){
  unsigned long  time1 = clock();
  while(1){
    if (clock() -  time1 >= seconds*1000*1000){
      break;
    }
  }
}

void flush_allocate(){
     const int size = 64*1024; // Allocate 64kb. Set much larger than L1
     char *c = (char *)malloc(size);
     for (int i = 0; i < 0xff; i++)
       for (int j = 0; j < size; j++)
         c[j] = i*j;
}
