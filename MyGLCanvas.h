#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#if defined(__APPLE__)
#  include <OpenGL/gl3.h> // defines OpenGL 3.0+ functions
#else
#  if defined(WIN32)
#    define GLEW_STATIC 1
#  endif
#  include <GL/glew.h>
#endif
#include <FL/glut.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <time.h>
#include <iostream>

#include "SceneObject.h"
#include "ShaderManager.h"
#include "ply.h"
#include "gfxDefs.h"
#include "Camera.h"

class ShaderManager;
class ply;


class MyGLCanvas : public Fl_Gl_Window {
public:

	// Length of our spline (i.e how many points do we randomly generate)


	glm::vec3 eyePosition;
	glm::vec3 rotVec;
	glm::vec3 lookatPoint;
	glm::vec3 lightPos;

	int viewAngle;
	int animateLight;
	float clipNear;
	float clipFar;
	float scaleFactor;
	float pixelWidth;
	float pixelHeight;

	std::vector<ply*> scenePLYObjects;
	void addPLY(const std::string& plyFile);
	MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	

	void reloadShaders();

private:
	glm::vec3 generateRay(int pixelX, int pixelY);
	glm::vec3 getEyePoint(int pixelX, int pixelY, int screenWidth, int screenHeight);
	glm::vec3 getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t);
	double intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);

	void draw();
	void drawScene();

	void initShaders();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);

	SceneObject* myObject;
	ShaderManager* myShaderManager;
	ply* myPLY;
	

	bool firstTime;
	Camera camera;
	glm::vec3 spherePosition;
	bool castRay;
	int mouseX = 0;
	int mouseY = 0;
	glm::vec3 oldCenter;
	glm::vec3 oldIsectPoint;
	float oldT;
	bool drag;

};

#endif // !MYGLCANVAS_H