CXX = gcc
CXXFLAGS = -I$(INCLUDES)
TARGET = server_example
SRCS = ../api_files/Server/server.c ../api_files/Server/gcoap/gcoap_driver.c server_example.c
OBJS = $(SRCS:.c=.o)
INCLUDES = .

.PHONY: all clean

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
.c.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@
clean:
	$(RM) $(OBJS) $(TARGET)
