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

class CylCoords
{
public:
	float angle;
	float height;
	bool show;
	MatrixIngridients result;

	void calc_result();
};

class Cube
{

public:

	
	MatrixIngridients target;
	MatrixIngridients source;
	MatrixIngridients postInterp;
	
	int parent;
	float phase;
	float interp;
	bool show;
	bool hit;
	bool dosin;
	Cube();
	Cube(int x, int y, int z, glm::vec3 centeroffset);
	Cube(int x, int y, int z, glm:: vec3 centeroffset, int cubesSize);
	void sendModelMatrix(std::shared_ptr<Program> prog, glm::mat4 parentM);
	void sendModelMatrix(std::shared_ptr<Program> prog, std::vector<Cube>& elements, glm::mat4 parentM);
	void drawElement(std::shared_ptr<Program> prog, std::vector<Cube> &elements, glm::mat4 parentM);
	void interpBetween();
	void resetInterp();
    void beShot(int myIndex);

private:
	glm::mat4 cached_no_scale;
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