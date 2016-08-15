#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "cssarev.h"




#include <math.h>

#define EMPIRICAL_CACHE_ACCESS_TIME 1050

#define HUGEPAGES_AVAILABLE 64

#define MEASURES 1000


volatile char **hp[HUGEPAGES_AVAILABLE];		// huge pages allocated
volatile char **hpt[CACHE_L3_ASSOCIATIVITY];	// huge pages under test


void randHpt();
void primeHpt();
void reprimeHpt();
unsigned long int probeHpt();


int main(int argc, char* argv[])
{

//    FILE *fp;
//    volatile unsigned long int x = 0;
    int i;
    unsigned long int tt;
    size_t mem_length = (size_t)MB(2);
//    volatile char *F = mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
//    F[2048] = 0xAA;

    for (i=0; i<HUGEPAGES_AVAILABLE; ++i){
    	hp[i] = (volatile char **)mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    	//printf("%p\n", hp[i]);
    }

    for (i = 0 ; i < MEASURES; ++i){
    	randHpt();
    	primeHpt();
    	reprimeHpt();
    	tt = probeHpt();
    	printf("%lu\n", tt);
    }
    

    for (i=0; i<HUGEPAGES_AVAILABLE; ++i){
    	munmap((void*)hp[i], MB(2));
    }


   return 0;
}



void randHpt() {
	int i, j, idx, x;
	int idxs[CACHE_L3_ASSOCIATIVITY];
	int found;
	volatile char **tmp;

	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) idxs[i] = -1;

	for (i=0; i<CACHE_L3_ASSOCIATIVITY; ++i) {
		do {
			found = 0;
			idx = rand_int(HUGEPAGES_AVAILABLE);
//			printf("idx = %d\n", idx);
			for (j = 0; j < i; ++j) {
				if (idxs[j] == idx) {
					found = 1;
					break;
				}
			}
		} while(found == 1);
		hpt[i] = hp[idx];
		idxs[i] = idx;
	}
	for (int i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		tmp = hpt[i];
		tmp[0] = (volatile char *)hpt[(i+1)%CACHE_L3_ASSOCIATIVITY];
		tmp[KB(32)/sizeof(volatile char **)] = (volatile char *)(hpt[(i+1)%CACHE_L3_ASSOCIATIVITY] + KB(32)/sizeof(volatile char **));
	}
	
	init_prime = (volatile char **)hpt[0];
	init_reprime = (volatile char **)hpt[0] + KB(32)/sizeof(volatile char **);

/*
	printf("-------------------------------------------\n");
	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		x += ((int*)hpt[i])[0];
		printf("%d\t-\t%p\t-\t%012p\n", idxs[i], hpt[i], (void *)(get_pfn((void*)hpt[i]) << 12));
	}


	printf("----------------PRIME---------------------------\n");
	tmp = (volatile char **)hpt[0];
	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		printf("%p\n", tmp);
		tmp = (volatile char **)*tmp;
	}

	printf("--------------RE-PRIME--------------------------\n");
	tmp = (volatile char **)hpt[0] + KB(32)/sizeof(volatile char **);
	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		printf("%p\n", tmp);
		tmp = (volatile char **)*tmp;
	}
*/

}


void primeHpt() {
	int i;
	volatile char **tmp;
	tmp = init_prime;
	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		tmp = (volatile char **)*tmp;
	}
}

void reprimeHpt() {
	int i;
	volatile char **tmp;
	tmp = init_reprime;
	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		tmp = (volatile char **)*tmp;
	}
}

unsigned long int  probeHpt() {
	int i;
	unsigned long int begin, end;
	volatile char **tmp;
	tmp = (volatile char **)hpt[0];
    TIMESTAMP_START;
	for (i = 0; i < CACHE_L3_ASSOCIATIVITY; ++i) {
		tmp = (volatile char **)*tmp;
	}
    TIMESTAMP_STOP;
    begin = get_global_timestamp_start();
    end = get_global_timestamp_stop();
    return (end-begin);
}

