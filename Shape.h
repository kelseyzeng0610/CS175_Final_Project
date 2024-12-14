#ifndef SHAPE_H
#define SHAPE_H

#include <FL/gl.h>
#include <FL/glu.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// New imports to support our data structures
#include <unordered_map>
#include <vector>
#include <array>
#include <glm/gtc/type_ptr.hpp>

#define PI glm::pi<float>()

enum OBJ_TYPE
{
	SHAPE_CUBE = 0,
	SHAPE_CYLINDER = 1,
	SHAPE_CONE = 2,
	SHAPE_SPHERE = 3,
};

class Shape
{
public:
	static int m_segmentsX;
	static int m_segmentsY;
	virtual void setTransform(glm::vec3 translate, glm::vec3 rotate, glm::vec3 scale) {
        glm::mat4 T = glm::translate(glm::mat4(1.0f), translate);
        glm::mat4 Rx = glm::rotate(glm::mat4(1.0f), rotate.x, glm::vec3(1,0,0));
        glm::mat4 Ry = glm::rotate(glm::mat4(1.0f), rotate.y, glm::vec3(0,1,0));
        glm::mat4 Rz = glm::rotate(glm::mat4(1.0f), rotate.z, glm::vec3(0,0,1));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), scale);
        
        // Final transform = T * Rz * Ry * Rx * S
        transformMatrix = T * Rz * Ry * Rx * S;
    }

	// Points List
	static std::vector<std::array<float, 3>> m_points;
	// Point Index
	static int lastIndex;
	// Point Index to Normal Map
	static std::unordered_map<int, glm::vec3> m_normals;

	Shape()
	{
		setSegments(10, 10);
	};
	~Shape() {};

	void setSegments(int segX, int segY)
	{
		m_segmentsX = segX;
		m_segmentsY = segY;
	}

	virtual OBJ_TYPE getType() = 0;
	virtual void draw() {};
	virtual void drawNormal() {};

protected:

	glm::mat4 transformMatrix;
	void normalizeNormal(float x, float y, float z)
	{
		normalizeNormal(glm::vec3(x, y, z));
	};

	void normalizeNormal(glm::vec3 v)
	{
		glm::vec3 tmpV = glm::normalize(v);
		glNormal3f(tmpV.x, tmpV.y, tmpV.z);
	};

	// New helper function to store points for drawing normals
	void storeNormal(std::array<float, 3> vertex, glm::vec3 normal)
	{
		m_normals[lastIndex] = normal;
		m_points.push_back(vertex);
		lastIndex++;
	}
};

#endif