 # Project 4B
 # NAME: Zhonglin Zhang
 # EMAIL: evanzhang@ucla.edu
 # ID: 005030520

default:
	gcc -Wall -Wextra -lmraa -lm -g -o lab4b lab4b.c

check:
	{ echo "STOP"; sleep 2; echo "START"; sleep 2; echo "LOG"; sleep 2; echo "OFF"; } | ./lab4b --log=log.txt

	test $$? -eq 0

	grep "START" log.txt > /dev/null
	test $$? -eq 0

	grep "STOP" log.txt > /dev/null
	test $$? -eq 0

	grep "LOG" log.txt > /dev/null
	test $$? -eq 0

	grep "OFF" log.txt > /dev/null
	test $$? -eq 0

	grep "SHUTDOWN" log.txt > /dev/null
	test $$? -eq 0


clean:
	rm -rf lab4b *.tar.gz *.txt

dist:
	tar -czvf lab4b-005030520.tar.gz lab4b.c Makefile README
