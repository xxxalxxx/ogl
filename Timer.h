#ifndef ALM_TIMER_H
#define ALM_TIMER_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Timer
{
public:
    Timer();
    float tick();
    float getCurrentTime() const;
    float getTotalTime() const;
    

private:
    float mStartTime, mCurrTime, mPrevTime, mTotalTime;
};

inline Timer::Timer(): mStartTime(glfwGetTime()), mCurrTime(0.0f), mPrevTime(0.0f), mTotalTime(0.0f)
{
    
}

inline float Timer::getCurrentTime() const
{
    return mCurrTime;
}

inline float Timer::getTotalTime() const
{
    return mTotalTime;
}

inline float Timer::tick()
{
    mCurrTime = glfwGetTime();
    float dt = mCurrTime - mPrevTime;
    mPrevTime = mCurrTime;
    
    mTotalTime = mCurrTime - mStartTime;

    return dt;
}

#endif
