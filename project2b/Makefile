# NAME: Zhonglin Zhang
# EMAIL: evanzhang@ucla.edu
# ID: 005030520

FLAGS = -Wall -Wextra -g -lm -pthread -lprofiler -lrt -o
C_FILE = lab2_list.c SortedList.c
OBJ_FILE = lab2_list
CSV_FILE = lab2b_list.csv 
PROF_PATH = /usr/lib64/
TARGET = SortedList.h  Makefile $(C_FILE) $(CSV_FILE) profile.out *.png exec.sh lab2_list.gp README

default:
	gcc $(FLAGS) $(OBJ_FILE) lab2_list.c

tests: exec.sh
	chmod +x exec.sh
	./exec.sh

profile: default
	rm -f ./raw.gperf
	LD_PRELOAD=$(PROF_PATH)libprofiler.so.0 CPUPROFILE=raw.gperf \
	           ./lab2_list --threads=12 --iterations=1000 --sync=s
	pprof --text lab2_list raw.gperf > ./profile.out
	pprof --list=thread_oper lab2_list raw.gperf >> ./profile.out
	rm -f ./raw.gerf

graphs: tests
	gnuplot lab2_list.gp

dist: profile graphs
	tar cvzf lab2b-005030520.tar.gz $(TARGET)

clean:
	rm lab2_list
	rm lab2b-005030520.tar.gz
	rm -f *.gperf libprofiler*
