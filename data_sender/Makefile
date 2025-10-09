TARGET := bin/app

CC        := gcc
CSTD      := -std=c17
WARN      := -Wall -Wextra
CFLAGS := -D_GNU_SOURCE

OPT       := -O2
DBG       := -g

LDFLAGS   :=
LDLIBS    := -lm

SRC_DIRS  := src ../library
INC_DIRS  :=  ../library

SRCS      := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))

OBJS      := $(patsubst %.c,build/obj/%.o,$(SRCS))
DEPS      := $(OBJS:.o=.d)


.PHONY: all
all: $(TARGET)

# Link step
$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LDLIBS)

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
