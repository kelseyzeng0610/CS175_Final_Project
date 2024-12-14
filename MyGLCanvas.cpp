#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

int Shape::m_segmentsX;
int Shape::m_segmentsY;

// Data structures required for rendering shapes
int Shape::lastIndex;
std::vector<std::array<float, 3>> Shape::m_points;
std::unordered_map<int, glm::vec3> Shape::m_normals;

// Struct for storing each object in the scenegraph described by an xml file
std::vector<SceneObject> sceneObjects;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l)
{
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);



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
  	// parser = NULL;
	// sphere = new Sphere();

	camera.setViewAngle(viewAngle);
	camera.setNearPlane(clipNear);
	camera.setFarPlane(clipFar);
	// Set the mode so we are modifying our objects.
	camera.orientLookVec(eyePosition, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	isectOnly = 1;
	segmentsX = segmentsY = 10;
	sphere = new Sphere();
	cube = new Cube();
	cylinder = new Cylinder();
	cone = new Cone();
}

MyGLCanvas::~MyGLCanvas()
{
  delete cube;
  delete cylinder;
  delete cone;
  delete sphere;
}

// loop over objectList and select the one that is hit by the ray
// returns the closest object and its index
std::pair<ObjectNode, int> MyGLCanvas::closestObject(glm::vec3 rayOriginPoint, int mouseX, int mouseY){
	std::pair<ObjectNode, int> closest_obj_info;

	glm::vec3 intersection_obj = glm::vec3(0);
	glm::vec3 intersection = glm::vec3(0);
	glm::vec3 normal = glm::vec3(0);

	ObjectNode* closest_obj = nullptr;
	double closest_t = INFINITY;

	ObjectNode small;
	// for (ObjectNode my_p : objectList) {
	// }
 
	return closest_obj_info;
}

void MyGLCanvas::addPLY(const std::string& plyFile) {
	ply* newPLY = new ply(plyFile);
	
	newPLY->buildArrays();
	newPLY->bindVBO(myShaderManager->program);
	scenePLYObjects.push_back(newPLY);
}

void MyGLCanvas::draw() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (!valid())
	{ // this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

		// Set the base texture of our object. Note that loading gl texture can
		//  only happen after the gl context has been established
		// if (myObject->baseTexture == NULL)
		// {
		// 	myObject->setTexture(0, "./data/pink.ppm");
		// }
		// // Set a second texture layer to our object
		// if (myObject->blendTexture == NULL)
		// {
		// 	myObject->setTexture(1, "./data/smile.ppm");
		// }

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

void MyGLCanvas::drawShape(OBJ_TYPE type) {
	// printf("drawing here\n");

    switch (type) {
        case SHAPE_CUBE:
			drawCube();
            // shape = sphere;
            break;
        case SHAPE_CYLINDER:
			drawCylinder();
            
            break;
        case SHAPE_CONE:
			drawCone();

            break;
        case SHAPE_SPHERE:
			drawSphere();
            break;
        default:
            
    }
}

void MyGLCanvas::drawSphere() {
	sphere->setSegments(segmentsX, segmentsY);
    sphere->draw();
}

void MyGLCanvas::drawCube() {
	cube->setSegments(segmentsX, segmentsY);
    cube->draw();
}

void MyGLCanvas::drawCylinder() {
	cylinder->setSegments(segmentsX, segmentsY);
    cylinder->draw();	
}

void MyGLCanvas::drawCone() {
	cone->setSegments(segmentsX, segmentsY);
    cone->draw();
}



void MyGLCanvas::drawObject() {
	//add object properties

	//cast ray onto object
	glMatrixMode(GL_MODELVIEW);
	// Set the mode so we are modifying our objects.
	camera.orientLookVec(eyePosition, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
	glLoadMatrixf(glm::value_ptr(camera.getModelViewMatrix()));

	if (castRay == true)
	{
		// std::cout << mouseX << std::endl;
		glm::vec3 eyePointP = getEyePoint(mouseX, mouseY, pixelWidth, pixelHeight);
		// std::cout << eyePointP.x << std::endl;
		// std::cout << eyePointP.y << std::endl;
		// std::cout << eyePointP.z << std::endl;
		glm::vec3 rayV = generateRay(mouseX, mouseY);	
		glm::vec3 sphereTransV(spherePosition[0], spherePosition[1], spherePosition[2]);

		float t = intersect(eyePointP, rayV, glm::translate(glm::mat4(1.0), sphereTransV));
		glm::vec3 isectPointWorldCoord = getIsectPointWorldCoord(eyePointP, rayV, t);

		if (t > 0)
		{
			glColor3f(1, 0, 0);
			glPushMatrix();
			glTranslated(spherePosition[0], spherePosition[1], spherePosition[2]);
			glutWireCube(1.0f);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(isectPointWorldCoord[0], isectPointWorldCoord[1], isectPointWorldCoord[2]);
			glutSolidSphere(0.05f, 10, 10);
			glPopMatrix();
			printf("hit!\n");
		}
		else
		{
			printf("miss!\n");
		}
	}

	glm::vec3 rotateLightPos = glm::vec3(sin(lightPos.x), lightPos.y, cos(lightPos.z));
	//TODO: pass lighting information to the shaders
	GLuint lightPosLoc = glGetUniformLocation(myShaderManager->program, "lightPosition");
    glUniform3f(lightPosLoc, rotateLightPos.x, rotateLightPos.y, rotateLightPos.z);


	glDisable(GL_POLYGON_OFFSET_FILL);
	glColor3f(1.0, 1.0, 1.0);
	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	glPushMatrix();
	glRotatef(90, 0, 1, 0);
	drawShape(objType);
	// drawCone();
	// // drawCylinder();
	// // drawCube();
	// myObject->drawTexturedSphere(); //TODO:draw shape here
	
	glPopMatrix();

	glPopMatrix();

}


// void MyGLCanvas::drawObject(ObjectNode node, glm::mat4 trans)
void MyGLCanvas::drawScene()
{
	//loop over objects to draw
	drawObject();
}


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

		//NOTE:step 2: store new location in list

    }
    return (1);
	case FL_MOVE:
	case FL_PUSH:
		printf("mouse push\n");
		//step 1: generate ray from mouse click
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
	case FL_KEYUP:
	case FL_MOUSEWHEEL:
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

void MyGLCanvas::setShape(OBJ_TYPE type) {
    objType = type;
    printf("set shape to: %d\n", type);
}
void MyGLCanvas::setSegments() {
    shape->setSegments(segmentsX, segmentsY);
}

// Logic derived from lab04
std::vector<double> MyGLCanvas::intersectWithSphere(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{
  // Transform ray and eye point to object space
  glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
  glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f);

  glm::vec3 p = glm::vec3(eyePointPO);
  glm::vec3 d = glm::normalize(glm::vec3(rayVO));

  double A = glm::dot(glm::vec3(d), glm::vec3(d));
  double B = 2.0f * glm::dot(glm::vec3(d), glm::vec3(p));
  double C = glm::dot(glm::vec3(p), glm::vec3(p)) - 0.25;

  double discriminant = (B * B) - (4.0 * A * C);

  std::vector<double> results;

  if (discriminant < 0)
  {
    return results;
  }

  // Calculate both intersection points
  double t1 = (-B - sqrt(discriminant)) / (2.0 * A);
  double t2 = (-B + sqrt(discriminant)) / (2.0 * A);

  if (t1 > 0) results.push_back(t1);
  if (t2 > 0) results.push_back(t2);

  return results;
}

std::vector<double> MyGLCanvas::intersectWithCube(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{
  // Transform eye point and ray from world coords to object space
  glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
  glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f);

  glm::vec3 p = glm::vec3(eyePointPO);
  glm::vec3 d = glm::normalize(glm::vec3(rayVO));

  double t_min = INFINITY;
  glm::vec3 intersection;

  std::vector<double> results;

  // Right face of cube, x = 0.5
  if (d.x != 0)
  {
    double t_right = (0.5 - p.x) / d.x;
    intersection = p + float(t_right) * d;
    if (t_right > 0 &&
        intersection.y >= -0.5 && intersection.y <= 0.5 &&
        intersection.z >= -0.5 && intersection.z <= 0.5)
    {
      results.push_back(t_right);
    }
  }

  // Left face of cube, x = -0.5
  if (d.x != 0)
  {
    double t_left = (-0.5 - p.x) / d.x;
    intersection = p + float(t_left) * d;
    if (t_left > 0 &&
        intersection.y >= -0.5 && intersection.y <= 0.5 &&
        intersection.z >= -0.5 && intersection.z <= 0.5)
    {
      results.push_back(t_left);
    }
  }

  // Top face of cube, y = 0.5
  if (d.y != 0)
  {
    double t_top = (0.5 - p.y) / d.y;
    intersection = p + float(t_top) * d;
    if (t_top > 0 &&
        intersection.x >= -0.5 && intersection.x <= 0.5 &&
        intersection.z >= -0.5 && intersection.z <= 0.5)
    {
      results.push_back(t_top);
    }
  }

  // Bottom face of cube, y = -0.5
  if (d.y != 0)
  {
    double t_bottom = (-0.5 - p.y) / d.y;
    intersection = p + float(t_bottom) * d;
    if (t_bottom > 0 &&
        intersection.x >= -0.5 && intersection.x <= 0.5 &&
        intersection.z >= -0.5 && intersection.z <= 0.5)
    {
      results.push_back(t_bottom);
    }
  }

  // Front face of cube, z = 0.5
  if (d.z != 0)
  {
    double t_front = (0.5 - p.z) / d.z;
    intersection = p + float(t_front) * d;
    if (t_front > 0 &&
        intersection.x >= -0.5 && intersection.x <= 0.5 &&
        intersection.y >= -0.5 && intersection.y <= 0.5)
    {
      results.push_back(t_front);
    }
  }

  // Back face of cube, z = -0.5
  if (d.z != 0)
  {
    double t_back = (-0.5 - p.z) / d.z;
    intersection = p + float(t_back) * d;
    if (t_back > 0 &&
        intersection.x >= -0.5 && intersection.x <= 0.5 &&
        intersection.y >= -0.5 && intersection.y <= 0.5)
    {
      results.push_back(t_back);
    }
  }

  // Return smallest positive t here
  return results;
}

std::vector<double> MyGLCanvas::intersectWithCylinder(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{
  // Transform ray and eye point to object space
  glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
  glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f);

  glm::vec3 p = glm::vec3(eyePointPO);
  glm::vec3 d = glm::normalize(glm::vec3(rayVO));

  double t_min = INFINITY;

  // Expand (px + tdx)^2 + (pz + tdz)^2 = 0.25 to find A, B and C
  double A = d.x * d.x + d.z * d.z;
  double B = 2.0f * (p.x * d.x + p.z * d.z);
  double C = p.x * p.x + p.z * p.z - 0.25;

  double discriminant = (B * B) - (4.0f * A * C);

  std::vector<double> results;

  if (discriminant >= 0 && A != 0)
  {
    double t1 = (-B - sqrt(discriminant)) / (2.0 * A);
    double t2 = (-B + sqrt(discriminant)) / (2.0 * A);

    // Check if intersection y value is between (-0.5, 0.5) before potentially setting new t_min
    if (t1 > 0)
    {
      glm::vec3 intersection = p + float(t1) * d;
      if (intersection.y >= -0.5f && intersection.y <= 0.5f)
      {
        results.push_back(t1);
      }
    }
    if (t2 > 0)
    {
      glm::vec3 intersection = p + float(t2) * d;
      if (intersection.y >= -0.5f && intersection.y <= 0.5f)
      {
        results.push_back(t2);
      }
    }
  }

  // Check intersection of the two caps
  if (d.y != 0)
  {
    double t_top = (0.5f - p.y) / d.y;
    double t_bottom = (-0.5f - p.y) / d.y;

    if (t_top > 0)
    {
      glm::vec3 intersection = p + float(t_top) * d;
      if (intersection.x * intersection.x + intersection.z * intersection.z <= 0.25)
      {
        results.push_back(t_top);
      }
    }

    if (t_bottom > 0)
    {
      glm::vec3 intersection = p + float(t_bottom) * d;
      if (intersection.x * intersection.x + intersection.z * intersection.z <= 0.25)
      {
        results.push_back(t_bottom);
      }
    }
  }

  return results;
}

std::vector<double> MyGLCanvas::intersectWithCone(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{
  glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
  glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f);

  glm::vec3 p = glm::vec3(eyePointPO);
  glm::vec3 d = glm::normalize(glm::vec3(rayVO));

  double t_min = INFINITY;

  double A = d.x * d.x + d.z * d.z - 0.25 * d.y * d.y;
  double B = 2.0f * (p.x * d.x + p.z * d.z) + (0.25 - (0.5 * p.y)) * d.y;
  double C = p.x * p.x + p.z * p.z - 0.25 * (0.5 - p.y) * (0.5 - p.y);

  double discriminant = (B * B) - (4.0f * A * C);

  std::vector<double> results;

  if (discriminant >= 0 && A != 0)
  {
    double t1 = (-B - sqrt(discriminant)) / (2.0 * A);
    double t2 = (-B + sqrt(discriminant)) / (2.0 * A);

    if (t1 > 0)
    {
      glm::vec3 intersection = p + float(t1) * d;
      if (intersection.y >= -0.5f && intersection.y <= 0.5f)
      {
        results.push_back(t1);
      }
    }
    if (t2 > 0)
    {
      glm::vec3 intersection = p + float(t2) * d;
      if (intersection.y >= -0.5f && intersection.y <= 0.5f)
      {
        results.push_back(t2);
      }
    }
  }

  if (d.y != 0)
  {
    double t_cap = (-0.5f - p.y) / d.y;

    if (t_cap > 0)
    {
      glm::vec3 intersection = p + float(t_cap) * d;
      if (intersection.x * intersection.x + intersection.z * intersection.z <= 0.25)
      {
        results.push_back(t_cap);
      }
    }
  }

  return results;
}

// void MyGLCanvas::setpixel(GLubyte *buf, int x, int y, int r, int g, int b)
// {
//   pixelWidth = camera.getScreenWidth();
//   buf[(y * pixelWidth + x) * 3 + 0] = (GLubyte)r;
//   buf[(y * pixelWidth + x) * 3 + 1] = (GLubyte)g;
//   buf[(y * pixelWidth + x) * 3 + 2] = (GLubyte)b;
// }
