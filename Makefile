.PHONY: all, debug, clean, touch

NAME    = libalg
SOURCES = $(shell find . -name "*.c")
OBJECTS = $(SOURCES:%.c=%.o)

all: debug = off
all: flags = -D NDEBUG
all: touch $(NAME).a

debug: debug = on
debug: flags = -ggdb
debug: touch $(NAME).a

clean:
	find . -maxdepth 2 ! -type d \( -perm -111 -or -name "*\.a" -or -name "*\.o" \) -exec rm {} \;


$(NAME).a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.c
	gcc -c -Wall $(flags) -o $@ $<


touch:
	$(shell [ -f debug -a "$(debug)" = "off" ] && { touch $(SOURCES); rm debug; })
	$(shell [ ! -f debug -a "$(debug)" = "on" ] && { touch $(SOURCES); touch debug; })
