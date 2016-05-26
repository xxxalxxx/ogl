TARGET=myengine
CXX=g++
CXXFLAGS=-O2 -Wall -Werror -std=c++11 
RM=rm -f
LDLIBS=$(shell pkg-config --libs glfw3) -lm -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -ldl -pthread -lSOIL -lassimp -lIL 
SRCS=Main.cpp Window.cpp Engine.cpp Camera.cpp Shader.cpp Quad.cpp
OBJ_DIR := obj
OBJS=$(patsubst %.cpp,$(OBJ_DIR)/%.o, $(SRCS))

$(TARGET) : $(OBJS)

$(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $? -o $@ $(LDLIBS) 

