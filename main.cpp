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

using namespace std;

class MyAppWindow;
MyAppWindow *win;

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

	Fl_Slider *scaleXSlider;
	Fl_Slider *scaleYSlider;
	Fl_Slider *scaleZSlider;
	Fl_Check_Button* uniformScaleToggle;


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
			auto it = std::find_if(canvas->objectList.begin(),
								canvas->objectList.end(),
								[selectedId](const ObjectNode& obj) { return obj.id == selectedId; });

			if (it != canvas->objectList.end()) {
				// Update the sliders to reflect the selected object's color
				rSlider->value(it->red);
				gSlider->value(it->green);
				bSlider->value(it->blue);

				rotXSlider->value(it->rotation.x);
				rotYSlider->value(it->rotation.y);
				rotZSlider->value(it->rotation.z);

				
				scaleXSlider->value(it->scale.x);
                scaleYSlider->value(it->scale.y);
                scaleZSlider->value(it->scale.z);
                scaleSlider->value(it->scale.x);

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

	// Someone changed one of the sliders
	static void toggleCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Button*)w)->value();
		printf("value: %d\n", value);
		*((int*)userdata) = value;
	}

	static void sliderCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Slider*)w)->value();
		printf("value: %d\n", value);
		*((float*)userdata) = value;
	}

	static void sliderFloatCB(Fl_Widget *w, void *userdata) {
        float value = ((Fl_Slider *)w)->value();
        printf("value: %f\n", value);
        *((float *)userdata) = value;
    }

	static void resetSceneCB(Fl_Widget* w, void* data) {
        cout << "Reest Scene" << endl;
        win->canvas->resetScene();
        // win->updateGUIValues();
        win->canvas->redraw();
    }

	static void redCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
		int selectedId = win->canvas->selectedObjId;

		if (selectedId != -1) {
			auto it = std::find_if(
				win->canvas->objectList.begin(),
				win->canvas->objectList.end(),
				[selectedId](const ObjectNode& obj) { return obj.id == selectedId; });

			if (it != win->canvas->objectList.end()) {
				// Update the red values based on the slider
				it->red = win->rSlider->value();
				win->canvas->redraw();
			}
		}
    }

    static void greenCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
		int selectedId = win->canvas->selectedObjId;

		if (selectedId != -1) {
			auto it = std::find_if(
				win->canvas->objectList.begin(),
				win->canvas->objectList.end(),
				[selectedId](const ObjectNode& obj) { return obj.id == selectedId; });

			if (it != win->canvas->objectList.end()) {
				// Update the green values based on the slider
				it->green = win->gSlider->value();
				win->canvas->redraw();
			}
		}
    }

    static void blueCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
		int selectedId = win->canvas->selectedObjId;

		if (selectedId != -1) {
			auto it = std::find_if(
				win->canvas->objectList.begin(),
				win->canvas->objectList.end(),
				[selectedId](const ObjectNode& obj) { return obj.id == selectedId; });

			if (it != win->canvas->objectList.end()) {
				// Update the blue values based on the slider
				it->blue = win->bSlider->value();
				win->canvas->redraw();
			}
		}
    }

	static void sizeCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("value: %d\n", value);
		// TODO: add size property
    }

	static void renderCB(Fl_Widget*w, void*data) {
		cout << "render scene" << endl;
		// TODO: add ray tracing
		// win->canvas->renderScene();
	}
	
	static void maxRecursionDepthSliderCB(Fl_Widget* w, void* userdata) {
		int value = win->maxRecursionDepthSlider->value();
		win->canvas->maxRecursionDepth = value;
	}

	static void segmentsCB(Fl_Widget* w, void* userdata) {
		int value = ((Fl_Slider*)w)->value();
		printf("value: %d\n", value);
		*((int*)userdata) = value;
		win->canvas->setSegments();
	}

	static void rotationSliderCB(Fl_Widget* w, void* userdata) {
    int selectedId = win->canvas->selectedObjId;


    if (selectedId != -1) {
        auto it = std::find_if(
            win->canvas->objectList.begin(),
            win->canvas->objectList.end(),
            [selectedId](const ObjectNode& obj) { return obj.id == selectedId; });

        if (it != win->canvas->objectList.end()) {
          
            it->rotation.x = win->rotXSlider->value();
            it->rotation.y = win->rotYSlider->value();
            it->rotation.z = win->rotZSlider->value();
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
                [selectedId](const ObjectNode &obj) { return obj.id == selectedId; });

            if (it != win->canvas->objectList.end()) {
                if (w == win->scaleSlider) {
                    // Uniform scaling
                    float uniformScale = win->scaleSlider->value();
                    it->scale.x = uniformScale;
                    it->scale.y = uniformScale;
                    it->scale.z = uniformScale;
                } else if (w == win->scaleXSlider) {
                    it->scale.x = win->scaleXSlider->value();
                } else if (w == win->scaleYSlider) {
                    it->scale.y = win->scaleYSlider->value();
                } else if (w == win->scaleZSlider) {
                    it->scale.z = win->scaleZSlider->value();
                }

                win->canvas->redraw();
            }
        }
    }
	
    static void cameraRotateCB(Fl_Widget* w, void* userdata) {
        win->canvas->camera.setRotUVW(win->CamerarotXSlider->value(), win->CamerarotYSlider->value(), win->CamerarotZSlider->value());
        // glm::vec3 lookV = win->canvas->camera.getLookVector();
        // lookV = glm::normalize(lookV);
        // win->lookXSlider->value(lookV.x);
        // win->lookYSlider->value(lookV.y);
        // win->lookZSlider->value(lookV.z);
		win->canvas->redraw();
    }

    static void angleSliderCB(Fl_Widget* w, void* userdata) {
		float angle = win->angleSlider->value();        
		win->canvas->camera.setViewAngle(angle);
		win->canvas->redraw();
    }
};

//TODO: DRAW SHAPES
static void radioButtonCB(Fl_Widget* w, void* userdata) {
        // const char* value = ((Fl_Button*)w)->label();
        // if (strcmp("Cube", value) == 0) {
        //     win->canvas->setShape(SHAPE_CUBE);
        // }
        // else if (strcmp("Cylinder", value) == 0) {
        //     win->canvas->setShape(SHAPE_CYLINDER);
        // }
        // else if (strcmp("Cone", value) == 0) {
        //     win->canvas->setShape(SHAPE_CONE);
        // }
        // else if (strcmp("Sphere", value) == 0) {
        //     win->canvas->setShape(SHAPE_SPHERE);
        // }
        // else if (strcmp("Special", value) == 0) {
        //     win->canvas->setShape(SHAPE_SPECIAL1);
        // }
        // else if (strcmp("Scene", value) == 0) {
        //     win->canvas->setShape(SHAPE_SCENE);
        // }
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



// static void addShape() {
// 	//TODO: load the shape
// }

static void createXML() {
	//TODO: export to xml

}


MyAppWindow::MyAppWindow(int W, int H, const char*L) : Fl_Window(W, H, L) {
	begin();
	// OpenGL window

	canvas = new MyGLCanvas(10, 10, w() - 320, h() - 20);

	Fl_Pack* pack = new Fl_Pack(w() - 310, 30, 150, h(), "");
	pack->box(FL_DOWN_FRAME);
	pack->type(Fl_Pack::VERTICAL);
	pack->spacing(30);
	pack->begin();

    Fl_Pack* loadPack = new Fl_Pack(w() - 100, 30, 100, h(), "Reset");
    loadPack->box(FL_DOWN_FRAME);
    loadPack->labelfont(1);
    loadPack->type(Fl_Pack::VERTICAL);
    loadPack->spacing(0);
    loadPack->begin();

    resetSceneButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Reset Scene");
    resetSceneButton->callback(resetSceneCB, (void*)this);

    loadPack->end();

	// render options
    Fl_Pack *buttonsPack = new Fl_Pack(w() - 100, 30, 100, h(), "Render");
    buttonsPack->box(FL_DOWN_FRAME);
    buttonsPack->labelfont(1);
    buttonsPack->type(Fl_Pack::VERTICAL);
    buttonsPack->spacing(0);
    buttonsPack->begin();

	wireButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Wireframe");
	wireButton->callback(toggleCB, (void*)(&(canvas->wireframe)));
	wireButton->value(canvas->wireframe);

    smoothButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Smooth");
    smoothButton->value(canvas->smooth);
    smoothButton->callback(toggleCB, (void *)(&(canvas->smooth)));

    fillButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Fill");
    fillButton->value(canvas->fill);
    fillButton->callback(toggleCB, (void *)(&(canvas->fill)));

    normalButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "Show Normal");
    normalButton->value(canvas->normal);
    normalButton->callback(toggleCB, (void *)(&(canvas->normal)));

		renderButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Render!");
		renderButton->callback(renderCB, (void*)this);
		
		isectButton = new Fl_Check_Button(0, 0, pack->w() - 20, 20, "isectOnly");
		isectButton->value(canvas->isectOnly);
		isectButton->callback(toggleCB, (void*)(&(canvas->isectOnly)));

		maxRecursionDepthSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
		maxRecursionDepthSlider->align(FL_ALIGN_TOP);
		maxRecursionDepthSlider->type(FL_HOR_SLIDER);
		maxRecursionDepthSlider->step(1);
		maxRecursionDepthSlider->bounds(0, 5);
		maxRecursionDepthSlider->value(canvas->maxRecursionDepth);
		maxRecursionDepthSlider->callback(maxRecursionDepthSliderCB);

    buttonsPack->end();



	// options for drawing primitive
	Fl_Pack* radioPack = new Fl_Pack(w() - 100, 30, 100, h(), "Shape");
	
	addShapeButton = new Fl_Button(0, 0, pack->w() - 20, 25, "Select Shape");
    addShapeButton->callback(addShapeCB, (void*)this);	// primitive properties
	//slider for controlling number of segments in X
	Fl_Box* segmentsXTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "SegmentsX");
	segmentsXSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	segmentsXSlider->align(FL_ALIGN_TOP);
	segmentsXSlider->type(FL_HOR_SLIDER);
	segmentsXSlider->bounds(3, 60);
	segmentsXSlider->step(1);
	segmentsXSlider->value(canvas->segmentsX);
	segmentsXSlider->callback(segmentsCB, (void*)(&(canvas->segmentsX)));


	//slider for controlling number of segments in Y
	Fl_Box* segmentsYTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "SegmentsY");
	segmentsYSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	segmentsYSlider->align(FL_ALIGN_TOP);
	segmentsYSlider->type(FL_HOR_SLIDER);
	segmentsYSlider->bounds(3, 60);
	segmentsYSlider->step(1);
	segmentsYSlider->value(canvas->segmentsY);
	segmentsYSlider->callback(segmentsCB, (void*)(&(canvas->segmentsY)));

    Fl_Box* redBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Red");
	rSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rSlider->align(FL_ALIGN_TOP);
	rSlider->type(FL_HOR_SLIDER);
	rSlider->bounds(0, 255);
	rSlider->step(1);
	rSlider->value(255); // Default value set to 255
	rSlider->callback(redCB);

    Fl_Box* greenBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Green");
    gSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	gSlider->align(FL_ALIGN_TOP);
	gSlider->type(FL_HOR_SLIDER);
	gSlider->bounds(0, 255);
	gSlider->step(1);
	gSlider->value(255); // Default value set to 255
	gSlider->callback(greenCB);

    Fl_Box* blueBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Blue");
    bSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	bSlider->align(FL_ALIGN_TOP);
	bSlider->type(FL_HOR_SLIDER);
	bSlider->bounds(0, 255);
	bSlider->step(1);
	bSlider->value(255); // Default value set to 255
	bSlider->callback(blueCB);

   

	drawButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Create XML");
	drawButton->callback((Fl_Callback*)createXML);
    radioPack->end();

	pack->end();
	// second column slider
	Fl_Pack* packCol2 = new Fl_Pack(w() - 155, 30, 150, h(), "");
	packCol2->box(FL_DOWN_FRAME);
	packCol2->type(Fl_Pack::VERTICAL);
	packCol2->spacing(30);
	packCol2->begin();

	Fl_Pack *objectPack = new Fl_Pack(w() - 100, 30, 100, h(), "Orientation");
	objectPack->box(FL_DOWN_FRAME);
	objectPack->labelfont(1);
	objectPack->type(Fl_Pack::VERTICAL);
	objectPack->spacing(0);
	objectPack->begin();

	Fl_Box *rotXTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateX");
	rotXSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotXSlider->align(FL_ALIGN_TOP);
	rotXSlider->type(FL_HOR_SLIDER);
	rotXSlider->bounds(-359, 359);
	rotXSlider->step(1);
	rotXSlider->value(canvas->rotVec.x);
	// rotXSlider->callback(rotationSliderCB,(void *)(&(canvas->rotVec.x)));
	rotXSlider->callback(rotationSliderCB, (void*)this);

	Fl_Box *rotYTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateY");
	rotYSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotYSlider->align(FL_ALIGN_TOP);
	rotYSlider->type(FL_HOR_SLIDER);
	rotYSlider->bounds(-359, 359);
	rotYSlider->step(1);
	rotYSlider->value(canvas->rotVec.y);
	// rotYSlider->callback(rotationSliderCB, (void *)(&(canvas->rotVec.y)));
	rotYSlider->callback(rotationSliderCB, (void*)this);

	Fl_Box *rotZTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateZ");
	rotZSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotZSlider->align(FL_ALIGN_TOP);
	rotZSlider->type(FL_HOR_SLIDER);
	rotZSlider->bounds(-359, 359);
	rotZSlider->step(1);
	rotZSlider->value(canvas->rotVec.z);
	// rotZSlider->callback(rotationSliderCB,,(void *)(&(canvas->rotVec.z)));
	rotZSlider->callback(rotationSliderCB, (void*)this);

	Fl_Box *scaleTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "UniformScale");
	scaleSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	scaleSlider->align(FL_ALIGN_TOP);
	scaleSlider->type(FL_HOR_SLIDER);
	scaleSlider->bounds(0.1, 2);
	scaleSlider->value(canvas->scale);
	// scaleSlider->callback(sliderFloatCB, (void *)(&(canvas->scale)));
	scaleSlider->callback(scaleSliderCB, (void*)this);
	objectPack->end();


	uniformScaleToggle = new Fl_Check_Button(0, 0, pack->w() - 10,10 , "Enable Uniform Scale");
    uniformScaleToggle->callback(uniformScaleToggleCB, (void *)this);

	Fl_Box *scaleXBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Scale X");
    scaleXSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
    scaleXSlider->align(FL_ALIGN_TOP);
    scaleXSlider->type(FL_HOR_SLIDER);
    scaleXSlider->bounds(0.1, 5);
    scaleXSlider->value(1.0);
    scaleXSlider->callback(scaleSliderCB, (void *)this);

    Fl_Box *scaleYBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Scale Y");
    scaleYSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
    scaleYSlider->align(FL_ALIGN_TOP);
    scaleYSlider->type(FL_HOR_SLIDER);
    scaleYSlider->bounds(0.1, 5);
    scaleYSlider->value(1.0);
    scaleYSlider->callback(scaleSliderCB, (void *)this);

    Fl_Box *scaleZBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Scale Z");
    scaleZSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
    scaleZSlider->align(FL_ALIGN_TOP);
    scaleZSlider->type(FL_HOR_SLIDER);
    scaleZSlider->bounds(0.1, 5);
    scaleZSlider->value(1.0);
    scaleZSlider->callback(scaleSliderCB, (void *)this);
	
	// pack->end();




  	scaleSlider->deactivate();
    scaleXSlider->activate();
    scaleYSlider->activate();
    scaleZSlider->activate();
	uniformScaleToggle->value(0);
	Fl_Pack *cameraPack = new Fl_Pack(w() - 100, 30, 100, h(), "Camera");
	cameraPack->box(FL_DOWN_FRAME);
	cameraPack->labelfont(1);
	cameraPack->type(Fl_Pack::VERTICAL);
	cameraPack->spacing(0);
	cameraPack->begin();

	Fl_Box *CamerarotXTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Camera U");
	CamerarotXSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	CamerarotXSlider->align(FL_ALIGN_TOP);
	CamerarotXSlider->type(FL_HOR_SLIDER);
	CamerarotXSlider->bounds(-359, 359);
	CamerarotXSlider->step(1);
	CamerarotXSlider->value(canvas->camera.rotU);
	// rotXSlider->callback(rotationSliderCB,(void *)(&(canvas->rotVec.x)));
	CamerarotXSlider->callback(cameraRotateCB, (void*)this);

	Fl_Box *CamerarotYTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Camera V");
	CamerarotYSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	CamerarotYSlider->align(FL_ALIGN_TOP);
	CamerarotYSlider->type(FL_HOR_SLIDER);
	CamerarotYSlider->bounds(-359, 359);
	CamerarotYSlider->step(1);
	CamerarotYSlider->value(canvas->camera.rotV);
	// rotYSlider->callback(rotationSliderCB, (void *)(&(canvas->rotVec.y)));
	CamerarotYSlider->callback(cameraRotateCB, (void*)this);

	Fl_Box *CamerarotZTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Camera W");
	CamerarotZSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	CamerarotZSlider->align(FL_ALIGN_TOP);
	CamerarotZSlider->type(FL_HOR_SLIDER);
	CamerarotZSlider->bounds(-359, 359);
	CamerarotZSlider->step(1);
	CamerarotZSlider->value(canvas->camera.rotW);
	// rotZSlider->callback(rotationSliderCB,,(void *)(&(canvas->rotVec.z)));
	CamerarotZSlider->callback(cameraRotateCB, (void*)this);
	

	Fl_Box *viewAngelTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "View Angel");
	angleSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	angleSlider->align(FL_ALIGN_TOP);
	angleSlider->type(FL_HOR_SLIDER);
	angleSlider->bounds(1, 179);
	angleSlider->value(canvas->camera.viewAngle);
	// scaleSlider->callback(sliderFloatCB, (void *)(&(canvas->scale)));
	angleSlider->callback(angleSliderCB, (void*)this);
	cameraPack->end();
	packCol2->end();
	pack -> end();

	end();
}


/**************************************** main() ********************/
int main(int argc, char **argv) {
	glutInit(&argc, argv);
	win = new MyAppWindow(900, 600, "Dragging Object");
	win->resizable(win);
    // Set the callback to reflect the current value of selected object
	win->canvas->onSelectionChanged = []() {
		win->updateSelectedObject();
	};
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}