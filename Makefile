EXECUTABLE = dualheap

CC = g++
CFLAGS = -g -Wall

SRCDIR := src
TARGETDIR := target
TARGET := $(TARGETDIR)/bin/$(EXECUTABLE)

SOURCES := $(shell find $(SRCDIR) -name "*.cpp")
OBJECTS := $(patsubst $(SRCDIR)/%,$(TARGETDIR)/%,$(SOURCES:.cpp=.o))
INCLUDES := -Iinclude

exe: $(TARGET)
	@$(TARGET) $(filter-out $@, $(MAKECMDGOALS))

$(TARGET): $(OBJECTS)
	@mkdir -p $(shell dirname $(TARGET))
	@echo "Compiling $(TARGET)..."
	@$(CC) $(CFLAGS) $^ -o $(TARGET)

$(TARGETDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(TARGETDIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

test:
	@echo "TODO"

doc:
	@echo "TODO"

clean:
	rm -r $(TARGETDIR)

.PHONY: clean

%:
	@true
