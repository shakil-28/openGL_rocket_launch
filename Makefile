# =============================================
# ROCKET SIMULATOR MAKEFILE
# =============================================

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2 -I.
DEBUG_FLAGS = -g -DDEBUG
RELEASE_FLAGS = -O3 -DNDEBUG

# Platform detection for OpenGL/GLUT libraries
UNAME_S := $(shell uname -s)

# Platform-specific library flags
ifeq ($(UNAME_S),Linux)
    # Linux
    LDFLAGS = -lGL -lGLU -lglut -lm
endif
ifeq ($(UNAME_S),Darwin)
    # macOS
    LDFLAGS = -framework GLUT -framework OpenGL -framework Cocoa
endif
ifeq ($(UNAME_S),Windows_NT)
    # Windows (MinGW)
    LDFLAGS = -lopengl32 -lglu32 -lglut32 -lm
endif

# Source files
SRCS = main.cpp \
       globals.cpp \
       graphics_functions.cpp \
       animation_functions.cpp \
       graphics_algorithms.cpp

# Header files
HEADERS = types_structures.h \
          constants_config.h \
          globals_extern.h \
          functions_declarations.h \
          graphics_algorithms.h \
          main_includes.h

# Object files (in build directory)
OBJDIR = build
OBJS = $(SRCS:%.cpp=$(OBJDIR)/%.o)

# Executable name
TARGET = rocket_simulator

# Dependency files
DEPS = $(OBJS:.o=.d)

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[1;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

# =============================================
# BUILD RULES
# =============================================

# Default build (release mode)
all: release

# Release build
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)
	@echo "$(GREEN)✅ Release build complete!$(NC)"

# Debug build
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)
	@echo "$(BLUE)🔧 Debug build complete!$(NC)"

# Link object files to create executable
$(TARGET): $(OBJS)
	@echo "$(YELLOW)🔗 Linking executable...$(NC)"
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "$(GREEN)🚀 Executable created: $(TARGET)$(NC)"

# Compile source files to object files with dependency generation
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	@echo "$(BLUE)📝 Compiling $<...$(NC)"
	@$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

# Create build directory if it doesn't exist
$(OBJDIR):
	@mkdir -p $(OBJDIR)

# Include dependency files
-include $(DEPS)

# =============================================
# UTILITY TARGETS
# =============================================

# Clean build files
clean:
	@echo "$(RED)🧹 Cleaning build files...$(NC)"
	@rm -rf $(OBJDIR) $(TARGET)
	@echo "$(GREEN)✅ Clean complete!$(NC)"

# Run the program
run: $(TARGET)
	@echo "$(GREEN)🚀 Launching rocket simulator...$(NC)"
	@./$(TARGET)

# Run with debug output
debug-run: debug
	@echo "$(BLUE)🐛 Running in debug mode...$(NC)"
	@./$(TARGET)

# Build and run in one command
br: clean release run

# List source files
list:
	@echo "$(YELLOW)📁 Source files:$(NC)"
	@for file in $(SRCS); do echo "  $$file"; done
	@echo "$(YELLOW)📁 Header files:$(NC)"
	@for file in $(HEADERS); do echo "  $$file"; done

# Display project info
info:
	@echo "$(BLUE)================================$(NC)"
	@echo "$(GREEN)🚀 ROCKET SIMULATOR PROJECT$(NC)"
	@echo "$(BLUE)================================$(NC)"
	@echo "Compiler: $(CXX)"
	@echo "Platform: $(UNAME_S)"
	@echo "Libraries: $(LDFLAGS)"
	@echo "Target: $(TARGET)"
	@echo "Build dir: $(OBJDIR)"
	@echo "Source files: $(words $(SRCS))"
	@echo "Header files: $(words $(HEADERS))"
	@echo "$(BLUE)================================$(NC)"

# Create a simple backup
backup:
	@tar -czf rocket_simulator_backup_$(shell date +%Y%m%d_%H%M%S).tar.gz *.cpp *.h Makefile
	@echo "$(GREEN)📦 Backup created!$(NC)"

# Help target
help:
	@echo "$(BLUE)================================$(NC)"
	@echo "$(GREEN)🚀 ROCKET SIMULATOR MAKEFILE HELP$(NC)"
	@echo "$(BLUE)================================$(NC)"
	@echo "$(YELLOW)Available targets:$(NC)"
	@echo "  $(GREEN)all$(NC) or $(GREEN)release$(NC) - Build release version (default)"
	@echo "  $(GREEN)debug$(NC)           - Build with debug symbols"
	@echo "  $(GREEN)clean$(NC)           - Remove all build files"
	@echo "  $(GREEN)run$(NC)             - Build and run release version"
	@echo "  $(GREEN)debug-run$(NC)       - Build and run debug version"
	@echo "  $(GREEN)br$(NC)              - Clean, build, and run (quick)"
	@echo "  $(GREEN)list$(NC)            - List all source files"
	@echo "  $(GREEN)info$(NC)            - Display project information"
	@echo "  $(GREEN)backup$(NC)          - Create a backup tarball"
	@echo "  $(GREEN)help$(NC)            - Show this help message"
	@echo "$(BLUE)================================$(NC)"

# =============================================
# PHONY TARGETS
# =============================================
.PHONY: all release debug clean run debug-run br list info backup help