CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -Iinclude

SRC = src/max30102.c
TEST = tests/test_max30102.c
TEST_BIN = tests/test_max30102

.PHONY: test clean

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(SRC) $(TEST)
	$(CC) $(CFLAGS) -o $@ $(SRC) $(TEST) -lm

clean:
	rm -f $(TEST_BIN)
