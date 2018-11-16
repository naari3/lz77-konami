CC := gcc

test: clean lz77-konami.o
	@gcc -o test test.c lz77-konami.o
	./test
.PHONY: clean
clean:
	-@$(RM) test *.o
