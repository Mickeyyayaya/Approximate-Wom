all:
		gcc -O3 experiment_sandbox.c -o test1 -g
		gcc -O3 experiment4.c -o test2 -g
		gcc -O3 experiment2.c -o test3 -g
		time ./test1
		time ./test2
		time ./test3