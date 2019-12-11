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

Billboard::Billboard() { myCubeDim = 64; }

static inline float map(
    float value, float min1, float max1, float min2, float max2)
{
    return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
}

std::shared_ptr<Program> Billboard::initShader(std::string resourceDirectory)
{
    std::shared_ptr<Program> bbprog = std::make_shared<Program>();
    bbprog->setVerbose(true);
    bbprog->setShaderNames(resourceDirectory + "/bb_vertex.glsl",
                           resourceDirectory + "/bb_frag.glsl");
    if (!bbprog->init())
    {
        std::cerr << "One or more shaders failed to compile... exiting!"
                  << std::endl;
        exit(1);  // make a breakpoint here and check the output window for
                  // the error message!
    }
    bbprog->addUniform("P");
    bbprog->addUniform("V");
    bbprog->addUniform("ScaleM");
    bbprog->addUniform("campos");
    bbprog->addUniform("light1pos");
    bbprog->addUniform("title_tex");
    bbprog->addUniform("normal_map_tex");
    bbprog->addUniform("myCubeDim");
    bbprog->addUniform("interp");
    bbprog->addUniform("animation_stage");
    bbprog->addAttribute("vertPos");
    bbprog->addAttribute("vertNor");
    bbprog->addAttribute("vertTex");
    bbprog->addAttribute("InstanceStartPos");
    bbprog->addAttribute("InstanceMidPos");
    bbprog->addAttribute("InstanceEndPos");
    bbprog->addAttribute("InstancePhases");
    bbprog->addAttribute("InstanceTexOffset");
    return bbprog;
}

void Billboard::initEverythingElse(std::shared_ptr<Program>& bbprog)
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

    std::vector<float> poses_start = std::vector<float>();
    std::vector<float> poses_mid = std::vector<float>();
    std::vector<float> poses_end = std::vector<float>();
    std::vector<float> offsets_tex_coords = std::vector<float>();
    std::vector<float> phases = std::vector<float>();

    for (int x = 0; x < myCubeDim; x++)
    {
        for (int y = 0; y < myCubeDim; y++)
        {
            BBCube cube = BBCube();

            cube.target.pos = glm::vec3((x) * (1.0f / myCubeDim),
                                        (y) * (0.5f / myCubeDim), -0.65);
            cube.target.pos.x -= 1 / 2.0f;
            cube.target.pos.y -= 1 / 4.0f;

            cube.target.scale =
                glm::vec3(1.0f / myCubeDim, 1.0f / myCubeDim, 1.0f / myCubeDim);
            cube.source = cube.target;
            // cube.source.scale *= 0.001;
            cube.source.pos.z -= 200;
            // cube.source.pos.z += map(rand() % 1000, 0, 1000, -75, 75);
            cube.source.pos.x += map(rand() % 10000, 0, 10000, -90, 90);
            cube.source.pos.y += map(rand() % 10000, 0, 10000, -90, 90);

            cube.phase = (1.0f - length(glm::vec2(cube.target.pos.x,
                                                  cube.target.pos.y))) *
                         0.8;
            // cube.phase = map(2-(-cube.target.pos.x + cube.target.pos.y), -2,
            // 2, 0, 1);
            cube.phase += map(rand() % 1000, 0, 1000, -0.02, 0.02);
            cube.phase += 0.5;
            cube.resetInterp();
            cube.dosin = 1;
            cube.texOffset.x = x * (1.0f / myCubeDim);
            cube.texOffset.y = y * (1.0f / myCubeDim);

            poses_start.push_back(cube.source.pos.x);
            poses_start.push_back(cube.source.pos.y);
            poses_start.push_back(cube.source.pos.z);
            poses_mid.push_back(cube.target.pos.x);
            poses_mid.push_back(cube.target.pos.y);
            poses_mid.push_back(cube.target.pos.z);

            BBCube pcube = BBCube();
            pcube = cube;
            pcube.source = cube.target;
            pcube.target = pcube.source;
            pcube.target.pos.z -= 1000;
            // pcube.target.scale *= 0.001f;
            pcube.dosin = 1;
            pcube.phase =
                map(1 - glm::abs(cube.target.pos.x), 0, 1, -0.2, -0.05);
            pcube.phase += map(rand() % 1000, 0, 1000, -0.01, 0.01);
            pcube.resetInterp();

            poses_end.push_back(pcube.target.pos.x);
            poses_end.push_back(pcube.target.pos.y);
            poses_end.push_back(pcube.target.pos.z);

            phases.push_back(cube.phase);
            phases.push_back(pcube.phase);

            offsets_tex_coords.push_back(cube.texOffset.x);
            offsets_tex_coords.push_back(cube.texOffset.y);

            targetScale = cube.target.scale;
        }
    }

    glGenBuffers(1, &IBID_Poses_Mid);
    glBindBuffer(GL_ARRAY_BUFFER, IBID_Poses_Mid);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * poses_mid.size(),
                 poses_mid.data(), GL_STATIC_DRAW);
    int position_loc = glGetAttribLocation(bbprog->pid, "InstanceMidPos");

    for (int i = 0; i < (myCubeDim * myCubeDim); i++)
    {
        glEnableVertexAttribArray(position_loc + i);
        glVertexAttribPointer(position_loc + i, 3, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 3, 0);
        glVertexAttribDivisor(position_loc + i, 1);
    }

    glGenBuffers(1, &IBID_Poses_Start);
    glBindBuffer(GL_ARRAY_BUFFER, IBID_Poses_Start);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * poses_start.size(),
                 poses_start.data(), GL_STATIC_DRAW);
    position_loc = glGetAttribLocation(bbprog->pid, "InstanceStartPos");

    for (int i = 0; i < (myCubeDim * myCubeDim); i++)
    {
        glEnableVertexAttribArray(position_loc + i);
        glVertexAttribPointer(position_loc + i, 3, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 3, 0);
            glVertexAttribDivisor(position_loc + i, 1);
    }

    glGenBuffers(1, &IBID_Poses_End);
    glBindBuffer(GL_ARRAY_BUFFER, IBID_Poses_End);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * poses_end.size(),
                 poses_end.data(), GL_STATIC_DRAW);
    position_loc = glGetAttribLocation(bbprog->pid, "InstanceEndPos");

    for (int i = 0; i < (myCubeDim * myCubeDim); i++)
    {
        glEnableVertexAttribArray(position_loc + i);
        glVertexAttribPointer(position_loc + i, 3, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 3, 0);
            glVertexAttribDivisor(position_loc + i, 1);
    }

    glGenBuffers(1, &IBID_Phases);
    glBindBuffer(GL_ARRAY_BUFFER, IBID_Phases);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * phases.size(), phases.data(),
                 GL_STATIC_DRAW);
    position_loc = glGetAttribLocation(bbprog->pid, "InstancePhases");

    for (int i = 0; i < (myCubeDim * myCubeDim); i++)
    {
        glEnableVertexAttribArray(position_loc + i);
        glVertexAttribPointer(position_loc + i, 2, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 2, 0);
            glVertexAttribDivisor(position_loc + i, 1);
    }

    glGenBuffers(1, &IBID_TexCoordsOffset);
    glBindBuffer(GL_ARRAY_BUFFER, IBID_TexCoordsOffset);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * offsets_tex_coords.size(),
                 offsets_tex_coords.data(), GL_STATIC_DRAW);
    position_loc = glGetAttribLocation(bbprog->pid, "InstanceTexOffset");

    for (int i = 0; i < (myCubeDim * myCubeDim); i++)
    {
        glEnableVertexAttribArray(position_loc + i);
        glVertexAttribPointer(position_loc + i, 2, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 2, 0);
            glVertexAttribDivisor(position_loc + i, 1);
    }

    // indices
    glGenBuffers(1, &IndexBufferId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * 6, rect_indi,
                 GL_STATIC_DRAW);

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
    glGenTextures(1, &TextureN);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, TextureN);
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
}

int Billboard::draw(std::shared_ptr<Program>& bbprog,
                    glm::vec3 campos,
                    double frametime,
                    glm::mat4 P,
                    glm::mat4 V)
{
    bbprog->bind();

    glBindVertexArray(VertexArrayID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);

    glUniform3f(bbprog->getUniform("campos"), campos.x, campos.y, campos.z);
    static float light_w = 0;
    static float instance_interp_w = 0;
    static int animation_stage = 0;
    if((animation_stage == 1 && instance_interp_w > 0.2f) || animation_stage > 1)
    {
    light_w += 4*frametime;
    }

    instance_interp_w += frametime / 5.0f;

    glm::vec4 l1p = glm::vec4(0, 0.5, -5, 0);
    l1p += glm::vec4(-light_w + 5, 0, -5, 0);

    glUniform3f(bbprog->getUniform("light1pos"), l1p.x, l1p.y, l1p.z);

    glm::mat4 Vi = glm::transpose(V);
    Vi[0][3] = 0;
    Vi[1][3] = 0;
    Vi[2][3] = 0;

    glUniformMatrix4fv(bbprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
    glUniformMatrix4fv(bbprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);

    glUniform1i(bbprog->getUniform("myCubeDim"), myCubeDim);

    if (animation_stage < 1)
    {
        animation_stage = 1;
        instance_interp_w = -1;
    }
    else if (animation_stage < 2 && instance_interp_w > 1.0f)
    {
        animation_stage = 2;
        instance_interp_w = 0;
    }
    else if (animation_stage < 3 && instance_interp_w > 1.0f)
    {
        animation_stage = 3;
        instance_interp_w = 0;
    }

    glUniform1i(bbprog->getUniform("animation_stage"), animation_stage);
    glUniform1f(bbprog->getUniform("interp"), instance_interp_w);

    glm::mat4 ScaleM;

    ScaleM = glm::scale(glm::mat4(1), targetScale);
    glUniformMatrix4fv(bbprog->getUniform("ScaleM"), 1, GL_FALSE,
                       &ScaleM[0][0]);
    glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0,
                            (myCubeDim * myCubeDim));

    glBindVertexArray(0);

    bbprog->unbind();

    return animation_stage;
}
