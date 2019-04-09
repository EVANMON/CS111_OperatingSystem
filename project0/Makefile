# NAME: Zhonglin Zhang
# EMAIL: evanzhang@ucla.edu
# ID: 005030520

default: lab0.c
	gcc -g -Wall -Wextra -o lab0 lab0.c
check: lab0
	
	@echo -e "\n===== Input Option Check... =====" 
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" > infile.txt 
	@./lab0 --input=infile.txt > outfile.txt  
	@diff infile.txt outfile.txt;\
	if [ $$? -eq 0 ]; \
	then \
		echo "** The infile and outfile are the same."; \
	else \
	 	echo "** The infile and outfile are not the same."; \
	fi
	@rm infile.txt outfile.txt

    
	@echo -e "\n===== Output Option Check... ====="
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" > infile.txt 
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" | ./lab0 --output=outfile.txt 
	@diff infile.txt outfile.txt;\
	if [ $$? -eq 0 ]; \
	then \
		echo "** The infile and outfile are the same."; \
	else \
	 	echo "** The infile and outfile are not the same."; \
	fi
	@rm outfile.txt

	
	@echo -e "\n===== Unrecognized Argument Check... ====="
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" > infile.txt
	@./lab0 --unknown=infile.txt;\
	if [ $$? -eq 1 ]; \
	then \
		echo "** Find the unrecognized argument"; \
	else \
		echo "** Fail to find the unrecognized argument"; \
	fi
	@rm infile.txt

	@echo -e "\n===== Input file Open Check... ====="
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" > infile.txt
	@./lab0 --input=unknown;\
	if [ $$? -eq 2 ]; \
	then \
		echo "** Find the problem in file open"; \
	else \
		echo "** Fail to find the open problem"; \
	fi
	@rm infile.txt

	@echo -e "\n===== Output file Open Check... ====="
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" > infile.txt
	@./lab0 --input=infile.txt --output --catch;\
	if [ $$? -eq 3 ]; \
	then \
		echo "** Find the problem in file create"; \
	else \
		echo "** Fail to find the create problem"; \
	fi
	@rm infile.txt

	@echo -e "\n===== SIGSEGV Receiving Check... ====="
	@echo "Hello World\nName: Zhonglin Zhang\nUID: 005030520" > infile.txt
	@./lab0 --input=infile.txt --output=outfile.txt --segfault --catch;\
	if [ $$? -eq 4 ]; \
	then \
		echo "** Successfully Receive SIGSEGV"; \
	else \
		echo "** Fail to Receive SIGSEGV"; \
	fi
	@rm infile.txt outfile.txt

clean:
	rm -rf lab0 *.txt *.tar.gz

dist:
	tar -zcvf "lab0-005030520.tar.gz" lab0.c Makefile README *.png

