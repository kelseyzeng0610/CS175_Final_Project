#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>

int Shape::m_segmentsX;
int Shape::m_segmentsY;


int Shape::lastIndex;
std::vector<std::array<float, 3>> Shape::m_points;
std::unordered_map<int, glm::vec3> Shape::m_normals;

// Struct for storing each object in the scenegraph described by an xml file
std::vector<SceneObject> sceneObjects;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l)
{
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

	// eyePosition = glm::vec3(0.0f, 0.0f, 5.0f);
	// lookatPoint = glm::vec3(0.0f, 0.0f, 0.0f);
	// rotVec = glm::vec3(0.0f, 0.0f, 0.0f);
  nextObjectId = 0;
  selectedObjId = -1;

	pixelWidth = w;
	pixelHeight = h;

	wireframe = 0;
	viewAngle = 60;
	clipNear = 0.01f;
	clipFar = 10.0f;

	castRay = false;
	drag = false;
	mouseX = 0;
	mouseY = 0;
	spherePosition = glm::vec3(0, 0, 0);

	

	camera.setViewAngle(viewAngle);
	camera.setNearPlane(clipNear);
	camera.setFarPlane(clipFar);

  
  eyePosition = glm::vec3(2.0f, 2.0f, 2.0f);
  glm::vec3 lookVector = glm::normalize(glm::vec3(-2.0f, -2.0f, -2.0f)); // from (2,2,2) to (0,0,0)
  glm::vec3 upVector = glm::vec3(0,1,0);
  camera.orientLookVec(eyePosition, lookVector, upVector);
  camera.orientLookAt(eyePosition, camera.getLookVector(), camera.getUpVector()); 
  // camera.orientLookAt(eyePosition, glm::vec3(0,0,0), glm::vec3(0,1,0));            

	isectOnly = 1;
	segmentsX = segmentsY = 10;
  fileIndex = 0;

}

void MyGLCanvas::setupCamera() {
    
    glm::mat4 view = glm::lookAt(eyePosition, lookatPoint, glm::vec3(0.0f, 1.0f, 0.0f));

    
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(glm::value_ptr(view));
}


MyGLCanvas::~MyGLCanvas()
{
  delete cube;
  delete cylinder;
  delete cone;
  delete sphere;
  objectList.clear();
  

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


double MyGLCanvas::intersect(ObjectNode& obj, glm::vec3 eyePointP, glm::vec3 rayV)
{
    double closest_t = INFINITY;

    // Correctly generate the transformation matrix based on the object's current state
    glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), obj.translate) * 
                                glm::rotate(glm::mat4(1.0f), glm::radians(obj.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                                glm::rotate(glm::mat4(1.0f), glm::radians(obj.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                                glm::rotate(glm::mat4(1.0f), glm::radians(obj.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                                glm::scale(glm::mat4(1.0f), obj.scale);

    glm::mat4 invTransformMatrix = glm::inverse(transformMatrix);

    // Transform ray and eye point into object space
    glm::vec3 invEyePoint = glm::vec3(invTransformMatrix * glm::vec4(eyePointP, 1.0f));
    glm::vec3 invRay = glm::vec3(invTransformMatrix * glm::vec4(rayV, 0.0f));

    glm::vec3 p = glm::vec3(invEyePoint);
    glm::vec3 d = glm::normalize(glm::vec3(invRay));

    std::vector<double> results;

    // Intersection test based on primitive type
    switch (obj.primitive->getType()) {
        case SHAPE_CUBE:
            results = intersectWithCube(p, d, glm::mat4(1.0f));  // Already in object space
            break;
        case SHAPE_SPHERE:
            results = intersectWithSphere(p, d, glm::mat4(1.0f)); // Already in object space
            break;
        case SHAPE_CYLINDER:
            results = intersectWithCylinder(p, d, glm::mat4(1.0f)); // Already in object space
            break;
        case SHAPE_CONE:
            results = intersectWithCone(p, d, glm::mat4(1.0f)); // Already in object space
            break;
        default:
            break;
    }

    ObjectNode* closest_obj = nullptr;

    for (double t : results) {
        if (t > 0) {
            glm::vec3 obj_intersection = p + float(t) * d;
            glm::vec4 world_intersection = transformMatrix * glm::vec4(obj_intersection, 1.0f);
            double dist = glm::length(glm::vec3(world_intersection) - eyePointP);

            if (dist < closest_t) {
                closest_t = dist;
                closest_obj = &obj;
            }
        }
    }

    if (closest_obj != nullptr) {
        return closest_t;
    } else {
        return -1.0;
    }
}

void MyGLCanvas::draw()
{
  
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	if (!valid())
	{ // this is called when the GL canvas is set up for the first time or when it is resized...
		printf("establishing GL context\n");

	

		glViewport(0, 0, w(), h());
		updateCamera(w(), h());

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// glShadeModel(GL_SMOOTH);
		glShadeModel(GL_FLAT);

		GLfloat light_pos0[] = {eyePosition.x, eyePosition.y, eyePosition.z, 0.0f};
		GLfloat ambient[] = {0.7f, 0.7f, 0.7f, 1.0f};
		GLfloat diffuse[] = {0.5f, 0.5f, 0.5f, 1.0f};

		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);

		glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
		glEnable(GL_COLOR_MATERIAL);

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);

		/****************************************/
		/*          Enable z-buferring          */
		/****************************************/

		glEnable(GL_DEPTH_TEST);
		glPolygonOffset(1, 1);
	}

  glm::mat4 projection = camera.getProjectionMatrix();
  glMatrixMode(GL_PROJECTION);
  glLoadMatrixf(glm::value_ptr(projection));

  // Then load the modelview from the camera
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(glm::value_ptr(camera.getModelViewMatrix()));
	drawObjects();
  drawAxis();

  glFlush();
}

void MyGLCanvas::drawObjects() {
    for (ObjectNode& obj : objectList) {
        if (obj.parent == nullptr) { // Start with root nodes
            drawNode(&obj, glm::mat4(1.0f));
        }
    }
}


void MyGLCanvas::drawNode(ObjectNode* node, glm::mat4 parentTransform) {
    if (!node) return;

    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), node->translate) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(node->rotation.x), glm::vec3(1, 0, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(node->rotation.y), glm::vec3(0, 1, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(node->rotation.z), glm::vec3(0, 0, 1)) *
                               glm::scale(glm::mat4(1.0f), node->scale);

    glm::mat4 worldTransform = parentTransform * localTransform;

    glPushMatrix();
    glMultMatrixf(glm::value_ptr(worldTransform));

    // Set the color dynamically
    float red = node->red / 255.0f;
    float green = node->green / 255.0f;
    float blue = node->blue / 255.0f;
    glColor3f(red, green, blue);

    // Draw the primitive if it exists
    if (node->primitive) {
        node->primitive->draw();
    }

    glPopMatrix();

    // Recursively draw children
    for (ObjectNode* child : node->children) {
        drawNode(child, worldTransform);
    }
}

int MyGLCanvas::selectObject(int mouseX, int mouseY) {
    glm::vec3 eyePoint = camera.getEyePoint();
    glm::vec3 rayDir = generateRay(mouseX, mouseY);

    const ObjectNode* closestObj = nullptr;
    float closestT = INFINITY;

    // Start testing from root nodes
    for (ObjectNode& obj : objectList) {
        if (obj.parent == nullptr) { // Only test root nodes
            testObjectIntersection(&obj, glm::mat4(1.0f), eyePoint, rayDir, closestObj, closestT);
        }
    }

    // Update selected object ID
    if (closestObj) {
        selectedObjId = closestObj->id;  
        if (onSelectionChanged) {
            onSelectionChanged(); // Notify selection
        }
        return selectedObjId;
    }

    // Deselect if no object is selected
    if (selectedObjId != -1) {
        selectedObjId = -1;  
        if (onSelectionChanged) {
            onSelectionChanged();
        }
    }
    return -1;
}

void MyGLCanvas::testObjectIntersection(ObjectNode* node, glm::mat4 parentTransform,
                                        glm::vec3 eyePoint, glm::vec3 rayDir,
                                        const ObjectNode*& closestObj, float& closestT) {
    if (!node) return;

    // Accumulate the world transformation
    glm::mat4 localTransform = glm::translate(glm::mat4(1.0f), node->translate) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(node->rotation.x), glm::vec3(1, 0, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(node->rotation.y), glm::vec3(0, 1, 0)) *
                               glm::rotate(glm::mat4(1.0f), glm::radians(node->rotation.z), glm::vec3(0, 0, 1)) *
                               glm::scale(glm::mat4(1.0f), node->scale);

    glm::mat4 worldTransform = parentTransform * localTransform;
    glm::mat4 invTransform = glm::inverse(worldTransform);

    // Transform ray into object space
    glm::vec3 transformedEye = glm::vec3(invTransform * glm::vec4(eyePoint, 1.0f));
    glm::vec3 transformedRay = glm::vec3(invTransform * glm::vec4(rayDir, 0.0f));

    // Perform intersection test
    std::vector<double> results;
    switch (node->primitive->getType()) {
        case SHAPE_CUBE:
            results = intersectWithCube(transformedEye, transformedRay, glm::mat4(1.0f));
            break;
        case SHAPE_SPHERE:
            results = intersectWithSphere(transformedEye, transformedRay, glm::mat4(1.0f));
            break;
        case SHAPE_CYLINDER:
            results = intersectWithCylinder(transformedEye, transformedRay, glm::mat4(1.0f));
            break;
        case SHAPE_CONE:
            results = intersectWithCone(transformedEye, transformedRay, glm::mat4(1.0f));
            break;
        default:
            break;
    }

    // Find the closest intersection
    for (double t : results) {
        if (t > 0) {
            glm::vec3 intersection = transformedEye + static_cast<float>(t) * transformedRay;
            double worldDistance = glm::length(glm::vec3(worldTransform * glm::vec4(intersection, 1.0f)) - eyePoint);
            if (worldDistance < closestT) {
                closestT = worldDistance;
                closestObj = node;
            }
        }
    }

    // Recursively test children
    for (ObjectNode* child : node->children) {
        testObjectIntersection(child, worldTransform, eyePoint, rayDir, closestObj, closestT);
    }
}



void MyGLCanvas::updateCamera(int width, int height)
{
    float xy_aspect = (float)width / (float)height;

    camera.setScreenSize(width, height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glm::mat4 projection = camera.getProjectionMatrix();
    glLoadMatrixf(glm::value_ptr(projection));

    // Update the viewport to the new window dimensions
    glViewport(0, 0, width, height);
}


int MyGLCanvas::handle(int e)
{
    switch (e)
    {
    case FL_PUSH:
        mouseX = (int)Fl::event_x();
        mouseY = (int)Fl::event_y();

        if (Fl::event_button() == FL_LEFT_MOUSE) {
            // Left mouse button: Select object
            castRay = true;
            selectedObjId = selectObject(mouseX, mouseY);

            if (selectedObjId != -1) {
                printf("Object selected: ID %d\n", selectedObjId);
            } else {
                printf("No object selected.\n");
            }
        } else if ((Fl::event_button() == FL_RIGHT_MOUSE) && !drag) {
    // Find the selected object by ID
    auto it = std::find_if(objectList.begin(), objectList.end(),
        [this](const ObjectNode& obj) { return obj.id == selectedObjId; });

    if (it != objectList.end()) {
        // Generate ray and compute intersection
        glm::vec3 eyePoint = getEyePoint(mouseX, mouseY, pixelWidth, pixelHeight);
        glm::vec3 rayDir = generateRay(mouseX, mouseY);

        double t = intersect(*it, eyePoint, rayDir);
        if (t > 0) {
            // Initialize drag state
            drag = true;
            oldCenter = it->translate; // Store the current center position
            oldT = t;                 // Store the intersection parameter t
            oldIsectPoint = getIsectPointWorldCoord(eyePoint, rayDir, t); // Intersection point

            printf("Dragging started. Object ID: %d, Initial Intersection: (%f, %f, %f)\n",
                   it->id, oldIsectPoint.x, oldIsectPoint.y, oldIsectPoint.z);
        }
    }
}

        return 1;
   
    case FL_DRAG:
    mouseX = (int)Fl::event_x();
    mouseY = (int)Fl::event_y();

    if (drag && selectedObjId != -1) {
    auto it = std::find_if(objectList.begin(), objectList.end(),
                           [this](const ObjectNode& obj) { return obj.id == selectedObjId; });

    if (it != objectList.end()) {
        ObjectNode* node = &(*it);

        // Get the ray in world space
        glm::vec3 eyePoint = getEyePoint(mouseX, mouseY, pixelWidth, pixelHeight);
        glm::vec3 rayDir = generateRay(mouseX, mouseY);

        // Compute the new intersection in world space
        glm::vec3 newIntersection = getIsectPointWorldCoord(eyePoint, rayDir, oldT);

        // Get the parent's transformation matrix
        glm::mat4 parentTransform = glm::mat4(1.0f);
        if (node->parent) {
            parentTransform = glm::translate(glm::mat4(1.0f), node->parent->translate) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(node->parent->rotation.x), glm::vec3(1, 0, 0)) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(node->parent->rotation.y), glm::vec3(0, 1, 0)) *
                              glm::rotate(glm::mat4(1.0f), glm::radians(node->parent->rotation.z), glm::vec3(0, 0, 1)) *
                              glm::scale(glm::mat4(1.0f), node->parent->scale);
        }

        // Compute the offset in local space
        glm::vec4 localIntersection = glm::inverse(parentTransform) * glm::vec4(newIntersection, 1.0f);
        glm::vec3 offset = glm::vec3(localIntersection) - oldIsectPoint;

        // Update translation in local space
        node->translate += offset;

        // Update references for next drag event
        oldIsectPoint = glm::vec3(localIntersection);

        // Redraw to reflect changes
        redraw();
    }
}

    return 1;




    case FL_RELEASE:
        if (Fl::event_button() == FL_LEFT_MOUSE) {
            castRay = false;
        } else if (Fl::event_button() == FL_RIGHT_MOUSE) {
            if (drag) {
                // Reset drag state
                drag = false;
                oldT = 0.0f;
                oldIsectPoint = glm::vec3(0.0f);
                oldCenter = glm::vec3(0.0f);

                printf("Dragging finished for Object ID: %d\n", selectedObjId);
            }
        }
        return 1;


    case FL_KEYUP: {
        printf("Keyboard event: key pressed: %c\n", Fl::event_key());
        int key = Fl::event_key();
        float step = 0.1f; // adjust how fast the eyePosition changes
        bool updated = false;

        // Define a small rotation angle in degrees
        float angleDegrees = 5.0f; 
        float angleRad = glm::radians(angleDegrees);

        // We'll rotate around the X-axis, so define the rotation matrix:
        glm::mat4 rotationMatrix(1.0f);

        if (key == 'w') { // Move up
            glm::vec3 rightVec = glm::normalize(glm::cross(camera.getLookVector(), camera.getUpVector()));
            glm::vec3 upMovementVec = glm::normalize(glm::cross(rightVec, camera.getLookVector()));
            eyePosition += upMovementVec * step;
            updated = true;
        }
        else if (key == 's') { // Move down
            glm::vec3 rightVec = glm::normalize(glm::cross(camera.getLookVector(), camera.getUpVector()));
            glm::vec3 downMovementVec = glm::normalize(glm::cross(camera.getLookVector(), rightVec));
            eyePosition += downMovementVec * step;
            updated = true;
        }
        else if (key == 'a') { // Move left (perpendicular to look vector and up vector)
            // Compute a left vector from cross products
            glm::vec3 leftVec = glm::normalize(glm::cross(camera.getUpVector(), camera.getLookVector()));
            eyePosition += leftVec * step;
            updated = true;
        } 
        else if (key == 'd') { // Move right
            glm::vec3 rightVec = glm::normalize(glm::cross(camera.getLookVector(), camera.getUpVector()));
            eyePosition += rightVec * step;
            updated = true;
        }

        if (updated) {
            // Apply the rotation to the eyePosition
            glm::vec4 pos(eyePosition, 1.0f);
            pos = rotationMatrix * pos;
            eyePosition = glm::vec3(pos);
            camera.orientLookAt(eyePosition, glm::vec3(0,0,0), glm::vec3(0,1,0));            
            // Update projection if needed and redraw
            updateCamera(w(), h());
            redraw();
        }
        break;
    }

    case FL_MOUSEWHEEL:
        printf("Mouse wheel: dx: %d, dy: %d\n", Fl::event_dx(), Fl::event_dy());
        eyePosition.z += Fl::event_dy() * -0.05f;
        updateCamera(w(), h());
        break;
    }

    return Fl_Gl_Window::handle(e);
}


void MyGLCanvas::resize(int x, int y, int w, int h)
{
    Fl_Gl_Window::resize(x, y, w, h);
    pixelWidth = w;
    pixelHeight = h;
    updateCamera(w, h);
    redraw();
    printf("resize called: width=%d, height=%d\n", w, h);
}

void MyGLCanvas::drawAxis()
{
	glDisable(GL_LIGHTING);
	glBegin(GL_LINES);
	glColor3f(1.0, 0.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(1.0, 0, 0);
	glColor3f(0.0, 1.0, 0.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0.0, 1.0, 0);
	glColor3f(0.0, 0.0, 1.0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1.0);
	glEnd();
	glEnable(GL_LIGHTING);
}

void MyGLCanvas::resetScene() {
	//TODO: reset scene to default 
    objectList.clear();
    nextObjectId = 0;
    selectedObjId = -1;
    redraw();
}

// void MyGLCanvas::setShape(OBJ_TYPE type) {
//     objType = type;
//     addObject(type);
//     printf("set shape to: %d\n", type);
//     redraw();
// }


void MyGLCanvas::setShape(OBJ_TYPE type, bool isChild) {
    objType = type;

    if (isChild && selectedObjId != -1) {
        auto it = std::find_if(objectList.begin(), objectList.end(),
                               [this](const ObjectNode& obj) { return obj.id == selectedObjId; });

        if (it != objectList.end()) {
            addObject(type, &(*it));
            printf("Added child shape of type %d to parent ID %d\n", type, selectedObjId);
        }
    } else {
        addObject(type);
        printf("Added root shape of type %d\n", type);
    }

    redraw();
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


void MyGLCanvas::addObject(OBJ_TYPE type, ObjectNode* parent) {
    ObjectNode* node = new ObjectNode();
    node->id = nextObjectId++;
    node->translate = glm::vec3(0.0f, 0.0f, 0.0f);
    node->scale = glm::vec3(1.0f, 1.0f, 1.0f);
    node->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    node->red = 255;
    node->green = 255;
    node->blue = 255;

    switch (type) {
        case SHAPE_SPHERE: node->primitive = new Sphere(); break;
        case SHAPE_CUBE: node->primitive = new Cube(); break;
        case SHAPE_CYLINDER: node->primitive = new Cylinder(); break;
        case SHAPE_CONE: node->primitive = new Cone(); break;
        default: node->primitive = nullptr; break;
    }

    // Set parent-child relationship
    if (parent) {
        node->parent = parent;
        node->translate = glm::vec3(0.0f, 1.0f, 0.0f);

        parent->children.push_back(node);
    }

    // Add node to the flattened list
    objectList.push_back(*node);

    redraw();
}



// void MyGLCanvas::addObject(OBJ_TYPE type) {
//   ObjectNode node;
 
//   node.id = nextObjectId++;
//   node.translate = glm::vec3(0.0f, 0.0f, 0.0f);
//   node.scale = glm::vec3(1.0f, 1.0f, 1.0f);
//   node.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
//   node.oldCenter = glm::vec3(0.0f, 0.0f, 0.0f);
//   node.red = 255;
//   node.green = 255;
//   node.blue = 255;
  

//     switch (type) {
//         case SHAPE_SPHERE:
//             node.primitive = new Sphere();
//             break;
//         case SHAPE_CUBE:
//             node.primitive = new Cube();
//             break;
//         case SHAPE_CYLINDER:
//             node.primitive = new Cylinder();
//             break;
//         case SHAPE_CONE:
//             node.primitive = new Cone();
//             break;
//         default:
//             node.primitive = new Sphere();
//             break;
//     }
//     if (node.primitive != nullptr) {
//       node.primitive->setSegments(segmentsX, segmentsY);
//        objectList.push_back(node);
//     }else{
//       printf("Object ID %d has a null primitive\n", node.id);
//     }
   
    


// }

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
