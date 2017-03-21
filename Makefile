SOURCES=$(shell find . -name "*.cpp")
OBJECTS=$(SOURCES:%.cpp=%.o)
TARGET=portalengine
#CXXFLAGS= -Wall -pedantic -std=c++11
CXXFLAGS= -w -std=c++11
LDLIBS=-lGL -lGLEW -lglfw -lSOIL

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(LINK.cpp) $^ $(LOADLIBES) $(LDLIBS) -o $@

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJECTS)
	rm *~

test: all
	./portalengine
