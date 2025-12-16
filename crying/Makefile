TARGET := bin/app

CC        := gcc
CSTD      := -std=c17
WARN      := -Wall -Wextra
CFLAGS := -D_GNU_SOURCE

APP_MAIN    := src/main.c


OPT       := -O0
DBG       := -g

LDFLAGS   :=
LDLIBS    := -lm -lpthread

SRC_DIRS  := src ../library
INC_DIRS  := include ../library

SRCS      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
SRCS_NOMAIN := $(filter-out $(APP_MAIN),$(SRCS))

OBJS      := $(patsubst %.c,build/obj/%.o,$(SRCS_NOMAIN))
MAIN_OBJ := $(patsubst %.c,build/obj/%.o,$(APP_MAIN))
DEPS      := $(OBJS:.o=.d) $(MAIN_OBJ:.o=.d)


.PHONY: all
all: $(TARGET)

# Link step
$(TARGET): $(OBJS) $(MAIN_OBJ)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) $(MAIN_OBJ) -o $@ $(LDFLAGS) $(LDLIBS)

build/obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CSTD) $(WARN) $(OPT) $(DBG) $(CFLAGS) -MMD -MP \
	-D_XOPEN_SOURCE=500 \
		$(addprefix -I,$(INC_DIRS)) \
		-c $< -o $@

# Auto-include generated dependency files
-include $(DEPS)


.PHONY: clean
clean: 
	rm -rf build/obj
	rm -f $(TARGET)
	rm -f $(DEPS)

TEST_DIR := tests
UNITY_DIR := $(TEST_DIR)/unity

UTILS_DIR := $(TEST_DIR)/utils

TEST_RUNNER := $(TEST_DIR)/test_runner.c
TEST_CASES  := $(filter-out $(TEST_RUNNER), $(wildcard $(TEST_DIR)/*.c))
TEST_SRC    := $(TEST_RUNNER) $(TEST_CASES) $(UNITY_DIR)/unity.c $(UTILS_DIR)/utils.c
TEST_BIN := bin/tests/run_tests


.PHONY: test
test: all
	@mkdir -p $(dir $(TEST_BIN))
	$(CC) $(CSTD) $(WARN) $(OPT) $(DBG) $(CFLAGS) \
		$(addprefix -I, $(INC_DIRS)) -I$(UNITY_DIR) -I$(UTILS_DIR) \
		$(TEST_SRC) $(OBJS) -o $(TEST_BIN) $(LDFLAGS) $(LDLIBS)

