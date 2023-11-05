CXX = gcc
CXXFLAGS = -I$(INCLUDES)
TARGET = example
SRCS = ../api_files/Server/server.c server_example.c
OBJS = $(SRCS:.c=.o)
INCLUDES = .

.PHONY: all clean

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
$(OBJS): $(SRCS)
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	$(RM) $(OBJS) $(TARGET)
