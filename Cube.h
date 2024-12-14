#ifndef CUBE_H
#define CUBE_H

#include "Shape.h"

class Cube : public Shape
{
public:
	Cube() {};
	~Cube() {};

	OBJ_TYPE getType()
	{
		return SHAPE_CUBE;
	}

	// void draw(){
	// 	 glutSolidCube(1.0f); 

	// }

	void draw()
	{
		m_points.clear();
		lastIndex = 0;
		m_normals.clear();
		float offsetX = 1.0f / m_segmentsX;
		float offsetY = 1.0f / m_segmentsY;

		glBegin(GL_TRIANGLES);
		for (int i = 0; i < m_segmentsX; i++)
		{
			for (int j = 0; j < m_segmentsY; j++)
			{
				// find better names
				float p11 = -0.5f + (i * offsetX);
				float p12 = 0.5f - (j * offsetY);
				float p21 = -0.5f + ((i + 1) * offsetX);
				float p22 = 0.5f - ((j + 1) * offsetY);

				// front
				glNormal3f(0.0f, 0.0f, 1.0f);

				glVertex3f(p21, p22, 0.5f);
				glVertex3f(p11, p22, 0.5f);
				glVertex3f(p21, p12, 0.5f);
				storeNormal({ p21, p22, 0.5f }, { 0.0f, 0.0f, 1.0f });
				storeNormal({ p11, p22, 0.5f }, { 0.0f, 0.0f, 1.0f });
				storeNormal({ p21, p12, 0.5f }, { 0.0f, 0.0f, 1.0f });

				glVertex3f(p11, p12, 0.5f);
				glVertex3f(p21, p12, 0.5f);
				glVertex3f(p11, p22, 0.5f);
				storeNormal({ p11, p12, 0.5f }, { 0.0f, 0.0f, 1.0f });
				storeNormal({ p21, p12, 0.5f }, { 0.0f, 0.0f, 1.0f });
				storeNormal({ p11, p22, 0.5f }, { 0.0f, 0.0f, 1.0f });

				// back
				glNormal3f(0.0f, 0.0f, -1.0f);

				glVertex3f(p11, p12, -0.5f);
				glVertex3f(p21, p12, -0.5f);
				glVertex3f(p21, p22, -0.5f);
				storeNormal({ p11, p12, -0.5f }, { 0.0f, 0.0f, -1.0f });
				storeNormal({ p21, p12, -0.5f }, { 0.0f, 0.0f, -1.0f });
				storeNormal({ p21, p22, -0.5f }, { 0.0f, 0.0f, -1.0f });

				glVertex3f(p21, p22, -0.5f);
				glVertex3f(p11, p22, -0.5f);
				glVertex3f(p11, p12, -0.5f);
				storeNormal({ p21, p22, -0.5f }, { 0.0f, 0.0f, -1.0f });
				storeNormal({ p11, p22, -0.5f }, { 0.0f, 0.0f, -1.0f });
				storeNormal({ p11, p12, -0.5f }, { 0.0f, 0.0f, -1.0f });

				// left
				glNormal3f(-1.0f, 0.0f, 0.0f);

				glVertex3f(-0.5f, p22, p21);
				glVertex3f(-0.5f, p22, p11);
				glVertex3f(-0.5f, p12, p21);
				storeNormal({ -0.5f, p22, p21 }, { -1.0f, 0.0f, 0.0f });
				storeNormal({ -0.5f, p22, p11 }, { -1.0f, 0.0f, 0.0f });
				storeNormal({ -0.5f, p12, p21 }, { -1.0f, 0.0f, 0.0f });

				glVertex3f(-0.5f, p12, p11);
				glVertex3f(-0.5f, p12, p21);
				glVertex3f(-0.5f, p22, p11);
				storeNormal({ -0.5f, p12, p11 }, { -1.0f, 0.0f, 0.0f });
				storeNormal({ -0.5f, p12, p21 }, { -1.0f, 0.0f, 0.0f });
				storeNormal({ -0.5f, p22, p11 }, { -1.0f, 0.0f, 0.0f });

				// right
				glNormal3f(1.0f, 0.0f, 0.0f);

				glVertex3f(0.5f, p22, p21);
				glVertex3f(0.5f, p22, p11);
				glVertex3f(0.5f, p12, p11);
				storeNormal({ 0.5f, p22, p21 }, { 1.0f, 0.0f, 0.0f });
				storeNormal({ 0.5f, p22, p11 }, { 1.0f, 0.0f, 0.0f });
				storeNormal({ 0.5f, p12, p11 }, { 1.0f, 0.0f, 0.0f });

				glVertex3f(0.5f, p12, p11);
				glVertex3f(0.5f, p12, p21);
				glVertex3f(0.5f, p22, p21);
				storeNormal({ 0.5f, p12, p11 }, { 1.0f, 0.0f, 0.0f });
				storeNormal({ 0.5f, p12, p21 }, { 1.0f, 0.0f, 0.0f });
				storeNormal({ 0.5f, p22, p21 }, { 1.0f, 0.0f, 0.0f });

				// top
				glNormal3f(0.0f, 1.0f, 0.0f);

				glVertex3f(p22, 0.5f, p11);
				glVertex3f(p12, 0.5f, p21);
				glVertex3f(p22, 0.5f, p21);
				storeNormal({ p22, 0.5f, p11 }, { 0.0f, 1.0f, 0.0f });
				storeNormal({ p12, 0.5f, p21 }, { 0.0f, 1.0f, 0.0f });
				storeNormal({ p22, 0.5f, p21 }, { 0.0f, 1.0f, 0.0f });

				glVertex3f(p22, 0.5f, p11);
				glVertex3f(p12, 0.5f, p11);
				glVertex3f(p12, 0.5f, p21);
				storeNormal({ p22, 0.5f, p11 }, { 0.0f, 1.0f, 0.0f });
				storeNormal({ p12, 0.5f, p11 }, { 0.0f, 1.0f, 0.0f });
				storeNormal({ p12, 0.5f, p21 }, { 0.0f, 1.0f, 0.0f });

				// bottom
				glNormal3f(0.0f, -1.0f, 0.0f);

				glVertex3f(p12, -0.5f, p21);
				glVertex3f(p22, -0.5f, p21);
				glVertex3f(p22, -0.5f, p11);
				storeNormal({ p12, -0.5f, p21 }, { 0.0f, -1.0f, 0.0f });
				storeNormal({ p22, -0.5f, p21 }, { 0.0f, -1.0f, 0.0f });
				storeNormal({ p22, -0.5f, p11 }, { 0.0f, -1.0f, 0.0f });

				glVertex3f(p12, -0.5f, p11);
				glVertex3f(p22, -0.5f, p11);
				glVertex3f(p12, -0.5f, p21);
				storeNormal({ p12, -0.5f, p11 }, { 0.0f, -1.0f, 0.0f });
				storeNormal({ p22, -0.5f, p11 }, { 0.0f, -1.0f, 0.0f });
				storeNormal({ p12, -0.5f, p21 }, { 0.0f, -1.0f, 0.0f });
			}
		}
		glEnd();
	}

	void drawNormal()
	{
		glBegin(GL_LINES);
		float normalLength = 0.1f;

		for (const auto &point : m_normals)
		{
			std::array<float, 3> vertex = m_points[point.first];
			glm::vec3 normal = point.second;

			float x = vertex[0];
			float y = vertex[1];
			float z = vertex[2];

			glVertex3f(x, y, z);
			glVertex3f(x + normal.x * normalLength, y + normal.y * normalLength, z + normal.z * normalLength);
		}
		glEnd();
	}

private:
	// Helper function to draw normal lines
	void drawNormalLine(float x, float y, float z, float dx, float dy, float dz)
	{
		glBegin(GL_LINES);
		glColor3f(1.0f, 0.0f, 0.0f);				// Color for normals (red)
		glVertex3f(x, y, z);								// Start of the normal line
		glVertex3f(x + dx, y + dy, z + dz); // End of the normal line
		glEnd();
	}
};

#endif