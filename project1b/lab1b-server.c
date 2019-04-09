// Project 1B - Server
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
#include <termios.h>
#include <sys/types.h>
#include <poll.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <zlib.h>
#include <assert.h>

#define BUF_SIZE 1024
pid_t c_pid;
int pipefd1[2], pipefd2[2];
int sockfd, portno, newsockfd;
socklen_t clilen;

void closeAtExit() {
    close(pipefd1[1]);
    close(pipefd2[0]);
    close(sockfd);
    close(newsockfd);
}

void sig_handler(int sig) {
    if (sig == SIGINT) {
        kill(c_pid, SIGINT);
    }
    if (sig == SIGPIPE) {
        exit(1);
    }
}

int main(int argc, char *argv[]) {
    atexit(closeAtExit);
    signal(SIGINT, sig_handler);
    signal(SIGPIPE, sig_handler);
    
    char buffer[BUF_SIZE];
    char oldbuffer[BUF_SIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int i = 0;
    int opt = 0;
    int comp_opt = -1;
    static struct option long_options[] = {
        {"port",      required_argument,   NULL,  0},
        {"compress",  no_argument,         NULL,  1},
        {0,           0,                   0,     0}
    };
    //Extract Options
    while(1) {
        int option_idx = 0;
        i = 0;
        opt = getopt_long(argc, argv, "", long_options, &option_idx);
        if (opt == -1) break;
        switch(opt) {
            case 0:
                portno = atoi(optarg);
                break;
            case 1:
                /*To be finished*/
                comp_opt = 1;
                break;
            default:
                fprintf(stderr, "Unrecognized Option: %s\n", argv[optind-1]);
                fprintf(stderr, "%s\n", "The correct form: ./lab1b-server [--port=YourPortNum] \
                    [--compress]");
                exit(1);
        }
    } // End of While-Loop: Get Options

    //socket
    if (portno < 0) portno = 1032; //default port num
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "%s\n", "ERROR Opening socket!");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "%s\n", "ERROR on binding!");
        exit(1);
    } 
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if (newsockfd < 0) {
        fprintf(stderr, "%s\n", "ERROR on accept!");
        exit(1);
    }
    //bzero(buffer, BUF_SIZE);
    //n_bytes = read(newsockfd, b)
    // socket end
    
    //fork
    if(pipe(pipefd1) < 0 || pipe(pipefd2) < 0) {
        fprintf(stderr, "%s\n", "ERROR Opening pipe!");
        exit(1);
    }
    c_pid = fork();
    if (c_pid == -1) {
        fprintf(stderr, "%s\n", "ERROR on fork!");
        exit(1);
    }
    // Child Process
    if (c_pid == 0) {
        close(pipefd1[1]);
        close(pipefd2[0]);
        dup2(pipefd1[0], STDIN_FILENO);
        dup2(pipefd2[1], STDOUT_FILENO);
        dup2(pipefd2[1], STDERR_FILENO);
        close(pipefd1[0]);
        close(pipefd2[1]);
        char *cmd = "/bin/bash";
        char *argv[2];
        argv[0] = "/bin/bash";
        argv[1] = NULL;
        if (execvp(cmd, argv) < 0) {
            fprintf(stderr, "%s\n", "execvp error!");
            exit(1);
        }
    } //Child End

    // Parent Process
    else {
        int n_bytes=0;
        int status;
        pid_t w;

        struct pollfd fds[2];
        int ret;
        ssize_t ret_in;
        fds[0].fd = newsockfd;
        fds[0].events = POLLIN | POLLHUP | POLLERR;
        fds[0].revents = 0;
        fds[1].fd = pipefd2[0];
        fds[1].events = POLLIN | POLLHUP | POLLERR;
        fds[1].revents = 0;
        close(pipefd1[0]);
        close(pipefd2[1]);
        while (1) {
            ret = poll(fds, 2, 0);
            z_stream defstream;
            z_stream infstream;
            if (ret >= 0) {
                if (fds[0].revents & POLLIN) {
                    bzero(buffer, BUF_SIZE);
                    ret_in = read(newsockfd, buffer, BUF_SIZE);
                    if (ret_in < 0) {
                        fprintf(stderr, "%s\n", "ERROR reading!");
                        exit(1);
                    }
                    if (comp_opt == 1 && ret_in > 0) {
                        strcpy(oldbuffer, buffer);
                        //z_stream infstream;
                        infstream.zalloc = Z_NULL;
                        infstream.zfree = Z_NULL;
                        infstream.opaque = Z_NULL;
                        //infstream.avail_in = (uInt)((char *) defstream.next_out - oldbuffer);
                        infstream.avail_in = ret_in;
                        infstream.next_in = (Bytef *) oldbuffer;
                        infstream.avail_out = BUF_SIZE;
                        infstream.next_out = (Bytef *) buffer;

                        inflateInit(&infstream);
                        //fprintf(stderr, "%d\n", ret_in);
                        //fprintf(stderr, "%s\n", buffer);
                        //inflate(&infstream, Z_SYNC_FLUSH);
                        do {
                            inflate(&infstream, Z_SYNC_FLUSH);
                            /*
                            if (inflate(&infstream, Z_SYNC_FLUSH) != Z_OK) {
                                fprintf(stderr, "%s\n", "ERROR on inflate!");
                                exit(1);
                            }
                            */
                        } while (infstream.avail_in > 0); 
                        inflateEnd(&infstream);
                        n_bytes = BUF_SIZE-infstream.avail_out;
                        for (i=0; i<n_bytes; ++i) {
                            if (buffer[i] == '\r' || buffer[i] == '\n') {
                                n_bytes = write(pipefd1[1], "\n", (ssize_t) 1);
                            }
                            else if (buffer[i] == 0x03) {
                                //fprintf(stderr, "%s\n", "^C");
                                kill(c_pid, SIGINT);
                            }
                            else if (buffer[i] == 0x04) {
                                //fprintf(stderr, "%s\n", "^D");
                                close(pipefd1[1]);
                            }
                            else{
                                write(pipefd1[1], buffer+i, (ssize_t) 1);
                            }
                        }
                    } else {
                        for (i=0; i<ret_in; ++i) {
                            if (buffer[i] == '\r' || buffer[i] == '\n') {
                                n_bytes = write(pipefd1[1], "\n", (ssize_t) 1);
                            }
                            else if (buffer[i] == 0x03) {
                                //fprintf(stderr, "%s\n", "^C");
                                kill(c_pid, SIGINT);
                            }
                            else if (buffer[i] == 0x04) {
                                //fprintf(stderr, "%s\n", "^D");
                                close(pipefd1[1]);
                            }
                            else{
                                write(pipefd1[1], buffer+i, (ssize_t) 1);
                            }
                        }
                    }

                } // socket input

                if (fds[1].revents & POLLIN) {
                    ret_in = read(pipefd2[0], buffer, BUF_SIZE);
                    if (comp_opt == 1 && ret_in > 0) {
                        strcpy(oldbuffer, buffer);
                        //z_stream defstream;
                        defstream.zalloc = Z_NULL;
                        defstream.zfree = Z_NULL;
                        defstream.opaque = Z_NULL;
                        //ret = deflateInit;
                        defstream.avail_in = ret_in;
                        defstream.next_in = (Bytef *)oldbuffer;
                        defstream.avail_out = BUF_SIZE;
                        defstream.next_out = (Bytef *)buffer;
                        deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
                        //deflate(&defstream, Z_FINISH);
                        
                        do {
                            if(deflate(&defstream, Z_SYNC_FLUSH) != Z_OK) {
                                fprintf(stderr, "%s\n", "ERROR on deflate!");
                                exit(1);
                            }
                        } while (defstream.avail_in > 0); 
                        n_bytes = BUF_SIZE-defstream.avail_out;
                        write(newsockfd, &buffer, n_bytes);
                        deflateEnd(&defstream); 
                    } // compression end
                    else {
                        for (i=0; i<ret_in; ++i) {
                            if(buffer[i] == '\n') {
                                write(newsockfd, "\r\n", (ssize_t) 2);
                            }
                            else {
                                write(newsockfd, buffer+i, (ssize_t) 1);
                            }
                        }
                    }
                } // shell input

                if (fds[0].revents & (POLLHUP | POLLERR)) {
                    close(pipefd1[1]);
                    kill(c_pid, SIGHUP);
                    w = waitpid(c_pid, &status, 0);
                    if (w == -1) {
                        fprintf(stderr, "%s\n", "ERROR on waitpid!");
                        exit(1);
                    }
                    else {
                        close(pipefd2[0]);
                        fprintf(stderr,  "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
                        exit(0);
                    }
                } // socket error

                if (fds[1].revents & (POLLHUP | POLLERR)) {
                    close(pipefd1[1]);
                    w = waitpid(c_pid, &status, 0);
                    if (w == -1) {
                        fprintf(stderr, "%s\n", "ERROR on waitpid!");
                        exit(1);
                    }
                    else {
                        close(pipefd2[0]);
                        fprintf(stderr,  "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));
                        exit(0);
                    }
                } // shell error
            }
        } // Parent pipe end
    }// Parent End
    exit(0);
}