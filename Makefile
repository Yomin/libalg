.PHONY: all, debug, clean, touch

NAME    = libalg
SOURCES = $(shell find . -name "*.c")
OBJECTS = $(SOURCES:%.c=%.o)

all: debug =
all: touch $(NAME).a

debug: debug = -ggdb
debug: touch $(NAME).a

clean:
	find . -maxdepth 2 ! -type d \( -perm -111 -or -name "*\.a" -or -name "*\.o" \) -exec rm {} \;


rj.o: .c
	gcc -Wall -c $(debug) -D NDEBUG -o $@ $<

$(NAME).a: $(OBJECTS)
	ar rcs $@ $(OBJECTS)

%.o: %.c
	gcc -c -Wall -ggdb -o $@ $<


touch:
	$(shell [ -f debug -a -z "$(debug)" ] && { touch $(SOURCES); rm debug; })
	$(shell [ ! -f debug -a -n "$(debug)" ] && { touch $(SOURCES); touch debug; })
