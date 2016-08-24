#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include "cssarev.h"




#include <math.h>

#define EMPIRICAL_CACHE_ACCESS_TIME_LOW  70
#define EMPIRICAL_CACHE_ACCESS_TIME_HIGH 100

#define HUGEPAGES_AVAILABLE 512

#define MEASURES 1000


volatile char **hp[HUGEPAGES_AVAILABLE+1];		// huge pages allocated
volatile char **hpt[HUGEPAGES_AVAILABLE+1];		// huge pages under test

int count;
int prefix;
int idx;

void randHpt();
void primeHpt();
void reprimeHpt();
unsigned long int probeHpt();
unsigned long int  probeprobeSingle(void *addr);
void addSingleHpt();
void primeHptExt();
void reprimeHptExt();
unsigned long int probeHptExt();

int main(int argc, char* argv[])
{

//    FILE *fp;
//    volatile unsigned long int x = 0;
    int i;
	int x = 0;
    unsigned long int tt;
    size_t mem_length = (size_t)MB(2);
//    volatile char *F = mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
//    F[2048] = 0xAA;

    for (i=0; i<HUGEPAGES_AVAILABLE; ++i){
    	hp[i] = (volatile char **)mmap(NULL, mem_length, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB, -1, 0);
    	//printf("%p\n", hp[i]);
    	x += ((int*)hp[i])[0];
 		x += hpt[i];
    }

    unsigned long int tmp_tt;
    int dumy;
    prefix = 0x00;
    idx = 0;
    count = CACHE_L3_ASSOCIATIVITY;

    for (i=0;i<HUGEPAGES_AVAILABLE-CACHE_L3_ASSOCIATIVITY;++i) {
    	randHpt();
	    tt = 999999999;
	    for (dumy=0; dumy < 20000; ++dumy) {
	    	primeHpt();
	    	//reprimeHpt();
	    	tmp_tt = probeHpt();
	    	if (tt > tmp_tt) tt = tmp_tt;
	    	//if(dumy%10==0) printf("-");
	    }
	    printf("%lu\n", tt/count);
	    if (tt/count >= EMPIRICAL_CACHE_ACCESS_TIME_LOW) break;
	    count++;
    }

    printf("count\t:\t%d\n", count);
    for (i = 0; i < count; ++i) printf("%p\t", hpt[i]);
    printf("\n-------------------------------------------------\n");

	int in = 0;
	int out = 0;

	idx = count;
//for (int k = 0; k < 100; ++k) {
    volatile char **tmp;
    for (i=16*count; i<16*HUGEPAGES_AVAILABLE; ++i){
//	printf("idx:%d-prefix:%d ", idx, prefix);
    	addSingleHpt();
    	tt = 999999999;
	    for (dumy=0; dumy < 100000; ++dumy) {
	    	primeHptExt();
	    	reprimeHptExt();
	    	tmp_tt = probeHptExt();;
	    	if (tt > tmp_tt) tt = tmp_tt;
	    }
	    if (tt/(count+1) < EMPIRICAL_CACHE_ACCESS_TIME_HIGH) {
		printf("%lu\t", tt/(count+1));
	    	printf("OUT\t");
	    	out++;
	    } else {
	    	printf("%lu\t", tt/(count+1));
	    	printf("IN\t");
	    	in++;
//		printf("%d\t:\t%012p - ", haswell_i7_4600m_cache_slice_alg((void *)(get_pfn((void*)hpt[count]) << 12)), (void *)(get_pfn((void*)hpt[count]) << 12));
//		printPtr2binSageMatrix((void *)(get_pfn((void*)hpt[count]) << 12));
	    }
	    
	    printf("%d\t:\t%012p - ", sandybridge_i5_2435m_cache_slice_alg_m2((void *)(get_pfn((void*)hpt[count]) << 12)), (void *)(get_pfn((void*)hpt[count]) << 12));
	    printf("%d\t:\t%012p - ", haswell_i7_4600m_cache_slice_alg((void *)(get_pfn((void*)hpt[count]) << 12)), (void *)(get_pfn((void*)hpt[count]) << 12));
	    printPtr2bin((void *)(get_pfn((void*)hpt[count]) << 12));
    }
//}
    printf("in\t:\t%d\tout\t:\t%d\n", in, out);


/*
    for (i = 0 ; i < MEASURES; ++i){
    	primeHpt();
    	reprimeHpt();
    	tt = probeHpt();
    	printf("%lu\n", tt);
    }
*/    

    for (i=0; i<HUGEPAGES_AVAILABLE; ++i){
    	munmap((void*)hp[i], MB(2));
    }


   return 0;
}



void randHpt() {
	int i;
	volatile char **tmp;

	for (i=0; i<count; ++i) {
		hpt[i] = hp[i];
	}

	for (i = 0; i < count; ++i) {
		tmp = hpt[i];
		tmp[0] = (volatile char *)hpt[(i+1)%count];
		tmp[KB(32)/sizeof(volatile char **)] = (volatile char *)(hpt[(i+1)%count] + KB(32)/sizeof(volatile char **));
	}
	
	init_prime = (volatile char **)hpt[0];
	init_reprime = (volatile char **)hpt[0] + KB(32)/sizeof(volatile char **);
	

	int sl0 = 0;
	int sl1 = 0;
	for (i = 0; i < count+1; ++i) {
		if (haswell_i7_4600m_cache_slice_alg((void *)hpt[i]) == 0) {
			sl0++;
		} else {
			sl1++;
		}
	}
	printf("slice 0\t:\t%d\nslice 1\t:\t%d\n", sl0, sl1);
}


void addSingleHpt() {
	int i;
	volatile char **tmp;
	//volatile char **tmp2;
	//for (i=0; i<count; ++i) {
	//	hpt[i] = hp[i];
	//}

	tmp = hp[idx];
	hpt[count] = &tmp[(prefix<<17)/8];
//	hpt[count] = hp[(prefix<<17)/8 + idx];

	for (int i = 0; i < count+1; ++i) {
		tmp = hpt[i];
		tmp[0] = (volatile char *)hpt[(i+1)%(count+1)];
		tmp[KB(32)/sizeof(volatile char **)] = (volatile char *)(hpt[(i+1)%(count+1)] + KB(32)/sizeof(volatile char **));
	}
	
	init_prime = (volatile char **)hpt[0];
	init_reprime = (volatile char **)hpt[0] + KB(32)/sizeof(volatile char **);

	prefix++;
	if (prefix == 0x10) { prefix = 0x00; idx++; }

}


void primeHpt() {
	int i;
	volatile char **tmp;
	tmp = init_prime;
	for (i = 0; i < count; ++i) {
		tmp = (volatile char **)*tmp;
	}
}

void primeHptExt() {
	int i;
	volatile char **tmp;
	tmp = init_prime;
	for (i = 0; i < count+1; ++i) {
		tmp = (volatile char **)*tmp;
	}
}

void reprimeHpt() {
	int i;
	volatile char **tmp;
	tmp = init_reprime;
	for (i = 0; i < count; ++i) {
		tmp = (volatile char **)*tmp;
	}
}

void reprimeHptExt() {
	int i;
	volatile char **tmp;
	tmp = init_reprime;
	for (i = 0; i < count+1; ++i) {
		tmp = (volatile char **)*tmp;
	}
}

unsigned long int  probeHpt() {
	int i;
	unsigned long int begin, end;
	volatile char **tmp;
	tmp = (volatile char **)hpt[0];
    TIMESTAMP_START;
	for (i = 0; i < count; ++i) {
		tmp = (volatile char **)*tmp;
	}
    TIMESTAMP_STOP;
    begin = get_global_timestamp_start();
    end = get_global_timestamp_stop();
    return (end-begin);
}

unsigned long int  probeHptExt() {
	int i;
	unsigned long int begin, end;
	volatile char **tmp;
	tmp = (volatile char **)hpt[0];
    TIMESTAMP_START;
	for (i = 0; i < count+1; ++i) {
		tmp = (volatile char **)*tmp;
	}
    TIMESTAMP_STOP;
    begin = get_global_timestamp_start();
    end = get_global_timestamp_stop();
    return (end-begin);
}

unsigned long int  probeprobeSingle(void *addr) {
	unsigned long int begin, end;
	volatile char **tmp;
    TIMESTAMP_START;
	tmp = (volatile char **)addr;
    TIMESTAMP_STOP;
    begin = get_global_timestamp_start();
    end = get_global_timestamp_stop();
    return (end-begin);
}




