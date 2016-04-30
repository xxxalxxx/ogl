#ifndef ALM_TIMER_H
#define ALM_TIMER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Timer
{
public:
    Timer();
    float tick();
    

private:
    float mStartTime, mCurrTime, mPrevTime;
};

inline Timer::Timer(): mStartTime(glfwGetTime()), mCurrTime(0.0f), mPrevTime(0.0f)
{
    
}

inline float Timer::tick()
{
    float now = glfwGetTime();
    float dt = now - mPrevTime;
    mPrevTime = now;
    return dt;
}

#endif
