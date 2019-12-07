
#include "Billboard.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "GLSL.h"
#define STB_IMAGE_IMPLEMENTATION
#include <GLFW/glfw3.h>
#include "Cube.h"
#include "stb_image.h"

Billboard::Billboard(){    myCubeDim = 10;

static inline float map(
    float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

void Billboard::init(std::shared_ptr<Program>& bbprog)
{
    glm::vec3 rect_pos[4];
    glm::vec2 rect_tex[4];
    glm::vec3 rect_norm[4];
    int rect_indi[6] = {0, 1, 2, 0, 2, 3};
    rect_pos[0] = glm::vec3(0, 0, 0);
    rect_pos[1] = glm::vec3(1, 0, 0);
    rect_pos[2] = glm::vec3(1, 0.5, 0);
    rect_pos[3] = glm::vec3(0, 0.5, 0);
    rect_norm[0] = glm::vec3(0, 0, 1);
    rect_norm[1] = glm::vec3(0, 0, 1);
    rect_norm[2] = glm::vec3(0, 0, 1);
    rect_norm[3] = glm::vec3(0, 0, 1);
    rect_tex[0] = glm::vec2(0, 0);
    rect_tex[1] = glm::vec2(1.0f / myCubeDim, 0);
    rect_tex[2] = glm::vec2(1.0f / myCubeDim, 1.0f / myCubeDim);
    rect_tex[3] = glm::vec2(0, 1.0f / myCubeDim);

    // generate the VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    // positions
    glGenBuffers(1, &PositionBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, PositionBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, rect_pos,
                 GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // normals
    glGenBuffers(1, &NormBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, NormBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 4, rect_norm,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // texture coords
    glGenBuffers(1, &TexCoordsBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordsBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * 4, rect_tex,
                 GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // indices
    glGenBuffers(1, &IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, rect_indi,
                 GL_STATIC_DRAW);

    bbCubes = std::vector<BBCube>();
    bbCubesPost = std::vector<BBCube>();

    for (int x = 0; x < myCubeDim; x++)
    {
        for (int y = 0; y < myCubeDim; y++)
        {
            BBCube cube = BBCube();

            cube.target.pos = glm::vec3((x) * (1.0f / myCubeDim),
                                        (y) * (0.5f / myCubeDim), -1.5);
            cube.target.pos.x -= 1 / 2.0f;
            cube.target.pos.y -= 1 / 4.0f;

            cube.target.scale =
                glm::vec3(1.0f / myCubeDim, 1.0f / myCubeDim, 1.0f / myCubeDim);
            cube.source = cube.target;
            // cube.source.scale *= 0.001;
            cube.source.pos.z -= 500;
            cube.source.pos.x = map(rand() % 1000, 0, 1000, -100, 100);
            cube.source.pos.y = map(rand() % 1000, 0, 1000, -100, 100);

            cube.phase = (1.0f - length(glm::vec2(cube.target.pos.x,
                                                  cube.target.pos.y))) *
                         0.5;
            cube.phase += map(rand() % 1000, 0, 1000, -0.05, 0.05);
            cube.phase += map(rand() % 1000, 0, 1000, 0, 0.05);
            cube.phase += 0.4;
            cube.resetInterp();
            cube.dosin = 1;
            cube.texOffset.x = x * (1.0f / myCubeDim);
            cube.texOffset.y = y * (1.0f / myCubeDim);

            BBCube pcube = BBCube();
            pcube = cube;
            pcube.source = cube.target;
            pcube.target = pcube.source;
            pcube.target.pos.z -= 1000;
            // pcube.target.scale *= 0.001f;
            pcube.dosin = 1;
            pcube.phase = map(1 - glm::abs(cube.target.pos.x), 0, 1, -0.2, 0);
            pcube.phase += map(rand() % 1000, 0, 1000, -0.01, 0.01);
            pcube.resetInterp();

            bbCubes.push_back(cube);
            bbCubesPost.push_back(pcube);
        }
    }

    glBindVertexArray(0);

    int width, height, channels;
    char filepath[1000];

    std::string resourceDirectory = "../resources";

    glUseProgram(bbprog->pid);

    std::string str = resourceDirectory + "/black2.png";
    strcpy(filepath, str.c_str());
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
    glGenTextures(1, &Texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint Tex1Location = glGetUniformLocation(
        bbprog->pid,
        "title_tex");  // tex, tex2... sampler in the fragment shader
    glUniform1i(Tex1Location, 0);

    str = resourceDirectory + "/NormalMap.png";
    strcpy(filepath, str.c_str());
    data = stbi_load(filepath, &width, &height, &channels, 4);
    glGenTextures(1, &Texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint Tex2Location = glGetUniformLocation(
        bbprog->pid, "normal_map_tex");  // tex, tex2... sampler in the
                                         // fragment shader
    glUniform1i(Tex2Location, 1);

    bbCubes = std::vector<BBCube>();
    bbCubesPost = std::vector<BBCube>();

    for (int x = 0; x < myCubeDim; x++)
    {
        for (int y = 0; y < myCubeDim; y++)
        {
            BBCube cube = BBCube();
           
            cube.target.pos = glm::vec3((x ) * (1.0f / myCubeDim),
                          (y ) * (0.5f / myCubeDim), -1.5);
            cube.target.pos.x -= 1 / 2.0f;
            cube.target.pos.y -= 1 / 4.0f;

            cube.target.scale = glm::vec3(1.0f / myCubeDim, 1.0f / myCubeDim, 1.0f / myCubeDim);
            cube.source = cube.target;
            //cube.source.scale *= 0.001;
            cube.source.pos.z -= 100;
            cube.source.pos.x = map(rand() % 1000, 0, 1000, -100, 100);
            cube.source.pos.y = map(rand() % 1000, 0, 1000, -100, 100);
					  
            cube.phase = (1.0f- length(glm::vec2(cube.target.pos.x, cube.target.pos.y))) * 0.5;
            cube.phase += map(rand() % 1000, 0, 1000, -0.05, 0.05);
            cube.phase += map(rand() % 1000, 0, 1000, 0, 0.05);
            cube.phase += 0.4;
            cube.resetInterp();
            cube.dosin = 1;
		    cube.texOffset.x = x * (1.0f / myCubeDim);
            cube.texOffset.y = y * (1.0f / myCubeDim);
            

			 BBCube pcube = BBCube();
            pcube = cube;
            pcube.source = cube.target;
            pcube.target = pcube.source;
            pcube.target.pos.z -= 1000;
            //pcube.target.scale *= 0.001f;
            pcube.dosin = 1;
            pcube.phase = map(1 - glm::abs(cube.target.pos.x), 0, 1, -0.2, 0);
            pcube.phase += map(rand() % 1000, 0, 1000, -0.01, 0.01);
            pcube.resetInterp();

			bbCubes.push_back(cube);
			bbCubesPost.push_back(pcube);
        }
    }
}

void Billboard::draw(std::shared_ptr<Program>& bbprog,
                     glm::vec3 campos,
                     double frametime,
                     glm::mat4 P,
                     glm::mat4 V)
{
    bbprog->bind();

    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);

    glUniform3f(bbprog->getUniform("campos"), campos.x, campos.y, campos.z);
    static float w = 0;
    w += frametime;

    glm::vec4 l1p = glm::vec4(0, 0, -5, 0);
    l1p =
        glm::rotate(glm::mat4(1), sin(w * 2) / 2.0f, glm::vec3(0, 1, 0)) * l1p;

    glUniform3f(bbprog->getUniform("light1pos"), l1p.x, l1p.y, l1p.z);

    glm::mat4 Vi = glm::transpose(V);
    Vi[0][3] = 0;
    Vi[1][3] = 0;
    Vi[2][3] = 0;

    glUniformMatrix4fv(bbprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(bbprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);

    if (glfwGetTime() < 12)
        for (int i = 0; i < bbCubes.size(); i++)
        {
            BBCube& cube = bbCubes.data()[i];
            cube.interp += frametime * 0.045;
            cube.interpBetween();
            glUniform2f(bbprog->getUniform("texOffset"), cube.texOffset.x,
                        cube.texOffset.y);
            cube.sendModelMatrix(bbprog, glm::mat4(1));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }
    else
        for (int i = 0; i < bbCubesPost.size(); i++)
        {
            BBCube& cube = bbCubesPost.data()[i];
            cube.interp += frametime * 0.15;
            cube.interpBetween();
            glUniform2f(bbprog->getUniform("texOffset"), cube.texOffset.x,
                        cube.texOffset.y);
            cube.sendModelMatrix(bbprog, glm::mat4(1));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);
        }

    glBindVertexArray(0);

    bbprog->unbind();
}
