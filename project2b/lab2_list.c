// Project 2B 
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
#include <signal.h>
#include "SortedList.c"

#define THREADS 1
#define ITERATIONS 2
#define YIELD 3
#define SYNCHRONIZATION 4
#define LISTS 5

int key_len = 4;
int opt_yield = 0;
int thread_num = 1;
int iter_num = 1;
int sublist_num = 1;
char opt_sync = 'n';
long long cntr = 0;
int lock_oper = 0;
int tot_ele;
pthread_mutex_t *mutex_lock;
int *spin_lock;
pthread_t *tid;

SortedList_t *ele_list;
SortedList_t *heads;
int *tid_list;
long long *ttime_list;

void mfree() {
    free(tid);
    free(heads);
    free(ele_list);
    free(tid_list);
    free(ttime_list);
    if (opt_sync == 'm') free(mutex_lock);
    if (opt_sync == 's') free(spin_lock);
}

void create_list() {
    int i, j;
    // int tot_ele = thread_num * iter_num;
    for (i=0; i<tot_ele; ++i) {
        char *key = (char *) malloc(sizeof(char) * (key_len+1)); //?
        for (j=0; j<key_len; ++j) {
            key[j] = (char) 'a' + rand() % 26;
        }
        key[j] = '\0';
        ele_list[i].key = key;
    }
}

void sig_handler(int sig) {
    if (sig == SIGSEGV) {
        fprintf(stderr, "%s\n", "Find Segmentation Error");
        mfree();
        exit(2);
    }
}


int hash_func(SortedListElement_t *ele) {
    int i, key;
    int tmp = 0;
    for (i=0; i<(int) strlen(ele->key); ++i) {
        tmp += (int) ele->key[i];
    }
    key = tmp % sublist_num;
    return key;
}

void* thread_oper(void* tid) {
    unsigned int t_id = *((int*) tid);
	int iter=0;
	int start = t_id * iter_num;
    int end = (t_id+1) * iter_num;
    int key, i;
    struct timespec stime, etime;
    long long ltime = 0;
    // printf("%s\n", ele_list[iter].key);
	for (iter=start; iter<end; ++iter) {
        key = hash_func(&ele_list[iter]);
		switch (opt_sync) {
			case 'm':
                clock_gettime(CLOCK_MONOTONIC, &stime);
				pthread_mutex_lock(&mutex_lock[key]);
                clock_gettime(CLOCK_MONOTONIC, &etime);
				SortedList_insert(&heads[key], &ele_list[iter]);
                pthread_mutex_unlock(&mutex_lock[key]);
                ltime += (long long) (etime.tv_sec - stime.tv_sec) * 1000000000 + \
                                    (etime.tv_nsec - stime.tv_nsec);
                lock_oper++;
				break;
			case 's':
                clock_gettime(CLOCK_MONOTONIC, &stime);
				while (__sync_lock_test_and_set(&spin_lock[key], 1));
                clock_gettime(CLOCK_MONOTONIC, &etime);
				SortedList_insert(&heads[key], &ele_list[iter]);
                __sync_lock_release(&spin_lock[key]);
                ltime += (long long) (etime.tv_sec - stime.tv_sec) * 1000000000 + \
                                    (etime.tv_nsec - stime.tv_nsec);
                lock_oper++;
				break;
			case 'n':
				SortedList_insert(&heads[key], &ele_list[iter]);
				break;
			default:
				fprintf(stderr, "%s\n", "Unknown Sync Method");
                mfree();
				exit(1);
		}
	}

    int list_len=0;
    for (i=0; i<sublist_num; ++i) {
        switch (opt_sync) {
            case 'm':
                clock_gettime(CLOCK_MONOTONIC, &stime);
                pthread_mutex_lock(&mutex_lock[i]);
                clock_gettime(CLOCK_MONOTONIC, &etime);
                list_len += SortedList_length(&heads[i]);
                pthread_mutex_unlock(&mutex_lock[i]);
                ltime += (long long) (etime.tv_sec - stime.tv_sec) * 1000000000 + \
                                    (etime.tv_nsec - stime.tv_nsec);
                lock_oper++;
                break;
            case 's':
                clock_gettime(CLOCK_MONOTONIC, &stime);
                while (__sync_lock_test_and_set(&spin_lock[i], 1));
                clock_gettime(CLOCK_MONOTONIC, &etime);
                list_len += SortedList_length(&heads[i]);
                __sync_lock_release(&spin_lock[i]);
                ltime += (long long) (etime.tv_sec - stime.tv_sec) * 1000000000 + \
                                    (etime.tv_nsec - stime.tv_nsec);
                lock_oper++;
                break;
            case 'n':
                list_len += SortedList_length(&heads[i]);
                break;
            default:
                fprintf(stderr, "%s\n", "Unknown Sync Method");
                mfree();
                exit(1);
        }
    }

    // printf("%d\n", list_len);
    for (iter=start; iter<end; ++iter) {
        key = hash_func(&ele_list[iter]);
        switch (opt_sync) {
            case 'm':
                clock_gettime(CLOCK_MONOTONIC, &stime);
                pthread_mutex_lock(&mutex_lock[key]);
                clock_gettime(CLOCK_MONOTONIC, &etime);
                if (!SortedList_lookup(&heads[key], ele_list[iter].key)) {
                    fprintf(stderr, "%s\n", "Error: Didn't find the element to delete!");
                    mfree();
                    exit(2);
                }
                if (SortedList_delete(&ele_list[iter]) != 0) {
                    fprintf(stderr, "%s\n", "Error on SortedList_delete!");
                    mfree();
                    exit(2);
                }
                pthread_mutex_unlock(&mutex_lock[key]);
                ltime += (long long) (etime.tv_sec - stime.tv_sec) * 1000000000 + \
                                    (etime.tv_nsec - stime.tv_nsec);
                lock_oper++;
                break;
            case 's':
                clock_gettime(CLOCK_MONOTONIC, &stime);
                while (__sync_lock_test_and_set(&spin_lock[key], 1));
                clock_gettime(CLOCK_MONOTONIC, &etime);
                if (!SortedList_lookup(&heads[key], ele_list[iter].key)) {
                    fprintf(stderr, "%s\n", "Error: Didn't find the element to delete!");
                    mfree();
                    exit(2);
                }
                if (SortedList_delete(&ele_list[iter]) != 0) {
                    fprintf(stderr, "%s\n", "Error on SortedList_delete!");
                    mfree();
                    exit(2);
                }
                __sync_lock_release(&spin_lock[key]);
                ltime += (long long) (etime.tv_sec - stime.tv_sec) * 1000000000 + \
                                    (etime.tv_nsec - stime.tv_nsec);
                lock_oper++;
                break;
            case 'n':
                if (!SortedList_lookup(&heads[key], ele_list[iter].key)) {
                    fprintf(stderr, "%s\n", "Error: Didn't find the element to delete!");
                    mfree();
                    exit(2);
                }
                if (SortedList_delete(&ele_list[iter]) != 0) {
                    fprintf(stderr, "%s\n", "Error on SortedList_delete!");
                    mfree();
                    exit(2);
                }
                break;
            default:
                fprintf(stderr, "%s\n", "Unknown Sync Method");
                mfree();
                exit(1);
        }
    }
    ttime_list[t_id] += ltime;
    return NULL;
}


int main(int argc, char *argv[]) {
	int i, opt;
	static struct option long_options[] = {
    {"threads",       required_argument,       NULL,   THREADS         },
    {"iterations",    required_argument,       NULL,   ITERATIONS      },
    {"yield",         required_argument,       NULL,   YIELD           },
    {"sync",          required_argument,       NULL,   SYNCHRONIZATION },
    {"lists",         required_argument,       NULL,   LISTS           },
    {0,               0,                       0,      0}
    };
    signal(SIGSEGV, &sig_handler);
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
    			// opt_yield = 1;
                for (i=0; i<(int) strlen(optarg); ++i) {
                    switch (optarg[i]) {
                        case 'i':
                            opt_yield |= INSERT_YIELD;
                            break;
                        case 'l':
                            opt_yield |= LOOKUP_YIELD;
                            break;
                        case 'd':
                            opt_yield |= DELETE_YIELD;
                            break;
                        default:
                            fprintf(stderr, "%s\n", "Unknown Yield Argu");
                            exit(1);
                    }
                }
    			break;
    		case SYNCHRONIZATION:
    			opt_sync = optarg[0];
                if (opt_sync != 'm' && opt_sync != 's') {
                    fprintf(stderr, "%s\n", "Unknown Sync Method");
                    exit(1);
                }
    			break; 
            case LISTS:
                sublist_num = atoi(optarg);
                break; 
    		default:
    			fprintf(stderr, "%s%s\n", "Unrecognized Option: ", argv[optind-1]);	
    			exit(1);
    	} 			
    }
    if (opt_sync == 'm') {
        mutex_lock = (pthread_mutex_t *)malloc(sublist_num * sizeof(pthread_mutex_t));
        for (i=0; i<sublist_num; ++i) {
            if (pthread_mutex_init(&mutex_lock[i], NULL)!=0) {
                fprintf(stderr, "%s\n", "Mutex Init Error");
                mfree();
                exit(1);
            }
        }		
    } else if (opt_sync == 's') {
        spin_lock = (int *)malloc(sublist_num * sizeof(int));
        for (i=0; i<sublist_num; ++i) {
            spin_lock[i] = 0;
        }
    }

    char tag[30];
    strcpy(tag, "list");
    if (opt_yield == 0) strcat(tag, "-none");
    else {
        strcat(tag, "-");
        if (opt_yield & 0x01) strcat(tag, "i");
        if (opt_yield & 0x02) strcat(tag, "d");
        if (opt_yield & 0x04) strcat(tag, "l");
    }
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
            mfree();
    		exit(1);
    }

    heads = (SortedList_t*) malloc(sublist_num * sizeof(SortedListElement_t));
    for (i=0; i<sublist_num; ++i) {
        SortedListElement_t new_head;
        new_head.key = NULL;
        new_head.next = NULL;
        new_head.prev = NULL;
        heads[i] = new_head;
    }

    tot_ele = thread_num * iter_num;
    ele_list = (SortedList_t*) malloc(tot_ele * sizeof(SortedListElement_t));
    create_list();

    tid_list = (int *) malloc(thread_num * sizeof(int));
    for (i=0; i<thread_num; ++i) {
        tid_list[i] = i;
    }

    ttime_list = (long long *) malloc(thread_num * sizeof(long long));
    for (i=0; i<thread_num; ++i) {
        ttime_list[i] = 0;
    }

    struct timespec start_t, end_t;
    clock_gettime(CLOCK_MONOTONIC, &start_t);

    tid = (pthread_t*) malloc(thread_num * sizeof(pthread_t));
    if (!tid) {
    	fprintf(stderr, "%s\n", "Pthread Malloc Error");
        mfree();
    	exit(1);
    }

    for (i=0; i<thread_num; ++i) {
    	if (pthread_create(&tid[i], NULL, &thread_oper , (void *)&tid_list[i])!=0) {
    		fprintf(stderr, "%s%d\n", "Pthread Create Error on ", i+1);
    		mfree();
    		exit(1);
    	}
    }

    long long tot_lock_time = 0;
    for (i=0; i<thread_num; ++i) {
    	if (pthread_join(tid[i], NULL)!=0) {
    		fprintf(stderr, "%s\n", "Pthread Join Error");
    		mfree();
    		exit(1);
    	}
        tot_lock_time += ttime_list[i];
    }

    clock_gettime(CLOCK_MONOTONIC, &end_t);

    int final_len = 0;
    int sub_len;
    for (i=0; i<sublist_num; ++i) {
        sub_len = SortedList_length(&heads[i]);
        if (sub_len != 0) {
            fprintf(stderr, "%s\n", "Corrupted List Error");
            mfree();
            exit(2);
        }
        final_len += sub_len;
    }
    if (final_len != 0) {
        fprintf(stderr, "%s\n", "Corrupted List Error");
        mfree();
        exit(2);
    }

    long long tot_oper = thread_num * iter_num * 3;
    long long wall_time = (end_t.tv_sec - start_t.tv_sec) * 1000000000 + \
                          (end_t.tv_nsec - start_t.tv_nsec);
    long long ave_wall_time = wall_time / tot_oper;
    long long ave_lock_time;
    if(lock_oper == 0) ave_lock_time = 0;
    else ave_lock_time = tot_lock_time / lock_oper;
    printf("%s,%d,%d,%d,%lld,%lld,%lld,%lld\n", tag, thread_num, iter_num, sublist_num,\
    	   tot_oper, wall_time, ave_wall_time, ave_lock_time);
    mfree();
    exit(0);
}
