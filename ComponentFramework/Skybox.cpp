#include "Skybox.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Mesh.h"
#include "Shader.h"

using std::cout;
using std::endl;

Skybox::Skybox(const char* negx_, const char* negy_, const char* negz_, const char* posx_, const char* posy_, const char* posz_): 
	negx(negx_),
	negy(negy_),
	negz(negz_),
	posx(posx_),
	posy(posy_),
	posz(posz_),
	textureID(),
	cube()
{	
	//LoadImages();
}
Skybox::~Skybox() {}

// Load images
bool Skybox::LoadImages() {
	//return false;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	/*******************************************************************/
	/**************************** NEGATIVE X ***************************/
	/*******************************************************************/
	{
		SDL_Surface* textureSurface = IMG_Load(negx);
		if (textureSurface == nullptr) return false;

		int mode = (SDL_GetPixelFormatDetails(textureSurface->format)->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, mode, textureSurface->w,
			textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels
		);

		SDL_DestroySurface(textureSurface);
	}

	/*******************************************************************/
	/**************************** NEGATIVE Y ***************************/
	/*******************************************************************/
	{
		SDL_Surface* textureSurface = IMG_Load(negy);
		if (textureSurface == nullptr) return false;

		int mode = (SDL_GetPixelFormatDetails(textureSurface->format)->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, mode, textureSurface->w,
			textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels
		);

		SDL_DestroySurface(textureSurface);
	}

	/*******************************************************************/
	/**************************** NEGATIVE Z ***************************/
	/*******************************************************************/
	{
		SDL_Surface* textureSurface = IMG_Load(negz);
		if (textureSurface == nullptr) return false;

		int mode = (SDL_GetPixelFormatDetails(textureSurface->format)->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, mode, textureSurface->w,
			textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels
		);

		SDL_DestroySurface(textureSurface);
	}

	/*******************************************************************/
	/**************************** POSITIVE X ***************************/
	/*******************************************************************/
	{
		SDL_Surface* textureSurface = IMG_Load(posx);
		if (textureSurface == nullptr) return false;

		int mode = (SDL_GetPixelFormatDetails(textureSurface->format)->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, mode, textureSurface->w,
			textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels
		);

		SDL_DestroySurface(textureSurface);
	}

	/*******************************************************************/
	/**************************** POSITIVE Y ***************************/
	/*******************************************************************/
	{
		SDL_Surface* textureSurface = IMG_Load(posx);
		if (textureSurface == nullptr) return false;

		int mode = (SDL_GetPixelFormatDetails(textureSurface->format)->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, mode, textureSurface->w,
			textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels
		);

		SDL_DestroySurface(textureSurface);
	}

	/*******************************************************************/
	/**************************** POSITIVE Z ***************************/
	/*******************************************************************/
	{
		SDL_Surface* textureSurface = IMG_Load(posz);
		if (textureSurface == nullptr) return false;

		int mode = (SDL_GetPixelFormatDetails(textureSurface->format)->bytes_per_pixel == 4) ? GL_RGBA : GL_RGB;
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, mode, textureSurface->w,
			textureSurface->h, 0, mode, GL_UNSIGNED_BYTE, textureSurface->pixels
		);

		SDL_DestroySurface(textureSurface);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0); // unbind texture

	// Mesh
	cube = new Mesh("meshes/Cube.obj");
	if (!cube->OnCreate()) {
		cout << "Mesh loading error" << endl;
		return false;
	};

	// Shader
	shader = new Shader("shaders/skyboxVert.glsl", "shaders/skyboxFrag.glsl");
	if (!shader->OnCreate()) {
		cout << "Shader loading error" << endl;
		return false;
	};

	return true;
}

void Skybox::Render() const {
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	cube->Render();
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

}