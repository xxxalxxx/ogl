#include "Camera.h"



void Camera::rebuildPerspective()
{
    mProj = glm::perspective(mFOV, mAspect, mNear, mFar);
}

void Camera::rebuildView()
{
    mView = glm::lookAt(mEye, mAt, UP);
}

void Camera::setFOV(float FOV)
{    
    if(FOV < MIN_FOV_RADIANS)
        mFOV = MIN_FOV_RADIANS;
    else if(FOV > MAX_FOV_RADIANS) 
        mFOV = MAX_FOV_RADIANS;
    else 
         mFOV = FOV;
}

void Camera::setFar(float far)
{
    if(far < 0.0f)
        mFar = 0.0f;
    else 
        mFar = far;
     
    if(mNear > mFar)
        std::swap(mNear, mFar);
}

}

void Camera::setNear(float near)
{
    if(near < 0.0f)
        mNear = 0.0f;
    else 
        mNear = near;
    
    if(mNear > mFar)
        std::swap(mNear, mFar);
}


