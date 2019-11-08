

#include "GLSL.h"
#include "program.h"
#include "Cube.h"
#include <memory>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define PI_CONST ((float)( 103993.0f / 33102.0f))

using namespace glm;


	MatrixIngridients::MatrixIngridients()
	{
		pos = glm::vec3(0, 0, 0);
		anchor = glm::vec3(0, 0, 0);
		rot = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
	}

	glm::vec3 v3_linear_interpolate(glm::vec3 &a, glm::vec3 &b, float z)
	{
		glm::vec3 result;

		result.x = a.x * z + (1 - z) * b.x;
		result.y = a.y * z + (1 - z) * b.y;
		result.z = a.z * z + (1 - z) * b.z;

		return result;
	}

	void MatrixIngridients::self_interpolate_between(MatrixIngridients &a, MatrixIngridients &b, float z)
	{
		pos = v3_linear_interpolate(a.pos, b.pos, z);
		rot = v3_linear_interpolate(a.rot, b.rot, z);
		scale = v3_linear_interpolate(a.scale, b.scale, z);
		anchor = v3_linear_interpolate(a.anchor, b.anchor, z);
	}

	glm::mat4 MatrixIngridients::calc_no_scale()
	{
		glm::mat4 rotM = glm::mat4(1);
		rotM *= glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));

		rotM *= glm::rotate(glm::mat4(1), rot.x, glm::vec3(1, 0, 0));
		rotM *= glm::rotate(glm::mat4(1), rot.z, glm::vec3(0, 0, 1));
		glm::mat4 transM = glm::translate(glm::mat4(1), pos);
		glm::mat4 anchorM = glm::translate(glm::mat4(1), anchor);

		return transM * rotM * anchorM;
	}

	glm::mat4 MatrixIngridients::calc_scale(glm::mat4 noscale)
	{
		glm::mat4 scaleMat = glm::scale(glm::mat4(1), scale);
		return noscale * scaleMat;
	}


	static inline float map(float value, float min1, float max1, float min2, float max2) {
		return min2 + (value - min1) * (max2 - min2) / (max1 - min1);
	}


	void Cube::init(int x, int y, int z, glm::vec3 centeroffset)
	{
		cached_no_scale = glm::mat4(1);
		parent = -1;
		phase = 1;

		vec3 pos = vec3(x, y, z);
		pos -= centeroffset;
		vec3 base = pos;
		source.pos = pos;
		target.pos = vec3(pos.x , pos.y , pos.z - 150 );
		source.scale = vec3(0.5, 0.5, 0.5);
		target.scale = vec3(0.00001, 0.00001, 0.00001);
		interp = 0;
		postInterp.scale = vec3(0.5, 0.5, 0.5);
	}

	Cube::Cube()
	{
		init(0, 0, 0, glm::vec3(0,0,0));
	}

	Cube::Cube(int x, int y, int z, glm::vec3 centeroffset)
	{
		init(x, y, z, centeroffset);
	}


	Cube::Cube(int x, int y, int z, vec3 centeroffset, int cubesSize)
	{
		init(x, y, z, centeroffset);
		phase = map( (rand() % 3000) + (z * 1.0 * sign(z) ) * (4000.0f/(cubesSize * 1.0f)), 0, 7000, 0, 0.3);
		interp = phase;
	}

	void Cube::sendModelMatrix(std::shared_ptr<Program> prog, std::vector<Cube>& elements, glm::mat4 parentM)
	{
		cached_no_scale = postInterp.calc_no_scale();

		if (parent >= 0)
		{
			cached_no_scale = elements.data()[parent].cached_no_scale * cached_no_scale;
		}

		glm::mat4 M = parentM * postInterp.calc_scale(cached_no_scale);

		glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, &M[0][0]);
	}

	void Cube::drawElement(std::shared_ptr<Program> prog, std::vector<Cube> &elements, glm::mat4 parentM)
	{
		sendModelMatrix(prog, elements, parentM);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, (void*)0);
	}

	

	void Cube::interpBetween()
	{
		float z;
		
		z = interp;

		z = glm::clamp(z, 0.0f, 1.0f);
		//z -= (((int)z));

		z = map(z, 0.0f, 1.0f, -1*PI_CONST * 0.5, PI_CONST * 0.5f);
		z = sin(z);
		z = map(z, -1, 1, 0, 1);
		z = glm::clamp(z, 0.0f, 1.0f);
		postInterp.self_interpolate_between(target, source, z);
	}

	void Cube::resetInterp()
	{
		interp = phase;
	}

	
	CubeModel::CubeModel()
	{
		//values = MatrixIngridients();
		elements = std::vector<Cube>();
	}

	void CubeModel::draw(std::shared_ptr<Program> prog)
	{
		glm::mat4 M = glm::mat4(1);

		for (int i = 0; i < elements.size(); i++)
		{
			elements.at(i).drawElement(prog, elements, M);
		}
	}
