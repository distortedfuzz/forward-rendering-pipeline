#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Helpers.h"
#include "Rotation.h"
#include "Scaling.h"
#include "Scene.h"

#include <unordered_set>

#include "Translation.h"
#include "Triangle.h"
#include "tinyxml2.h"
#include "Color.h"

using namespace tinyxml2;
using namespace std;

/*
	Parses XML file
*/
Scene::Scene(const char* xmlPath)
{
    const char* str;
    XMLDocument xmlDoc;
    XMLElement* xmlElement;

    xmlDoc.LoadFile(xmlPath);

    XMLNode* rootNode = xmlDoc.FirstChild();

    // read background color
    xmlElement = rootNode->FirstChildElement("BackgroundColor");
    str = xmlElement->GetText();
    sscanf(str, "%lf %lf %lf", &backgroundColor.r, &backgroundColor.g, &backgroundColor.b);

    // read culling
    xmlElement = rootNode->FirstChildElement("Culling");
    if (xmlElement != NULL)
    {
        str = xmlElement->GetText();

        if (strcmp(str, "enabled") == 0)
        {
            this->cullingEnabled = true;
        }
        else
        {
            this->cullingEnabled = false;
        }
    }

    // read cameras
    xmlElement = rootNode->FirstChildElement("Cameras");
    XMLElement* camElement = xmlElement->FirstChildElement("Camera");
    XMLElement* camFieldElement;
    while (camElement != NULL)
    {
        Camera* camera = new Camera();

        camElement->QueryIntAttribute("id", &camera->cameraId);

        // read projection type
        str = camElement->Attribute("type");

        if (strcmp(str, "orthographic") == 0)
        {
            camera->projectionType = ORTOGRAPHIC_PROJECTION;
        }
        else
        {
            camera->projectionType = PERSPECTIVE_PROJECTION;
        }

        camFieldElement = camElement->FirstChildElement("Position");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf", &camera->position.x, &camera->position.y, &camera->position.z);

        camFieldElement = camElement->FirstChildElement("Gaze");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf", &camera->gaze.x, &camera->gaze.y, &camera->gaze.z);

        camFieldElement = camElement->FirstChildElement("Up");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf", &camera->v.x, &camera->v.y, &camera->v.z);

        camera->gaze = normalizeVec3(camera->gaze);
        camera->u = crossProductVec3(camera->gaze, camera->v);
        camera->u = normalizeVec3(camera->u);

        camera->w = inverseVec3(camera->gaze);
        camera->v = crossProductVec3(camera->u, camera->gaze);
        camera->v = normalizeVec3(camera->v);

        camFieldElement = camElement->FirstChildElement("ImagePlane");
        str = camFieldElement->GetText();
        sscanf(str, "%lf %lf %lf %lf %lf %lf %d %d",
               &camera->left, &camera->right, &camera->bottom, &camera->top,
               &camera->near, &camera->far, &camera->horRes, &camera->verRes);

        camFieldElement = camElement->FirstChildElement("OutputName");
        str = camFieldElement->GetText();
        camera->outputFilename = string(str);

        this->cameras.push_back(camera);

        camElement = camElement->NextSiblingElement("Camera");
    }

    // read vertices
    xmlElement = rootNode->FirstChildElement("Vertices");
    XMLElement* vertexElement = xmlElement->FirstChildElement("Vertex");
    int vertexId = 1;

    while (vertexElement != NULL)
    {
        Vec3* vertex = new Vec3();
        Color* color = new Color();

        vertex->colorId = vertexId;

        str = vertexElement->Attribute("position");
        sscanf(str, "%lf %lf %lf", &vertex->x, &vertex->y, &vertex->z);

        str = vertexElement->Attribute("color");
        sscanf(str, "%lf %lf %lf", &color->r, &color->g, &color->b);

        this->vertices.push_back(vertex);
        this->colorsOfVertices.push_back(color);

        vertexElement = vertexElement->NextSiblingElement("Vertex");

        vertexId++;
    }

    // read translations
    xmlElement = rootNode->FirstChildElement("Translations");
    XMLElement* translationElement = xmlElement->FirstChildElement("Translation");
    while (translationElement != NULL)
    {
        Translation* translation = new Translation();

        translationElement->QueryIntAttribute("id", &translation->translationId);

        str = translationElement->Attribute("value");
        sscanf(str, "%lf %lf %lf", &translation->tx, &translation->ty, &translation->tz);

        this->translations.push_back(translation);

        translationElement = translationElement->NextSiblingElement("Translation");
    }

    // read scalings
    xmlElement = rootNode->FirstChildElement("Scalings");
    XMLElement* scalingElement = xmlElement->FirstChildElement("Scaling");
    while (scalingElement != NULL)
    {
        Scaling* scaling = new Scaling();

        scalingElement->QueryIntAttribute("id", &scaling->scalingId);
        str = scalingElement->Attribute("value");
        sscanf(str, "%lf %lf %lf", &scaling->sx, &scaling->sy, &scaling->sz);

        this->scalings.push_back(scaling);

        scalingElement = scalingElement->NextSiblingElement("Scaling");
    }

    // read rotations
    xmlElement = rootNode->FirstChildElement("Rotations");
    XMLElement* rotationElement = xmlElement->FirstChildElement("Rotation");
    while (rotationElement != NULL)
    {
        Rotation* rotation = new Rotation();

        rotationElement->QueryIntAttribute("id", &rotation->rotationId);
        str = rotationElement->Attribute("value");
        sscanf(str, "%lf %lf %lf %lf", &rotation->angle, &rotation->ux, &rotation->uy,
               &rotation->uz);

        this->rotations.push_back(rotation);

        rotationElement = rotationElement->NextSiblingElement("Rotation");
    }

    // read meshes
    xmlElement = rootNode->FirstChildElement("Meshes");

    XMLElement* meshElement = xmlElement->FirstChildElement("Mesh");
    while (meshElement != NULL)
    {
        Mesh* mesh = new Mesh();

        meshElement->QueryIntAttribute("id", &mesh->meshId);

        // read projection type
        str = meshElement->Attribute("type");

        if (strcmp(str, "wireframe") == 0)
        {
            mesh->type = WIREFRAME_MESH;
        }
        else
        {
            mesh->type = SOLID_MESH;
        }

        // read mesh transformations
        XMLElement* meshTransformationsElement = meshElement->FirstChildElement("Transformations");
        XMLElement* meshTransformationElement = meshTransformationsElement->FirstChildElement(
            "Transformation");

        while (meshTransformationElement != NULL)
        {
            char transformationType;
            int transformationId;

            str = meshTransformationElement->GetText();
            sscanf(str, "%c %d", &transformationType, &transformationId);

            mesh->transformationTypes.push_back(transformationType);
            mesh->transformationIds.push_back(transformationId);

            meshTransformationElement = meshTransformationElement->NextSiblingElement(
                "Transformation");
        }

        mesh->numberOfTransformations = mesh->transformationIds.size();

        // read mesh faces
        char* row;
        char* cloneStr;
        int v1, v2, v3;
        XMLElement* meshFacesElement = meshElement->FirstChildElement("Faces");
        str = meshFacesElement->GetText();
        cloneStr = strdup(str);

        row = strtok(cloneStr, "\n");
        while (row != NULL)
        {
            int result = sscanf(row, "%d %d %d", &v1, &v2, &v3);

            if (result != EOF)
            {
                mesh->triangles.push_back(Triangle(v1, v2, v3));
            }
            row = strtok(NULL, "\n");
        }
        mesh->numberOfTriangles = mesh->triangles.size();
        this->meshes.push_back(mesh);

        meshElement = meshElement->NextSiblingElement("Mesh");
    }
}

void Scene::assignColorToPixel(int i, int j, Color c)
{
    this->image[i][j].r = c.r;
    this->image[i][j].g = c.g;
    this->image[i][j].b = c.b;
}

/*
	Initializes image with background color
*/
void Scene::initializeImage(Camera* camera)
{
    if (this->image.empty())
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            vector<Color> rowOfColors;
            vector<double> rowOfDepths;

            for (int j = 0; j < camera->verRes; j++)
            {
                rowOfColors.push_back(this->backgroundColor);
                rowOfDepths.push_back(1.01);
            }

            this->image.push_back(rowOfColors);
            this->depth.push_back(rowOfDepths);
        }
    }
    else
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            for (int j = 0; j < camera->verRes; j++)
            {
                assignColorToPixel(i, j, this->backgroundColor);
                this->depth[i][j] = 1.01;
                this->depth[i][j] = 1.01;
                this->depth[i][j] = 1.01;
            }
        }
    }
}

/*
	If given value is less than 0, converts value to 0.
	If given value is more than 255, converts value to 255.
	Otherwise returns value itself.
*/
int Scene::makeBetweenZeroAnd255(double value)
{
    if (value >= 255.0)
        return 255;
    if (value <= 0.0)
        return 0;
    return (int)(value);
}

/*
	Writes contents of image (Color**) into a PPM file.
*/
void Scene::writeImageToPPMFile(Camera* camera)
{
    ofstream fout;

    fout.open(camera->outputFilename.c_str());

    fout << "P3" << endl;
    fout << "# " << camera->outputFilename << endl;
    fout << camera->horRes << " " << camera->verRes << endl;
    fout << "255" << endl;

    for (int j = camera->verRes - 1; j >= 0; j--)
    {
        for (int i = 0; i < camera->horRes; i++)
        {
            fout << makeBetweenZeroAnd255(this->image[i][j].r) << " "
                << makeBetweenZeroAnd255(this->image[i][j].g) << " "
                << makeBetweenZeroAnd255(this->image[i][j].b) << " ";
        }
        fout << endl;
    }
    fout.close();
}

/*
	Converts PPM image in given path to PNG file, by calling ImageMagick's 'convert' command.
*/
void Scene::convertPPMToPNG(string ppmFileName)
{
    string command;

    // TODO: Change implementation if necessary.
    command = "./magick convert " + ppmFileName + " " + ppmFileName + ".png";
    system(command.c_str());
}

/*
	Transformations, clipping, culling, rasterization are done here.
*/
void Scene::forwardRenderingPipeline(Camera* camera)
{
    double eye[4][4] = {
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1}
    };

    Matrix4 camera_matrix = camera->camera_transformation();
    Matrix4 viewport_matrix = camera->viewport_transformation();

    auto wireframe_viewport_positions = std::vector<std::vector<std::vector<Vec4>>>();

    auto vertex_to_viewport_positions = std::vector<std::vector<std::vector<Vec4>>>();
    for (int i = 0; i < this->meshes.size(); i++)
    {
        Matrix4 transformation_matrix = Matrix4(eye);
        for (int j = meshes[i]->numberOfTransformations - 1; j >= 0; j--)
        {
            char transformation_type = meshes[i]->transformationTypes[j];
            int transformation_id = meshes[i]->transformationIds[j];

            if (transformation_type == 't')
            {
                auto translation = *translations[transformation_id - 1];
                Matrix4 translation_matrix = translation.translate();
                transformation_matrix = multiplyMatrixWithMatrix(
                    transformation_matrix, translation_matrix);
            }
            else if (transformation_type == 's')
            {
                auto scaling = *scalings[transformation_id - 1];
                Matrix4 scaling_matrix = scaling.scale();
                transformation_matrix = multiplyMatrixWithMatrix(
                    transformation_matrix, scaling_matrix);
            }
            else if (transformation_type == 'r')
            {
                auto rotation = *rotations[transformation_id - 1];
                Matrix4 rotation_matrix = rotation.rotate();
                transformation_matrix = multiplyMatrixWithMatrix(
                    transformation_matrix, rotation_matrix);
            }
        }

        transformation_matrix = multiplyMatrixWithMatrix(
            camera_matrix, transformation_matrix);

        if (camera->projectionType == 0)
        {
            Matrix4 ortographic_matrix = camera->ortographic_transformation();
            transformation_matrix = multiplyMatrixWithMatrix(
                ortographic_matrix, transformation_matrix);
        }
        else if (camera->projectionType == 1)
        {
            Matrix4 perspective_matrix = camera->perspective_transformation();
            transformation_matrix = multiplyMatrixWithMatrix(
                perspective_matrix, transformation_matrix);
        }

        auto wireframe_mesh = std::vector<std::vector<Vec4>>();
        auto mesh_vertex_to_viewport_positions = std::vector<std::vector<Vec4>>();
        for (int j = 0; j < meshes[i]->triangles.size(); j++)
        {
            Vec4 transformed_vertices[3];
            for (int k = 0; k < 3; k++)
            {
                Vec3* vertex = vertices[meshes[i]->triangles[j].vertexIds[k] - 1];

                Vec4 extended_vertex;
                extended_vertex.x = vertex->x;
                extended_vertex.y = vertex->y;
                extended_vertex.z = vertex->z;
                extended_vertex.t = 1.0;

                Vec4 transformed_vertex = multiplyMatrixWithVec4(
                    transformation_matrix, extended_vertex);

                transformed_vertex.colorId = vertex->colorId;

                if (camera->projectionType == 1)
                {
                    double t = transformed_vertex.t;
                    transformed_vertex.x = transformed_vertex.x / t;
                    transformed_vertex.y = transformed_vertex.y / t;
                    transformed_vertex.z = transformed_vertex.z / t;
                    transformed_vertex.t = 1.0;
                }

                transformed_vertices[k] = transformed_vertex;
            }

            Vec4 camera_pos;
            camera_pos.x = camera->position.x;
            camera_pos.y = camera->position.y;
            camera_pos.z = camera->position.z;
            camera_pos.t = 1.0;

            camera_pos = multiplyMatrixWithVec4(camera_matrix, camera_pos);
            Vec3 cam_pos;
            cam_pos.x = camera_pos.x;
            cam_pos.y = camera_pos.y;
            cam_pos.z = camera_pos.z;

            bool culled = this->cullingEnabled;
            if (this->cullingEnabled)
            {
                Vec3 a = Vec3(transformed_vertices[0]);
                Vec3 b = Vec3(transformed_vertices[1]);
                Vec3 c = Vec3(transformed_vertices[2]);

                Vec3 normal =
                    crossProductVec3(subtractVec3(a, b), subtractVec3(c, b));
                Vec3 camera_ray = subtractVec3(a, cam_pos);

                culled = dotProductVec3(camera_ray, normal) > 0;
            }

            auto triangle_vertex_to_viewport_positions = std::vector<Vec4>();

            if (!culled)
            {
                if(meshes[i]->type == 0){
                    auto line = std::vector<Vec4>();

                    //first line
                    Vec4 wireframe_vertex1 = transformed_vertices[0];
                    Vec4 wireframe_vertex2 = transformed_vertices[1];

                    camera->clipping(wireframe_vertex1, wireframe_vertex2);


                    Vec4 viewport_position =
                        multiplyMatrixWithVec4(viewport_matrix, wireframe_vertex1);

                    line.push_back(viewport_position);

                    viewport_position =
                        multiplyMatrixWithVec4(viewport_matrix, wireframe_vertex2);
                    line.push_back(viewport_position);

                    wireframe_mesh.push_back(line);
                    line.clear();

                    //second line
                    wireframe_vertex2 = transformed_vertices[1];
                    Vec4 wireframe_vertex3 = transformed_vertices[2];

                    camera->clipping(wireframe_vertex1, wireframe_vertex2);

                    viewport_position =
                        multiplyMatrixWithVec4(viewport_matrix, wireframe_vertex2);
                    line.push_back(viewport_position);
                    viewport_position =
                        multiplyMatrixWithVec4(viewport_matrix, wireframe_vertex3);
                    line.push_back(viewport_position);

                    wireframe_mesh.push_back(line);
                    line.clear();

                    //third line
                    wireframe_vertex1 = transformed_vertices[0];
                    wireframe_vertex3 = transformed_vertices[2];

                    camera->clipping(wireframe_vertex1, wireframe_vertex3);

                    viewport_position =
                        multiplyMatrixWithVec4(viewport_matrix, wireframe_vertex1);
                    line.push_back(viewport_position);

                    viewport_position =
                        multiplyMatrixWithVec4(viewport_matrix, wireframe_vertex3);
                    line.push_back(viewport_position);

                    wireframe_mesh.push_back(line);

                }else{
                    for (int k = 0; k < 3; k++)
                    {
                      Vec4 viewport_position =
                          multiplyMatrixWithVec4(viewport_matrix, transformed_vertices[k]);
                      viewport_position.colorId = transformed_vertices[k].colorId;
                      triangle_vertex_to_viewport_positions.push_back(viewport_position);
                    }
                }

            }
            mesh_vertex_to_viewport_positions.push_back(triangle_vertex_to_viewport_positions);
        }
        vertex_to_viewport_positions.push_back(mesh_vertex_to_viewport_positions);
        wireframe_viewport_positions.push_back(wireframe_mesh);
    }

    auto depths = std::vector<std::vector<double>>();
    auto pixels = std::vector<std::vector<Color>>();
    for (int i = 0; i < camera->horRes; i++)
    {
        auto row_depths = std::vector<double>();
        auto row_pixels = std::vector<Color>();

        for (int j = 0; j < camera->verRes; j++)
        {
            row_depths.push_back(INFINITY);
            row_pixels.push_back(backgroundColor);
        }

        depths.push_back(row_depths);
        pixels.push_back(row_pixels);
    }

    std::vector<Vec3> points;
    for (int i = 0; i < meshes.size(); i++)
    {
        if(meshes[i]->type == 0){
            //i can be problematic but all wireframe or all solid

            for(int u = 0; u < wireframe_viewport_positions[i].size(); u++)
            {

                if (wireframe_viewport_positions[i][u].size() == 0)
                {
                    continue;
                }

                Vec3 vertex1;
                vertex1.x = wireframe_viewport_positions[i][u][0].x;
                vertex1.y = wireframe_viewport_positions[i][u][0].y;
                vertex1.z = wireframe_viewport_positions[i][u][0].z;
                vertex1.colorId = wireframe_viewport_positions[i][u][0].colorId;

                Vec3 vertex2;
                vertex2.x = wireframe_viewport_positions[i][u][1].x;
                vertex2.y = wireframe_viewport_positions[i][u][1].y;
                vertex2.z = wireframe_viewport_positions[i][u][1].z;
                vertex2.colorId = wireframe_viewport_positions[i][u][1].colorId;

                if(vertex1.x < camera->horRes && vertex1.x >= -0.5&&
                    vertex1.y < camera->verRes && vertex1.y >= -0.5 &&
                    vertex2.x < camera->horRes && vertex2.x >= -0.5 &&
                    vertex2.y < camera->verRes && vertex2.y >= -0.5  ){
                    Color::line_rasterization(
                        vertex1,
                        vertex2,
                        *colorsOfVertices[vertex1.colorId - 1],
                        *colorsOfVertices[vertex2.colorId - 1],
                        depths,
                        pixels
                    );
                }


            }
        }else{
            for (auto triangle_viewport_positions : vertex_to_viewport_positions[i])
            {
                if (triangle_viewport_positions.size() == 0)
                {
                    continue;
                }

                Vec3 vertex1;
                vertex1.x = triangle_viewport_positions[0].x;
                vertex1.y = triangle_viewport_positions[0].y;
                vertex1.z = triangle_viewport_positions[0].z;
                vertex1.colorId = triangle_viewport_positions[0].colorId;

                Vec3 vertex2;
                vertex2.x = triangle_viewport_positions[1].x;
                vertex2.y = triangle_viewport_positions[1].y;
                vertex2.z = triangle_viewport_positions[1].z;
                vertex2.colorId = triangle_viewport_positions[1].colorId;

                Vec3 vertex3;
                vertex3.x = triangle_viewport_positions[2].x;
                vertex3.y = triangle_viewport_positions[2].y;
                vertex3.z = triangle_viewport_positions[2].z;
                vertex3.colorId = triangle_viewport_positions[2].colorId;


                Color::triangle_rasterization(
                    vertex1,
                    vertex2,
                    vertex3,
                    *colorsOfVertices[vertex1.colorId - 1],
                    *colorsOfVertices[vertex2.colorId - 1],
                    *colorsOfVertices[vertex3.colorId - 1],
                    depths,
                    pixels
                    );

            }
        }

    }
    for (int i = 0; i < pixels.size(); i++)
    {
        auto& row = pixels[i];
        for (int j = 0; j < row.size(); j++)
        {
            image[i][j].r = makeBetweenZeroAnd255(row[j].r);
            image[i][j].g = makeBetweenZeroAnd255(row[j].g);
            image[i][j].b = makeBetweenZeroAnd255(row[j].b);
        }
    }
}
