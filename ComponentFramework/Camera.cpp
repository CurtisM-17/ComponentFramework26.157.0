#include "Camera.h"
#include <MMath.h>

Camera::Camera()
{
	// Setting up field of view angle, aspect ratio, near & far z planes
	projectionMatrix = MMath::perspective(45, 16.0f / 9.0f, 0.5f, 100.0f);
}

Camera::~Camera()
{
}

Matrix4 Camera::GetViewMatrix() const
{
	Matrix4 oppositeTranslation = MMath::translate(-pos);
	// Rotate the other way
	Matrix4 oppositeRotation = MMath::toMatrix4(QMath::inverse(orientation));

	return oppositeRotation * oppositeTranslation;
}
