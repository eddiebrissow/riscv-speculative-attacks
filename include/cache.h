#ifndef CACHE_H
#define CACHE_H
#include <metal/cache.h>
#include <metal/cpu.h>
#include <metal/machine.h>
#include <metal/memory.h>
#include <metal/csr.h>

// cache values
// TODO: check that these parameters are right
#define L1_SETS 64
#define L1_SET_BITS 6  // note: this is log2Ceil(L1_SETS)
#define L1_WAYS 2 //note: this looks like there are 8 ways
#define L1_BLOCK_SZ_BYTES 32
#define L1_BLOCK_BITS 5 // note: this is log2Ceil(L1_BLOCK_SZ_BYTES)
#define L1_SZ_BYTES (L1_SETS*L1_WAYS*L1_BLOCK_SZ_BYTES)
#define FULL_MASK 0xFFFF
#define OFF_MASK (~(FULL_MASK << L1_BLOCK_BITS))
#define TAG_MASK (FULL_MASK << (L1_SET_BITS + L1_BLOCK_BITS))
#define SET_MASK (~(TAG_MASK | OFF_MASK))

#define ITM_ADDRESS 0x08000000
#define ITM_SIZE 8192
#define DTM_ADDRESS 0x80000000
#define DTM_SIZE 16384



/* ----------------------------------
 * |                  Cache address |
 * ----------------------------------
 * |       tag |      idx |  offset |
 * ----------------------------------
 * | 63 <-> 12 | 11 <-> 6 | 5 <-> 0 |
 * ----------------------------------
 */

// setup array size of cache to "put" in the cache on $ flush
// guarantees contiguous set of addrs that is at least the sz of cache
// 5 so that you can hit more
// uint8_t dummyMem[512];

/**
 * Flush the cache of the address given since RV64 does not have a
 * clflush type of instruction. Clears any set that has the same idx bits
 * as the address input range.
 *
 * Note: This does not work if you are trying to flush dummyMem out of the
 * cache.
 *
 * @param addr starting address to clear the cache
 * @param sz size of the data to remove in bytes
 */


uint8_t dummyMem[L1_SZ_BYTES];

/**
 * Flush the cache of the address given since RV64 does not have a
 * clflush type of instruction. Clears any set that has the same idx bits
 * as the address input range.
 *
 * Note: This does not work if you are trying to flush dummyMem out of the
 * cache.
 *
 * @param addr starting address to clear the cache
 * @param sz size of the data to remove in bytes
 */
void flushCache(uint32_t addr, uint32_t sz){
    //printf("Flushed addr(0x%x) tag(0x%x) set(0x%x) off(0x%x) sz(%d)\n", addr, (addr & TAG_MASK) >> (L1_SET_BITS + L1_BLOCK_BITS), (addr & SET_MASK) >> L1_BLOCK_BITS, addr & OFF_MASK, sz);

    // find out the amount of blocks you want to clear
    uint32_t numSetsClear = sz >> L1_BLOCK_BITS;
    printf("numSetsClear %d\n", numSetsClear);
    if ((sz & OFF_MASK) != 0){
        numSetsClear += 1;
    }
    if (numSetsClear > L1_SETS){
        // flush entire cache with no rollover (makes the function finish faster) 
        numSetsClear = L1_SETS;
    }
    
    printf("tagmask(0x%d)\n", (((addr & SET_MASK) >> L1_BLOCK_BITS)) << L1_BLOCK_BITS);

    // temp variable used for nothing
    uint8_t dummy = 0;

    // this mem address is the start of a contiguous set of memory that will fit inside of the
    // cache
    // thus it has the following properties
    // 1. dummyMem <= alignedMem < dummyMem + sizeof(dummyMem)
    // 2. alignedMem has idx = 0 and offset = 0 
    uint32_t alignedMem = (((uint32_t)&dummyMem) + L1_SZ_BYTES) & TAG_MASK;
    printf("alignedMem(0x%x)\n", alignedMem);
        
    for (uint32_t i = 0; i < numSetsClear; ++i){
        // offset to move across the sets that you want to flush
        uint32_t setOffset = (((addr & SET_MASK) >> L1_BLOCK_BITS) + i) << L1_BLOCK_BITS;
        // printf("setOffset(0x%x)\n", setOffset);

        // since there are L1_WAYS you need to flush the entire set
        for(uint32_t j = 0; j < 8*L1_WAYS; ++j){
            // offset to reaccess the set
            uint32_t wayOffset = j << (L1_BLOCK_BITS + L1_SET_BITS);
            // printf("wayOffset(0x%x)\n", wayOffset);

            // evict the previous cache block and put in the dummy mem
            dummy = *((uint8_t*)(alignedMem + setOffset + wayOffset));
            // printf("evict read(0x%x)\n", alignedMem + setOffset + wayOffset);
        }
    }
    // asm volatile ("fence.i" ::: "memory");
}



//int flushCache(uint32_t addr, uint32_t sz){

 //   uint32_t alignedMem = (((uint32_t)&dummyMem) + L1_SZ_BYTES) & TAG_MASK;
    // // printf("alignedMem(0x%x)\n", alignedMem);
    // // find out the amount of blocks you want to clear
  //  uint32_t numSetsClear = sz >> L1_BLOCK_BITS;
    // if ((sz & OFF_MASK) != 0){
    //     numSetsClear += 1;
    // }
    // if (numSetsClear > L1_SETS){
    //     // flush entire cache with no rollover (makes the function finish faster) 
    //     numSetsClear = L1_SETS;
    // }
    
    // // printf("numSetsClear(%d)\n", numSetsClear);

    // // temp variable used for nothing
    // uint8_t dummy = 0;

    // for (uint32_t i = 0; i < numSetsClear; ++i){
    //     // offset to move across the sets that you want to flush
    //     uint32_t setOffset = (((addr & SET_MASK) >> L1_BLOCK_BITS) + i) << L1_BLOCK_BITS;
    //     // printf("setOffset(0x%x)\n", setOffset);

    //     // since there are L1_WAYS you need to flush the entire set
    //     for(uint32_t j = 0; j < 4*L1_WAYS; ++j){
    //         // offset to reaccess the set
    //         uint32_t wayOffset = j << (L1_BLOCK_BITS + L1_SET_BITS);
    //         // printf("wayOffset(0x%x)\n", wayOffset);

    //         // evict the previous cache block and put in the dummy mem
    //         dummy = *((uint8_t*)(alignedMem + setOffset + wayOffset));
    //         asm volatile ("fence.i" ::: "memory");
    //         // printf("evict read(0x%x)\n", alignedMem + setOffset + wayOffset);
    //     }
    // }
















    // printf("Flushed addr(0x%x) tag(0x%x) set(0x%x) off(0x%x) sz(%d)\n", addr, (addr & TAG_MASK) >> (L1_SET_BITS + L1_BLOCK_BITS), (addr & SET_MASK) >> L1_BLOCK_BITS, addr & OFF_MASK, sz);
    // int dummy;
    // for(int i=0; i<sz; i+=4){
        // dummy = *((uint8_t*)(addr+i));
        // for (int i=0; i<addr; i++){

        // }
        // asm volatile ("fence.i" ::: "memory");
    // }

    // metal_cpu_get_current_hartid(addr, sz);
    // metal_dcache_l1_flush(20, (uintptr_t)&addr);
    // if (metal_dcache_l1_available(metal_cpu_get_current_hartid()) == 0) {
    //     // abort since hardware dont support it
    //     return 0;
    // }else{
    //     return 1;
    // }
    // return 0;
    // metal_dcache_l1_flush(metal_cpu_get_current_hartid(), (uintptr_t)NULL);
    // metal_dcache_l1_flush(metal_cpu_get_current_hartid(), (uintptr_t)NULL);
    // __asm__ __volatile__(".word 0xfc000073" : : : "memory");
    // metal_dcache_l1_flush(metal_cpu_get_current_hartid(), (uintptr_t)&addr);
    // metal_dcache_l1_discard(metal_cpu_get_current_hartid(), (uintptr_t)NULL);
    // __asm__ __volatile__(".word 0xfc200073" : : : "memory");
    // metal_dcache_l1_discard(metal_cpu_get_current_hartid(), (uintptr_t)&addr);
    


    // uintptr_t ms1 = 0, ms2 = 0;
    // __asm__ __volatile__("csrr %0, mtvec \n\t"
    //                     "la %1, 1f \n\t"
    //                     "csrw mtvec, %1 \n\t"
    //                     ".insn i 0x73, 0, x0, %2, -0x40 \n\t"
    //                     ".align 2\n\t"
    //                     "1: \n\t"
    //                     "csrw mtvec, %0 \n\t"
    //                     : "+r"(ms1), "+r"(ms2)
    //                     : "r"((uintptr_t)&addr));
    // find out the amount of blocks you want to clear
    // uint32_t numSetsClear = sz >> L1_BLOCK_BITS;
    // if ((sz & OFF_MASK) != 0){
        // numSetsClear += 1;
    // }
    // if (numSetsClear > L1_SETS){
        // flush entire cache with no rollover (makes the function finish faster) 
        // numSetsClear = L1_SETS;
    // }
    
    // printf("numSetsClear(%d)\n", numSetsClear);

    // temp variable used for nothing
    // uint8_t dummy = 0;

    // this mem address is the start of a contiguous set of memory that will fit inside of the
    // cache
    // thus it has the following properties
    // 1. dummyMem <= alignedMem < dummyMem + sizeof(dummyMem)
    // 2. alignedMem has idx = 0 and offset = 0 
    // uint32_t alignedMem = (((uint32_t)&dummyMem) + L1_SZ_BYTES) & TAG_MASK;
    // printf("alignedMem(0x%x)\n", alignedMem);
    // printf("addr 0x%x\n", addr);
    // uint8_t a;
    // printf("size %d\n", sizeof(a));
    // int j;

    // uint32_t offSet = ITM_ADDRESS + ITM_SIZE + 4;
    // volatile uint32_t* p = (volatile uint32_t *) offSet;
    // *p = 0x00;
    // for (uint32_t i=0; i< sz; i++){
    //     uint32_t offSet = ITM_ADDRESS + ITM_SIZE + i *10;
    //     volatile uint8_t* p = (volatile uint8_t *) offSet;
    //     *p = 0x00;
    // }
    // for (uint32_t i = 0; i < DTM_SIZE; i+=4){
    //     uint32_t offSet = DTM_ADDRESS + i;
    //     // if (offSet == addr) continue;
    //     volatile uint32_t* p = (volatile uint32_t *) offSet;
    //     // volatile uint32_t* p2 = (volatile uint32_t *) addr;
    //     // printf("evict read(0x%x)\n",(uint32_t) *p);
    //     if ((uint32_t) *p == addr){
    //         // printf("evict read(0x%x/0x%x)\n", offSet, (uint32_t) p);
    //         // dummy = *((uint8_t*)(offSet-sz));
         
    //     }
    //     // dummy = *((uint8_t*)(offSet));
    //     *p = 0x00;
         
    // }

    // return 0;
        

// }

#endif