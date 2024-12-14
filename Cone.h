#ifndef CONE_H
#define CONE_H

#include "Shape.h"
#include "Cylinder.h"
#include <cmath>
#include <glm/gtc/type_ptr.hpp>


typedef glm::vec2 CirclePoint;

class Cone : public Shape
{
public:
    Cone() {};
    ~Cone() {};

    OBJ_TYPE getType()
    {
        return SHAPE_CONE;
    }

    glm::vec3 manualNormalize(float x, float z, float radial_mag = 0.894f, float vertical_mag = 0.447f) {
        // Compute the radial distance (r)
        float r = sqrt(x * x + z * z);

        // Avoid division by zero
        if (r == 0.0f) return glm::vec3(0.0f, vertical_mag, 0.0f);  // In case of the cone tip or origin

        // Scale the x and z coordinates by the radial magnitude
        float scaled_x = radial_mag * (x / r);
        float scaled_z = radial_mag * (z / r);

        // The y component is the vertical magnitude
        float scaled_y = vertical_mag;

        // Normalize the resulting vector
        float length = sqrt(scaled_x * scaled_x + scaled_y * scaled_y + scaled_z * scaled_z);

        return glm::vec3(scaled_x / length, scaled_y / length, scaled_z / length);
    }



    void draw()
    {
        m_points.clear();
        lastIndex = 0;
        m_normals.clear();

        std::vector<std::vector<CirclePoint>> points = calculate_circle_points();
        // divide sides into segments based on m_segmentsY
        float offsetY = 1.0f / m_segmentsY;


        // draw base
        std::vector<CirclePoint> base = points[0];
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < base.size(); i++)
        {
            CirclePoint p1 = base[i];
            CirclePoint p2 = base[i];

            // wrap around
            if (i == base.size() - 1)
                p2 = base[0];
            else
                p2 = base[i + 1];

            glm::vec3 normal = glm::vec3(0.0f, -1.0f, 0.0f);
            glNormal3fv(glm::value_ptr(normal));
            glVertex3f(0.0f, -0.5f, 0.0f);
            storeNormal({ 0.0f, -0.5f, 0.0f }, normal);
            glVertex3f(p1.x, -0.5f, p1.y);
            storeNormal({ p1.x, -0.5f, p1.y }, normal);
            glVertex3f(p2.x, -0.5f, p2.y);
            storeNormal({ p2.x, -0.5f, p2.y }, normal);
        }
        glEnd();

        // draw cone sides
        glBegin(GL_TRIANGLES);
        for (int i = 0; i < m_segmentsY; i++)
        {
            std::vector<CirclePoint> p1 = points[i];
            std::vector<CirclePoint> p2 = points[0];

            if (!(i == m_segmentsY - 1))
                p2 = points[i + 1];

            for (int j = 0; j < m_segmentsX; j++)
            {
                CirclePoint p11 = p1[j];
                CirclePoint p12 = p1[0];

                CirclePoint p21 = p2[j];
                CirclePoint p22 = p2[0];

                glm::vec3 normal;

                // wrap around
                if (!(j == m_segmentsX - 1))
                    p12 = p1[j + 1];
                if (!(j == m_segmentsX - 1))
                    p22 = p2[j + 1];

                normalizeNormal(p11.x, 0.0f, p11.y);

                // draw apex
                if (i == m_segmentsY - 1)
                {
                    normalizeNormal(0.0f, 0.0f, 0.0f);
                    glVertex3f(0, -0.5f + (i + 1) * offsetY, 0);
                    storeNormal({ 0, -0.5f + (i + 1) * offsetY, 0 }, { 0.0f, 0.0f, 0.0f });


                    normal = manualNormalize(p12.x, p12.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);
                    glVertex3f(p12.x, -0.5f + i * offsetY, p12.y);
                    storeNormal({ p12.x, -0.5f + i * offsetY, p12.y }, { p12.x, 0.0f, p12.y });

                    normal = manualNormalize(p11.x, p11.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);
                    glVertex3f(p11.x, -0.5f + i * offsetY, p11.y);
                    storeNormal({ p11.x, -0.5f + i * offsetY, p11.y }, { p11.x, 0.0f, p11.y });


                }
                // draw sides
                else
                {

                    // top triangles
                    normal = manualNormalize(p22.x, p22.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);
                    glVertex3f(p22.x, -0.5f + (i + 1) * offsetY, p22.y);
                    storeNormal({ p22.x, -0.5f + (i + 1) * offsetY, p22.y }, { p22.x, 0.0f, p22.y });


                    normal = manualNormalize(p11.x, p11.y);


                    normalizeNormal(normal[0], normal[1], normal[2]);

                    normalizeNormal(p11.x, sqrt(p11.x * p11.x + p11.y * p11.y), p11.y);
                    glVertex3f(p11.x, -0.5f + i * offsetY, p11.y);
                    storeNormal({ p11.x, -0.5f + i * offsetY, p11.y }, { p11.x, 0.0f, p11.y });

                    normal = manualNormalize(p21.x, p21.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);

                    glVertex3f(p21.x, -0.5f + (i + 1) * offsetY, p21.y);
                    storeNormal({ p21.x, -0.5f + (i + 1) * offsetY, p21.y }, { p21.x, 0.0f, p21.y });


                    // bottom triangles
                    normal = manualNormalize(p22.x, p22.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);
                    glVertex3f(p22.x, -0.5f + (i + 1) * offsetY, p22.y);
                    storeNormal({ p22.x, -0.5f + (i + 1) * offsetY, p22.y }, { p22.x, 0.0f, p22.y });


                    normal = manualNormalize(p12.x, p12.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);

                    glVertex3f(p12.x, -0.5f + i * offsetY, p12.y);
                    storeNormal({ p12.x, -0.5f + i * offsetY, p12.y }, { p12.x, 0.0f, p12.y });

                    normal = manualNormalize(p11.x, p11.y);
                    normalizeNormal(normal[0], normal[1], normal[2]);
                    glVertex3f(p11.x, -0.5f + i * offsetY, p11.y);
                    storeNormal({ p11.x, -0.5f + i * offsetY, p11.y }, { p11.x, 0.0f, p11.y });


                }
            }
        }
        glEnd();
    };

    void drawNormal()
    {
        float normalLength = 0.1f;
        std::vector<std::vector<CirclePoint>> points = calculate_circle_points();
        float offsetY = 1.0f / m_segmentsY;

        glBegin(GL_LINES);
        for (const auto& point : m_normals)
        {
            std::array<float, 3> vertex = m_points[point.first];
            glm::vec3 normal = point.second;

            float x = vertex[0];
            float y = vertex[1];
            float z = vertex[2];

            if (y == -0.5f)
            {
                // Base normal (pointing down)
                glVertex3f(x, y, z);
                glVertex3f(x, y - normalLength, z);

                if (!(x == 0.0f && z == 0.0f))
                {
                    // Normalize the vector
                    float length = sqrt(normal.x * normal.x + normal.z * normal.z);
                    normal.y = 0.5f; // Approximation of the cone's slope
                    normal.x /= length;
                    normal.z /= length;
                    // Side normal
                    glVertex3f(x, y, z);
                    glVertex3f(x + normal.x * normalLength,
                        y + normal.y * normalLength,
                        z + normal.z * normalLength);
                }
            }
            else
            {
                // Normalize the vector
                float length = sqrt(normal.x * normal.x + normal.z * normal.z);
                normal.y = 0.5f; // Approximation of the cone's slope
                normal.x /= length;
                normal.z /= length;
                // Side normal
                glVertex3f(x, y, z);
                glVertex3f(x + normal.x * normalLength,
                    y + normal.y * normalLength,
                    z + normal.z * normalLength);
            }
        }
        glEnd();
    }

private:
    std::vector<std::vector<CirclePoint>> calculate_circle_points()
    {
        float offsetR = 0.5f / m_segmentsY;

        std::vector<std::vector<CirclePoint>> points;
        for (int j = 0; j < m_segmentsY; j++)
        {
            std::vector<CirclePoint> circlePoints;
            float radius = 0.5f - (j * offsetR);
            for (int i = 0; i < m_segmentsX; ++i)
            {
                float theta = i * (2 * PI / m_segmentsX);
                float x = radius * cos(theta);
                float y = radius * sin(theta);
                circlePoints.emplace_back(CirclePoint(x, y));
            }
            points.emplace_back(circlePoints);
        }
        return points;
    }
};

#endif
