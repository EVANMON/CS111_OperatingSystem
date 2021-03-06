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
#include "SortedList.c"

#define THREADS 1
#define ITERATIONS 2
#define YIELD 3
#define SYNCHRONIZATION 4

int key_len = 4;
int opt_yield = 0;
int thread_num = 1;
int iter_num = 1;
char opt_sync = 'n';
long long cntr = 0;
int tot_ele;
pthread_mutex_t mutex_lock;
int spin_lock = 0;

SortedList_t *ele_list;
SortedListElement_t *list_head;
int *tid_list;

void create_list() {
    int i, j;
    // int tot_ele = thread_num * iter_num;
    for (i=0; i<tot_ele; ++i) {
        char *key = malloc(sizeof(char) * (key_len+1)); //?
        for (j=0; j<key_len; ++j) {
            key[j] = (char) 'a' + rand() % 26;
        }
        key[j] = '\0';
        ele_list[i].key = key;
    }
}


void* thread_oper(void* tid) {
    unsigned int t_id = *((int*) tid);
	int iter=0;
	int start = t_id * iter_num;
    int end = (t_id+1) * iter_num;
    // printf("%s\n", ele_list[iter].key);
	for (iter=start; iter<end; ++iter) {
		switch (opt_sync) {
			case 'm':
				pthread_mutex_lock(&mutex_lock);
				SortedList_insert(list_head, &ele_list[iter]);
				pthread_mutex_unlock(&mutex_lock);
				break;
			case 's':
				while (__sync_lock_test_and_set(&spin_lock, 1));
				SortedList_insert(list_head, &ele_list[iter]);
				__sync_lock_release(&spin_lock);
				break;
			case 'n':
				SortedList_insert(list_head, &ele_list[iter]);
				break;
			default:
				fprintf(stderr, "%s\n", "Unknown Sync Method");
				exit(1);
		}
	}

    int list_len;
    switch (opt_sync) {
        case 'm':
            pthread_mutex_lock(&mutex_lock);
            list_len = SortedList_length(list_head);
            pthread_mutex_unlock(&mutex_lock);
            break;
        case 's':
            while (__sync_lock_test_and_set(&spin_lock, 1));
            list_len = SortedList_length(list_head);
            __sync_lock_release(&spin_lock);
            break;
        case 'n':
            list_len = SortedList_length(list_head);
            break;
        default:
            fprintf(stderr, "%s\n", "Unknown Sync Method");
            exit(1);
    }
    // printf("%d\n", list_len);
    for (iter=start; iter<end; ++iter) {
        switch (opt_sync) {
            case 'm':
                pthread_mutex_lock(&mutex_lock);
                if (!SortedList_lookup(list_head, ele_list[iter].key)) {
                    fprintf(stderr, "%s\n", "Error: Didn't find the element to delete!");
                    exit(2);
                }
                if (SortedList_delete(&ele_list[iter]) != 0) {
                    fprintf(stderr, "%s\n", "Error on SortedList_delete!");
                    exit(2);
                }
                pthread_mutex_unlock(&mutex_lock);
                break;
            case 's':
                while (__sync_lock_test_and_set(&spin_lock, 1));
                if (!SortedList_lookup(list_head, ele_list[iter].key)) {
                    fprintf(stderr, "%s\n", "Error: Didn't find the element to delete!");
                    exit(2);
                }
                if (SortedList_delete(&ele_list[iter]) != 0) {
                    fprintf(stderr, "%s\n", "Error on SortedList_delete!");
                    exit(2);
                }
                __sync_lock_release(&spin_lock);
                break;
            case 'n':
                if (!SortedList_lookup(list_head, ele_list[iter].key)) {
                    fprintf(stderr, "%s\n", "Error: Didn't find the element to delete!");
                    exit(2);
                }
                if (SortedList_delete(&ele_list[iter]) != 0) {
                    fprintf(stderr, "%s\n", "Error on SortedList_delete!");
                    exit(2);
                }
                break;
            default:
                fprintf(stderr, "%s\n", "Unknown Sync Method");
                exit(1);
        }
    }
    return NULL;
}


int main(int argc, char *argv[]) {
	int i, opt;
	static struct option long_options[] = {
    {"threads",       required_argument,       NULL,   THREADS         },
    {"iterations",    required_argument,       NULL,   ITERATIONS      },
    {"yield",         required_argument,       NULL,   YIELD           },
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
    		exit(1);
    }

    list_head = (SortedListElement_t*) malloc(sizeof(SortedListElement_t));
    SortedListElement_t* head = list_head;
    head->key = NULL;
    head->next = NULL;
    head->prev = NULL;
    
    tot_ele = thread_num * iter_num;
    ele_list = (SortedList_t*) malloc(tot_ele * sizeof(SortedListElement_t));
    create_list();

    tid_list = malloc(thread_num * sizeof(int));
    for (i=0; i<thread_num; ++i) {
        tid_list[i] = i;
    }

    pthread_t* tid = (pthread_t*) malloc(thread_num * sizeof(pthread_t));
    if (!tid) {
    	fprintf(stderr, "%s\n", "Pthread Malloc Error");
    	exit(1);
    }

    struct timespec start_t, end_t;
    clock_gettime(CLOCK_MONOTONIC, &start_t);

    for (i=0; i<thread_num; ++i) {
    	if (pthread_create(&tid[i], NULL, &thread_oper , (void *)&tid_list[i])!=0) {
    		fprintf(stderr, "%s%d\n", "Pthread Create Error on ", i+1);
    		free(tid);
            free(ele_list);
            free(tid_list);
            free(list_head);
    		exit(1);
    	}
    }
    for (i=0; i<thread_num; ++i) {
    	if (pthread_join(tid[i], NULL)!=0) {
    		fprintf(stderr, "%s\n", "Pthread Join Error");
    		free(tid);
            free(ele_list);
            free(tid_list);
            free(list_head);
    		exit(1);
    	}
    }

    clock_gettime(CLOCK_MONOTONIC, &end_t);

    int final_len = SortedList_length(list_head);
    if (final_len != 0) {
        fprintf(stderr, "%s\n", "Corrupted List Error");
        free(tid);
        free(ele_list);
        free(list_head);
        free(tid_list);
        exit(2);
    }

    long long tot_oper = thread_num * iter_num * 3;
    long long wall_time = (end_t.tv_sec - start_t.tv_sec) * pow(10, 9) + \
                          (end_t.tv_nsec - start_t.tv_nsec);
    long long ave_time = wall_time / tot_oper;
    printf("%s,%d,%d,%d,%lld,%lld,%lld\n", tag, thread_num, iter_num, 1,\
    	   tot_oper, wall_time, ave_time);
    free(tid);
    free(ele_list);
    free(list_head);
    free(tid_list);
    exit(0);
}