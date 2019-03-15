NAME = magruka
TARGET = bin/$(NAME)
MANPAGE = $(NAME).1
PREFIX ?= /usr/local
LIBS = -lm -lSDL2 -lSDL2_image
.PHONY: all debug release install clean

all: $(TARGET) assets/img/spritesheet.png

bin/%.o: src/%.c $(wildcard src/*.h)
	@mkdir -p bin
	gcc $(FLAGS) -Wall -Wextra -Wpedantic $(LIBS) -c $< -o $@

$(TARGET): $(patsubst src/%.c, bin/%.o, $(wildcard src/*.c))
	@mkdir -p bin
	gcc $(FLAGS) -Wall -Wextra -Wpedantic $(LIBS) $^ -o $@

debug: FLAGS = -g -O0

debug: $(TARGET)

release: FLAGS = -O3

release: $(TARGET)
	strip -s -R .comment -R .gnu.version $(TARGET)

install: $(TARGET)
	install -D $(TARGET) $(DESTDIR)$(PREFIX)/$(TARGET)
	install -Dm644 $(MANPAGE) $(DESTDIR)$(PREFIX)/share/man/man1/$(MANPAGE)

assets/img/spritesheet.png: assets/img/spritesheet-unscaled.png
	convert assets/img/spritesheet-unscaled.png -sample 400% assets/img/spritesheet.png

clean:
	rm -rf bin
