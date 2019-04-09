 # Project 4C
 # NAME: Zhonglin Zhang
 # EMAIL: evanzhang@ucla.edu
 # ID: 005030520

default:
	gcc -Wall -Wextra -lmraa -lssl -lcrypto -lm -g -o lab4c_tcp lab4c_tcp.c
	gcc -Wall -Wextra -lmraa -lssl -lcrypto -lm -g -o lab4c_tls lab4c_tls.c

clean:
	rm -rf lab4c_tcp lab4c_tls *.tar.gz *.txt

dist:
	tar -czvf lab4c-005030520.tar.gz lab4c_tcp.c lab4c_tls.c Makefile README
