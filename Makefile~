BIN_DIR := bin
INC_DIR := include
SRC_DIR := src
OBJ_DIR := obj

TARGET=$(BIN_DIR)/myengine
SRCS=main.cpp texture_manager.cpp file_system.cpp window.cpp engine.cpp camera.cpp shader.cpp quad.cpp technique.cpp model_technique.cpp skinned_model_technique.cpp skinned_model.cpp model.cpp anim_node.cpp mesh.cpp light.cpp g_buffer.cpp ssao.cpp sphere.cpp cone.cpp skybox.cpp heightmap.cpp 
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS))
VPATH=include:src

CXX=g++
CXXFLAGS=-Wall -Werror -std=c++11 -I. -Isrc -Iinclude
RM=rm -f
LDLIBS=$(shell pkg-config --libs glfw3) -lm -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -ldl -pthread -lassimp

all: clean $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS) 

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(LDLIBS) 

.PHONY : clean

clean:
	$(RM) $(OBJ_DIR)/*.o $(TARGET)
