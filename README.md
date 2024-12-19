# Scene Builder 1.0

Welcome to Scene Builder 1.0, a powerful tool for creating and managing 3D scenes. This application allows you to create, manipulate, and render various 3D shapes, apply textures, and export your scenes to XML format for further use.

## Features

- **Create and Manipulate Shapes**: Add various shapes like cubes, spheres, cylinders, and cones to your scene.
- **Texture Management**: Apply textures to your shapes using PPM image files.
- **Transformations**: Translate, rotate, and scale your shapes to create complex scenes.
- **Scene Export**: Export your scene to an XML file for easy sharing and further processing.
- **Interactive UI**: Use sliders and buttons to interactively modify your scene.

## Getting Started

![alt text](https://github.com/kelseyzeng0610/SceneBuilder1.0/blob/main/example.png)

### Prerequisites

- Ensure you have the following libraries installed:
  - FLTK (Fast Light Toolkit)
  - OpenGL
  - GLM (OpenGL Mathematics)

### Building the Project

1. Clone the repository:
    ```sh
    git clone https://github.com/yourusername/SceneBuilder1.0.git
    cd CS175_Final_Project
    ```

2. Build the project using the provided Makefile:
    ```sh
    make
    ```

3. Run the application:
    ```sh
    ./final
    ```

### Using the Application

1. **Adding Shapes**:
   - Click the "Add Shape" button to open the shape selection window.
   - Choose a shape (Cube, Sphere, Cylinder, Cone) and click "OK".
   - Select an object by left clicking it.
   - Dragging by using two fingers.

2. **Transforming Shapes**:
   - Use the sliders to translate, rotate, and scale the selected shape.
   - The transformations include:
     - **Translation**: Move(drag) the shape along the X, Y, and Z axes.
     - **Rotation**: Rotate(using the slider) the shape around the X, Y, and Z axes.
     - **Scaling**: Scale the shape uniformly or along individual axes.


3. **Add Children Shapes**:
   - use the button "Add child" to add a child object to the current selected object.
   - The child node would inherit the parent's Transformation matrix with initialization.


5. **Exporting the Scene**:
   - Click the "Create XML" button to export the current scene to an XML file.
   - The XML file will contain all the shapes, their transformations, and textures.
6. **Camera Rotation and UVW shift**:
   - use "W","A","S","D" to shift camera angle.
   - scroll up and down to zoom in / zoom out.


### Example

Here is an example of how to create a simple scene:

1. Add a cube to the scene.
2. Translate the cube along the X-axis.
3. Add a sphere and scale it up.
4. Export the scene to an XML file.

## Code Overview

### Main Components

- **

main.cpp

**: Contains the main application logic and UI callbacks.
- **

MyGLCanvas.cpp

**: Handles OpenGL rendering and scene management.
- **

SceneObject.cpp

**: Defines the 

SceneObject

 class for managing individual shapes and their textures.
- **

ppm.cpp

**: Provides functionality for loading and manipulating PPM image files.

### Key Classes

- **

MyAppWindow in main.cpp

**: The main application window class, which includes UI elements and their callbacks.
- **

MyGLCanvas in MyGLCanvas.cpp

**: The OpenGL canvas for rendering the 3D scene.
- **

SceneObject

**: Represents a 3D object in the scene, including its geometry and texture.
- **

ppm

**: Handles loading and manipulating PPM image files for textures.

## Contributing

We welcome contributions to improve Scene Builder 1.0. Please fork the repository and submit pull requests with your enhancements.

## License

This project is licensed under the MIT License. See the LICENSE file for details.

## Acknowledgements

- FLTK for the GUI framework.
- OpenGL for rendering.
- GLM for mathematical operations.
- This is a final project from CS175 Computer Graphics @ Tufts - some of the source code are contributed gracefully by our Prof.Remco Chang in previous homework.

Enjoy building your 3D scenes with Scene Builder 1.0! 
