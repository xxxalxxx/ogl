TARGET=myengine
CXX=g++
CXXFLAGS=-Wall -Werror -std=c++11 -ISOIL -Iglm
RM=rm -f
LDLIBS=$(shell pkg-config --libs glfw3) -lm -lGLEW -lglfw3 -lGL -lX11 -lXi -lXrandr -lXxf86vm -lXinerama -lXcursor -lrt -lm -ldl -pthread -lSOIL -lassimp 
SRCS=main.cpp Window.cpp Engine.cpp Camera.cpp Shader.cpp Quad.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $? -o $@ $(LDLIBS) 

clean:
	$(RM) $(OBJS) $(TARGET)
