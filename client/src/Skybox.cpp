
#include "Skybox.h"
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "Cube.h"
#include "GLSL.h"
#include "stb_image.h"

static float piconst = 103993.0f / 33102.0f;

#define Y_LOW ((float) -0.50)
#define Y_HIGH ((float) 0.50)
#define TEX_LOW (0)
#define TEX_HIGH (1)

void Skybox::init(std::shared_ptr<Program>& skyboxprog)
{
    std::vector<float> cyl_vertex = std::vector<float>();
    std::vector<float> cyl_tex_coords = std::vector<float>();

    int samples = 256;
    float delta = 2 * piconst * (1.0 / samples);
    int i = 0;
    for (i = 0; i < samples; i++)
    {
        float angle = delta * i;
        float x = cos(angle);
        float y = sin(angle);

        cyl_vertex.push_back(x);
        cyl_vertex.push_back(Y_LOW);
        cyl_vertex.push_back(y);
        cyl_tex_coords.push_back((i) / ((float) samples)); //i / ((float) samples));
        cyl_tex_coords.push_back(TEX_LOW);

        cyl_vertex.push_back(x);
        cyl_vertex.push_back(Y_HIGH);
        cyl_vertex.push_back(y);
        cyl_tex_coords.push_back((i) / ((float) samples));
        cyl_tex_coords.push_back(TEX_HIGH);//i / ((float) samples));

        angle = delta * (i + 1);
        x = cos(angle);
        y = sin(angle);

        cyl_vertex.push_back(x);
        cyl_vertex.push_back(Y_HIGH);
        cyl_vertex.push_back(y);
        cyl_tex_coords.push_back((i+1) / ((float) samples));
        cyl_tex_coords.push_back(TEX_HIGH);//(i+1) / ((float) samples));

        cyl_vertex.push_back(x);
        cyl_vertex.push_back(Y_LOW);
        cyl_vertex.push_back(y);
        cyl_tex_coords.push_back((i+1) / ((float) samples));
        cyl_tex_coords.push_back(TEX_LOW);//(i+1) / ((float) samples));

        cyl_vertex.push_back(x);
        cyl_vertex.push_back(Y_HIGH);
        cyl_vertex.push_back(y);
        cyl_tex_coords.push_back((i+1) / ((float) samples));
        cyl_tex_coords.push_back(TEX_HIGH);//(i+1) / ((float) samples));

        angle = delta * i;
        x = cos(angle);
        y = sin(angle);

        cyl_vertex.push_back(x);
        cyl_vertex.push_back(Y_LOW);
        cyl_vertex.push_back(y);
        cyl_tex_coords.push_back((i) / ((float) samples));
        cyl_tex_coords.push_back(TEX_LOW);//i / ((float) samples));
    }

    // generate the VAO
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    // positions
    glGenBuffers(1, &PositionBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, PositionBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cyl_vertex.size(),
                 cyl_vertex.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // texture coords
    glGenBuffers(1, &TexCoordsBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, TexCoordsBufferId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * cyl_vertex.size(),
                 cyl_tex_coords.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glBindVertexArray(0);

    int width, height, channels;
    char filepath[1000];

    std::string resourceDirectory = "../resources";

    glUseProgram(skyboxprog->pid);

std::string str = resourceDirectory + "/8k_stars_milky_way.jpg";
    strcpy(filepath, str.c_str());
    unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
    glGenTextures(1, &Texture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, Texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint Tex1Location = glGetUniformLocation(
        skyboxprog->pid,
        "skybox_texture");  // tex, tex2... sampler in the fragment shader
    glUniform1i(Tex1Location, 2);


    printf("skybox init \n");
}

void Skybox::draw(std::shared_ptr<Program>& skyboxprog,
                  glm::mat4 P,
                  glm::mat4 V)
{

    skyboxprog->bind();

    glDisable(GL_DEPTH_TEST);
    glBindVertexArray(VertexArrayID);

    glm::mat4 M = glm::mat4(1);

    glUniformMatrix4fv(skyboxprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(skyboxprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
    glUniformMatrix4fv(skyboxprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);

    glDrawArrays(GL_TRIANGLES, 0, 256 * 6);

    glBindVertexArray(0);

    skyboxprog->unbind();

    glEnable(GL_DEPTH_TEST);
}
