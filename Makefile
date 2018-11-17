CC := gcc

test: clean lz77-konami.o mem/open_memstream.o mem/fmemopen.o
	@gcc -o test test.c lz77-konami.o mem/open_memstream.o mem/fmemopen.o
	./test
.PHONY: clean
clean:
	-@$(RM) test *.o
