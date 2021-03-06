#include "camera.h"

const glm::vec3 Camera::UP = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Camera::RIGHT = glm::vec3(1.0f, 0.0f, 0.0f);

const float Camera::MAX_FOV_RADIANS = M_PI_2;
const float Camera::MIN_FOV_RADIANS = M_PI_4;


void Camera::updateOrientation(double mouseX, double mouseY)
{
    double mouseDx = mMouseSensitivityScalar * (mouseX - mPrevMouseX);
    double mouseDy = mMouseSensitivityScalar * (mouseY - mPrevMouseY);

    mYaw -= mouseDx;//invert angle
    mPrevMouseX = mouseX;

    float newPitch = mPitch + mouseDy;
    
    if(fabs(newPitch) < M_PI_2)
    {
        mPitch = newPitch;
        mPrevMouseY = mouseY;
    }
    
    glm::quat orientation = glm::angleAxis(mPitch, RIGHT) 
                          * glm::angleAxis(mYaw, UP);
    mView = glm::mat4_cast(orientation);

    rebuildView();
}

void Camera::updateFOV(double scrollDirection)
{
    setFOV(mFOV + scrollDirection * mScrollSensitivityScalar);     
}

void Camera::rebuildPerspective()
{
    mProj = glm::perspective(mFOV, mAspect, mNear, mFar);
}

void Camera::rebuildView()
{   
    float offsetX = mView[0].x * mEye.x + mView[1].x * mEye.y + mView[2].x * mEye.z;
    float offsetY = mView[0].y * mEye.x + mView[1].y * mEye.y + mView[2].y * mEye.z;
    float offsetZ = mView[0].z * mEye.x + mView[1].z * mEye.y + mView[2].z * mEye.z;

    mView[3].x = -offsetX;
    mView[3].y = -offsetY;
    mView[3].z = -offsetZ;  

}


void Camera::moveStraight(float direction, float dt)
{
    float c = direction * mForwardMovementScalar * dt;
    //scale in direction of look vector
    mEye -= c * glm::vec3(mView[0].z, mView[1].z, mView[2].z); 
    rebuildView();
}


void Camera::moveSideways(float direction, float dt)
{  
    float c = direction * mSidewaysMovementScalar * dt; 
    //scale in direction of right vector
    mEye += c * glm::vec3(mView[0].x, mView[1].x, mView[2].x);
    rebuildView();
}

void Camera::setFOV(float FOV)
{   
    std::cout << "FOV " << FOV << std::endl; 
    mFOV = FOV;
    Utils::clamp(mFOV, MIN_FOV_RADIANS, MAX_FOV_RADIANS);
}

void Camera::setFar(float far)
{
    mFar = far < 0.0f ? 0.0f : far;
     
    if(mNear > mFar)
        std::swap(mNear, mFar);
}

void Camera::setNear(float near)
{
    mNear = near < 0.0f ? 0.0f : near;
    
    if(mNear > mFar)
        std::swap(mNear, mFar);
}

