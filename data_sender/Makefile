CC := gcc
CFLAGS := -std=c17 -Wall -Wextra -O2 -Iinclude -Itests -I../library
LDFLAGS := 

SRC_DIRS := src ../library
SRC := $(foreach dir, $(SRC_DIRS), $(wildcard src/*.c))
SRC := $(filter-out src/main.c, $(SRC))
OBJ := $(SRC:.c=.o)

TEST_SRC := 
TEST_BIN := bin/tests

APP := app.a

.PHONY: all lib test clean

all:lib

lib: $(OBJ)
	ar rcs $(APP) $(OBJ)

test: lib
	@mkdir -p $(TEST_BIN)
	$(CC) $(CFLAGS) $(TEST_SRC) $(APP) -o $(TEST_BIN)/run_tests $(LDFLAGS)
	@echo "Running unit tests..."
	$(TEST_BIN)/run_tests

clean:
	rm -rf $(TEST_BIN)
	rm -f $(OBJ) $(APP)
	rm -rf build

	