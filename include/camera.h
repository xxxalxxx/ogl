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

#include "utils.h"

#define INITIAL_NEAR_PLANE 0.1f
#define INITIAL_FAR_PLANE 1000.0f
#define INITIAL_FOV_RADIANS ((float)M_PI_4)
#define INITIAL_FORWARD_MOVEMENT_SCALAR 20.0f
#define INITIAL_SIDEWAYS_MOVEMENT_SCALAR 20.0f
#define INITIAL_SCROLL_SENSITIVITY_SCALAR 0.05f
#define INITIAL_MOUSE_SENSITIVITY_SCALAR 0.003f

class Camera
{
public:
    Camera(float aspect);
    void updateOrientation(double mouseX, double mouseY);
    void updateFOV(double scrollDirection);
    void rebuildView();
    void rebuildPerspective();

    void setFOV(float FOV);//radians
    void setNear(float near);
    void setFar(float far);
    void setAspect(float aspect);

    glm::mat4& getProj();
    glm::mat4& getView();

    glm::vec3& getEye();
    
    float getFOV();

    void moveStraight(float direction, float dt);
    void moveSideways(float direction, float dt);

private:
    float mNear, mFar, mFOV, mAspect;
    float mPitch, mYaw;
    float mForwardMovementScalar, mSidewaysMovementScalar, mMouseSensitivityScalar, mScrollSensitivityScalar;
    double mPrevMouseX, mPrevMouseY;
    glm::vec3 mEye; 
    const static glm::vec3 UP, RIGHT;
    const static float MAX_FOV_RADIANS, MIN_FOV_RADIANS;

    glm::mat4 mProj, mView; 
 
};

inline Camera::Camera(float aspect): 
mNear(INITIAL_NEAR_PLANE), mFar(INITIAL_FAR_PLANE), 
mFOV( 0.5f * (MAX_FOV_RADIANS + MIN_FOV_RADIANS) ), mAspect(aspect), 
mPitch(0.0f), mYaw(0.0f),
mForwardMovementScalar(INITIAL_FORWARD_MOVEMENT_SCALAR), 
mSidewaysMovementScalar(INITIAL_SIDEWAYS_MOVEMENT_SCALAR), 
mMouseSensitivityScalar(INITIAL_MOUSE_SENSITIVITY_SCALAR),
mScrollSensitivityScalar(INITIAL_SCROLL_SENSITIVITY_SCALAR),
mEye(0.0f, 0.0f, 4.0f) 
{
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

inline glm::vec3& Camera::getEye()
{
    return mEye;
}

inline void Camera::setAspect(float aspect)
{
    mAspect = aspect;
}

#endif
