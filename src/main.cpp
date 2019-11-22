/*
ZJ Wood CPE 471 Lab 3 base code
*/

#include <glad/glad.h>
#include <algorithm>
#include <iostream>
#include <vector>

#include "Cube.h"
#include "GLSL.h"
#include "MatrixStack.h"
#include "Program.h"
#include "message.h"
#include "webclient.h"

#include "Shape.h"
#include "WindowManager.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
using namespace std;

#define PI_CONST ((float)(103993.0f / 33102.0f))
#define PLAYER_CURSOR_COUNT 256

static uint8_t tmpWriteBuf[256];

double get_last_elapsed_time()
{
    static double lasttime = glfwGetTime();
    double actualtime = glfwGetTime();
    double difference = actualtime - lasttime;
    lasttime = actualtime;
    return difference;
}

class camera
{
   public:
    glm::vec3 pos, rot;
    int w, a, s, d;
    camera()
    {
        w = a = s = d = 0;
        pos = glm::vec3(0, 0, 0);
        rot = glm::vec3(0, 0, 0);
    }
    glm::mat4 process(double ftime)
    {
        float speed = 0;
        if (w == 1)
        {
            speed = 1 * ftime;
        }
        else if (s == 1)
        {
            speed = -1 * ftime;
        }
        float yangle = 0;
        if (a == 1)
            yangle = -1 * ftime;
        else if (d == 1)
            yangle = 1 * ftime;
        rot.y += yangle * 2;
        glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
        glm::vec4 dir = glm::vec4(0, 0, speed, 1);
        dir = dir * R;
        pos += glm::vec3(dir.x, dir.y, dir.z);
        glm::mat4 T = glm::translate(glm::mat4(1), pos);
        return R * T;
    }
};

camera mycam;

class Application : public EventCallbacks
{
   public:
    int kn = 0;
    int md = 0;
    WindowManager *windowManager = nullptr;

    // Our shader program
    std::shared_ptr<Program> prog, shapeprog;

    // Contains vertex information for OpenGL
    GLuint VertexArrayID;
    GLuint CylinderArrayID;

    // Data necessary to give our box to OpenGL
    GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

    GLuint CylinderVertexBufferId, CylinderVertexColorId,
        CylinderBufferIndeciesID;

    Shape shape;
    InterpObject ball;
    MessageContext *msg_context;

    float temporary_cursor;

    int arm_l, arm_r;
    std::vector<Target> cylinders;
    std::vector<CylCoords> cursors;

    TargetManager cubes;
    int cubesSize = 5;

    void keyCallback(
        GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        if (key == GLFW_KEY_W && action == GLFW_PRESS)
        {
            mycam.w = 1;
        }
        if (key == GLFW_KEY_W && action == GLFW_RELEASE)
        {
            mycam.w = 0;
        }
        if (key == GLFW_KEY_S && action == GLFW_PRESS)
        {
            mycam.s = 1;
        }
        if (key == GLFW_KEY_S && action == GLFW_RELEASE)
        {
            mycam.s = 0;
        }
        if (key == GLFW_KEY_A && action == GLFW_PRESS)
        {
            mycam.a = 1;
        }
        if (key == GLFW_KEY_A && action == GLFW_RELEASE)
        {
            mycam.a = 0;
        }
        if (key == GLFW_KEY_D && action == GLFW_PRESS)
        {
            mycam.d = 1;
        }
        if (key == GLFW_KEY_D && action == GLFW_RELEASE)
        {
            mycam.d = 0;
        }
        if (key == GLFW_KEY_N && action == GLFW_PRESS) kn = 1;
        if (key == GLFW_KEY_N && action == GLFW_RELEASE) kn = 0;
        if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE)
        {
            msg_context->mutex_boxes.lock();
            for (int i = 0; i < cubes.elements.size(); i++)
            {
                cubes.elements.data()[i].beShot(i,msg_context->player_id, &msg_context->color_list);
            }
            msg_context->mutex_boxes.unlock();
        }
    }

    // callback for the mouse when clicked move the triangle when helper
    // functions written
    void mouseCallback(GLFWwindow *window, int button, int action, int mods)
    {
        double posX, posY;
        float newPt[2];
        if (action == GLFW_PRESS)
        {
            md = 1;
        }
        else if (action == GLFW_RELEASE)
        {
            md = 0;
        }
    }

    // if the window is resized, capture the new size and reset the viewport
    void resizeCallback(GLFWwindow *window, int in_width, int in_height)
    {
        // get the window size - may be different then pixels for retina
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
    }

    static inline float map(
        float value, float min1, float max1, float min2, float max2)
    {
        return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
    }

    void initCubeModel()
    {
        ball.show = 0;

        for (int i = 0; i < 0x80; i++)
        {
            Target cube = Target();
            cube.show = 0;
            cubes.elements.push_back(cube);
        }
    }

    /*Note that any gl calls must always happen after a GL state is initialized
     */
    void initGeom()
    {
        string resourceDirectory = "../resources";
        // try t800.obj or F18.obj ...
        shape.loadMesh(resourceDirectory + "/sphere.obj");
        shape.resize();
        shape.init();

        // generate the VAO
        glGenVertexArrays(1, &VertexArrayID);
        glBindVertexArray(VertexArrayID);

        // generate vertex buffer to hand off to OGL
        glGenBuffers(1, &VertexBufferID);
        // set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

        GLfloat cube_vertices[] = {
            // front
            -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
            // back
            -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0,
            // tube 8 - 11
            -1.0, -1.0, 1.0, 1.0, -1.0, 1.0, 1.0, 1.0, 1.0, -1.0, 1.0, 1.0,
            // 12 - 15
            -1.0, -1.0, -1.0, 1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0, 1.0, -1.0

        };
        // actually memcopy the data - only do this once
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices,
                     GL_DYNAMIC_DRAW);

        // we need to set up the vertex array
        glEnableVertexAttribArray(0);
        // key function to get up how many elements to pull out at a time (3)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        // color
        GLfloat cube_colors[] = {
            // front colors
            0.8,
            0.8,
            1.0,
            0.8,
            0.8,
            1.0,
            0.8,
            0.8,
            1.0,
            0.8,
            0.8,
            1.0,
            // back colors
            1,
            0.8,
            1,
            1,
            0.8,
            1,
            1,
            0.8,
            1,
            1,
            0.8,
            1,
            // tube colors
            0.5,
            1.0,
            1.0,
            0.5,
            1.0,
            1.0,
            0.5,
            1.0,
            1.0,
            0.5,
            1.0,
            1.0,
            1.0,
            1.0,
            0.8,
            1.0,
            1.0,
            0.8,
            1.0,
            1.0,
            0.8,
            1.0,
            1.0,
            0.8,
        };
        glGenBuffers(1, &VertexColorIDBox);
        // set the current state to focus on our vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, VertexColorIDBox);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_colors), cube_colors,
                     GL_STATIC_DRAW);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);

        glGenBuffers(1, &IndexBufferIDBox);
        // set the current state to focus on our vertex buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
        GLushort cube_elements[] = {

            // front
            0, 1, 2, 2, 3, 0,
            // back
            7, 6, 5, 5, 4, 7,
            // tube 8-11, 12-15
            8, 12, 13, 8, 13, 9, 9, 13, 14, 9, 14, 10, 10, 14, 15, 10, 15, 11,
            11, 15, 12, 11, 12, 8

        };
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements),
                     cube_elements, GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    // General OGL initialization - set OGL state here
    void init(const std::string &resourceDirectory)
    {
        GLSL::checkVersion();

        // Set background color.
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        // Enable z-buffer test.
        glEnable(GL_DEPTH_TEST);
        // Enable blending/transparency
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // glCullFace(GL_FRONT_AND_BACK);

        // Initialize the GLSL program.
        prog = std::make_shared<Program>();
        prog->setVerbose(true);
        prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl",
                             resourceDirectory + "/shader_fragment.glsl");
        if (!prog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!"
                      << std::endl;
            exit(1);  // make a breakpoint here and check the output window for
                      // the error message!
        }
        prog->addUniform("P");
        prog->addUniform("V");
        prog->addUniform("M");
        prog->addUniform("bonuscolor");
        prog->addAttribute("vertPos");
        prog->addAttribute("vertColor");
        // Initialize the GLSL program.
        shapeprog = std::make_shared<Program>();
        shapeprog->setVerbose(true);
        shapeprog->setShaderNames(resourceDirectory + "/shape_vertex.glsl",
                                  resourceDirectory + "/shape_fragment.glsl");
        if (!shapeprog->init())
        {
            std::cerr << "One or more shaders failed to compile... exiting!"
                      << std::endl;
            exit(1);  // make a breakpoint here and check the output window for
                      // the error message!
        }
        shapeprog->addUniform("P");
        shapeprog->addUniform("V");
        shapeprog->addUniform("M");
        shapeprog->addUniform("camPos");
        shapeprog->addAttribute("vertPos");
        shapeprog->addAttribute("vertNor");
        shapeprog->addAttribute("vertTex");
    }

    /****DRAW
    This is the most important function in your program - this is where you
    will actually issue the commands to draw any geometry you have set up to
    draw
    ********/
    void render()
    {
        double frametime = get_last_elapsed_time();
        // Get current frame buffer size.
        int width, height;
        glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
        float aspect = width / (float)height;
        glViewport(0, 0, width, height);
        // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Clear framebuffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create the matrix stacks - please leave these alone for now

        glm::mat4 V, M, P;  // View, Model and Perspective matrix
        V = glm::mat4(1);
        M = glm::mat4(1);
        // Apply orthographic projection....
        P = glm::perspective((float)(3.14159 / 4.),
                             (float)((float)width / (float)height), 0.1f,
                             1000.0f);  // so much type casting... GLM metods
                                        // are quite funny ones

        // animation with the model matrix:
        static float w = 0.0;
        w += 0.03;  // rotation angle
        static float t = 0;
        t += 0.01;
        float trans = 0;  // sin(t) * 2;
        glm::mat4 T = glm::mat4(1.0f);
        glm::mat4 RotateY =
            glm::rotate(glm::mat4(1.0f), w, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 TransZ =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0));
        glm::mat4 TransX =
            glm::translate(glm::mat4(1.0f), glm::vec3(0.4f, 0.0f, 0.0));
        glm::mat4 S = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));

        // Draw the box using GLSL.
        prog->bind();

        V = mycam.process(frametime);
        // send the matrices to the shaders
        glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
        glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);

        glBindVertexArray(VertexArrayID);

        double xpos, ypos;

        glfwGetCursorPos(windowManager->getHandle(), &xpos, &ypos);

        float angle =
            map(xpos, 0, width, sin(-PI_CONST * 0.25), sin(PI_CONST * 0.25));
        angle = asin(-angle);
        angle -= fmod(mycam.rot.y, 2 * PI_CONST);
        // angle -= mycam.rot.y;
        angle += PI_CONST;
        float vangle = map(-ypos, -height, 0, 0, 1);

        assignBytesFromFloat(tmpWriteBuf, angle, 5);
        assignBytesFromFloat(tmpWriteBuf + 5, vangle, 5);
        clientMsgWrite(MSG_CURSOR_UPDATE, tmpWriteBuf, 10);

        CylCoords myCursor;
        myCursor.angle = angle;
        myCursor.height = vangle;
        myCursor.calc_result();

        if (md)
        {
            ball.source.scale = vec3(4, 4, 4);
            ball.source.pos = vec3(0, -8, 0);
            ball.source.rot = vec3(0, 0, 0);
            ball.target = ball.source;

            ball.target.pos = myCursor.result.pos;
            ball.target.scale = vec3(0.1, 0.1, 0.1);
            ball.phase = 0;
            ball.show = 1;
            ball.resetInterp();
        }

        if (ball.interp > 1)
        {
            ball.show = 0;
        }

        ball.interp += 3 * frametime;
        ball.interpBetween();

        if (msg_context->player_id >= 0)
        {
            myCursor.calc_result();
            M = myCursor.result.calc_scale(myCursor.result.calc_no_scale());
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
            vec3 clr = msg_context->color_list.get_color(msg_context->player_id);
            glUniform3f(prog->getUniform("bonuscolor"), clr.x, clr.y, clr.z);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);
        }

        msg_context->mutex_cursors.lock();
        for (int i = 0; i < cursors.size(); i++)
        {
            if (cursors.data()[i].show)
            {
                /* printf("%3.3f %3.3f \n", myCursor.angle,
                        cursors.data()[i].angle);*/

                if (msg_context->player_id == i) continue;

                cursors.data()[i].calc_result();
                M = cursors.data()[i].result.calc_scale(
                    cursors.data()[i].result.calc_no_scale());
                vec3 clr = msg_context->color_list.get_color(i);
                glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE,
                                   &M[0][0]);

                glUniform3f(prog->getUniform("bonuscolor"), clr.x, clr.y,
                            clr.z);

                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void *)0);
            }
        }
        msg_context->mutex_cursors.unlock();

        msg_context->mutex_boxes.lock();
        for (int i = 0; i <= cubes.elements.size(); i++)
        {
            Target *cube = &cubes.elements.data()[i];
            if (cube->show || i == 0)
            {
                if (!cube->hit)
                {
                    cube->interp += frametime / 5;
                    cube->interpBetween();
                }
                else if (cube->interp < 1)
                {
                    cube->interp += frametime / 2;
                    cube->interpBetween();
                }

                if (!cube->hit && ball.show &&
                    distance(ball.postInterp.pos, cube->postInterp.pos) < 1)
                {
                    cube->beShot(i, msg_context->player_id, &msg_context->color_list	);
                }
                if (cube->hit && cube->interp > 1)
                {
                    cube->show = 0;
                }
                if (cube->hit)
                {
                    glUniform3f(prog->getUniform("bonuscolor"), cube->bonuscolor.r, cube->bonuscolor.g, cube->bonuscolor.b);
                }
                else
                {
                    glUniform3f(prog->getUniform("bonuscolor"), 1, 1, 1);
                }

                cube->drawTarget(prog, cubes.elements, mat4(1));
            }
        }
        msg_context->mutex_boxes.unlock();

        glBindVertexArray(0);
        prog->unbind();

        if (ball.show)
        {
            shapeprog->bind();

            glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
            glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
            glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);

            ball.sendModelMatrix(shapeprog, glm::mat4(1));
            shape.draw(shapeprog);

            shapeprog->unbind();
        }
    }
};
//******************************************************************************************
int main(int argc, char **argv)
{
    std::string resourceDir =
        "../resources";  // Where the resources are loaded from
    if (argc >= 2)
    {
        resourceDir = argv[1];
    }

    Application *application = new Application();

    /* your main will always include a similar set up to establish your window
            and GL context, etc. */
    WindowManager *windowManager = new WindowManager();
    windowManager->init(1920, 1080);
    windowManager->setEventCallbacks(application);
    application->windowManager = windowManager;

    /* This is the code that will likely change program to program as you
            may need to initialize or set up different data and state */
    // Initialize scene.
    application->init(resourceDir);
    application->initGeom();
    application->initCubeModel();

    glfwSetInputMode(application->windowManager->getHandle(), GLFW_CURSOR,
                     GLFW_CURSOR_HIDDEN);

    MessageContext context;
    context.color_list = ColorList();
    context.player_id = -1;
    application->msg_context = &context;
    context.boxes = &application->cubes.elements;

    application->cursors = std::vector<CylCoords>();
    for (int i = 0; i < PLAYER_CURSOR_COUNT; i++)
    {
        CylCoords c;
        c.show = 0;
        application->cursors.push_back(c);
    }
    context.cursors = &application->cursors;

    clientbegin(&context);

    static int count = 0;

    bool keepReading = 1;
    std::thread readThread = std::thread(repeatedRead);
    // std::thread flushThread = std::thread(repeatedWrite);

    // Loop until the user closes the window.
    while (!glfwWindowShouldClose(windowManager->getHandle()))
    {
        // Render scene.
        application->render();

        // Swap front and back buffers.
        glfwSwapBuffers(windowManager->getHandle());
        // Poll for and process events.
        glfwPollEvents();

        /*if (count == 0)
        {
            clientflush();
        }

        count = (count + 1) & 0xF;*/
    }
    stopRepeatedRead();
    // stopRepeatedWrite();
    readThread.join();
    // flushThread.join();

    // Quit program.
    windowManager->shutdown();
    return 0;
}
