#include "Camera.h"

void Camera::updateOrientation(double mouseX, double mouseY)
{
    mMouseSensitivityScalar = 0.025;
    double mouseDx = mMouseSensitivityScalar*(mouseX - mPrevMouseX);
    double mouseDy = mMouseSensitivityScalar*(mouseY - mPrevMouseY);

    mYaw -= mouseDx;//invert angle
    mPitch += mouseDy;

    glm::quat orientation = glm::angleAxis(mPitch, RIGHT);
    orientation *= glm::angleAxis(mYaw, UP);
    mOrientaion = glm::mat4_cast(orientation);

    mPrevMouseX = mouseX;
    mPrevMouseY = mouseY;
    
 //   std::cout<< "pitch:" <<mPitch << " yaw:" << mYaw <<" " << std::endl;

}

void Camera::rebuildPerspective()
{
    mProj = glm::perspective(mFOV, mAspect, mNear, mFar);
}

void Camera::rebuildView()
{   
    mView = mOrientaion;
   
    float offsetX = mOrientaion[0].x * mEye.x + mOrientaion[1].x * mEye.y + mOrientaion[2].x * mEye.z;
    float offsetY = mOrientaion[0].y * mEye.x + mOrientaion[1].y * mEye.y + mOrientaion[2].y * mEye.z;
    float offsetZ = mOrientaion[0].z * mEye.x + mOrientaion[1].z * mEye.y + mOrientaion[2].z * mEye.z;

    mView[3].x = offsetX;
    mView[3].y = offsetY;
    mView[3].z = offsetZ;  
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

void Camera::setNear(float near)
{
    if(near < 0.0f)
        mNear = 0.0f;
    else 
        mNear = near;
    
    if(mNear > mFar)
        std::swap(mNear, mFar);
}


