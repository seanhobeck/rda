# compiler and compiler flags
CC := gcc
CFLAGS := -g -O0 -std=c17
CFLAGS += -Iinclude

# derive include directories (-I) from header locations in src/
INCLUDES := $(shell find src -type d | sort -u)

# all source files
SRCS_ALL := $(shell find src/ -name '*.c')

# app: include main.c, exclude entry.c
APP_EXCLUDE := src/entry.c
APP_SRCS := $(filter-out $(APP_EXCLUDE), $(SRCS_ALL))
APP_OBJS := $(patsubst src/%.c, build/obj/app/%.o, $(APP_SRCS))

# libs: include entry.c, exclude tmain.c
LIB_EXCLUDE := src/tmain.c
LIB_SRCS := $(filter-out $(LIB_EXCLUDE), $(SRCS_ALL))
LIB_OBJS := $(patsubst src/%.c, build/obj/lib/%.o, $(LIB_SRCS))

# final executables / libraries
TARGET := rda
LIBDIR := build/lib
SHLIB := $(LIBDIR)/librda.so
STLIB := $(LIBDIR)/librda.a

# default target (build app; tmain.c included, entry.c excluded)
all: $(TARGET)

# application build.
$(TARGET): $(APP_OBJS)
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $^ -o $@

# compile app objects
build/obj/app/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# shared library (.so) — needs -fPIC
$(SHLIB): $(LIB_OBJS)
	@mkdir -p $(LIBDIR)
	$(CC) -shared $^ -o $@

# static library (.a)
$(STLIB): $(LIB_OBJS)
	@mkdir -p $(LIBDIR)
	ar rcs $@ $^

# compile lib objects with -fPIC for shared compatibility
build/obj/lib/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -fPIC -c $< -o $@

# convenience target to build both libraries
.PHONY: libs
libs: $(SHLIB) $(STLIB)

# debug target
.PHONY: debug
debug: CFLAGS += -O0 -g
debug: $(TARGET)

# clean target
.PHONY: clean
clean:
	rm -rf build $(TARGET)
