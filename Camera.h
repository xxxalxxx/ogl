#ifndef ALM_CAMERA_H
#define ALM_CAMERA_H
#include <math.h>
#include <algorithm>
#include <iostream>
#include "glm/vec3.hpp"
#include "glm/vec4.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/quaternion.hpp"


#define INITIAL_NEAR_PLANE 0.1f
#define INITIAL_FAR_PLANE 100.0f
#define INITIAL_FOV_RADIANS ((float)M_PI_4)
#define INITIAL_FORWARD_MOVEMENT_SCALAR 0.1f
#define INITIAL_SIDEWAYS_MOVEMENT_SCALAR 0.1f

#define INITIAL_MOUSE_SENSITIVITY_SCALAR 0.05f;

#define MAX_FOV_RADIANS ((float)M_PI)
#define MIN_FOV_RADIANS 0.0f

#define UP glm::vec3(0.0f,1.0f,0.0f)
#define RIGHT glm::vec3(1.0f,0.0f,0.0f)

class Camera
{
public:
    Camera(float aspect);
    void setOrientaion();
    void updateOrientation(double mouseX, double mouseY);

    void rebuildView();
    void rebuildPerspective();

    void setFOV(float FOV);//radians
    void setNear(float near);
    void setFar(float far);
    void setAspect(float aspect);

    glm::mat4& getProj();
    glm::mat4& getView();
    
     
    float getFOV();

private:

    float mNear, mFar, mFOV, mAspect;
    float mPitch, mYaw;
    float mForwardMovementScalar, mSidewaysMovementScalar, mMouseSensitivityScalar;
    double mPrevMouseX, mPrevMouseY;
    glm::vec3 mEye, mAt, mUp, mRight, mForward;
    glm::mat4 mProj, mView;
    glm::mat4 mOrientaion;
    glm::mat4 mTranslation;
 
};

inline Camera::Camera(float aspect): mNear(INITIAL_NEAR_PLANE), mFar(INITIAL_FAR_PLANE), mFOV(INITIAL_FOV_RADIANS), mAspect(aspect), mPitch(0.0f), mYaw(0.0f), mEye(0.0f, 0.0f,-4.0f), mAt(0.0f,0.0f,0.0f), mUp(UP), mRight(RIGHT)
{ 
    mForward = mAt - mEye;
    mTranslation = glm::translate(glm::mat4(1.0f), mEye);
    rebuildView();
    rebuildPerspective();
}

inline float Camera::getFOV()
{
    return mFOV;
}

inline glm::mat4& Camera::getProj()
{
    return mProj;
}

inline glm::mat4& Camera::getView()
{
    return mView;
}

inline void Camera::setAspect(float aspect)
{
    mAspect = aspect;
}

#endif
