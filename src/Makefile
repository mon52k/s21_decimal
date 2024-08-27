CC=gcc
CFLAGS=-std=c11 -pedantic -Wall -Werror -Wextra
CTESTFLAG=-Wall -Werror -Wextra -std=gnu11 -std=c11
OS := $(shell uname -s)
ifeq ($(OS),Linux)
	OPEN_CMD = xdg-open
	TEST_LIBS = -lcheck -lsubunit -lrt -lm -pthread
endif
ifeq ($(OS),Darwin)
	OPEN_CMD = open
	TEST_LIBS = -lcheck
endif

all: s21_decimal.a

clean:
	rm -rf *.o *.g* *.info *.out report *.a *.dSYM test

rebuild: clean all

test: s21_convertation.c s21_decimal.h helper.c s21_arithmetic.c s21_bit_manipulation.c s21_other.c unit_tests.c
	$(CC) $(CTESTFLAG) s21_convertation.c helper.c s21_arithmetic.c s21_bit_manipulation.c s21_other.c unit_tests.c $(TEST_LIBS) -o test
	./test

clang:
	cp ../materials/linters/.clang-format .
	clang-format -n *.c *.h 
	clang-format -i *.c *.h
	clang-format -n *.c *.h
	rm .clang-format

gcov_report:
	rm -f *.g*
	$(CC) $(CFLAGS) --coverage s21_convertation.c helper.c s21_arithmetic.c s21_bit_manipulation.c s21_other.c unit_tests.c $(TEST_LIBS) -o test
	./test
	lcov -t test -o rep.info -c -d .
	genhtml -o report rep.info
	$(OPEN_CMD) ./report/index.html
	rm -rf *.gcda *.gcno *.info

s21_decimal.a:
	$(CC) $(CFLAGS) -c s21_convertation.c helper.c s21_arithmetic.c s21_bit_manipulation.c s21_other.c
	ar rc s21_decimal.a *.o
	ranlib s21_decimal.a
	cp s21_decimal.a libs21_decimal.a
	rm -rf *.o