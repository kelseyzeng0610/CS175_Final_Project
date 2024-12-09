/*  =================== File Information =================
	File Name: main.cpp
	Description:
	Author: Michael Shah

	Purpose: Driver for 3D program to load .ply models 
	Usage:	
	===================================================== */

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
    Fl_Slider *scaleSlider;
	MyGLCanvas* canvas;

	Fl_Slider* rSlider;
    Fl_Slider* gSlider;
    Fl_Slider* bSlider;
	Fl_Slider* sizeSlider;

public:
	// APP WINDOW CONSTRUCTOR
	MyAppWindow(int W, int H, const char*L = 0);

	static void idleCB(void* userdata) {
		win->canvas->redraw();
	}

	void updateGUIValues() {
		//TODO: reset values to default
        // wireButton->value(canvas->wireframe);
        // fillButton->value(canvas->fill);
        // smoothButton->value(canvas->smooth);
        // normalButton->value(canvas->normal);

        // segmentsXSlider->value(canvas->segmentsX);
        // segmentsYSlider->value(canvas->segmentsY);

        // rotUSlider->value(canvas->camera->rotU);
        // rotVSlider->value(canvas->camera->rotV);
        // rotWSlider->value(canvas->camera->rotW);

        // eyeXSlider->value(canvas->camera->getEyePoint().x);
        // eyeYSlider->value(canvas->camera->getEyePoint().y);
        // eyeZSlider->value(canvas->camera->getEyePoint().z);

        // lookXSlider->value(canvas->camera->getLookVector().x);
        // lookYSlider->value(canvas->camera->getLookVector().y);
        // lookZSlider->value(canvas->camera->getLookVector().z);

        // nearSlider->value(canvas->camera->getNearPlane());
        // farSlider->value(canvas->camera->getFarPlane());
        // angleSlider->value(canvas->camera->getViewAngle());
    }

private:
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
        win->updateGUIValues();
        win->canvas->redraw();
    }

	static void redCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("value: %d\n", value);
        // TODO: add red color property
    }

    static void greenCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("value: %d\n", value);
		// TODO: add green color property
    }

    static void blueCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("value: %d\n", value);
		// TODO: add blue color property
    }

	static void sizeCB(Fl_Widget* w, void* userdata) {
        int value = ((Fl_Slider*)w)->value();
        printf("value: %d\n", value);
		// TODO: add size property
    }
};

//TODO: DRAW SHAPES
static void radioButtonCB(Fl_Widget* w, void* userdata) {
        const char* value = ((Fl_Button*)w)->label();
        if (strcmp("Cube", value) == 0) {
            win->canvas->setShape(SHAPE_CUBE);
        }
        else if (strcmp("Cylinder", value) == 0) {
            win->canvas->setShape(SHAPE_CYLINDER);
        }
        else if (strcmp("Cone", value) == 0) {
            win->canvas->setShape(SHAPE_CONE);
        }
        else if (strcmp("Sphere", value) == 0) {
            win->canvas->setShape(SHAPE_SPHERE);
        }
        // else if (strcmp("Special", value) == 0) {
        //     win->canvas->setShape(SHAPE_SPECIAL1);
        // }
        // else if (strcmp("Scene", value) == 0) {
        //     win->canvas->setShape(SHAPE_SCENE);
        // }
	}

static void addShape() {
	//TODO: load the shape
}

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

    buttonsPack->end();



	// options for drawing primitive
	Fl_Pack* radioPack = new Fl_Pack(w() - 100, 30, 100, h(), "Shape");
	radioPack->box(FL_DOWN_FRAME);
	radioPack->labelfont(1);
	radioPack->type(Fl_Pack::VERTICAL);
	radioPack->spacing(0);
	radioPack->begin();
	{ Fl_Round_Button* tmpButton = new Fl_Round_Button(0, 0, pack->w() - 20, 20, "Cube");
	tmpButton->type(102);
	tmpButton->down_box(FL_ROUND_DOWN_BOX);
	tmpButton->value(1);
	tmpButton->callback((Fl_Callback*)radioButtonCB);
	}
	{ Fl_Round_Button* tmpButton = new Fl_Round_Button(0, 0, pack->w() - 20, 20, "Cylinder");
	tmpButton->type(102);
	tmpButton->down_box(FL_ROUND_DOWN_BOX);
	tmpButton->callback((Fl_Callback*)radioButtonCB);
	}
	{ Fl_Round_Button* tmpButton = new Fl_Round_Button(0, 0, pack->w() - 20, 20, "Cone");
	tmpButton->type(102);
	tmpButton->down_box(FL_ROUND_DOWN_BOX);
	tmpButton->callback((Fl_Callback*)radioButtonCB);
	}
	{ Fl_Round_Button* tmpButton = new Fl_Round_Button(0, 0, pack->w() - 20, 20, "Sphere");
	tmpButton->type(102);
	tmpButton->down_box(FL_ROUND_DOWN_BOX);
	tmpButton->callback((Fl_Callback*)radioButtonCB);
	}
	// primitive properties
    Fl_Box* redBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Red");
	rSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rSlider->align(FL_ALIGN_TOP);
	rSlider->type(FL_HOR_SLIDER);
	rSlider->bounds(0, 255);
	rSlider->step(1);
	rSlider->callback(redCB);

    Fl_Box* greenBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Green");
    gSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	gSlider->align(FL_ALIGN_TOP);
	gSlider->type(FL_HOR_SLIDER);
	gSlider->bounds(0, 255);
	gSlider->step(1);
	gSlider->callback(greenCB);

    Fl_Box* blueBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Blue");
    bSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	bSlider->align(FL_ALIGN_TOP);
	bSlider->type(FL_HOR_SLIDER);
	bSlider->bounds(0, 255);
	bSlider->step(1);
	bSlider->callback(blueCB);

    Fl_Box* sliderBox = new Fl_Box(0, 0, pack->w() - 20, 20, "Size");
    sizeSlider = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	sizeSlider->align(FL_ALIGN_TOP);
	sizeSlider->type(FL_HOR_SLIDER);
	sizeSlider->bounds(1, 3);
	sizeSlider->step(1);
	sizeSlider->callback(sizeCB);

	drawButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Add Shape");
	drawButton->callback((Fl_Callback*)addShape);

	drawButton = new Fl_Button(0, 0, pack->w() - 20, 20, "Create XML");
	drawButton->callback((Fl_Callback*)createXML);
    radioPack->end();

	pack->end();
	// slider for camera
	Fl_Pack* packCol2 = new Fl_Pack(w() - 155, 30, 150, h(), "");
	packCol2->box(FL_DOWN_FRAME);
	packCol2->type(Fl_Pack::VERTICAL);
	packCol2->spacing(30);
	packCol2->begin();

	Fl_Pack *cameraPack = new Fl_Pack(w() - 100, 30, 100, h(), "Camera");
	cameraPack->box(FL_DOWN_FRAME);
	cameraPack->labelfont(1);
	cameraPack->type(Fl_Pack::VERTICAL);
	cameraPack->spacing(0);
	cameraPack->begin();

	Fl_Box *rotXTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateX");
	rotXSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotXSlider->align(FL_ALIGN_TOP);
	rotXSlider->type(FL_HOR_SLIDER);
	rotXSlider->bounds(-359, 359);
	rotXSlider->step(1);
	rotXSlider->value(canvas->rotVec.x);
	rotXSlider->callback(sliderFloatCB, (void *)(&(canvas->rotVec.x)));

	Fl_Box *rotYTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateY");
	rotYSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotYSlider->align(FL_ALIGN_TOP);
	rotYSlider->type(FL_HOR_SLIDER);
	rotYSlider->bounds(-359, 359);
	rotYSlider->step(1);
	rotYSlider->value(canvas->rotVec.y);
	rotYSlider->callback(sliderFloatCB, (void *)(&(canvas->rotVec.y)));

	Fl_Box *rotZTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "RotateZ");
	rotZSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	rotZSlider->align(FL_ALIGN_TOP);
	rotZSlider->type(FL_HOR_SLIDER);
	rotZSlider->bounds(-359, 359);
	rotZSlider->step(1);
	rotZSlider->value(canvas->rotVec.z);
	rotZSlider->callback(sliderFloatCB, (void *)(&(canvas->rotVec.z)));

	Fl_Box *scaleTextbox = new Fl_Box(0, 0, pack->w() - 20, 20, "Scale");
	scaleSlider          = new Fl_Value_Slider(0, 0, pack->w() - 20, 20, "");
	scaleSlider->align(FL_ALIGN_TOP);
	scaleSlider->type(FL_HOR_SLIDER);
	scaleSlider->bounds(0.1, 2);
	scaleSlider->value(canvas->scale);
	scaleSlider->callback(sliderFloatCB, (void *)(&(canvas->scale)));

	cameraPack->end();
	packCol2->end();
	// pack -> end();

	end();
}


/**************************************** main() ********************/
int main(int argc, char **argv) {
	win = new MyAppWindow(900, 600, "Dragging Object");
	win->resizable(win);
	Fl::add_idle(MyAppWindow::idleCB);
	win->show();
	return(Fl::run());
}