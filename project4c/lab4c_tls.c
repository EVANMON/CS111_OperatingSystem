// Project 4C TCP
// NAME: Zhonglin Zhang
// EMAIL: evanzhang@ucla.edu
// ID: 005030520

// sudo apt-get install lib

// ssl-dev

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <pthread.h>
#include <time.h>
#include <math.h>
#include <sched.h>
#include <signal.h>
#include <poll.h>
#include <mraa.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <openssl/ssl.h>


#define PERIOD 1
#define SCALE 2
#define LOG 3
#define ID 4
#define HOST 5

const int B = 4275;               // B value of the thermistor
const int R0 = 100000;            // R0 = 100k

struct timeval cur_time;
time_t next_time = 0;
struct tm *now;
char *logfile = NULL;
char *host = NULL;
char *id = NULL;
FILE* logfd = NULL;
char tscale = 'F';
int report = 1;
mraa_aio_context sensor;
int sfd;
SSL *ssl;
char record[50];

void press_button() {
	gettimeofday(&cur_time, 0); //?
	now = localtime(&(cur_time.tv_sec));
	// sprintf(buffer, "%d:%d:%d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
	sprintf(record, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
	SSL_write(ssl, record, strlen(record));
	memset(record, 0, 50);
	if (logfd) {
		fprintf(logfd, "%s\n", record);
		fflush(logfd);
	}
	mraa_aio_close(sensor);
	exit(0);
}

void error(char* msg) {
	press_button();
	fprintf(stderr, "%s\n", msg);
	exit(2);
}

float conv_temp(int ori_temp)
{
    // int a = analogRead(pinTempSensor);

    float R = 1023.0/ori_temp-1.0;
    R = R0*R;

    float temperature = 1.0/(log(R/R0)/B+1/298.15)-273.15; // convert to temperature via datasheet
    if (tscale == 'F') 
    	temperature = (temperature * 1.8) + 32;
    // Serial.print("temperature = ");
    // Serial.println(temperature);

    return temperature;
}

int main(int argc, char *argv[]) {
	int opt;
	int ptime = 1;
	int port = 0;
	float temp;
	ssize_t inret;
	char buffer[200];

	static struct option long_options[] = {
		{"period",   required_argument,    NULL,   PERIOD},
		{"scale",    required_argument,    NULL,   SCALE},
		{"log",      required_argument,    NULL,   LOG},
		{"id",       required_argument,    NULL,   ID},
		{"host",     required_argument,    NULL,   HOST},
		{0, 0, 0, 0}
	};

	while (1) {
		int option_idx = 0;
		opt = getopt_long(argc, argv, "", long_options, &option_idx);
		if (opt == -1) break;
		switch (opt) {
			case PERIOD:
			    ptime = atoi(optarg);
			    break;
			case SCALE:
				tscale = optarg[0];
				if (tscale != 'F' && tscale != 'C') {
					fprintf(stderr, "%s\n", "Unknown Argument");
					exit(1);
				}
				break;
			case LOG:
				logfile = optarg;
				logfd = fopen(logfile, "w");
				if (!logfd) {
					fprintf(stderr, "%s\n", "Open File Error");
					exit(1);
				}
				break;
			case ID:
				id = optarg;
				break;
			case HOST:
                host = optarg;
                break;

			default:
				fprintf(stderr, "%s\n", "Unrecognized Option");
				exit(1);
		}
	}
	port = atoi(argv[argc-1]);
	if (port == -1) {
		fprintf(stderr, "%s\n", "No Port Error");
		exit(1);
	}

	struct sockaddr_in srv_addr;
	struct hostent *server;

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd < 0) {
		fprintf(stderr, "Socket Init Error\n");
		exit(2);
	}
	server = gethostbyname(host);
	if (server == NULL) {
		fprintf(stderr, "Error: no such host as %s\n", host);
		exit(2);
	}
	memset((char *)&srv_addr, 0, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	memcpy((char *)&srv_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
	srv_addr.sin_port = htons(port);
	// srv_addr.sin_addr.s_addr = *(long *)(server->h_length);

	if (connect(sfd, (struct sockaddr *)&srv_addr, sizeof(srv_addr)) < 0){
		fprintf(stderr, "%s\n", "Error: fails to build connection");
		exit(2);
	}

	SSL_library_init();
	SSL_load_error_strings();
	OpenSSL_add_all_algorithms();

	SSL_CTX* sslContext = SSL_CTX_new(TLSv1_client_method());
	if (sslContext == NULL) {
		fprintf(stderr, "Error can't get SSL context. \n");
		exit(2);
	}
    ssl = SSL_new(sslContext);
    SSL_set_fd(ssl, sfd);
    if( SSL_connect(ssl) != 1 ) {
    	fprintf(stderr, "Error SSL handshake. \n");
    	exit(2);
    }

	struct pollfd pfds[1];
	pfds[0].fd = sfd;
	pfds[0].events = POLLIN;

	memset(record, 0, 50);
	sprintf(record, "ID=%s\n", id);
	SSL_write(ssl, record, strlen(record));
	memset(record, 0, 50);
	if (logfd) {
		fprintf(logfd, "ID=%s\n", id);
		fflush(logfd);
	}
	memset(record, 0, 50);

    // Sensor Initialization
	sensor = mraa_aio_init(1);
	if (!sensor) {
		fprintf(stderr, "%s\n", "Init Error on Sensor");
		exit(2);
	}

	while (1) {
		gettimeofday(&cur_time, 0);
		if (report && cur_time.tv_sec >= next_time) {
			now = localtime(&(cur_time.tv_sec));
			int o_temp = mraa_aio_read(sensor);
			temp = conv_temp(o_temp);
			sprintf(record, "%02d:%02d:%02d %.1f\n", now->tm_hour, now->tm_min, now->tm_sec, temp);
			SSL_write(ssl, record, strlen(record));
			memset(record, 0, 50);
			if (logfd) {
				fprintf(logfd, "%02d:%02d:%02d %.1f\n", now->tm_hour, now->tm_min, now->tm_sec, temp);
				fflush(logfd);
			}
			next_time = cur_time.tv_sec + ptime;
		}

		if (poll(pfds, 1, 0) > 0) {
			if (pfds[0].revents & POLLIN) {
				inret = SSL_read(ssl, buffer, 200);
				if (inret < 0) {
					fprintf(stderr, "%s\n", "Error: socket read fails");
					exit(2);
				}
				else if (inret == 0) press_button();
				else {
					char * cmd = strtok(buffer, "\n");
					while (cmd != NULL) {
						// buffer[inret-1] = '\0'; //???
						if (strcmp(cmd, "STOP")==0) {
							report = 0;
						}
						else if (strcmp(cmd, "START")==0) {
							report = 1;
						}
						else if (strcmp(cmd, "OFF")==0) {
							now = localtime(&(cur_time.tv_sec));
							// fprintf(stdout, "%s\n", cmd);
							sprintf(record, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
							SSL_write(ssl, record, strlen(record));
							memset(record, 0, 50);
							if (logfd) {
								fprintf(logfd, "%s\n", cmd);
								fprintf(logfd, "%02d:%02d:%02d SHUTDOWN\n", now->tm_hour, now->tm_min, now->tm_sec);
								fflush(logfd);
							}
							mraa_aio_close(sensor);
							exit(0);
						}
						else if (strstr(cmd, "PERIOD")!=NULL) {
							ptime = atoi(strchr(cmd, '=') + 1);
						}
						else if (strstr(cmd, "SCALE")!=NULL) {
							tscale = *(strchr(cmd, '=') + 1);
						}
						// else if (strstr(buffer, "LOG")!=NULL) ;
					        // fprintf(stdout, "%d %s\n", inret, buffer);
						if (logfd) {
							fprintf(logfd, "%s\n", cmd);
							fflush(logfd);
						}
						cmd = strtok(NULL, "\n");
					}
				}
			}
		}
	}
	mraa_aio_close(sensor);
	return 0;

}
