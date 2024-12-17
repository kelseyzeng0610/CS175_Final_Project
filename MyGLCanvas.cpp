#include "MyGLCanvas.h"
#include <glm/gtc/type_ptr.hpp>

int Shape::m_segmentsX;
int Shape::m_segmentsY;


int Shape::lastIndex;
std::vector<std::array<float, 3>> Shape::m_points;
std::unordered_map<int, glm::vec3> Shape::m_normals;
const double EPSILON = 1e-8;

// Struct for storing each object in the scenegraph described by an xml file
std::vector<SceneObject> sceneObjects;

MyGLCanvas::MyGLCanvas(int x, int y, int w, int h, const char *l) : Fl_Gl_Window(x, y, w, h, l)
{
	mode(FL_RGB | FL_ALPHA | FL_DEPTH | FL_DOUBLE);

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
  
  for (ObjectNode* obj : objectList) {
        delete obj->primitive; 
        delete obj;
    }
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
    
    float ndcX = (2.0f * pixelX / screenWidth) - 1.0f;
    float ndcY = 1.0f - (2.0f * pixelY / screenHeight);

  
    float aspectRatio = (float)screenWidth / screenHeight;
    float filmPlanDepth = camera.getFilmPlanDepth();

    
    glm::vec4 cameraPoint(ndcX * aspectRatio, ndcY, -filmPlanDepth, 1.0f);

    
    glm::mat4 viewToWorld = camera.getInverseModelViewMatrix();
    glm::vec4 worldPoint = viewToWorld * cameraPoint;

    return glm::vec3(worldPoint);
}

glm::vec3 MyGLCanvas::getIsectPointWorldCoord(glm::vec3 eye, glm::vec3 ray, float t)
{
	return eye + ray * t;
}


double MyGLCanvas::intersect(ObjectNode* obj, glm::vec3 eyePointP, glm::vec3 rayV) {
    double closest_t = INFINITY;

    // Correctly generate the transformation matrix based on the object's current state
    glm::mat4 transformMatrix = glm::translate(glm::mat4(1.0f), obj->translate) * 
                                glm::rotate(glm::mat4(1.0f), glm::radians(obj->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f)) *
                                glm::rotate(glm::mat4(1.0f), glm::radians(obj->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f)) *
                                glm::rotate(glm::mat4(1.0f), glm::radians(obj->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f)) *
                                glm::scale(glm::mat4(1.0f), obj->scale);

    glm::mat4 inverseTransformMatrix = glm::inverse(transformMatrix);

    // Transform ray and eye point into object space
    glm::vec3 invEyePoint = glm::vec3(inverseTransformMatrix * glm::vec4(eyePointP, 1.0f));
    glm::vec3 invRay = glm::vec3(inverseTransformMatrix * glm::vec4(rayV, 0.0f));

    glm::vec3 p = invEyePoint;
    glm::vec3 d = glm::normalize(invRay);

    std::vector<double> results;

    // Intersection test based on primitive type
    switch (obj->primitive->getType()) {
        case SHAPE_CUBE:
            results = intersectWithCube(eyePointP, rayV,inverseTransformMatrix);  
            break;
        case SHAPE_SPHERE:
            results = intersectWithSphere(eyePointP, rayV, inverseTransformMatrix); 
            break;
        case SHAPE_CYLINDER:
            results = intersectWithCylinder(eyePointP, rayV, inverseTransformMatrix); 
            break;
        case SHAPE_CONE:
            results = intersectWithCone(eyePointP, rayV, inverseTransformMatrix); 
            break;
        default:
            break;
    }

    ObjectNode* closest_obj = nullptr;

    for (double t : results) {
        if (t > 0) {
            glm::vec3 obj_intersection = p + static_cast<float>(t) * d;
            glm::vec4 world_intersection = transformMatrix * glm::vec4(obj_intersection, 1.0f);
            double dist = glm::length(glm::vec3(world_intersection) - eyePointP);

            if (dist < closest_t) {
                closest_t = dist;
                closest_obj = obj;
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
    for (ObjectNode* obj : objectList) {
        if (obj->parent == nullptr) { // Start with root nodes
            drawNode(obj, glm::mat4(1.0f));
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
    if (wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(1.0f, 1.0f, 1.0f);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  
        glColor3f(red, green, blue);
    }

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

    ObjectNode* closestObj = nullptr;
    float closestT = INFINITY;

    // Start testing from root nodes
    for (ObjectNode* obj : objectList) {
        if (obj->parent == nullptr) { // Only test root nodes
            testObjectIntersection(obj, glm::mat4(1.0f), eyePoint, rayDir, closestObj, closestT);
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

void MyGLCanvas::testObjectIntersection(ObjectNode* node, glm::mat4 parentTransform, glm::vec3 eyePoint, glm::vec3 rayDir, ObjectNode*& closestObj, float& closestT) {
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
            results = intersectWithCube(eyePoint, rayDir, invTransform);
            break;
        case SHAPE_SPHERE:
            results = intersectWithSphere(eyePoint, rayDir, invTransform);
            break;
        case SHAPE_CYLINDER:
            results = intersectWithCylinder(eyePoint, rayDir, invTransform);
            break;
        case SHAPE_CONE:
            results = intersectWithCone(eyePoint,rayDir, invTransform);
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
        // Right mouse button: Initiate dragging

      
        auto it = std::find_if(objectList.begin(), objectList.end(),
                           [this](ObjectNode* obj) { return obj->id == this->selectedObjId; });

        if (it != objectList.end()) {
            ObjectNode* node = *it;

            // Generate ray and compute intersection with the object
            glm::vec3 eyePoint = camera.getEyePoint();
            glm::vec3 rayDir = generateRay(mouseX, mouseY);

            double t = intersect(node, eyePoint, rayDir);

            printf("Intersection test result: %f\n", t);
            if (t > 0) {
                // Compute intersection point in world space
                glm::vec3 worldIntersection = getIsectPointWorldCoord(eyePoint, rayDir, t);

                // Define the drag plane: perpendicular to the view direction, passing through the intersection point
                dragPlaneNormal = camera.getLookVector();
                dragPlanePoint = worldIntersection;

                // Initialize drag state
                dragging = true;
                oldIsectPoint = worldIntersection; 

                printf("Dragging started. Object ID: %d, Initial Intersection: (%f, %f, %f)\n",
                       node->id, oldIsectPoint.x, oldIsectPoint.y, oldIsectPoint.z);
            }
        }
    }

    return 1;

   
   case FL_DRAG:
    mouseX = (int)Fl::event_x();
    mouseY = (int)Fl::event_y();

    if (dragging && selectedObjId != -1) {
        // Find the selected object by ID
        auto it = std::find_if(objectList.begin(), objectList.end(),
                           [this](ObjectNode* obj) { return obj->id == this->selectedObjId; });

        if (it != objectList.end()) {
            ObjectNode* node = *it;

            // Generate a new ray based on current mouse position
            glm::vec3 eyePoint = camera.getEyePoint();
            glm::vec3 rayDir = generateRay(mouseX, mouseY);

            // Compute intersection with the drag plane
            float denom = glm::dot(dragPlaneNormal, rayDir);
            if (fabs(denom) > 1e-6) { // Avoid division by zero
                glm::vec3 p0l0 = dragPlanePoint - eyePoint;
                float t = glm::dot(p0l0, dragPlaneNormal) / denom;
                if (t >= 0) {
                    glm::vec3 newIntersection = eyePoint + t * rayDir;

                    
                    glm::vec3 offset = newIntersection - oldIsectPoint;

                   
                    node->translate += offset;

                    
                    oldIsectPoint = newIntersection;

                   
                    redraw();
                }
            }
        }
    }

    return 1;




case FL_RELEASE:
    if (Fl::event_button() == FL_LEFT_MOUSE) {
        castRay = false;
    } else if (Fl::event_button() == FL_RIGHT_MOUSE) {
        if (dragging) {
            // Reset drag state
            dragging = false;
            dragPlaneNormal = glm::vec3(0.0f);
            dragPlanePoint = glm::vec3(0.0f);
            oldIsectPoint = glm::vec3(0.0f);

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
        // Target maximum position values for clamping when 'w' is pressed
        glm::vec3 maxEyePosition(0.165399f, 3.780443f, 0.165399f);
        glm::vec3 minEyePosition(0.164884f, -3.583608f, 0.164884f);

        if (key == 'w') { // Move up
            glm::vec3 rightVec = glm::normalize(glm::cross(camera.getLookVector(), camera.getUpVector()));
            glm::vec3 upMovementVec = glm::normalize(glm::cross(rightVec, camera.getLookVector()));

            // Update the eyePosition
            eyePosition += upMovementVec * step;
            printf("before clamp w: %f, %f, %f\n", eyePosition.x, eyePosition.y, eyePosition.z);

            // Clamp the eyePosition to the specified maximum values
            eyePosition.x = glm::max(eyePosition.x, maxEyePosition.x);
            // eyePosition.y = glm::min(eyePosition.y, maxEyePosition.y);
            // eyePosition.z = glm::max(eyePosition.z, maxEyePosition.z);

            printf("after clamp w: %f, %f, %f\n", eyePosition.x, eyePosition.y, eyePosition.z);
            updated = true;
        }
        else if (key == 's') { // Move down
            glm::vec3 rightVec = glm::normalize(glm::cross(camera.getLookVector(), camera.getUpVector()));
            glm::vec3 downMovementVec = glm::normalize(glm::cross(camera.getLookVector(), rightVec));

            // Update the eyePosition
            eyePosition += downMovementVec * step;
            printf("before clamp s: %f, %f, %f\n", eyePosition.x, eyePosition.y, eyePosition.z);

            // Clamp the eyePosition to the specified maximum values for 's'
            eyePosition.x = glm::max(eyePosition.x, minEyePosition.x);
            eyePosition.y = glm::max(eyePosition.y, minEyePosition.y);
            // eyePosition.z = glm::max(eyePosition.z, minEyePosition.z);

            printf("after clamp s: %f, %f, %f\n", eyePosition.x, eyePosition.y, eyePosition.z);
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
        // Get the current camera view angle
        float currentAngle = camera.getViewAngle();

        // Adjust the angle based on the scroll wheel movement (dy)
        float angleDelta = Fl::event_dy() * -1.0f; // Negative to match typical scroll behavior
        float newAngle = currentAngle + angleDelta;

        // Clamp the new angle to a reasonable range, e.g., between 10 and 90 degrees
        newAngle = glm::clamp(newAngle, 10.0f, 90.0f);

        // Set the new camera view angle
        camera.setViewAngle(newAngle);
        printf("Mouse wheel: dy: %d, new camera angle: %f\n", Fl::event_dy(), newAngle);

        // Redraw the canvas to reflect the changes
        redraw();
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

void MyGLCanvas::deleteSelectedObject() {
    if (selectedObjId == -1) {
        fl_message("No object is selected for deletion.");
        return;
    }

    auto it = std::find_if(objectList.begin(), objectList.end(),
                          [this](ObjectNode* obj) { return obj->id == this->selectedObjId; });

    if (it != objectList.end()) {
        ObjectNode* obj = *it;

        // If it's a child node, remove from parent's children list
        if (obj->parent) {
            auto& parentChildren = obj->parent->children;
            parentChildren.erase(
                std::remove(parentChildren.begin(), parentChildren.end(), obj),
                parentChildren.end()
            );
        }

        // Recursive function to delete node and all its children
        std::function<void(ObjectNode*)> deleteNodeAndChildren = [&](ObjectNode* node) {
            // Recursively delete all children first
            for (ObjectNode* child : node->children) {
                deleteNodeAndChildren(child);
            }
            
            // Remove from objectList
            objectList.erase(
                std::remove(objectList.begin(), objectList.end(), node),
                objectList.end()
            );
            
            delete node;
        };

        // Delete the node and all its children
        deleteNodeAndChildren(obj);
        
        selectedObjId = -1;
        redraw();
    }

    if (onSelectionChanged) {
        onSelectionChanged();
    }
}
 
  
void MyGLCanvas::setShape(OBJ_TYPE type, bool isChild) {
    objType = type;

    if (isChild && selectedObjId != -1) {
      auto it = std::find_if(objectList.begin(), objectList.end(),
                       [this](ObjectNode* obj) { return obj->id == this->selectedObjId; });


        if (it != objectList.end()) {
            addObject(type, *it);
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
    
    glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
    glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f); 

   
    glm::vec3 p = glm::vec3(eyePointPO);
    glm::vec3 d = glm::normalize(glm::vec3(rayVO));

    
    std::vector<double> results;


    double radius = 0.5;
    double radiusSq = radius * radius;

  
    double A = glm::dot(d, d); 
    double B = 2.0 * glm::dot(d, p);
    double C = glm::dot(p, p) - radiusSq;

    // Compute the discriminant
    double discriminant = (B * B) - (4.0 * A * C);

    // Handle cases based on the discriminant
    if (discriminant < -EPSILON)
    {
       
        return results;
    }
    else if (std::fabs(discriminant) < EPSILON)
    {
   
        double t = -B / (2.0 * A);
        if (t > EPSILON)
        {
            results.push_back(t);
        }
        
    }
    else
    {
      
        double sqrtDiscriminant = std::sqrt(discriminant);

        // Compute both roots
        double t1 = (-B - sqrtDiscriminant) / (2.0 * A);
        double t2 = (-B + sqrtDiscriminant) / (2.0 * A);

        
        if (t1 > EPSILON)
        {
            results.push_back(t1);
        }
        if (t2 > EPSILON && std::fabs(t2 - t1) > EPSILON)
        {
            results.push_back(t2);
        }
    }


    std::sort(results.begin(), results.end());

    return results;
}

std::vector<double> MyGLCanvas::intersectWithCube(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{
    
    glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
    glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f); 

  
    glm::vec3 p = glm::vec3(eyePointPO);
    glm::vec3 d = glm::normalize(glm::vec3(rayVO));

   
    std::vector<double> results;

   
    double cube_min = -0.5;
    double cube_max = 0.5;

   
    double t_min = -std::numeric_limits<double>::infinity();
    double t_max = std::numeric_limits<double>::infinity();

  
    double bounds[2] = { cube_min, cube_max };

    
    for (int i = 0; i < 3; ++i)
    {
        double origin = p[i];
        double direction = d[i];
        double slab_min = bounds[0];
        double slab_max = bounds[1];

        if (std::fabs(direction) < EPSILON)
        {
            
            if (origin < slab_min || origin > slab_max)
            {
                return results; 
            }
           
        }
        else
        {
            
            double t1 = (slab_min - origin) / direction;
            double t2 = (slab_max - origin) / direction;

            
            if (t1 > t2)
            {
                std::swap(t1, t2);
            }

            
            if (t1 > t_min)
            {
                t_min = t1;
            }
            if (t2 < t_max)
            {
                t_max = t2;
            }

          
            if (t_min > t_max)
            {
                return results; 
            }
        }
    }

    
    if (t_max < EPSILON)
    {
       
        return results; 
    }

  
    if (t_min > EPSILON)
    {
        results.push_back(t_min);
    }

    if (t_max > EPSILON && std::fabs(t_max - t_min) > EPSILON)
    {
        results.push_back(t_max);
    }

  
    std::sort(results.begin(), results.end());

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
        node->translate = glm::vec3(0.0f, 0.5f, 0.0f);

        parent->children.push_back(node);
    }

    // Add node to the flattened list
    objectList.push_back(node);

    redraw();
}



std::vector<double> MyGLCanvas::intersectWithCylinder(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{

    glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
    glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f); 


    glm::vec3 p = glm::vec3(eyePointPO);
    glm::vec3 d = glm::normalize(glm::vec3(rayVO));

  
    std::vector<double> results;

    
    double radius = 0.5;
    double radiusSq = radius * radius;
    double y_min = -0.5;
    double y_max = 0.5;

    
    double A = d.x * d.x + d.z * d.z;
    double B = 2.0 * (p.x * d.x + p.z * d.z);
    double C = p.x * p.x + p.z * p.z - radiusSq;

    
    double discriminant = B * B - 4.0 * A * C;

    // if a is zero,indicating the ray is parallel to the cylinder's axis
    if (std::fabs(A) < EPSILON)
    {
        if (std::fabs(B) < EPSILON)
        {
            // Both  a and b ; the ray is parallel to the cylinder's axis and outside the cylinder
            // No valid intersection with the lateral surface
        }
        else
        {
         
            double t = -C / B;
            if (t > EPSILON)
            {
                glm::vec3 intersection = p + static_cast<float>(t) * d;
             
                if (intersection.y >= y_min && intersection.y <= y_max)
                {
                    results.push_back(t);
                }
            }
        }
    }
    else
    {
        if (discriminant >= 0.0)
        {
            double sqrtDiscriminant = std::sqrt(discriminant);

            // Two possible solutions
            double t1 = (-B - sqrtDiscriminant) / (2.0 * A);
            double t2 = (-B + sqrtDiscriminant) / (2.0 * A);

           
            if (t1 > EPSILON)
            {
                glm::vec3 intersection1 = p + static_cast<float>(t1) * d;
               
                if (intersection1.y >= y_min && intersection1.y <= y_max)
                {
                    results.push_back(t1);
                }
            }

            if (t2 > EPSILON && std::fabs(t2 - t1) > EPSILON)
            {
                glm::vec3 intersection2 = p + static_cast<float>(t2) * d;
               
                if (intersection2.y >= y_min && intersection2.y <= y_max)
                {
                    results.push_back(t2);
                }
            }
        }
    }

    
    auto checkCapIntersection = [&](double y_cap, double t_cap) -> void
    {
        if (t_cap > EPSILON)
        {
            glm::vec3 intersection = p + static_cast<float>(t_cap) * d;
            // Check if the intersection point lies within the circular cap
            if ((intersection.x * intersection.x + intersection.z * intersection.z) <= radiusSq)
            {
                results.push_back(t_cap);
            }
        }
    };

 
    if (std::fabs(d.y) > EPSILON)
    {
        double t_top = (y_max - p.y) / d.y;
        checkCapIntersection(y_max, t_top);

       
        double t_bottom = (y_min - p.y) / d.y;
        checkCapIntersection(y_min, t_bottom);
    }

   
    std::sort(results.begin(), results.end());

    return results;
}
std::vector<double> MyGLCanvas::intersectWithCone(glm::vec3 eyePointP, glm::vec3 rayV, glm::mat4 transformMatrix)
{

    glm::vec4 eyePointPO = transformMatrix * glm::vec4(eyePointP, 1.0f);
    glm::vec4 rayVO = transformMatrix * glm::vec4(rayV, 0.0f);

  
    glm::vec3 p = glm::vec3(eyePointPO);
    glm::vec3 d = glm::normalize(glm::vec3(rayVO));

    // Initialize a vector to store intersection t values
    std::vector<double> results;


    double theta = M_PI / 4.0; 
    double cosTheta = std::cos(theta);
    double cosThetaSq = cosTheta * cosTheta;
    double sinThetaSq = 1.0 - cosThetaSq;

    

    double d_dot_a = d.y; 
    double p_dot_a = p.y;

    double a = d.x * d.x + d.z * d.z - cosThetaSq * d.y * d.y;
    double b = 2.0 * (p.x * d.x + p.z * d.z - cosThetaSq * p.y * d.y);
    double c = p.x * p.x + p.z * p.z - cosThetaSq * p.y * p.y;

   
    double discriminant = b * b - 4.0 * a * c;

    // Check if 'a' is near zero, indicating the ray is parallel to the cone's surface
    if (std::fabs(a) < EPSILON)
    {
        if (std::fabs(b) < EPSILON)
        {
            // Both 'a' and 'b' are near zero; no valid intersection
            // The ray lies on the surface of the cone or does not intersect
        }
        else
        {
            
            double t = -c / b;
            if (t > EPSILON)
            {
                glm::vec3 intersection = p + static_cast<float>(t) * d;
                if (intersection.y >= -0.5f && intersection.y <= 0.5f)
                {
                    results.push_back(t);
                }
            }
        }
    }
    else
    {
        if (discriminant >= 0.0)
        {
            double sqrtDiscriminant = std::sqrt(discriminant);

            double t1 = (-b - sqrtDiscriminant) / (2.0 * a);
            double t2 = (-b + sqrtDiscriminant) / (2.0 * a);

           
            if (t1 > EPSILON)
            {
                glm::vec3 intersection1 = p + static_cast<float>(t1) * d;
              
                if (intersection1.y >= -0.5f && intersection1.y <= 0.5f)
                {
                    results.push_back(t1);
                }
            }

            if (t2 > EPSILON && std::fabs(t2 - t1) > EPSILON)
            {
                glm::vec3 intersection2 = p + static_cast<float>(t2) * d;
                
                if (intersection2.y >= -0.5f && intersection2.y <= 0.5f)
                {
                    results.push_back(t2);
                }
            }
        }
    }

    
    if (std::fabs(d.y) > EPSILON)
    {
        double t_cap = (-0.5 - p.y) / d.y;
        if (t_cap > EPSILON)
        {
            glm::vec3 intersection_cap = p + static_cast<float>(t_cap) * d;
            // Check if the intersection point lies within the circular base
            if ((intersection_cap.x * intersection_cap.x + intersection_cap.z * intersection_cap.z) <= 0.25)
            {
                results.push_back(t_cap);
            }
        }
    }

    
    std::sort(results.begin(), results.end());

    return results;
}