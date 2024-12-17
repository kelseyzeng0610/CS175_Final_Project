#pragma once

#ifndef MYGLCANVAS_H
#define MYGLCANVAS_H

#include <FL/gl.h>
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


#define SPLINE_SIZE 100
#define COASTER_SPEED 0.0001


struct ObjectNode {
    int id;
    glm::vec3 translate, scale, rotation;
    int red, green, blue;
    Shape* primitive;

    ObjectNode* parent; // Pointer to parent node
    std::vector<ObjectNode*> children;

    ObjectNode() : parent(nullptr), primitive(nullptr) {}
};


class MyGLCanvas : public Fl_Gl_Window {
public:
    std::function<void()> onSelectionChanged; // callback for when selection changes

	// Length of our spline (i.e how many points do we randomly generate)


	glm::vec3 eyePosition;
	glm::vec3 rotVec;
	glm::vec3 lookatPoint;
	// glm::vec3 lookVector;
	// glm::vec3 upVector;

	int wireframe;
	int  viewAngle;
	float clipNear;
	float clipFar;
	Camera camera;
	float camera_near;
	float camera_far;

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
	int selectedObjId;
	int nextObjectId;
	int fileIndex;

	// used in flattened list to make update values
	std::vector<ObjectNode> objectList;


MyGLCanvas(int x, int y, int w, int h, const char *l = 0);
	~MyGLCanvas();
	void resetScene();
	void setShape(OBJ_TYPE type, bool isChild = false);
	void addObject(OBJ_TYPE type, ObjectNode* parent = nullptr);

	// void drawObject(ObjectNode node, glm::mat4 trans);
	void drawSphere();
	void drawCube();
	void drawCylinder();
	void drawCone();
	void setSegments();
	int handle(int e) override;
	void drawObjects();
	void setupCamera();
	int selectObject(int mouseX, int mouseY);
	void updateCamera(int width, int height);

	void drawNode(ObjectNode* node, glm::mat4 parent_transform);




	

private:
	glm::vec3 generateRay(int pixelX, int pixelY);
	glm::vec3 getEyePoint(int pixelX, int pixelY, int screenWidth, int screenHeight);
	glm::vec3 getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t);
	double intersect(ObjectNode& obj, glm::vec3 eyePointP, glm::vec3 rayV);

	void draw();

	void drawAxis();
	void drawGrid();
	

	
	void resize(int x, int y, int w, int h);

	SceneObject* myObject;
	bool castRay;
	bool drag;
	glm::vec3 oldCenter;
	glm::vec3 oldIsectPoint;
	float oldT;

	//// Used for intersection
	glm::vec3 spherePosition;

	int mouseX = 0;
	int mouseY = 0;
	void testObjectIntersection(ObjectNode* node, glm::mat4 parentTransform, glm::vec3 eyePoint, glm::vec3 rayDir, const ObjectNode*& closestObj, float& closestT);
	std::vector<double> intersectWithSphere(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	std::vector<double> intersectWithCube(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	std::vector<double> intersectWithCylinder(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	std::vector<double> intersectWithCone(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix);
	// void storeObjects(SceneNode* node, glm::mat4 parent_transform);
	// void setpixel(GLubyte* buf, int x, int y, int r, int g, int b);
	// void getClosestT();
	// void renderEnv();

};

#endif // !MYGLCANVAS_H