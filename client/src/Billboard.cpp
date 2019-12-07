
#include "Billboard.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "GLSL.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void Billboard::init(std::shared_ptr<Program>& bbprog)
{
		glm::vec3 rect_pos[4];
		glm::vec2 rect_tex[4];
		glm::vec3 rect_norm[4];
		int rect_indi[6] = {0,1,2,0,2,3};
		rect_pos[0] = glm::vec3(0, 0, 0);		rect_pos[1] = glm::vec3(1, 0, 0);		rect_pos[2] = glm::vec3(1, 1, 0);		rect_pos[3] = glm::vec3(0, 1, 0);
		rect_norm[0] = glm::vec3(0, 0, 1);		rect_norm[1] = glm::vec3(0, 0, 1);		rect_norm[2] = glm::vec3(0, 0, 1);		rect_norm[3] = glm::vec3(0, 0, 1);	
		rect_tex[0] = glm::vec2(0, 0);			rect_tex[1] = glm::vec2(1, 0);			rect_tex[2] = glm::vec2(1, 1);			rect_tex[3] = glm::vec2(0, 1);		
		

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);
		//positions
		glGenBuffers(1, &PositionBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, PositionBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*4, rect_pos, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		// normals
		glGenBuffers(1, &NormBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, NormBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*4, rect_norm, GL_STATIC_DRAW);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//texture coords
		glGenBuffers(1, &TexCoordsBufferId);
		glBindBuffer(GL_ARRAY_BUFFER, TexCoordsBufferId);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2)*4, rect_tex, GL_STATIC_DRAW);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
		//indices
		glGenBuffers(1, &IndexBufferId);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*6, rect_indi, GL_STATIC_DRAW);
		glBindVertexArray(0);

		int width, height, channels;
		char filepath[1000];

    std::string resourceDirectory = "../resources";
    
		glUseProgram(bbprog->pid);

    std::string str = resourceDirectory + "/hexagon-marble-tile-500x500.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
   
		GLuint Tex1Location = glGetUniformLocation(bbprog->pid, "title_tex");//tex, tex2... sampler in the fragment shader
		glUniform1i(Tex1Location, 0);
}

void Billboard::draw(std::shared_ptr<Program>& bbprog, glm::mat4 P, glm::mat4 V)
{
        bbprog->bind();


		glBindVertexArray(VertexArrayID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferId);
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, Texture);

    glm::mat4 Vi = glm::transpose(V);
    Vi[0][3] = 0;
    Vi[1][3] = 0;
    Vi[2][3] = 0;

            glm::mat4 TransZ = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f,-0.5f, -3));
            glm::mat4 M = TransZ * Vi;
                    glUniformMatrix4fv(bbprog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
        glUniformMatrix4fv(bbprog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
        glUniformMatrix4fv(bbprog->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)0);

		glBindVertexArray(0);		

        bbprog->unbind();
}


