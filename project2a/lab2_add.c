// Project 2A -- add
// NAME: Zhonglin Zhang
// EMAIL: evanzhang@ucla.edu
// ID: 005030520

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sched.h>

#define THREADS 1
#define ITERATIONS 2
#define YIELD 3
#define SYNCHRONIZATION 4


int opt_yield = 0;
int thread_num = 1;
int iter_num = 1;
char opt_sync = 'n';
long long cntr = 0;
pthread_mutex_t mutex_lock;
int spin_lock = 0;

void add(long long *pointer, long long value) {
    long long sum = *pointer + value;
    if (opt_yield)
            sched_yield();
    *pointer = sum;
}

void thread_add(int val) {
	int iter;
	long long old;
	for (iter=0; iter<iter_num; ++iter) {
		switch (opt_sync) {
			case 'm':
				pthread_mutex_lock(&mutex_lock);
				add(&cntr, val);
				pthread_mutex_unlock(&mutex_lock);
				break;
			case 's':
				while (__sync_lock_test_and_set(&spin_lock, 1));
				add(&cntr, val);
				__sync_lock_release(&spin_lock);
				break;
			case 'c':
				do {
					old = cntr;
					if (opt_yield)
			            sched_yield();
				} while (!__sync_bool_compare_and_swap(&cntr, old, old+val));
				break;
			case 'n':
				add(&cntr, val);
				break;
			default:
				fprintf(stderr, "%s\n", "Unknown Sync Method");
				exit(1);
		}
	}
}

void* thread_start() {
	thread_add(1);
	thread_add(-1);
	return NULL;
}

int main(int argc, char *argv[]) {
	int i, opt;
	static struct option long_options[] = {
    {"threads",       required_argument,       NULL,   THREADS         },
    {"iterations",    required_argument,       NULL,   ITERATIONS      },
    {"yield",         no_argument,             NULL,   YIELD           },
    {"sync",          required_argument,       NULL,   SYNCHRONIZATION },
    {0,               0,                       0,      0}
    };
    while (1) {
    	int option_idx = 0;
    	i = 0;
    	opt = getopt_long(argc, argv, "", long_options, &option_idx);
    	if (opt == -1) break;
    	switch (opt) {
    		case THREADS:
    			thread_num = atoi(optarg);
    			break;
    		case ITERATIONS:
    			iter_num = atoi(optarg);
    			break;
    		case YIELD:
    			opt_yield = 1;
    			break;
    		case SYNCHRONIZATION:
    			opt_sync = optarg[0];
    			break;  
    		default:
    			fprintf(stderr, "%s%s\n", "Unrecognized Option: ", argv[optind-1]);	
    			exit(1);
    	} 			
    }
    if (opt_sync == 'm') {
		if (pthread_mutex_init(&mutex_lock, NULL)!=0) {
			fprintf(stderr, "%s\n", "Mutex Init Error");
			exit(1);
		}		
    }

    char tag[20];
    strcpy(tag, "add");
    if (opt_yield == 1) strcat(tag, "-yield");
    switch (opt_sync) {
    	case 'n':
    		strcat(tag, "-none");
    		break;
    	case 'm':
    		strcat(tag, "-m");
    		break;
    	case 's':
    		strcat(tag, "-s");
    		break;
    	case 'c':
    		strcat(tag, "-c");
    		break;
    	default:
    		fprintf(stderr, "%s\n", "Unknown Sync Method");
    		exit(1);
    }

    pthread_t* tid = (pthread_t*) malloc(thread_num * sizeof(pthread_t));
    if (!tid) {
    	fprintf(stderr, "%s\n", "Pthread Malloc Error");
    	exit(1);
    }

    struct timespec start_t, end_t;
    clock_gettime(CLOCK_MONOTONIC, &start_t);

    for (i=0; i<thread_num; ++i) {
    	if (pthread_create(&tid[i], NULL, &thread_start, NULL)!=0) {
    		fprintf(stderr, "%s%d\n", "Pthread Create Error on ", i+1);
    		free(tid);
    		exit(1);
    	}
    }
    for (i=0; i<thread_num; ++i) {
    	if (pthread_join(tid[i], NULL)!=0) {
    		fprintf(stderr, "%s\n", "Pthread Join Error");
    		free(tid);
    		exit(1);
    	}
    }

    long long tot_oper = thread_num * iter_num * 2;
    clock_gettime(CLOCK_MONOTONIC, &end_t);
    long long wall_time = (end_t.tv_sec - start_t.tv_sec) * pow(10, 9) + \
                          (end_t.tv_nsec - start_t.tv_nsec);
    long long ave_time = wall_time / tot_oper;
    printf("%s,%d,%d,%lld,%lld,%lld,%lld\n", tag, thread_num, iter_num, tot_oper,\
    	   wall_time, ave_time, cntr);
    free(tid);
    if (cntr != 0) exit(2);
    exit(0);
}
