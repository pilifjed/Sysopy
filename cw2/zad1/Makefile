sysLibCmp: main.c
	gcc -Wall main.c -o sysLibCmp
	#################################################################################################
	# Correct programme run syntax:									#
	#	Generating:										#
	# 		./sysLibCmp generate outputFilename recordCount recordSize			#
	#	Sorting:										#
	#		./sysLibCmp sort Filename recordCount recordSize sys|lib			#
	#	Copying:										#	
	#		./sysLibCmp copy inputFilename outputFileneme recordCount recordSize sys|lib	#
	#################################################################################################

tests: sysLibCmp
	./sysLibCmp generate dane 500 4
	cp dane dane2
	./sysLibCmp sort dane 500 4 sys >> timeRes.txt
	./sysLibCmp sort dane2 500 4 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 512
	cp dane dane2
	./sysLibCmp sort dane 500 512 sys >> timeRes.txt
	./sysLibCmp sort dane2 500 512 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 4096
	cp dane dane2
	./sysLibCmp sort dane 500 4096 sys >> timeRes.txt
	./sysLibCmp sort dane2 500 4096 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 8192
	cp dane dane2
	./sysLibCmp sort dane 500 8192 sys >> timeRes.txt
	./sysLibCmp sort dane2 500 8192 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 4
	./sysLibCmp copy dane dane2 500 4 sys >> timeRes.txt
	./sysLibCmp copy dane dane2 500 4 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 512
	./sysLibCmp copy dane dane2 500 512 sys >> timeRes.txt
	./sysLibCmp copy dane dane2 500 512 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 4096
	./sysLibCmp copy dane dane2 500 4096 sys >> timeRes.txt
	./sysLibCmp copy dane dane2 500 4096 lib >> timeRes.txt
	
	./sysLibCmp generate dane 500 8192
	./sysLibCmp copy dane dane2 500 8192 sys >> timeRes.txt
	./sysLibCmp copy dane dane2 500 8192 lib >> timeRes.txt 
	
	rm dane dane2	
