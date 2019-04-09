// Project 1B - Client
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

#define BUF_SIZE 256

static struct termios oldTerm;

int termModeReset(void) {
    if (tcsetattr(STDIN_FILENO, TCSANOW, &oldTerm) < 0) return -1;
    return 0;
}

void termAtExit(void) {
    termModeReset();
}

void nonCanonicalSet(void) {
    struct termios non_canonical;
    non_canonical = oldTerm;
    non_canonical.c_iflag = ISTRIP; /* only lower 7 bits    */
    non_canonical.c_oflag = 0;      /* no processing    */
    non_canonical.c_lflag = 0;      /* no processing    */ 
    non_canonical.c_cc[VMIN] = 1;
    non_canonical.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSANOW, &non_canonical) < 0) {
        fprintf(stderr, "%s\n", "Non-Canonical Mode Setting Error!");
        exit(1);
    }
}

void termModeSet(void) {
    if (!isatty(STDIN_FILENO)) {
        fprintf(stderr, "Error: %d is not a terminal!\n", STDIN_FILENO);
        exit(1);
    }
    else {
        if (tcgetattr(STDIN_FILENO, &oldTerm) < 0) {
            fprintf(stderr, "%s\n", "tcgetattr error!");
            exit(1);
        }
        if (atexit(termAtExit) != 0) {
            fprintf(stderr, "%s\n", "atexit error!");
            exit(1);
        }
        nonCanonicalSet();
    }
}

int main(int argc, char *argv[]) {
    //termModeSet();
    int opt = 0;
    int portno = -1;
    char *log_file;
    int log_fd = -1;
    int comp_opt = -1;
    int i;
    z_stream defstream;
    z_stream infstream;
    static struct option long_options[] = {
        {"port",        required_argument, NULL, 0},
        {"log",         required_argument, NULL, 1},
        {"compress",    no_argument,       NULL, 2},
        {0,             0,                 0,    0}
    };
    while (1) {
        int option_idx = 0;
        i = 0;
        opt = getopt_long(argc, argv, "", long_options, &option_idx);
        if (opt == -1) break;
        switch(opt) {
            case 0:
                portno = atoi(optarg);
                break;
            case 1:
            /*
                while (optarg[i] == ' ') {++i;}
                if (optarg[i] == '-' || i == (int) strlen(optarg)) {
                    fprintf(stderr, "%s\n", "Error in --log Option: Missing Argument!");
                    exit(1);
                }
                else{ 
            */
                    //strcpy(log_file, optarg+i);
                    log_file = optarg;
                    errno = 0;
                    log_fd = open(log_file, 0666);
                    if (log_fd < 0) {
                        fprintf(stderr, "Error[%s]:Opening Log File Failed!\n", strerror(errno));
                        exit(1);
                    }
                //}
                break;
            case 2:
                comp_opt = 1;
            /*To be finished */
                break;
            default:
                fprintf(stderr, "Unrecognized Option: %s\n", argv[optind-1]);
                fprintf(stderr, "%s\n", "The correct form: ./lab1b-client [--port=YourPortNum] \
                    [--log=YourLogFile] [--compress]");
                exit(1);
        }
    } // End of While-Loop: Get Options

    termModeSet();

    // stocket 
    if (portno < 0) portno = 1032; //default port num
    struct sockaddr_in serv_addr;
    //struct hostent *server;
    int sockfd;
    char buffer[BUF_SIZE];
    char oldbuffer[BUF_SIZE];
    char log_buffer[2*BUF_SIZE];
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        fprintf(stderr, "%s\n", "ERROR opening socket!");
        exit(1);
    }
    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        fprintf(stderr, "%s\n", "ERROR connecting!");
        exit(1);
    } // Finish Opening Socket

    // Start input...
    ssize_t ret_in;
    int n_bytes, ret;
    struct pollfd fds[2];
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN | POLLHUP | POLLERR;
    fds[0].revents = 0;
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLHUP | POLLERR;
    fds[1].revents = 0;
    while(1){
        ret = poll(fds, 2, 0);
        //printf("RET: %d, %d, %d\n", ret, fds[0].fd, fds[1].fd);
        if (ret >= 0) {
            if (fds[0].revents & POLLIN) {
                bzero(buffer, BUF_SIZE);
                ret_in = read(STDIN_FILENO, buffer, BUF_SIZE);
                if (ret_in < 0) {
                    fprintf(stderr, "%s\n", "ERROR reading!");
                    exit(1);
                }
                strcpy(oldbuffer, buffer);
                if (comp_opt == 1 && ret_in > 0) {
                    defstream.zalloc = Z_NULL;
                    defstream.zfree = Z_NULL;
                    defstream.opaque = Z_NULL;
                    defstream.avail_in = ret_in;
                    defstream.next_in = (Bytef *) oldbuffer;
                    defstream.avail_out = BUF_SIZE;
                    defstream.next_out = (Bytef *) buffer;
                    deflateInit(&defstream, Z_DEFAULT_COMPRESSION);
                    
                    do {
                        if (deflate(&defstream, Z_SYNC_FLUSH) != Z_OK) {
                            fprintf(stderr, "%s\n", "ERROR on deflate!");
                            exit(1);
                        }
                    } while (defstream.avail_in > 0); 
                    //fprintf(stderr, "%s\n", buffer);
                    //fprintf(stderr, "%d\n", BUF_SIZE-defstream.avail_out);
                    write(sockfd, &buffer, BUF_SIZE-defstream.avail_out);
    
                    //deflateInit(&defstream, Z_BEST_COMPRESSION);
                    //deflate(&defstream, Z_FINISH);
                    deflateEnd(&defstream);
                    

                }
                
                for (i=0; i<ret_in; ++i) {
                    if (oldbuffer[i] == '\r' || oldbuffer[i] == '\n') {
                        write(STDOUT_FILENO, "\r\n", (ssize_t) 2);
                        //n_bytes = write(sockfd, "\r\n", (ssize_t) 2);
                    }
                    else {
                        write(STDOUT_FILENO, oldbuffer+i, (ssize_t) 1); //? how to set the size ?
                        //n_bytes = write(sockfd, buffer+i, (ssize_t) 1);
                    }
                    if (comp_opt < 0) {
                        write(sockfd, buffer+i, (ssize_t) 1);
                    }
                    /*
                    if (write(sockfd, buffer+i, (ssize_t) 1) < 0) {
                        fprintf(stderr, "%s\n", "ERROR writing to socket!");
                        exit(1);
                    } */
                    /*
                    if (comp_opt < 0) {
                        n_bytes = write(sockfd, buffer+i, (ssize_t) 1);
                        if (n_bytes < 0) {
                            fprintf(stderr, "%s\n", "ERROR writing to socket!");
                            exit(1);
                        }
                    }
                    */
                }

                if (log_fd > 0) {
                    /*
                   n_bytes = BUF_SIZE - defstream.avail_out;
                    for(i=0; i<n_bytes; ++i) {
                        write(log_fd, "SENT 1 byte: ", 13);
                        write(log_fd, buffer+i, (ssize_t) 1);
                        write(log_fd, '\n', (ssize_t) 1);
                    } */
                    
                    bzero(log_buffer, 2*BUF_SIZE);
                    snprintf(log_buffer, 2*BUF_SIZE, "SENT %d bytes: %s\n", i, buffer);
                    //strcpy(log_buffer, "SENT %d bytes: ", i);
                    //strcat(log_buffer, buffer);
                    //strcat(log_buffer, '\n');
                    write(log_fd, &log_buffer, 2*BUF_SIZE); 
                } 

                //n_bytes = (comp_opt == 1) ? (BUF_SIZE-defstream.avail_out) : ret_in;
            }
            if (fds[1].revents & POLLIN) {
                bzero(buffer, BUF_SIZE);
                ret_in = read(sockfd, buffer, BUF_SIZE);
                if (ret_in < 0) {
                    fprintf(stderr, "%s\n", "ERROR reading from socket!");
                    exit(1);
                }
                if (comp_opt == 1 && ret_in > 0) {
                    strcpy(oldbuffer, buffer);
                    infstream.zalloc = Z_NULL;
                    infstream.zfree = Z_NULL;
                    infstream.opaque = Z_NULL;
                    //infstream.avail_in = (uInt)((char *) defstream.next_out - oldbuffer);
                    infstream.avail_in = ret_in;
                    infstream.next_in = (Bytef *) oldbuffer;
                    infstream.avail_out = BUF_SIZE;
                    infstream.next_out = (Bytef *) buffer;

                    inflateInit(&infstream);
                    //inflate(&infstream, Z_SYNC_FLUSH);
                    //fprintf(stderr, "%s\n", buffer);
                    //fprintf(stderr, "%d\n", ret_in);
                    
                    do {
                        inflate(&infstream, Z_SYNC_FLUSH);
                        /*
                        if (inflate(&infstream, Z_SYNC_FLUSH) != Z_OK) {
                            fprintf(stderr, "%s\n", "ERROR on inflate!");
                            exit(1);
                        }
                        */
                    } while(infstream.avail_in > 0); 
                    inflateEnd(&infstream);
                    n_bytes = BUF_SIZE-infstream.avail_out;
                    for (i=0; i<n_bytes; ++i) {
                        if (buffer[i] == '\n' || buffer[i] == '\r') {
                            write(STDOUT_FILENO, "\r\n", (ssize_t) 2);
                        }
                        else
                            write(STDOUT_FILENO, buffer+i, (ssize_t) 1);
                    }
                }
                else {
                    for (i=0; i<ret_in; ++i) {
                        if (buffer[i] == '\r' || buffer[i] == '\n') {
                            write(1, "\r\n", (ssize_t) 2);
                        }
                        else {
                            write(1, buffer+i, (ssize_t) 1);
                        }
                    }
                }
                /*
                if (log_fd > 0) {
                    n_bytes = BUF_SIZE - defstream.avail_out;
                    for(i=0; i<n_bytes; ++i) {
                        write(log_fd, "SENT 1 byte: ", 13);
                        write(log_fd, buffer+i, (ssize_t) 1);
                        write(log_fd, '\n', (ssize_t) 1);
                    } */
                    /*
                    bzero(log_buffer, 2*BUF_SIZE);
                    snprintf(log_buffer, 2*BUF_SIZE, "SENT %d bytes: %s\n", i, buffer);
                    //strcpy(log_buffer, "SENT %d bytes: ", i);
                    //strcat(log_buffer, buffer);
                    //strcat(log_buffer, '\n');
                    write(log_fd, &log_buffer, 2*BUF_SIZE); 
                    
                } */
                
                if (log_fd > 0) {
                    bzero(log_buffer, 2*BUF_SIZE);
                    snprintf(log_buffer, 2*BUF_SIZE, "RECEIVED %d bytes: %s\n", i, buffer);
                    //strcpy(log_buffer, "RECEIVED %d bytes: ", i);
                    //strcat(log_buffer, buffer);
                    //strcat(log_buffer, '\n');
                    write(log_fd, log_buffer, 2*BUF_SIZE);
                } 
            }
            if (fds[0].revents & (POLLHUP | POLLERR)) {
                exit(1); // ??
            }
            if (fds[1].revents & (POLLHUP | POLLERR)) {
                exit(0); // ??
            }
        }
            
        else {
            fprintf(stderr, "%s\n", "ERROR on poll!");
            exit(1);
        }       
    }
    exit(0);
}




