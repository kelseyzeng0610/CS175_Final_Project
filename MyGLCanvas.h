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
#include "Camera.h"
#include "Shape.h"
#include "Sphere.h"
#include "Cone.h"
#include "Cube.h"
#include "Cylinder.h"
// #include "scene/SceneParser.h"
// #include "gfxDefs.h"



struct ObjectNode {
   Shape *primitive;
   glm::vec3 translate;
   glm::vec3 oldCenter;
};
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
	std::vector<ply*> scenePLYObjects;
	void addPLY(const std::string& plyFile);

	int   smooth;
    int   fill;
    int   normal;

	float pixelWidth;
	float pixelHeight;
	float scale;
	OBJ_TYPE objType;

	int isectOnly;
	int maxRecursionDepth;
	Sphere* sphere;
	Cube* cube;
	Cone* cone;
	Cylinder* cylinder;
	Shape* shape;
	int segmentsX, segmentsY;

	// used in flattened list to make update values
	std::vector<ObjectNode> objectList;
	// glm::vec3 curr_pos;
	// int curr_idx;


	// SceneParser* parser;
	// GLubyte* pixels = NULL;


MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	void resetScene();
	void setShape(OBJ_TYPE type);
	void drawShape(OBJ_TYPE type);
	void drawScene();
	// void drawObject(ObjectNode node, glm::mat4 trans);
	void drawSphere();
	void drawCube();
	void drawCylinder();
	void drawCone();
	void setSegments();

private:
	void draw();

	void initShaders();

	int handle(int);
	void resize(int x, int y, int w, int h);
	void updateCamera(int width, int height);

	SceneObject* myObject;
	Camera camera;
	bool castRay;
	bool drag;
	glm::vec3 oldCenter;
	glm::vec3 oldIsectPoint;
	float oldT;

	//// Used for intersection
	glm::vec3 spherePosition;

	int mouseX = 0;
	int mouseY = 0;

	std::vector<double> intersectWithSphere(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	std::vector<double> intersectWithCube(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	std::vector<double> intersectWithCylinder(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	std::vector<double> intersectWithCone(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	// void storeObjects(SceneNode* node, glm::mat4 parent_transform);
	// void setpixel(GLubyte* buf, int x, int y, int r, int g, int b);
	// void getClosestT();
	std::pair<ObjectNode, int> closestObject(glm::vec3 eye_pnt, int mouseX, int mouseY);
	void objectIterator();
	void drawObject();

	bool firstTime;
};

#endif // !MYGLCANVAS_H