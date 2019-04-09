# NAME: Zhonglin Zhang
# EMAIL: evanzhang@ucla.edu
# ID: 005030520

TARGET = lab2_add.c lab2_list.c SortedList.c SortedList.h Makefile lab2_add.csv lab2_list.csv lab2_add-1.png lab2_add-2.png lab2_add-3.png lab2_add-4.png lab2_add-5.png lab2_list-1.png lab2_list-2.png lab2_list-3.png lab2_list-4.png lab2_add.gp lab2_list.gp exec.sh README

default:
	gcc -Wall -Wextra -pthread -lrt -o lab2_add lab2_add.c
	gcc -Wall -Wextra -pthread -lrt -o lab2_list lab2_list.c
tests:
	chmod +x exec.sh
	./exec.sh
graphs:
	gnuplot lab2_add.gp
	gnuplot lab2_list.gp

dist: default tests graphs
	tar -czf lab2a-005030520.tar.gz $(TARGET)
clean:
	rm lab2_add lab2_list
	rm lab2a-005030520.tar.gz
