CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11 -O2 -Isrc `pkg-config --cflags gtk4`
LDFLAGS = `pkg-config --libs gtk4` -lwevtapi -ladvapi32

SRC_DIR = src
BLD_DIR = build
OBJ_DIR = $(BLD_DIR)/obj
BIN_DIR = $(BLD_DIR)/bin

SOURCES_MAIN = $(SRC_DIR)/main/main.c

SOURCES_CORE = \
    $(SRC_DIR)/core/types/event_record.c \
    $(SRC_DIR)/core/readers/windows/event_log_windows.c \
    $(SRC_DIR)/core/statistics/event_statistics.c

SOURCES_UI = \
    $(SRC_DIR)/ui/gtk/windows/main_window.c \
    $(SRC_DIR)/ui/gtk/components/menu_bar.c \
    $(SRC_DIR)/ui/gtk/components/tool_bar.c \
    $(SRC_DIR)/ui/gtk/components/sidebar.c \
    $(SRC_DIR)/ui/gtk/callbacks/action_handlers.c \
    $(SRC_DIR)/ui/models/event_models.c

SOURCES_DATA = \
    $(SRC_DIR)/data/export/csv_exporter.c \
    $(SRC_DIR)/data/import/csv_importer.c

SOURCES_UTILS = \
    $(SRC_DIR)/utils/conversion/string_utils.c \
    $(SRC_DIR)/utils/time/time_formatter.c \
    $(SRC_DIR)/utils/platform/privilege_check.c

SOURCES = $(SOURCES_MAIN) $(SOURCES_CORE) $(SOURCES_UI) \
          $(SOURCES_DATA) $(SOURCES_UTILS)

OBJECTS = $(SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/event_viewer.exe

.PHONY: all clean rebuild run directories help

all: directories $(TARGET)
	@echo ""
	@echo "  Build complete → $(TARGET)"
	@echo ""

$(TARGET): $(OBJECTS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

directories:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(BIN_DIR)

clean:
	rm -rf $(BLD_DIR)
	@echo "Build artifacts removed."

rebuild: clean all

run: all
	cd $(BIN_DIR) && ./event_viewer.exe

help:
	@echo ""
	@echo "  Windows Event Viewer — build targets"
	@echo "  ──────────────────────────────────────"
	@echo "  all      Build the application (default)"
	@echo "  clean    Remove all build artifacts"
	@echo "  rebuild  Clean then build"
	@echo "  run      Build and launch the executable"
	@echo "  help     Show this message"
	@echo ""
