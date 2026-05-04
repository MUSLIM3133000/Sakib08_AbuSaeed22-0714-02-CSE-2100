CXX     = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -O2 -Isrc `pkg-config --cflags gtk4`
LDFLAGS  = `pkg-config --libs gtk4` -lwevtapi -ladvapi32

SRC_DIR = src
BLD_DIR = build
OBJ_DIR = $(BLD_DIR)/obj
BIN_DIR = $(BLD_DIR)/bin

# ── MVC Source Files ──────────────────────────────────────────────────────────

SOURCES_MAIN = $(SRC_DIR)/main/main.cpp

SOURCES_MODEL = \
    $(SRC_DIR)/model/types/event_record.cpp \
    $(SRC_DIR)/model/repository/windows/event_log_windows.cpp \
    $(SRC_DIR)/model/statistics/event_statistics.cpp \
    $(SRC_DIR)/model/export/csv_exporter.cpp \
    $(SRC_DIR)/model/import/csv_importer.cpp

SOURCES_VIEW = \
    $(SRC_DIR)/view/windows/main_window.cpp \
    $(SRC_DIR)/view/components/menu_bar.cpp \
    $(SRC_DIR)/view/components/tool_bar.cpp \
    $(SRC_DIR)/view/components/sidebar.cpp

SOURCES_CONTROLLER = \
    $(SRC_DIR)/controller/action_handlers.cpp \
    $(SRC_DIR)/controller/event_controller.cpp

SOURCES_UTILS = \
    $(SRC_DIR)/utils/conversion/string_utils.cpp \
    $(SRC_DIR)/utils/time/time_formatter.cpp \
    $(SRC_DIR)/utils/platform/privilege_check.cpp

SOURCES = $(SOURCES_MAIN) $(SOURCES_MODEL) $(SOURCES_VIEW) \
          $(SOURCES_CONTROLLER) $(SOURCES_UTILS)

OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = $(BIN_DIR)/event_viewer.exe

.PHONY: all clean rebuild run directories help

all: directories $(TARGET)
	@echo ""
	@echo "  Build complete → $(TARGET)"
	@echo ""

$(TARGET): $(OBJECTS)
	$(CXX) $^ -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

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
	@echo "  Windows Event Viewer (MVC) — build targets"
	@echo "  ───────────────────────────────────────────"
	@echo "  all      Build the application (default)"
	@echo "  clean    Remove all build artifacts"
	@echo "  rebuild  Clean then build"
	@echo "  run      Build and launch the executable"
	@echo "  help     Show this message"
	@echo ""
