traverse: main.c
	gcc -Wall main.c -o traverse
	#################################################################################################
	# Correct programme run syntax:									#
	#												#	
	#		./traverse pathToDirectory [>|<|=] day month year hour minute second 		#
	#												#
	#												#
	#################################################################################################

test: traverse
	mkdir a1 a1/a2 a1/a22 a1/a2/a3
	touch a1/t1.txt a1/a2/t2.txt a1/a22/t22.txt a1/a2/a3/t3.txt
	mkdir b1
	ln -s ./b1 ./a1/a22
	./traverse ./a1 \> 03 07 1997 10 10 10 