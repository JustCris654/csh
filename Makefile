##
# See shell
#
# @file
# @version 0.1

build_and_run: main run

main.out: main.c
	clang main.c -o main.out

run: main.out
	./main.out

clean:
	rm main.out

# end
