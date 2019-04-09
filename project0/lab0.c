// Project 0
// NAME: Zhonglin Zhang
// EMAIL: evanzhang@ucla.edu
// ID: 005030520

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#define BUF_SIZE 20

void handle_signal(int signal) {
    if (signal == SIGSEGV)
        fprintf(stderr, "Caught Segmentation Fault\n");
    else fprintf(stderr, "Caught Other Signal: %d\n", signal);
    exit(4);
}

int main(int argc, char *argv[]) {
    char *infile, *outfile;
    int opt = 0;
    int ifd, ofd;
    int inputOpt = 0;
    int outputOpt = 0;
    int segfaultOpt = 0;
    //int catchOpt = 0;
    char *nullptr;
    int i;
    static struct option long_options[] = {
    {"input",    required_argument, NULL,   0},
    {"output",   required_argument, NULL,   1},
    {"segfault", no_argument,       NULL,   2},
    {"catch",    no_argument,       NULL,   3},
    {0,          0,                 0,      0}
    };
    while (1) {
        //int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        opt = getopt_long(argc, argv, "", long_options, &option_index);
        if (opt == -1) break;
        switch(opt) {
            case 0: //input
                i = 0;
                // if (optarg == NULL) {
                //     fprintf(stderr, "Error in '--input' option:\n");
                //     fprintf(stderr, "Long Option '--input' needs an argument\n");
                //     exit(2); 
                // }
                while (optarg[i] == ' ') {++i;}
                if (optarg[i] == '-' || i == (int) strlen(optarg)) {
                    fprintf(stderr, "Error in '--input' option:\n");
                    fprintf(stderr, "Long Option '--input' needs an argument\n");
                    exit(2); 
                }
                else{              
                    infile = optarg;
                    errno = 0;
                    ifd = open(infile, 0400);
                    if (ifd >= 0) {inputOpt = 1;} //There is 'input' option
                    else {
                        fprintf(stderr, "open() failed with error [%s]\n", strerror(errno));
                        fprintf(stderr, "Error in '--input' option:\n");
                        fprintf(stderr, "There is no such a file named %s\n", optarg);
                        exit(2);
                    }
                }
                break;
            case 1: //output
                i = 0;
                // if (optarg == NULL) {
                //     fprintf(stderr, "Error in '--output' option:\n");
                //     fprintf(stderr, "Long Option '--output' needs an argument\n");
                //     exit(2); 
                // }
                while (optarg[i] == ' ') {++i;}
                if (optarg[i] == '-' || i == (int) strlen(optarg)) {
                    fprintf(stderr, "Error in '--onput' option:\n");
                    fprintf(stderr, "Long Option '--output' needs an argument\n");
                    exit(3);
                }
                else{
                    outfile = optarg;
                    ofd = creat(outfile, 0666);
                    if (ofd >= 0) {outputOpt = 1;} //There is 'output' option
                    else {
                        fprintf(stderr, "creat() failed with error [%s]\n", strerror(errno));
                        fprintf(stderr, "Error in '--onput' option:\n");
                        fprintf(stderr, "There is no such a file named %s\n", optarg);
                        exit(3);
                    }
                }
                break;
            case 2: //segfault
                segfaultOpt = 1;
                break;
            case 3: //catch
                signal(SIGSEGV, handle_signal);
                break;
            default:
                // if (strcmp(long_options[option_index].name, "input")==0) {
                //     fprintf(stderr, "%s\n", long_options[option_index].name);
                //     fprintf(stderr, "Error in '--input' option:\n");
                //     fprintf(stderr, "Long Option '--input' needs an argument\n");
                //     exit(2);
                // }
                // else if (strcmp(long_options[option_index].name, "output")==0) {
                //     fprintf(stderr, "Error in '--output' option:\n");
                //     fprintf(stderr, "Long Option '--output' needs an argument\n");
                //     exit(3);
                // }
                // else{
                    fprintf(stderr, "There is an unknown option: %s\n", argv[optind-1]);
                    fprintf(stderr, "The correct usage: ./lab0 [--input=InputFile] [--output=OutputFile] [--segfault] [--catch]\n"); 
                    exit(1);
                //}
        }
    }
    if (inputOpt == 1) {
        close(0);
        dup(ifd);
        close(ifd);
    }
    if (outputOpt == 1) {
        close(1);
        dup(ofd);
        close(ofd);
    }
    if (segfaultOpt == 1) {
        nullptr = NULL;
        char *tmp = nullptr;
        tmp[0] = tmp[1];
    }
    ssize_t ret_in;
    char buffer[BUF_SIZE];
    while((ret_in = read(STDIN_FILENO, &buffer, BUF_SIZE)) > 0) {
        write(STDOUT_FILENO, &buffer, (ssize_t) ret_in);
    }
    exit(0);
}

