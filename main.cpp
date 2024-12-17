#include <string>
#include <iostream>
#include <fstream>
#include <math.h>
#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Pack.H>
#include <FL/Fl_Check_Button.H>
#include <FL/Fl_Value_Slider.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Gl_Window.H>
#include <FL/names.h>
#include <FL/gl.h>
#include <FL/glut.h>
#include <FL/glu.h>

#include <FL/Fl_Round_Button.H>
#include <FL/Fl_Value_Slider.H>

#include "MyGLCanvas.h"
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm> // For std::find_if

using namespace std;

// Forward declaration
class MyAppWindow;
MyAppWindow *win;

// Updated to handle ObjectNode* instead of ObjectNode
class MyAppWindow : public Fl_Window {
public:
    Fl_Button* wireButton;
    Fl_Button* drawButton;
    Fl_Button *smoothButton;
    Fl_Button *fillButton;
    Fl_Button *normalButton;
    Fl_Button* resetSceneButton;
    Fl_Slider *rotXSlider;
    Fl_Slider *rotYSlider;
    Fl_Slider *rotZSlider;
    Fl_Slider *CamerarotXSlider;
    Fl_Slider *CamerarotYSlider;
    Fl_Slider *CamerarotZSlider;
    Fl_Slider *scaleSlider;
    Fl_Slider *moveUSlider;
    Fl_Slider *moveDSlider;
    Fl_Slider *moveLSlider;
    Fl_Slider *moveRSlider;
    Fl_Slider *moveXYSlider;
    Fl_Slider *moveYZSlider;

	Fl_Button* deleteButton;

    Fl_Slider *scaleXSlider;
    Fl_Slider *scaleYSlider;
    Fl_Slider *scaleZSlider;
    Fl_Check_Button* uniformScaleToggle;

    Fl_Button* addChildButton;

    Fl_Slider *angleSlider;
    MyGLCanvas* canvas;

    Fl_Slider* rSlider;
    Fl_Slider* gSlider;
    Fl_Slider* bSlider;
    Fl_Slider* sizeSlider;

    Fl_Button* isectButton;
    Fl_Button* renderButton;
    Fl_Slider* maxRecursionDepthSlider;
    Fl_Button* addShapeButton;
    Fl_Slider* segmentsXSlider;
    Fl_Slider* segmentsYSlider;

public:
    // APP WINDOW CONSTRUCTOR
    MyAppWindow(int W, int H, const char*L = 0);

    static void idleCB(void* userdata) {
        win->canvas->redraw();
    }

    void updateSelectedObject() {
        int selectedId = canvas->selectedObjId;
        if (selectedId != -1) {
            // Updated to handle ObjectNode*
            auto it = std::find_if(canvas->objectList.begin(),
                                   canvas->objectList.end(),
                                   [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

            if (it != canvas->objectList.end()) {
                ObjectNode* obj = *it; // Dereference pointer

                // Update the sliders to reflect the selected object's color
                rSlider->value(obj->red);
                gSlider->value(obj->green);
                bSlider->value(obj->blue);

                rotXSlider->value(obj->rotation.x);
                rotYSlider->value(obj->rotation.y);
                rotZSlider->value(obj->rotation.z);

                scaleXSlider->value(obj->scale.x);
                scaleYSlider->value(obj->scale.y);
                scaleZSlider->value(obj->scale.z);
                scaleSlider->value(obj->scale.x); // Assuming uniform scale

                // Redraw them to show the updated value
                rSlider->redraw();
                gSlider->redraw();
                bSlider->redraw();

                rotXSlider->redraw();
                rotYSlider->redraw();
                rotZSlider->redraw();

                scaleSlider->redraw();
                scaleXSlider->redraw();
                scaleYSlider->redraw();
                scaleZSlider->redraw();
            }
        }
    }

private:

    static void uniformScaleToggleCB(Fl_Widget *w, void *userdata) {
        // Handle toggle for uniform scaling
        if (((Fl_Check_Button *)w)->value() == 1) {
            win->scaleXSlider->deactivate();
            win->scaleYSlider->deactivate();
            win->scaleZSlider->deactivate();
            win->scaleSlider->activate();
        } else {
            win->scaleXSlider->activate();
            win->scaleYSlider->activate();
            win->scaleZSlider->activate();
            win->scaleSlider->deactivate();
        }
    }

    // Updated to handle ObjectNode* instead of ObjectNode&
    static void toggleCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Button*)w)->value();
        printf("Toggle Button value: %d\n", value);
        *((int*)userdata) = value;
    }

    static void sliderCB(Fl_Widget* w, void* userdata) {
        float value = ((Fl_Slider*)w)->value();
        printf("Slider value: %f\n", value);
        *((float*)userdata) = value;
    }

    static void sliderFloatCB(Fl_Widget *w, void *userdata) {
        float value = ((Fl_Slider *)w)->value();
        printf("Float Slider value: %f\n", value);
        *((float *)userdata) = value;
    }

    static void resetSceneCB(Fl_Widget* w, void* data) {
        cout << "Reset Scene" << endl;
        win->canvas->resetScene();
        win->canvas->redraw();
    }
	static void delete_button_callback(Fl_Widget* w, void* data) {
		win->canvas->deleteSelectedObject();
    
}


    static void redCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("Red Slider value: %d\n", value);
        int selectedId = win->canvas->selectedObjId;

        if (selectedId != -1) {
            auto it = std::find_if(
                win->canvas->objectList.begin(),
                win->canvas->objectList.end(),
                [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

            if (it != win->canvas->objectList.end()) {
                ObjectNode* obj = *it;
                obj->red = win->rSlider->value();
                win->canvas->redraw();
            }
        }
    }

    static void greenCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("Green Slider value: %d\n", value);
        int selectedId = win->canvas->selectedObjId;

        if (selectedId != -1) {
            auto it = std::find_if(
                win->canvas->objectList.begin(),
                win->canvas->objectList.end(),
                [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

            if (it != win->canvas->objectList.end()) {
                ObjectNode* obj = *it;
                obj->green = win->gSlider->value();
                win->canvas->redraw();
            }
        }
    }

    static void blueCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("Blue Slider value: %d\n", value);
        int selectedId = win->canvas->selectedObjId;

        if (selectedId != -1) {
            auto it = std::find_if(
                win->canvas->objectList.begin(),
                win->canvas->objectList.end(),
                [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

            if (it != win->canvas->objectList.end()) {
                ObjectNode* obj = *it;
                obj->blue = win->bSlider->value();
                win->canvas->redraw();
            }
        }
    }

    static void sizeCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("Size Slider value: %d\n", value);
        // TODO: add size property
    }

    static void renderCB(Fl_Widget* w, void* data) {
        cout << "Render Scene" << endl;
        // TODO: add ray tracing
        // win->canvas->renderScene();
    }

    static void maxRecursionDepthSliderCB(Fl_Widget* w, void* userdata) {
        int value = win->maxRecursionDepthSlider->value();
        win->canvas->maxRecursionDepth = value;
    }

    static void segmentsCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("Segments Slider value: %d\n", value);
        *((int*)userdata) = value;
        win->canvas->setSegments();
    }

    static void rotationSliderCB(Fl_Widget* w, void* userdata) {
        int selectedId = win->canvas->selectedObjId;

        if (selectedId != -1) {
            auto it = std::find_if(
                win->canvas->objectList.begin(),
                win->canvas->objectList.end(),
                [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

            if (it != win->canvas->objectList.end()) {
                ObjectNode* obj = *it;
                obj->rotation.x = win->rotXSlider->value();
                obj->rotation.y = win->rotYSlider->value();
                obj->rotation.z = win->rotZSlider->value();
                win->canvas->redraw();
            }
        }
    }

    static void scaleSliderCB(Fl_Widget *w, void *userdata) {
        int selectedId = win->canvas->selectedObjId;

        if (selectedId != -1) {
            auto it = std::find_if(
                win->canvas->objectList.begin(),
                win->canvas->objectList.end(),
                [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

            if (it != win->canvas->objectList.end()) {
                ObjectNode* obj = *it;
                if (w == win->scaleSlider) {
                    // Uniform scaling
                    float uniformScale = win->scaleSlider->value();
                    obj->scale.x = uniformScale;
                    obj->scale.y = uniformScale;
                    obj->scale.z = uniformScale;
                } else if (w == win->scaleXSlider) {
                    obj->scale.x = win->scaleXSlider->value();
                } else if (w == win->scaleYSlider) {
                    obj->scale.y = win->scaleYSlider->value();
                } else if (w == win->scaleZSlider) {
                    obj->scale.z = win->scaleZSlider->value();
                }

                win->canvas->redraw();
            }
        }
    }

    static void cameraRotateCB(Fl_Widget* w, void* userdata) {
        win->canvas->camera.setRotUVW(win->CamerarotXSlider->value(), win->CamerarotYSlider->value(), win->CamerarotZSlider->value());
        win->canvas->redraw();
    }

    static void angleSliderCB(Fl_Widget* w, void* userdata) {
        float angle = win->angleSlider->value();
        win->canvas->camera.setViewAngle(angle);
        win->canvas->redraw();
    }
};

// Callback for radio buttons (if implemented)
static void radioButtonCB(Fl_Widget* w, void* userdata) {
    // Implement shape selection if needed
}

// Updated to handle ObjectNode* instead of ObjectNode&
static void addChildCB(Fl_Widget* w, void* userdata) {
    MyAppWindow* window = (MyAppWindow*)userdata;
    int selectedId = window->canvas->selectedObjId;

    if (selectedId == -1) {
        printf("No object selected! Please select a parent object first.\n");
        return;
    }

    // Find the selected object
    auto it = std::find_if(window->canvas->objectList.begin(),
                           window->canvas->objectList.end(),
                           [selectedId](ObjectNode* obj) { return obj->id == selectedId; });

    if (it == window->canvas->objectList.end()) {
        printf("Selected object not found.\n");
        return;
    }

    // Create the shape selection window
    Fl_Window* shapeWindow = new Fl_Window(300, 200, "Select Shape for Child");
    shapeWindow->user_data((void*)window);

    Fl_Box* shapeLabel = new Fl_Box(50, 50, 80, 25, "Shape:");
    shapeLabel->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);

    Fl_Choice* shapeChoice = new Fl_Choice(140, 50, 120, 25, "");
    shapeChoice->add("Cube");
    shapeChoice->add("Sphere");
    shapeChoice->add("Cylinder");
    shapeChoice->add("Cone");
    shapeChoice->value(0); // Default selection

    Fl_Button* okButton = new Fl_Button(60, 150, 80, 30, "OK");
    Fl_Button* cancelButton = new Fl_Button(160, 150, 80, 30, "Cancel");

    // Callback for the OK button
    okButton->callback([](Fl_Widget* btn, void* data) {
        Fl_Window* win = (Fl_Window*)data;
        MyAppWindow* appWin = (MyAppWindow*)win->user_data();
        Fl_Choice* choice = (Fl_Choice*)win->child(1);

        // Get the selected shape
        int selectedShape = choice->value();
        OBJ_TYPE type;
        switch (selectedShape) {
            case 0: type = SHAPE_CUBE; break;
            case 1: type = SHAPE_SPHERE; break;
            case 2: type = SHAPE_CYLINDER; break;
            case 3: type = SHAPE_CONE; break;
            default: type = SHAPE_SPHERE; break;
        }

        // Find the selected parent node
        int parentId = appWin->canvas->selectedObjId;
        auto parentIt = std::find_if(appWin->canvas->objectList.begin(),
                                     appWin->canvas->objectList.end(),
                                     [parentId](ObjectNode* obj) { return obj->id == parentId; });

        if (parentIt != appWin->canvas->objectList.end()) {
            // Add the child node
            appWin->canvas->setShape(type, *parentIt); // Pass the parent pointer
            printf("Child shape added to object ID %d\n", parentId);
        }

        win->hide();
    }, shapeWindow);

    // Callback for the Cancel button
    cancelButton->callback([](Fl_Widget* btn, void* data) {
        Fl_Window* win = (Fl_Window*)data;
        win->hide(); // Close the window
    }, shapeWindow);

    shapeWindow->end();
    shapeWindow->set_modal();
    shapeWindow->show();
}

// Helper function to get the corresponding name of that enum index
std::string getPrimitiveTypeName(int type) {
    switch (type) {
        case SHAPE_CUBE:
            return "cube";
        case SHAPE_CYLINDER:
            return "cylinder";
        case SHAPE_CONE:
            return "cone";
        case SHAPE_SPHERE:
            return "sphere";
        default:
            return "unknown";
    }
}

// Updated to handle ObjectNode* instead of ObjectNode&
static void writeSceneToXML() {
    printf("Creating XML\n");
    string filename = "scene" + to_string(win->canvas->fileIndex) + ".xml";
    win->canvas->fileIndex++;
    ofstream MyFile(filename);
    std::ostringstream xml;	// Write to the file

    // Start of the XML file
    xml << "<scenefile>\n";

    // Write global data
    xml << "<globaldata>\n"
        << "<diffusecoeff v=\"0.5\"/>\n"
        << "<specularcoeff v=\"0.5\"/>\n"
        << "<ambientcoeff v=\"0.5\"/>\n"
        << "</globaldata>\n";

    // Add light data
    xml << "<lightdata>\n";
    xml << "<id v=\"0\"/>\n";
    xml << "<color r=\"" << 0.5
        << "\" g=\"" << 0.5
        << "\" b=\"" << 0.5 << "\"/>\n";
    xml << "<position x=\"" << win->canvas->eyePosition.x 
        << "\" y=\"" << win->canvas->eyePosition.y
        << "\" z=\"" << win->canvas->eyePosition.z << "\"/>\n";
    xml << "    </lightdata>\n";

    // Write camera data
    glm::vec3 eyePos = win->canvas->camera.getEyePoint();
    glm::vec3 upVec = win->canvas->camera.getUpVector();
    float viewAngle = win->canvas->camera.getViewAngle();

    xml << "<cameradata>\n"
        << "<pos x=\"" << eyePos.x << "\" y=\"" << eyePos.y << "\" z=\"" << eyePos.z << "\"/>\n"
        << "<up x=\"" << upVec.x << "\" y=\"" << upVec.y << "\" z=\"" << upVec.z << "\"/>\n"
        << "<heightangle v=\"" << viewAngle << "\"/>\n"
        << "</cameradata>\n";

    // Start the root object
    xml << "<object type=\"tree\" name=\"root\">\n";

    // Loop through the object list
    for (const auto& obj : win->canvas->objectList) {
        xml << "<transblock>\n";

        // Write scale transformation
        xml << "<scale x=\"" << obj->scale.x
            << "\" y=\"" << obj->scale.y
            << "\" z=\"" << obj->scale.z << "\"/>\n";

        // Write translation transformation
        xml << "<translate x=\"" << obj->translate.x
            << "\" y=\"" << obj->translate.y
            << "\" z=\"" << obj->translate.z << "\"/>\n";

        // Write the primitive object
        xml << "<object type=\"primitive\" name=\"" << getPrimitiveTypeName(obj->primitive->getType()) << "\">\n";

        // Write color information (assuming diffuse color for simplicity)
        xml << "<diffuse r=\"" << obj->red / 255.0f
            << "\" g=\"" << obj->green / 255.0f
            << "\" b=\"" << obj->blue / 255.0f << "\"/>\n";

        xml << "</object>\n";

        xml << "</transblock>\n";
    }

    // End the root object
    xml << "</object>\n";

    // End of the XML file
    xml << "</scenefile>\n";

    // Write to file
    std::ofstream outFile(filename);
    if (outFile.is_open()) {
        outFile << xml.str();
        outFile.close();
        std::cout << "Scene written to " << filename << std::endl;
    } else {
        std::cerr << "Error: Could not open file " << filename << " for writing.\n";
    }
}

static void addShapeCB(Fl_Widget* w, void* userdata){
    MyAppWindow* window = (MyAppWindow*)userdata;
    Fl_Window* shapeWindow = new Fl_Window(300, 200, "Select Shape");
    shapeWindow->user_data((void*)window);

    Fl_Box* shapeLabel = new Fl_Box(50, 50, 80, 25, "Shape:");
    shapeLabel->align(FL_ALIGN_RIGHT | FL_ALIGN_INSIDE);
    Fl_Choice* shapeChoice = new Fl_Choice(140, 50, 120, 25, "");
    shapeChoice->add("Cube");
    shapeChoice->add("Sphere");
    shapeChoice->add("Cylinder");
    shapeChoice->add("Cone");
    shapeChoice->value(0); // Default selection
    Fl_Button* okButton = new Fl_Button(60, 150, 80, 30, "OK");
    Fl_Button* cancelButton = new Fl_Button(160, 150, 80, 30, "Cancel");

    okButton->callback([](Fl_Widget* btn, void* data) {
        Fl_Window* win = (Fl_Window*)data;
        MyAppWindow* appWin = (MyAppWindow*)win->user_data();
        Fl_Choice* choice = (Fl_Choice*)win->child(1);
        int selected = choice->value();
        switch (selected) {
            case 0: // Cube
                printf("Selected Cube\n");
                appWin->canvas->setShape(SHAPE_CUBE);
                break;
            case 1: // Sphere
                printf("Selected Sphere\n");
                appWin->canvas->setShape(SHAPE_SPHERE);
                break;
            case 2: // Cylinder
                printf("Selected Cylinder\n");
                appWin->canvas->setShape(SHAPE_CYLINDER);
                break;
            case 3: // Cone
                printf("Selected Cone\n");
                appWin->canvas->setShape(SHAPE_CONE);
                break;
        }
        win->hide(); 
    }, shapeWindow);

    cancelButton->callback([](Fl_Widget* btn, void* data) {
        Fl_Window* win = (Fl_Window*)data;
        win->hide(); // Close the shape selection window
    }, shapeWindow);
    shapeWindow->end();
    shapeWindow->set_modal(); // Make it a modal dialog
    shapeWindow->show();
}

MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
    begin();
    // OpenGL window
    // Adjusted to match pointer-based objectList
    canvas = new MyGLCanvas(10, 10, w() - 320, h() - 20);

    Fl_Pack* pack = new Fl_Pack(w() - 310, 30, 150, h() - 40, "Controls"); // Adjusted height
    pack->box(FL_DOWN_FRAME);
    pack->type(Fl_Pack::VERTICAL);
    pack->spacing(10); // Reduced spacing for better layout
    pack->begin();

    addChildButton = new Fl_Button(10, 10, pack->w() - 20, 30, "Add Child");
    addChildButton->callback(addChildCB, (void*)this);

	deleteButton = new Fl_Button(10, 50, pack->w() - 20, 30, "Delete Object");
	deleteButton->callback(delete_button_callback, (void*)this);


    // Reset Scene Button
    resetSceneButton = new Fl_Button(10, 50, pack->w() - 20, 30, "Reset Scene");
    resetSceneButton->callback(resetSceneCB, (void*)this);

    // Wireframe Toggle
    wireButton = new Fl_Check_Button(10, 90, pack->w() - 20, 30, "Wireframe");
    wireButton->callback(toggleCB, (void*)(&(canvas->wireframe)));
    wireButton->value(canvas->wireframe);

    // Add Shape Button
    addShapeButton = new Fl_Button(10, 130, pack->w() - 20, 30, "Add Shape");
    addShapeButton->callback(addShapeCB, (void*)this);	// primitive properties

    // SegmentsX Slider
    Fl_Box* segmentsXTextbox = new Fl_Box(10, 170, pack->w() - 20, 20, "Segments X:");
    segmentsXTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    segmentsXSlider = new Fl_Value_Slider(10, 200, pack->w() - 20, 20, "");
    segmentsXSlider->align(FL_ALIGN_TOP);
    segmentsXSlider->type(FL_HOR_SLIDER);
    segmentsXSlider->bounds(3, 60);
    segmentsXSlider->step(1);
    segmentsXSlider->value(canvas->segmentsX);
    segmentsXSlider->callback(segmentsCB, (void*)(&(canvas->segmentsX)));

    // SegmentsY Slider
    Fl_Box* segmentsYTextbox = new Fl_Box(10, 230, pack->w() - 20, 20, "Segments Y:");
    segmentsYTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    segmentsYSlider = new Fl_Value_Slider(10, 260, pack->w() - 20, 20, "");
    segmentsYSlider->align(FL_ALIGN_TOP);
    segmentsYSlider->type(FL_HOR_SLIDER);
    segmentsYSlider->bounds(3, 60);
    segmentsYSlider->step(1);
    segmentsYSlider->value(canvas->segmentsY);
    segmentsYSlider->callback(segmentsCB, (void*)(&(canvas->segmentsY)));

    // Color Sliders
    // Red Slider
    Fl_Box* redBox = new Fl_Box(10, 290, pack->w() - 20, 20, "Red:");
    redBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    rSlider = new Fl_Value_Slider(10, 320, pack->w() - 20, 20, "");
    rSlider->align(FL_ALIGN_TOP);
    rSlider->type(FL_HOR_SLIDER);
    rSlider->bounds(0, 255);
    rSlider->step(1);
    rSlider->value(255); // Default value set to 255
    rSlider->callback(redCB);

    // Green Slider
    Fl_Box* greenBox = new Fl_Box(10, 350, pack->w() - 20, 20, "Green:");
    greenBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    gSlider = new Fl_Value_Slider(10, 380, pack->w() - 20, 20, "");
    gSlider->align(FL_ALIGN_TOP);
    gSlider->type(FL_HOR_SLIDER);
    gSlider->bounds(0, 255);
    gSlider->step(1);
    gSlider->value(255); // Default value set to 255
    gSlider->callback(greenCB);

    // Blue Slider
    Fl_Box* blueBox = new Fl_Box(10, 410, pack->w() - 20, 20, "Blue:");
    blueBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    bSlider = new Fl_Value_Slider(10, 440, pack->w() - 20, 20, "");
    bSlider->align(FL_ALIGN_TOP);
    bSlider->type(FL_HOR_SLIDER);
    bSlider->bounds(0, 255);
    bSlider->step(1);
    bSlider->value(255); // Default value set to 255
    bSlider->callback(blueCB);

    // Create XML Button
    drawButton = new Fl_Button(10, 470, pack->w() - 20, 30, "Create XML");
    drawButton->callback((Fl_Callback*)writeSceneToXML);

    pack->end();

    // Second Column Pack for Orientation and Camera Controls
    Fl_Pack* packCol2 = new Fl_Pack(w() - 155, 30, 150, h() - 40);
    packCol2->box(FL_DOWN_FRAME);
    packCol2->type(Fl_Pack::VERTICAL);
    packCol2->spacing(10); // Reduced spacing for better layout
    packCol2->begin();

    // Orientation Pack
    Fl_Pack* objectPack = new Fl_Pack(10, 10, packCol2->w() - 20, 200, "Orientation");
    objectPack->box(FL_DOWN_FRAME);
    objectPack->labelfont(1);
    objectPack->type(Fl_Pack::VERTICAL);
    objectPack->spacing(5);
    objectPack->begin();

    // Rotate X Slider
    Fl_Box *rotXTextbox = new Fl_Box(10, 10, objectPack->w() - 20, 20, "Rotate X:");
    rotXTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    rotXSlider = new Fl_Value_Slider(10, 40, objectPack->w() - 20, 20, "");
    rotXSlider->align(FL_ALIGN_TOP);
    rotXSlider->type(FL_HOR_SLIDER);
    rotXSlider->bounds(-359, 359);
    rotXSlider->step(1);
    rotXSlider->value(canvas->rotVec.x);
    rotXSlider->callback(rotationSliderCB, (void*)this);

    // Rotate Y Slider
    Fl_Box *rotYTextbox = new Fl_Box(10, 70, objectPack->w() - 20, 20, "Rotate Y:");
    rotYTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    rotYSlider = new Fl_Value_Slider(10, 100, objectPack->w() - 20, 20, "");
    rotYSlider->align(FL_ALIGN_TOP);
    rotYSlider->type(FL_HOR_SLIDER);
    rotYSlider->bounds(-359, 359);
    rotYSlider->step(1);
    rotYSlider->value(canvas->rotVec.y);
    rotYSlider->callback(rotationSliderCB, (void*)this);

    // Rotate Z Slider
    Fl_Box *rotZTextbox = new Fl_Box(10, 130, objectPack->w() - 20, 20, "Rotate Z:");
    rotZTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    rotZSlider = new Fl_Value_Slider(10, 160, objectPack->w() - 20, 20, "");
    rotZSlider->align(FL_ALIGN_TOP);
    rotZSlider->type(FL_HOR_SLIDER);
    rotZSlider->bounds(-359, 359);
    rotZSlider->step(1);
    rotZSlider->value(canvas->rotVec.z);
    rotZSlider->callback(rotationSliderCB, (void*)this);

    // Uniform Scale Slider
    Fl_Box *scaleTextbox = new Fl_Box(10, 190, objectPack->w() - 20, 20, "Uniform Scale:");
    scaleTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    scaleSlider = new Fl_Value_Slider(10, 220, objectPack->w() - 20, 20, "");
    scaleSlider->align(FL_ALIGN_TOP);
    scaleSlider->type(FL_HOR_SLIDER);
    scaleSlider->bounds(0.1, 2.0);
    scaleSlider->step(0.1);
    scaleSlider->value(canvas->scale); // Assuming uniform scale
    scaleSlider->callback(scaleSliderCB, (void*)this);

    // Uniform Scale Toggle
    uniformScaleToggle = new Fl_Check_Button(10, 250, objectPack->w() - 20, 20, "Enable Uniform Scale");
    uniformScaleToggle->callback(uniformScaleToggleCB, (void *)this);

    // Scale X Slider
    Fl_Box *scaleXBox = new Fl_Box(10, 280, objectPack->w() - 20, 20, "Scale X:");
    scaleXBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    scaleXSlider = new Fl_Value_Slider(10, 310, objectPack->w() - 20, 20, "");
    scaleXSlider->align(FL_ALIGN_TOP);
    scaleXSlider->type(FL_HOR_SLIDER);
    scaleXSlider->bounds(0.1, 5.0);
    scaleXSlider->step(0.1);
    scaleXSlider->value(1.0);
    scaleXSlider->callback(scaleSliderCB, (void *)this);

    // Scale Y Slider
    Fl_Box *scaleYBox = new Fl_Box(10, 340, objectPack->w() - 20, 20, "Scale Y:");
    scaleYBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    scaleYSlider = new Fl_Value_Slider(10, 370, objectPack->w() - 20, 20, "");
    scaleYSlider->align(FL_ALIGN_TOP);
    scaleYSlider->type(FL_HOR_SLIDER);
    scaleYSlider->bounds(0.1, 5.0);
    scaleYSlider->step(0.1);
    scaleYSlider->value(1.0);
    scaleYSlider->callback(scaleSliderCB, (void *)this);

    // Scale Z Slider
    Fl_Box *scaleZBox = new Fl_Box(10, 400, objectPack->w() - 20, 20, "Scale Z:");
    scaleZBox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    scaleZSlider = new Fl_Value_Slider(10, 430, objectPack->w() - 20, 20, "");
    scaleZSlider->align(FL_ALIGN_TOP);
    scaleZSlider->type(FL_HOR_SLIDER);
    scaleZSlider->bounds(0.1, 5.0);
    scaleZSlider->step(0.1);
    scaleZSlider->value(1.0);
    scaleZSlider->callback(scaleSliderCB, (void *)this);

    objectPack->end();

    // Camera Pack
    Fl_Pack *cameraPack = new Fl_Pack(10, 460, packCol2->w() - 20, 200);
    cameraPack->box(FL_DOWN_FRAME);
    cameraPack->labelfont(1);
    cameraPack->type(Fl_Pack::VERTICAL);
    cameraPack->spacing(5);
    cameraPack->begin();

    // Camera Rotate U Slider
    Fl_Box *CamerarotXTextbox = new Fl_Box(10, 10, cameraPack->w() - 20, 20, "Camera U:");
    CamerarotXTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    CamerarotXSlider = new Fl_Value_Slider(10, 40, cameraPack->w() - 20, 20, "");
    CamerarotXSlider->align(FL_ALIGN_TOP);
    CamerarotXSlider->type(FL_HOR_SLIDER);
    CamerarotXSlider->bounds(-359, 359);
    CamerarotXSlider->step(1);
    CamerarotXSlider->value(canvas->camera.rotU);
    CamerarotXSlider->callback(cameraRotateCB, (void*)this);

    // Camera Rotate V Slider
    Fl_Box *CamerarotYTextbox = new Fl_Box(10, 70, cameraPack->w() - 20, 20, "Camera V:");
    CamerarotYTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    CamerarotYSlider = new Fl_Value_Slider(10, 100, cameraPack->w() - 20, 20, "");
    CamerarotYSlider->align(FL_ALIGN_TOP);
    CamerarotYSlider->type(FL_HOR_SLIDER);
    CamerarotYSlider->bounds(-359, 359);
    CamerarotYSlider->step(1);
    CamerarotYSlider->value(canvas->camera.rotV);
    CamerarotYSlider->callback(cameraRotateCB, (void*)this);

    // Camera Rotate W Slider
    Fl_Box *CamerarotZTextbox = new Fl_Box(10, 130, cameraPack->w() - 20, 20, "Camera W:");
    CamerarotZTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    CamerarotZSlider = new Fl_Value_Slider(10, 160, cameraPack->w() - 20, 20, "");
    CamerarotZSlider->align(FL_ALIGN_TOP);
    CamerarotZSlider->type(FL_HOR_SLIDER);
    CamerarotZSlider->bounds(-359, 359);
    CamerarotZSlider->step(1);
    CamerarotZSlider->value(canvas->camera.rotW);
    CamerarotZSlider->callback(cameraRotateCB, (void*)this);

    // View Angle Slider
    Fl_Box *viewAngelTextbox = new Fl_Box(10, 190, cameraPack->w() - 20, 20, "View Angle:");
    viewAngelTextbox->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
    angleSlider = new Fl_Value_Slider(10, 220, cameraPack->w() - 20, 20, "");
    angleSlider->align(FL_ALIGN_TOP);
    angleSlider->type(FL_HOR_SLIDER);
    angleSlider->bounds(1, 179);
    angleSlider->step(1);
    angleSlider->value(canvas->camera.getViewAngle());
    angleSlider->callback(angleSliderCB, (void*)this);

    cameraPack->end();
    packCol2->end();
    pack->end();
    packCol2->end();
    end();
}

/**************************************** main() ********************/
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    win = new MyAppWindow(900, 600, "Scene Builder 1.0");
    win->resizable(win->canvas); // Make the OpenGL canvas resizable
    // Set the callback to reflect the current value of selected object
    win->canvas->onSelectionChanged = []() {
        win->updateSelectedObject();
    };
    Fl::add_idle(MyAppWindow::idleCB);
    win->show();
    return(Fl::run());
}
