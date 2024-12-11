#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ShaderManager.h"



MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l) {
	mode(FL_OPENGL3 | FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);
	
	eyePosition = glm::vec3(0.0f, 0.0f, 3.0f);
	lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
	lightPos = eyePosition;
	
	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 10.0f;
	scaleFactor = 1.0f;
	animateLight = 1.0;

	firstTime = true;

	myObject = new SceneObject(175);
	myShaderManager = new ShaderManager();

	pixelWidth = w;
	pixelHeight = h;
	spherePosition = glm::vec3(0, 0, 0);
	mouseX = 0;
	mouseY = 0;
	castRay = false;
	drag = false;	
}

MyGLCanvas::~MyGLCanvas() {
    for(auto& obj : scenePLYObjects) {
        delete obj;
    }
    scenePLYObjects.clear();

    delete myShaderManager;
}

void MyGLCanvas::addPLY(const std::string& plyFile) {
	ply* newPLY = new ply(plyFile);
	
	newPLY->buildArrays();
	newPLY->bindVBO(myShaderManager->program);
	scenePLYObjects.push_back(newPLY);
}

void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid()) {  //this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
		if (firstTime == true) {
			firstTime = false;
			initShaders();
		}
	}

	// Clear the buffer of colors in each bit plane.
	// bit plane - A set of bits that are on or off (Think of a black and white image)
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	drawScene();
}

glm::vec3 MyGLCanvas::generateRay(int pixelX, int pixelY)
{
	glm::vec3 eyePos = camera.getEyePoint();

	glm::vec3 farPoint = getEyePoint(pixelX, pixelY, pixelWidth, pixelHeight);
	return glm::normalize(farPoint - eyePos);
}

glm::vec3 MyGLCanvas::getEyePoint(int pixelX, int pixelY, int screenWidth, int screenHeight)
{
	// Convert pixel coordinates to normalized device coordinates (-1 to 1)
	float ndcX = (2.0f * pixelX / screenWidth) - 1.0f;
	float ndcY = 1.0f - (2.0f * pixelY / screenHeight);

	// Scale based on camera's view angle and aspect ratio
	float aspectRatio = (float)screenWidth / screenHeight;
	float filmPlanDepth = camera.getFilmPlanDepth();

	// Create point in camera space (using film plane depth from camera)
	glm::vec4 cameraPoint(ndcX * aspectRatio, ndcY, -filmPlanDepth, 1.0f);

	// Transform from camera space to world space using inverse view matrix
	glm::mat4 viewToWorld = camera.getInverseModelViewMatrix();
	glm::vec4 worldPoint = viewToWorld * cameraPoint;

	return glm::vec3(worldPoint);
}

glm::vec3 MyGLCanvas::getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t)
{
	return eye + ray * t;
}

double MyGLCanvas::intersect(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{
	// transform eye point and ray into object space
	glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 0.0f);
	glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f);

	// transform sphere center
	glm::vec4 sphereCenterO = transformMatrix * glm::vec4(spherePosition, 0.0f);

	// adjust eye point relative to sphere center
	glm::vec4 relativeEyePoint = eyePointPO - sphereCenterO;

	double A = glm::dot(rayVO, rayVO);
	double B = 2.0 * glm::dot(rayVO, relativeEyePoint);
	double C = glm::dot(relativeEyePoint, relativeEyePoint) - 0.25;

	double discriminant = pow(B, 2) - 4.0 * A * C;

	// no intersection
	if (discriminant < 0)
		return -1.0;

	double t1 = (-B - sqrt(discriminant)) / (2.0 * A);
	double t2 = (-B + sqrt(discriminant)) / (2.0 * A);

	// return nearest intersection
	if (t1 > 0 && t2 > 0)
	{
		return std::min(t1, t2);
	}
	else if (t1 > 0)
	{
		return t1;
	}
	else if (t2 > 0)
	{
		return t2;
	}

	return -1.0;
}

void MyGLCanvas::drawScene() {
	//setting up camera info
	glm::mat4 modelViewMatrix = glm::lookAt(eyePosition, lookatPoint, glm::vec3(0.0f, 1.0f, 0.0f));
	modelViewMatrix = glm::rotate(modelViewMatrix, TO_RADIANS(rotVec.x), glm::vec3(1.0f, 0.0f, 0.0f));
	modelViewMatrix = glm::rotate(modelViewMatrix, TO_RADIANS(rotVec.y), glm::vec3(0.0f, 1.0f, 0.0f));
	modelViewMatrix = glm::rotate(modelViewMatrix, TO_RADIANS(rotVec.z), glm::vec3(0.0f, 0.0f, 1.0f));
	modelViewMatrix = glm::scale(modelViewMatrix, glm::vec3(scaleFactor, scaleFactor, scaleFactor));
	//SHADER: passing the projection matrix to the shader... the projection matrix will be called myProjectionMatrix in shader
	glUniformMatrix4fv(glGetUniformLocation(myShaderManager->program, "myModelviewMatrix"), 1, false, glm::value_ptr(modelViewMatrix));

	if (animateLight == 1) {
		lightPos.x += 0.05f;
		lightPos.y = 0.5f;
		lightPos.z += 0.05f;
	}

	glm::vec3 rotateLightPos = glm::vec3(sin(lightPos.x), lightPos.y, cos(lightPos.z));
	//TODO: pass lighting information to the shaders
	GLuint lightPosLoc = glGetUniformLocation(myShaderManager->program, "lightPosition");
    glUniform3f(lightPosLoc, rotateLightPos.x, rotateLightPos.y, rotateLightPos.z);


	//renders the object
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	for(auto& obj : scenePLYObjects) {
        obj->renderVBO();
    }
}


// void MyGLCanvas::drawScene()
// {
// 	glMatrixMode(GL_MODELVIEW);
// 	// Set the mode so we are modifying our objects.
// 	camera.orientLookVec(eyePosition, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
// 	glLoadMatrixf(glm::value_ptr(camera.getModelViewMatrix()));

// 	if (castRay == true)
// 	{
// 		// std::cout << mouseX << std::endl;
// 		glm::vec3 eyePointP = getEyePoint(mouseX, mouseY, pixelWidth, pixelHeight);
// 		// std::cout << eyePointP.x << std::endl;
// 		// std::cout << eyePointP.y << std::endl;
// 		// std::cout << eyePointP.z << std::endl;
// 		glm::vec3 rayV = generateRay(mouseX, mouseY);	
// 		glm::vec3 sphereTransV(spherePosition[0], spherePosition[1], spherePosition[2]);

// 		float t = intersect(eyePointP, rayV, glm::translate(glm::mat4(1.0), sphereTransV));
// 		glm::vec3 isectPointWorldCoord = getIsectPointWorldCoord(eyePointP, rayV, t);

// 		if (t > 0)
// 		{
// 			glColor3f(1, 0, 0);
// 			glPushMatrix();
// 			glTranslated(spherePosition[0], spherePosition[1], spherePosition[2]);
// 			glutWireCube(1.0f);
// 			glPopMatrix();
// 			glPushMatrix();
// 			glTranslatef(isectPointWorldCoord[0], isectPointWorldCoord[1], isectPointWorldCoord[2]);
// 			glutSolidSphere(0.05f, 10, 10);
// 			glPopMatrix();
// 			printf("hit!\n");
// 		}
// 		else
// 		{
// 			printf("miss!\n");
// 		}
// 	}

// 	glPushMatrix();

// 	// move the sphere to the designated position
// 	glTranslated(spherePosition[0], spherePosition[1], spherePosition[2]);

// 	glDisable(GL_POLYGON_OFFSET_FILL);
// 	glColor3f(1.0, 1.0, 1.0);
// 	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
// 	glPushMatrix();
// 	glRotatef(90, 0, 1, 0);
// 	// drawShape(OBJ_TYPE type);
// 	// myObject->drawTexturedSphere(); //TODO:ADD SHAPES
	
// 	glPopMatrix();

// 	glPopMatrix();
// }


void MyGLCanvas::updateCamera(int width, int height) {
	float xy_aspect;
	xy_aspect = (float)width / (float)height;

	//SHADER: passing the projection matrix to the shader... the projection matrix will be called myProjectionMatrix in shader
	glm::mat4 perspectiveMatrix;
	perspectiveMatrix = glm::perspective(TO_RADIANS(viewAngle), xy_aspect, clipNear, clipFar);
	glUniformMatrix4fv(glGetUniformLocation(myShaderManager->program, "myProjectionMatrix"), 1, false, glm::value_ptr(perspectiveMatrix));
}


int MyGLCanvas::handle(int e) {
	//static int first = 1;
#ifndef __APPLE__
	if (firstTime && e == FL_SHOW && shown()) {
		firstTime = 0;
		make_current();
		GLenum err = glewInit(); // defines pters to functions of OpenGL V 1.2 and above
		if (GLEW_OK != err)	{
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
		else {
			//SHADER: initialize the shader manager and loads the two shader programs
			initShaders();
		}
	}
#endif	
	//printf("Event was %s (%d)\n", fl_eventnames[e], e);
	switch (e) {
	case FL_DRAG:
		mouseX = (int)Fl::event_x();
		mouseY = (int)Fl::event_y();

		if (drag == true) {
			// get new ray from current mouse position
			glm::vec3 eyePoint = getEyePoint(mouseX, mouseY, pixelWidth, pixelHeight);
			glm::vec3 rayDir = generateRay(mouseX, mouseY);
			
			// use the same t-value along the new ray to get new intersection point
			glm::vec3 new_intersection = eyePoint + (rayDir * oldT);
			
			// move sphere by same offset to maintain relative position to intersection point
			glm::vec3 offset = oldIsectPoint - oldCenter;
			spherePosition = new_intersection - offset; //TODO: change to object 
		}
		return (1);
	case FL_MOVE:
		Fl::belowmouse(this);
		// printf("mouse move event (%d, %d)\n", (int)Fl::event_x(), (int)Fl::event_y());
		mouseX = (int)Fl::event_x();
		mouseY = (int)Fl::event_y();
		break;
	case FL_PUSH:
		printf("mouse push\n");
		if ((Fl::event_button() == FL_LEFT_MOUSE) && (castRay == false))
		{ // left mouse click -- casting Ray
			castRay = true;
		}
		else if ((Fl::event_button() == FL_RIGHT_MOUSE) && (drag == false))
		{ // right mouse click -- dragging
			// this code is run when the dragging first starts (i.e. the first frame).
			// it stores a bunch of values about the sphere's "original" position and information
			glm::vec3 eyePointP = getEyePoint(mouseX, mouseY, pixelWidth, pixelHeight);
			glm::vec3 rayV = generateRay(mouseX, mouseY);
			glm::vec3 sphereTransV(spherePosition[0], spherePosition[1], spherePosition[2]);
			float t = intersect(eyePointP, rayV, glm::translate(glm::mat4(1.0), sphereTransV));
			glm::vec3 isectPointWorldCoord = getIsectPointWorldCoord(eyePointP, rayV, t);

			// this only activates if the mouse intersects with the sphere to enable dragging
			if (t > 0)
			{
				drag = true;
				printf("drag is true\n");
				oldCenter = spherePosition;
				oldIsectPoint = isectPointWorldCoord;
				oldT = t;
			}
		}
		return (1);
	case FL_RELEASE:
		printf("mouse release\n");
		if (Fl::event_button() == FL_LEFT_MOUSE)
		{
			castRay = false;
		}
		else if (Fl::event_button() == FL_RIGHT_MOUSE)
		{
			drag = false;
		}
		return (1);
	case FL_KEYUP:
		printf("keyboard event: key pressed: %c\n", Fl::event_key());
		break;
	case FL_MOUSEWHEEL:
		printf("mousewheel: dx: %d, dy: %d\n", Fl::event_dx(), Fl::event_dy());
		break;
	}
	return Fl_Gl_Window::handle(e);
}

void MyGLCanvas::resize(int x, int y, int w, int h) {
	Fl_Gl_Window::resize(x, y, w, h);
	puts("resize called");
}

void MyGLCanvas::initShaders() {
	myShaderManager->initShader("shaders/330/test.vert", "shaders/330/test.frag");

    // Bind VBO for all existing .ply objects
    for(auto& obj : scenePLYObjects) {
        obj->buildArrays(); // Ensure VBOs are built
        obj->bindVBO(myShaderManager->program);
    }
}

void MyGLCanvas::reloadShaders() {
	myShaderManager->resetShaders();

    myShaderManager->initShader("shaders/330/test.vert", "shaders/330/test.frag");

    // Re-bind VBO for all .ply objects with the new shader program
    for(auto& obj : scenePLYObjects) {
        obj->bindVBO(myShaderManager->program);
    }
    invalidate(); // Request a redraw
}
