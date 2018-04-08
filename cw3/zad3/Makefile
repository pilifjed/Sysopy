compile: main.c
	gcc -Wall main.c -o main
	#################################################################################################
	# Correct programme run syntax:									#
	#												#	
	#		./main pathToFile CPUTimeLimit MemLimit						#
	#												#
	#												#
	#################################################################################################
test: compile
	echo ls -al > test1.txt
	echo ls >> test1.txt
	
	gcc -Wall waste.c -o waste
	echo ./waste c > test2.txt

	echo ./waste m 10 > test3.txt
	
	./main test1.txt 1 1
	./main test2.txt 1 1
	./main test3.txt 1 1
clear:
	rm main test1.txt test2.txt test3.txt waste
	
