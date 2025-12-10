# ─────────────────────────────────────────────────────────────
# Joker's Gambit – Makefile (Manjaro/Arch + raylib)
# ─────────────────────────────────────────────────────────────

TARGET   := jokersgambit                 # final binary name (lowercase is nicer)

# ── Source files (all .c files inside src/) ─────────────────
SRC_DIR  := src
SOURCES  := $(wildcard $(SRC_DIR)/*.c)    # automatically picks main.c gui.c aibots.c useraccount.c ...

# ── Object files (same basename, placed next to the binary) ─
OBJ_DIR  := obj
OBJECTS  := $(addprefix $(OBJ_DIR)/,$(notdir $(SOURCES:.c=.o)))

# ── Compiler & flags ───────────────────────────────────────
CC       := gcc
CFLAGS   := -Wall -Wextra -Wpedantic -O2 -march=native -pipe \
            -I. -I$(SRC_DIR)              # include project root + src/ for headers

# Use pkg-config for raylib (cleanest way on Arch/Manjaro)
LDFLAGS  := $(shell pkg-config --libs raylib) -lm -lcjson

# ── Rules ───────────────────────────────────────────────────
.PHONY: all run clean edit watch remove_data

# Make 'all' run 'remove_data' BEFORE building the target
all: remove_data $(TARGET)



# Link final executable
$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)
	@echo "Build finished -> ./$(TARGET)"

# Compile .c → .o (creates obj/ folder automatically)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create obj/ directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# ── Convenience targets ─────────────────────────────────────
run: $(TARGET)
	./$(TARGET)   # runs from project root so assets in cards/, keycards/ are found

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Cleaned $(TARGET) and object files"

# Open the whole project in VS Code (you already have this shortcut)
edit:
	code .

# Optional: rebuild on any source change (great with entr(1) or watchexec)
watch:
	@echo "Watching for changes... (Ctrl+C to stop)"
	@while true; do \
		$(MAKE) --silent; \
		inotifywait -qre close_write,move,create,delete $(SRC_DIR) || sleep 0.1; \
	done