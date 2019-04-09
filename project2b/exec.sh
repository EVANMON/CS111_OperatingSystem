rm -f lab2b_list.csv

# Part 1 / 2
./lab2_list --threads=1  --iterations=1000  --sync=m	 >> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=m	 >> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=m	 >> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=m	 >> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=m	 >> lab2b_list.csv
./lab2_list --threads=16  --iterations=1000  --sync=m	 >> lab2b_list.csv
./lab2_list --threads=24  --iterations=1000  --sync=m	 >> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=s	 >> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=s	 >> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=s	 >> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=s	 >> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=s	 >> lab2b_list.csv
./lab2_list --threads=16  --iterations=1000  --sync=s	 >> lab2b_list.csv
./lab2_list --threads=24  --iterations=1000  --sync=s	 >> lab2b_list.csv


# Part 3
./lab2_list --threads=1  --iterations=1  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=2  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=4  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=8  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=16  --yield=id --list=4	>> lab2b_list.csv

./lab2_list --threads=4  --iterations=1  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=2  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=4  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=8  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=16  --yield=id --list=4	>> lab2b_list.csv

./lab2_list --threads=8  --iterations=1  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=2  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=4  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=8  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=16  --yield=id --list=4	>> lab2b_list.csv

./lab2_list --threads=12  --iterations=1  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=2  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=4  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=8  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=16  --yield=id --list=4	>> lab2b_list.csv

./lab2_list --threads=16  --iterations=1  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=2  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=4  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=8  --yield=id --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=16  --yield=id --list=4	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=10  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=20  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=40  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=80  --yield=id --sync=s --list=4	>> lab2b_list.csv

./lab2_list --threads=4  --iterations=10  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=20  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=40  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=80  --yield=id --sync=s --list=4	>> lab2b_list.csv

./lab2_list --threads=8  --iterations=10  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=20  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=40  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=80  --yield=id --sync=s --list=4	>> lab2b_list.csv

./lab2_list --threads=12  --iterations=10  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=20  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=40  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=80  --yield=id --sync=s --list=4	>> lab2b_list.csv

./lab2_list --threads=16  --iterations=10  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=20  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=40  --yield=id --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=80  --yield=id --sync=s --list=4	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=10  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=20  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=40  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=1  --iterations=80  --yield=id --sync=m --list=4	>> lab2b_list.csv

./lab2_list --threads=4  --iterations=10  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=20  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=40  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=80  --yield=id --sync=m --list=4	>> lab2b_list.csv

./lab2_list --threads=8  --iterations=10  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=20  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=40  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=80  --yield=id --sync=m --list=4	>> lab2b_list.csv

./lab2_list --threads=12  --iterations=10  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=20  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=40  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=80  --yield=id --sync=m --list=4	>> lab2b_list.csv

./lab2_list --threads=16  --iterations=10  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=20  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=40  --yield=id --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=16  --iterations=80  --yield=id --sync=m --list=4	>> lab2b_list.csv

# Part 4
# ./lab2_list --threads=1  --iterations=1000  --sync=m --list=1	>> lab2b_list.csv
# ./lab2_list --threads=2  --iterations=1000  --sync=m --list=1	>> lab2b_list.csv
# ./lab2_list --threads=4  --iterations=1000  --sync=m --list=1	>> lab2b_list.csv
# ./lab2_list --threads=8  --iterations=1000  --sync=m --list=1	>> lab2b_list.csv
# ./lab2_list --threads=12  --iterations=1000  --sync=m --list=1	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=m --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=m --list=4	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=m --list=8	>> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=m --list=8	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=m --list=8	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=m --list=8	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=m --list=8	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=m --list=16	>> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=m --list=16	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=m --list=16	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=m --list=16	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=m --list=16	>> lab2b_list.csv

# Part 5
# ./lab2_list --threads=1  --iterations=1000  --sync=s --list=1	>> lab2b_list.csv
# ./lab2_list --threads=2  --iterations=1000  --sync=s --list=1	>> lab2b_list.csv
# ./lab2_list --threads=4  --iterations=1000  --sync=s --list=1	>> lab2b_list.csv
# ./lab2_list --threads=8  --iterations=1000  --sync=s --list=1	>> lab2b_list.csv
# ./lab2_list --threads=12  --iterations=1000  --sync=s --list=1	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=s --list=4	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=s --list=4	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=s --list=8	>> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=s --list=8	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=s --list=8	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=s --list=8	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=s --list=8	>> lab2b_list.csv

./lab2_list --threads=1  --iterations=1000  --sync=s --list=16	>> lab2b_list.csv
./lab2_list --threads=2  --iterations=1000  --sync=s --list=16	>> lab2b_list.csv
./lab2_list --threads=4  --iterations=1000  --sync=s --list=16	>> lab2b_list.csv
./lab2_list --threads=8  --iterations=1000  --sync=s --list=16	>> lab2b_list.csv
./lab2_list --threads=12  --iterations=1000  --sync=s --list=16	>> lab2b_list.csv
