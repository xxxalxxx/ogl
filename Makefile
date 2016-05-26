BIN_DIR := bin
TARGET= $(BIN_DIR)/myengine
CXX=g++
CXXFLAGS=-O2 -Wall -Werror -std=c++11 -I. -Isrc -Iinclude
RM=rm -f
LDLIBS=$(shell pkg-config --libs glfw3) -lm -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -ldl -pthread -lSOIL -lassimp -lIL
INC_DIR := include
SRC_DIR := src
SRCS=Main.cpp Window.cpp Engine.cpp Camera.cpp Shader.cpp Quad.cpp
OBJ_DIR := obj
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS))
VPATH =include:src

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDLIBS) 

clean:
	$(RM) $(OBJ_DIR)/*.o $(TARGET)
