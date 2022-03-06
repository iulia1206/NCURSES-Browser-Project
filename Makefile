all:build 

build: task1 task2 task3 task4 browser

task1: task1.c
	gcc -w -o task1 task1.c common.c
task2: task2.c
	gcc -o task2 task2.c common.c
task3: task3.c
	gcc -w -o task3 task3.c common.c
task4: task4.c
	gcc -w -o task4 task4.c
browser: browser.c
	gcc -w -o browser browser.c common.c -lncurses -lmenu 
