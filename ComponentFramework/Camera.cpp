#include "Camera.h"
#include <MMath.h>

Camera::Camera():
	skybox(nullptr)
{
	// Setting up field of view angle, aspect ratio, near & far z planes
	projectionMatrix = MMath::perspective(45, 16.0f / 9.0f, 0.5f, 100.0f);

	skybox = new Skybox(
		"textures/cn_tower/negx.jpg",
		"textures/cn_tower/negy.jpg",
		"textures/cn_tower/negz.jpg",
		"textures/cn_tower/posx.jpg",
		"textures/cn_tower/posy.jpg",
		"textures/cn_tower/posz.jpg"
	);

	skybox->LoadImages();
}

Camera::~Camera() {
}

Matrix4 Camera::GetViewMatrix() const {
	Matrix4 oppositeTranslation = MMath::translate(-pos);
	// Rotate the other way
	Matrix4 oppositeRotation = MMath::toMatrix4(QMath::inverse(orientation));

	return oppositeRotation * oppositeTranslation;
}

void Camera::Render() const {
	if(skybox == nullptr) return;

	Shader* shader = skybox->GetShader();

	glUseProgram(shader->GetProgram());
	glUniformMatrix4fv(shader->GetUniformID("orientationMatrix"), 1, GL_FALSE, MMath::toMatrix4(orientation));
	glUniformMatrix4fv(shader->GetUniformID("projectionMatrix"), 1, GL_FALSE, projectionMatrix);

	skybox->Render();
	glUseProgram(0);
}