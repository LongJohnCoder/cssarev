#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "ctattack.h"




#include <math.h>

#define EMPIRICAL_CACHE_ACCESS_TIME 1050

#define HUGEPAGES_AVAILABLE 128
#define HUGEPAGES_AVAILABLE 128

unsigned long int possible_key_space(int X[16][256]);

int main(int argc, char* argv[])
{

    FILE *fp;
    volatile unsigned long int x = 0;
    size_t mem_length = (size_t)MB(2);
    volatile char *B[HUGEPAGES_AVAILABLE];
//    volatile char *F = mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
//    F[2048] = 0xAA;

    for (i=0; i<HUGEPAGES_AVAILABLE; ++i){
    	B[i] = mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    }



   return 0;
}



void randArr(int len) {
	int arr[]
}

