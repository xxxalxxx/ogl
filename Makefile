BIN_DIR := bin
INC_DIR := include
SRC_DIR := src
OBJ_DIR := obj

TARGET=$(BIN_DIR)/myengine
SRCS=Main.cpp FileSystem.cpp TextureManager.cpp Window.cpp Engine.cpp Camera.cpp Shader.cpp Quad.cpp ModelTechnique.cpp SkinnedModelTechnique.cpp
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS))
VPATH=include:src

CXX=g++
CXXFLAGS=-O2 -Wall -Werror -std=c++11 -I. -Isrc -Iinclude
RM=rm -f
LDLIBS=$(shell pkg-config --libs glfw3) -lm -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -ldl -pthread -lSOIL -lassimp -lIL

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDLIBS) 

.PHONY : clean

clean:
	$(RM) $(OBJ_DIR)/*.o $(TARGET)
