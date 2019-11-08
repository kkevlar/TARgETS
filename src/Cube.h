#pragma once
#include "GLSL.h"
#include "program.h"
#include <memory>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

class MatrixIngridients
{
public:
	glm::vec3 pos, rot, scale, anchor;
	MatrixIngridients();
	glm::mat4 calc_no_scale();
	glm::mat4 calc_scale(glm::mat4 noscale);
	void self_interpolate_between(MatrixIngridients& a, MatrixIngridients& b, float z);
};

class Cube
{

public:

	
	MatrixIngridients target;
	MatrixIngridients source;
	
	int parent;
	float phase;
	float interp;
	
	Cube();
	Cube(int x, int y, int z, glm::vec3 centeroffset);
	Cube(int x, int y, int z, glm:: vec3 centeroffset, int cubesSize);
	void sendModelMatrix(std::shared_ptr<Program> prog, std::vector<Cube>& elements, glm::mat4 parentM);
	void drawElement(std::shared_ptr<Program> prog, std::vector<Cube> &elements, glm::mat4 parentM);
	void interpBetween();

private:
	glm::mat4 cached_no_scale;
	MatrixIngridients current;
	void init(int x, int y, int z, glm::vec3 centeroffset);
};

class CubeModel
{
public:
	std::vector<Cube> elements;

	int snowman_index;
	int pickaxe_index;

	CubeModel();

	void draw(std::shared_ptr<Program> prog);
};