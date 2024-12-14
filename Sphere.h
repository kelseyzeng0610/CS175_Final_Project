#ifndef SPHERE_H
#define SPHERE_H

#include "Shape.h"

class Sphere : public Shape
{
public:
	Sphere() {};
	~Sphere() {};

	OBJ_TYPE getType()
	{
		return SHAPE_SPHERE;
	}

	void draw()
	{
		m_points.clear();
		lastIndex = 0;
		m_normals.clear();
		std::vector<std::vector<std::array<float, 3>>> sphereVertices = calculateVertices();

		glBegin(GL_TRIANGLES);

		for (int i = 0; i < m_segmentsY; i++)
		{

			// same as cone, start from the bottom now we here
			std::vector<std::array<float, 3>> v1 = sphereVertices[i];
			std::vector<std::array<float, 3>> v2 = sphereVertices[0];

			// need to compare with i and the 'ring' above it, stopping at the top
			if (!(i == m_segmentsY - 1))
			{
				v2 = sphereVertices[i + 1];
			}

			for (int j = 0; j < m_segmentsX; j++)
			{

				// now go along the ring and draw lines at each point stored in the array

				// v11 is the point we're looking at
				std::array<float, 3> v11 = v1[j];
				std::array<float, 3> v12 = v1[(j + 1) % m_segmentsX]; // Wrap around for the next vertex

				// Corresponding vertices on the next latitude ring
				std::array<float, 3> v21 = v2[j];
				std::array<float, 3> v22 = v2[(j + 1) % m_segmentsX]; // Wrap around for the next vertex

				// wrap around
				if (!(j == m_segmentsX - 1))
					v12 = v1[j + 1];
				if (!(j == m_segmentsX - 1))
					v22 = v2[j + 1];

				normalizeNormal(v11[0], v11[1], v11[2]);

				if (i == 0)
				{
					// v2 is the ring 'above' the 'south pole'
					normalizeNormal(0, -0.5, 0);
					glVertex3f(0, -0.5, 0);
					storeNormal({0, -0.5, 0}, {0, -0.5, 0});

					normalizeNormal(v21[0], v21[1], v21[2]);
					glVertex3f(v21[0], v21[1], v21[2]);
					storeNormal({v21[0], v21[1], v21[2]}, {v21[0], v21[1], v21[2]});

					normalizeNormal(v22[0], v22[1], v22[2]);
					glVertex3f(v22[0], v22[1], v22[2]);
					storeNormal({v22[0], v22[1], v22[2]}, {v22[0], v22[1], v22[2]});
				}
				else if (i == m_segmentsY - 1)
				{
					normalizeNormal(v12[0], v12[1], v12[2]);
					glVertex3f(v12[0], v12[1], v12[2]);
					storeNormal({v12[0], v12[1], v12[2]}, {v12[0], v12[1], v12[2]});

					normalizeNormal(v11[0], v11[1], v11[2]);
					glVertex3f(v11[0], v11[1], v11[2]);
					storeNormal({v11[0], v11[1], v11[2]}, {v11[0], v11[1], v11[2]});

					normalizeNormal(0, 0.5, 0);
					glVertex3f(0, 0.5, 0);
					storeNormal({0, 0.5, 0}, {0, 0.5, 0});
				}
				else
				{
					// First triangle
					normalizeNormal(v11[0], v11[1], v11[2]);
					glVertex3f(v11[0], v11[1], v11[2]);
					storeNormal({v11[0], v11[1], v11[2]}, {v11[0], v11[1], v11[2]});

					normalizeNormal(v21[0], v21[1], v21[2]);
					glVertex3f(v21[0], v21[1], v21[2]);
					storeNormal({v21[0], v21[1], v21[2]}, {v21[0], v21[1], v21[2]});

					normalizeNormal(v22[0], v22[1], v22[2]);
					glVertex3f(v22[0], v22[1], v22[2]);
					storeNormal({v22[0], v22[1], v22[2]}, {v22[0], v22[1], v22[2]});

					// Second triangle

					normalizeNormal(v12[0], v12[1], v12[2]);
					glVertex3f(v12[0], v12[1], v12[2]);
					storeNormal({v12[0], v12[1], v12[2]}, {v12[0], v12[1], v12[2]});

					normalizeNormal(v11[0], v11[1], v11[2]);
					glVertex3f(v11[0], v11[1], v11[2]);

					normalizeNormal(v22[0], v22[1], v22[2]);
					glVertex3f(v22[0], v22[1], v22[2]);
				}
			}
		}

		glEnd();
	};

	void drawNormal()
	{
		float normalLength = 0.05f;
		std::vector<std::vector<std::array<float, 3>>> sphereVertices = calculateVertices();

		glBegin(GL_LINES);

		for (const auto &point : m_normals)
		{
			std::array<float, 3> vertex = m_points[point.first];
			glm::vec3 normal = point.second;

			float x = vertex[0];
			float y = vertex[1];
			float z = vertex[2];

			float normalX = normal.x * 2;
			float normalY = normal.y * 2;
			float normalZ = normal.z * 2;

			glVertex3f(x, y, z);
			glVertex3f(x + normalX * normalLength,
								 y + normalY * normalLength,
								 z + normalZ * normalLength);
		}

		glEnd();
	};

private:
	std::vector<std::vector<std::array<float, 3>>> calculateVertices()
	{

		std::vector<std::vector<std::array<float, 3>>> points;
		if (m_segmentsX == 0 || m_segmentsY == 0)
			return points;

		float offsetTheta = 2 * PI / m_segmentsX;
		float offsetPhi = PI / m_segmentsY;

		for (int i = 0; i < m_segmentsY; i++)
		{

			float latAngle = -PI / 2 + i * offsetPhi;

			std::vector<std::array<float, 3>> pointsAtOneLat;
			for (int j = 0; j < m_segmentsX; j++)
			{
				// this loop calculates all points on a horizontal ring going up the sphere

				float longAngle = j * offsetTheta;

				float x = 0.5f * cos(latAngle) * cos(longAngle); // r * cos(phi) * cos(theta)
				float y = 0.5f * sin(latAngle);									 // r * sin(phi)
				float z = 0.5f * cos(latAngle) * sin(longAngle); // r * cos(phi) * sin(theta)

				pointsAtOneLat.push_back({x, y, z});
			}

			points.push_back(pointsAtOneLat);
		}
		return points;
	}
};

#endif