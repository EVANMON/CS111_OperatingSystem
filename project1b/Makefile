# NAME: Zhonglin Zhang
# EMAIL: evanzhang@ucla.edu
# ID: 005030520

default: 
	gcc -g -lz -Wall -Wextra -o lab1b-client lab1b-client.c
	gcc -g -lz -Wall -Wextra -o lab1b-server lab1b-server.c


clean:
	rm -rf lab1b-server lab1b-client *.tar.gz *.txt

dist:
	tar -zcvf "lab1b-005030520.tar.gz" lab1b-client.c lab1b-server.c Makefile README