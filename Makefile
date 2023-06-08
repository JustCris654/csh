##
# See shell
#
# @file
# @version 0.1

main.out: main.c
	clang -Wall -Wextra main.c -o main.out

run: main.out
	./main.out

clean:
	rm main.out

# end
