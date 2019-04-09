.SILENT:
default:
	echo 'python lab3b.py $$1' > lab3b
	chmod +x ./lab3b

clean:
	rm -f *.tar.gz

dist: clean default
	tar -czf lab3b-304546929.tar.gz lab3b.py lab3b README Makefile
