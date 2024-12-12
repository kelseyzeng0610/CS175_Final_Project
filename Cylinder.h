#ifndef CYLINDER_H
#define CYLINDER_H

#include "Shape.h"
#include <array>
#include <vector>

class Cylinder : public Shape
{
public:
	Cylinder() {};
	~Cylinder() {};

	OBJ_TYPE getType()
	{
		return SHAPE_CYLINDER;
	}

	void draw()
	{
		m_points.clear();
		lastIndex = 0;
		m_normals.clear();
		std::vector<std::array<float, 3>> circleVertices = getCircleVertices();

		// draw top circle
		glBegin(GL_TRIANGLES);
		glm::vec3 normal = glm::vec3(0.0f, 1.0f, 0.0f);
		glNormal3fv(glm::value_ptr(normal));
		for (int i = m_segmentsX - 1; i > 0; i--)
		{
			glVertex3f(0.0f, 0.5f, 0.0f);
			storeNormal({0.0f, 0.5f, 0.0f}, normal);

			glVertex3f(circleVertices[i][0], 0.5f, circleVertices[i][2]);
			storeNormal({circleVertices[i][0], 0.5f, circleVertices[i][2]}, normal);

			glVertex3f(circleVertices[i - 1][0], 0.5f, circleVertices[i - 1][2]);
			storeNormal({circleVertices[i - 1][0], 0.5f, circleVertices[i - 1][2]}, normal);
		}
		glEnd();

		// draw last triangle on top circle
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.0f, 0.5f, 0.0f);
		glVertex3f(circleVertices[0][0], 0.5f, circleVertices[0][2]);
		glVertex3f(circleVertices[m_segmentsX - 1][0], 0.5f, circleVertices[m_segmentsX - 1][2]);
		glEnd();

		// draw bottom circle
		glBegin(GL_TRIANGLES);
		normal = glm::vec3(0.0f, -1.0f, 0.0f);
		glNormal3fv(glm::value_ptr(normal));
		for (int i = 0; i < m_segmentsX - 1; i++)
		{
			glVertex3f(0.0f, -0.5f, 0.0f);
			storeNormal({0.0f, -0.5f, 0.0f}, normal);

			glVertex3f(circleVertices[i][0], -0.5f, circleVertices[i][2]);
			storeNormal({circleVertices[i][0], -0.5f, circleVertices[i][2]}, normal);

			glVertex3f(circleVertices[i + 1][0], -0.5f, circleVertices[i + 1][2]);
			storeNormal({circleVertices[i + 1][0], -0.5f, circleVertices[i + 1][2]}, normal);
		}
		glEnd();

		// draw last triangle on bottom circle
		glBegin(GL_TRIANGLES);
		glNormal3f(0.0f, -1.0f, 0.0f);
		glVertex3f(0.0f, -0.5f, 0.0f);
		glVertex3f(circleVertices[m_segmentsX - 1][0], -0.5f, circleVertices[m_segmentsX - 1][2]);
		glVertex3f(circleVertices[0][0], -0.5f, circleVertices[0][2]);
		glEnd();

		// draw sides
		float heightOffset = 1.0f / m_segmentsY;
		for (int i = 0; i < m_segmentsX - 1; i++)
		{
			for (int j = 0; j < m_segmentsY; j++)
			{
				float currHeight = 0.5f - (heightOffset * j);
				float nextHeight = 0.5f - (heightOffset * (j + 1));
				glBegin(GL_TRIANGLES);
				// Triangle 1
				glNormal3f(circleVertices[i][0], 0.0f, circleVertices[i][2]);
				glVertex3f(circleVertices[i][0], currHeight, circleVertices[i][2]);
				storeNormal({circleVertices[i][0], currHeight, circleVertices[i][2]}, {circleVertices[i][0], 0.0f, circleVertices[i][2]});

				glNormal3f(circleVertices[i + 1][0], 0.0f, circleVertices[i + 1][2]);
				glVertex3f(circleVertices[i + 1][0], currHeight, circleVertices[i + 1][2]);
				storeNormal({circleVertices[i + 1][0], currHeight, circleVertices[i + 1][2]}, {circleVertices[i + 1][0], 0.0f, circleVertices[i + 1][2]});

				glNormal3f(circleVertices[i][0], 0.0f, circleVertices[i][2]);
				glVertex3f(circleVertices[i][0], nextHeight, circleVertices[i][2]);
				storeNormal({circleVertices[i][0], nextHeight, circleVertices[i][2]}, {circleVertices[i][0], 0.0f, circleVertices[i][2]});

				// Triangle 2
				glNormal3f(circleVertices[i + 1][0], 0.0f, circleVertices[i + 1][2]);
				glVertex3f(circleVertices[i + 1][0], currHeight, circleVertices[i + 1][2]);
				glNormal3f(circleVertices[i + 1][0], 0.0f, circleVertices[i + 1][2]);
				glVertex3f(circleVertices[i + 1][0], nextHeight, circleVertices[i + 1][2]);
				glNormal3f(circleVertices[i][0], 0.0f, circleVertices[i][2]);
				glVertex3f(circleVertices[i][0], nextHeight, circleVertices[i][2]);
				glEnd();

				// draw last side
				glBegin(GL_TRIANGLES);
				// Triangle 1
				glNormal3f(circleVertices[m_segmentsX - 1][0], 0.0f, circleVertices[m_segmentsX - 1][2]);
				glVertex3f(circleVertices[m_segmentsX - 1][0], currHeight, circleVertices[m_segmentsX - 1][2]);
				glNormal3f(circleVertices[0][0], 0.0f, circleVertices[0][2]);
				glVertex3f(circleVertices[0][0], currHeight, circleVertices[0][2]);
				glNormal3f(circleVertices[m_segmentsX - 1][0], 0.0f, circleVertices[m_segmentsX - 1][2]);
				glVertex3f(circleVertices[m_segmentsX - 1][0], nextHeight, circleVertices[m_segmentsX - 1][2]);

				// Triangle 2
				glNormal3f(circleVertices[0][0], 0.0f, circleVertices[0][2]);
				glVertex3f(circleVertices[0][0], currHeight, circleVertices[0][2]);
				glNormal3f(circleVertices[0][0], 0.0f, circleVertices[0][2]);
				glVertex3f(circleVertices[0][0], nextHeight, circleVertices[0][2]);
				glNormal3f(circleVertices[m_segmentsX - 1][0], 0.0f, circleVertices[m_segmentsX - 1][2]);
				glVertex3f(circleVertices[m_segmentsX - 1][0], nextHeight, circleVertices[m_segmentsX - 1][2]);
				glEnd();
			}
		}
	};

	void drawNormal()
	{
		float normalLength = 0.1f;
		std::vector<std::array<float, 3>> circleVertices = getCircleVertices();

		for (const auto &point : m_normals)
		{
			glBegin(GL_LINES);
			std::array<float, 3> vertex = m_points[point.first];
			glm::vec3 normal = point.second;

			float x = vertex[0];
			float y = vertex[1];
			float z = vertex[2];

			// top circle normals
			glVertex3f(x, y, vertex[2]);
			glVertex3f(x + normal.x * normalLength, y + normal.y * normalLength, z + normal.z * normalLength);
			glEnd();
		}
	};

private:
	float radius = 0.5f;

	std::vector<std::array<float, 3>> getCircleVertices()
	{
		std::vector<std::array<float, 3>> circleVertices;
		std::vector<float> theta(m_segmentsX);
		float thetaOffset = 2 * PI / m_segmentsX;
		float x, y, z;

		theta[0] = 0.0f;

		for (int i = 1; i < m_segmentsX; i++)
		{
			theta[i] = theta[i - 1] + thetaOffset;
		}

		for (int i = 0; i < m_segmentsX; i++)
		{
			x = radius * cos(theta[i]);
			y = 0.5f;
			z = radius * sin(theta[i]);

			circleVertices.push_back({x, y, z});
		}

		return circleVertices;
	}
};

#endif