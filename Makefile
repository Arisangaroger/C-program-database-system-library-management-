CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lcomctl32 -lcomdlg32

# Console version
CONSOLE_SRCS = console_ui.c file_ops.c
CONSOLE_OBJS = $(CONSOLE_SRCS:.c=.o)
CONSOLE_TARGET = library_system

# GUI version
GUI_SRCS = gui_ui.c file_ops.c
GUI_OBJS = $(GUI_SRCS:.c=.o)
GUI_TARGET = library_system_gui

# Header files
HDRS = models.h file_ops.h console_ui.h

all: $(CONSOLE_TARGET) $(GUI_TARGET)

$(CONSOLE_TARGET): $(CONSOLE_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(GUI_TARGET): $(GUI_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compilation
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -f $(CONSOLE_OBJS) $(GUI_OBJS) $(CONSOLE_TARGET) $(GUI_TARGET)

# Run
run: $(CONSOLE_TARGET)
	./$(CONSOLE_TARGET)

.PHONY: all clean run 