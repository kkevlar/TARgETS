#include "Shape.h"
#include <iostream>
#include "GLSL.h"
#include "Program.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;


void Shape::loadMesh(const string &meshName, string *mtlpath, unsigned char *(loadimage)(char const *, int *, int *, int *, int))
{
	// Load geometry
	// Some obj files contain material information.
	// We'll ignore them for this assignment.
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> objMaterials;
	string errStr;
	bool rc = 0;
	if (mtlpath)
		rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str(), mtlpath->c_str());
	else
		rc = tinyobj::LoadObj(shapes, objMaterials, errStr, meshName.c_str());


	if (!rc)
	{
		cerr << errStr << endl;
	}
	else if (shapes.size())
	{
		obj_count = shapes.size();
		posBuf = new std::vector<float>[shapes.size()];
		norBuf = new std::vector<float>[shapes.size()];
		texBuf = new std::vector<float>[shapes.size()];
		eleBuf = new std::vector<unsigned int>[shapes.size()];

		eleBufID = new unsigned int[shapes.size()];
		posBufID = new unsigned int[shapes.size()];
		norBufID = new unsigned int[shapes.size()];
		texBufID = new unsigned int[shapes.size()];
		vaoID = new unsigned int[shapes.size()];
		materialIDs = new unsigned int[shapes.size()];

		textureIDs = new unsigned int[shapes.size()];

		for (int i = 0; i < obj_count; i++)
		{
			//load textures
			textureIDs[i] = 0;
			//texture sky			
			posBuf[i] = shapes[i].mesh.positions;
			norBuf[i] = shapes[i].mesh.normals;
			texBuf[i] = shapes[i].mesh.texcoords;
			eleBuf[i] = shapes[i].mesh.indices;
			if (shapes[i].mesh.material_ids.size()>0)
				materialIDs[i] = shapes[i].mesh.material_ids[0];
			else
				materialIDs[i] = -1;

		}
	}
	//material:
	for (int i = 0; i < objMaterials.size(); i++)
		if (objMaterials[i].diffuse_texname.size()>0)
		{
			char filepath[1000];
			int width, height, channels;
			string filename = objMaterials[i].ambient_texname;
			int subdir = filename.rfind("\\");
			if (subdir > 0)
				filename = filename.substr(subdir + 1);
			string str = *mtlpath + filename;
			strcpy(filepath, str.c_str());
			//stbi_load(char const *filename, int *x, int *y, int *comp, int req_comp)

			unsigned char* data = loadimage(filepath, &width, &height, &channels, 4);
			glGenTextures(1, &textureIDs[i]);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureIDs[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
			//delete[] data;
		}

	int z;
	z = 0;
}

void Shape::resize()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;
	float scaleX, scaleY, scaleZ;
	float shiftX, shiftY, shiftZ;
	float epsilon = 0.001f;

	minX = minY = minZ = 1.1754E+38F;
	maxX = maxY = maxZ = -1.1754E+38F;

	// Go through all vertices to determine min and max of each dimension
	for (int i = 0; i < obj_count; i++)
		for (size_t v = 0; v < posBuf[i].size() / 3; v++)
		{
			if (posBuf[i][3 * v + 0] < minX) minX = posBuf[i][3 * v + 0];
			if (posBuf[i][3 * v + 0] > maxX) maxX = posBuf[i][3 * v + 0];

			if (posBuf[i][3 * v + 1] < minY) minY = posBuf[i][3 * v + 1];
			if (posBuf[i][3 * v + 1] > maxY) maxY = posBuf[i][3 * v + 1];

			if (posBuf[i][3 * v + 2] < minZ) minZ = posBuf[i][3 * v + 2];
			if (posBuf[i][3 * v + 2] > maxZ) maxZ = posBuf[i][3 * v + 2];
		}

	// From min and max compute necessary scale and shift for each dimension
	float maxExtent, xExtent, yExtent, zExtent;
	xExtent = maxX - minX;
	yExtent = maxY - minY;
	zExtent = maxZ - minZ;
	if (xExtent >= yExtent && xExtent >= zExtent)
	{
		maxExtent = xExtent;
	}
	if (yExtent >= xExtent && yExtent >= zExtent)
	{
		maxExtent = yExtent;
	}
	if (zExtent >= xExtent && zExtent >= yExtent)
	{
		maxExtent = zExtent;
	}
	scaleX = 2.0f / maxExtent;
	shiftX = minX + (xExtent / 2.0f);
	scaleY = 2.0f / maxExtent;
	shiftY = minY + (yExtent / 2.0f);
	scaleZ = 2.0f / maxExtent;
	shiftZ = minZ + (zExtent / 2.0f);

	// Go through all verticies shift and scale them
	for (int i = 0; i < obj_count; i++)
		for (size_t v = 0; v < posBuf[i].size() / 3; v++)
		{
			posBuf[i][3 * v + 0] = (posBuf[i][3 * v + 0] - shiftX) * scaleX;
			assert(posBuf[i][3 * v + 0] >= -1.0f - epsilon);
			assert(posBuf[i][3 * v + 0] <= 1.0f + epsilon);
			posBuf[i][3 * v + 1] = (posBuf[i][3 * v + 1] - shiftY) * scaleY;
			assert(posBuf[i][3 * v + 1] >= -1.0f - epsilon);
			assert(posBuf[i][3 * v + 1] <= 1.0f + epsilon);
			posBuf[i][3 * v + 2] = (posBuf[i][3 * v + 2] - shiftZ) * scaleZ;
			assert(posBuf[i][3 * v + 2] >= -1.0f - epsilon);
			assert(posBuf[i][3 * v + 2] <= 1.0f + epsilon);
		}
}

void Shape::init()
{
	for (int i = 0; i < obj_count; i++)

	{


		// Initialize the vertex array object
		glGenVertexArrays(1, &vaoID[i]);
		glBindVertexArray(vaoID[i]);

		// Send the position array to the GPU
		glGenBuffers(1, &posBufID[i]);
		glBindBuffer(GL_ARRAY_BUFFER, posBufID[i]);
		glBufferData(GL_ARRAY_BUFFER, posBuf[i].size() * sizeof(float), posBuf[i].data(), GL_STATIC_DRAW);

		// Send the normal array to the GPU
		if (norBuf[i].empty())
		{
			norBufID[i] = 0;
		}
		else
		{
			glGenBuffers(1, &norBufID[i]);
			glBindBuffer(GL_ARRAY_BUFFER, norBufID[i]);
			glBufferData(GL_ARRAY_BUFFER, norBuf[i].size() * sizeof(float), norBuf[i].data(), GL_STATIC_DRAW);
		}

		// Send the texture array to the GPU
		if (texBuf[i].empty())
		{
			texBufID[i] = 0;
		}
		else
		{
			glGenBuffers(1, &texBufID[i]);
			glBindBuffer(GL_ARRAY_BUFFER, texBufID[i]);
			glBufferData(GL_ARRAY_BUFFER, texBuf[i].size() * sizeof(float), texBuf[i].data(), GL_STATIC_DRAW);
		}

		// Send the element array to the GPU
		glGenBuffers(1, &eleBufID[i]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf[i].size() * sizeof(unsigned int), eleBuf[i].data(), GL_STATIC_DRAW);

		// Unbind the arrays
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		assert(glGetError() == GL_NO_ERROR);
	}
}
void Shape::draw(const shared_ptr<Program> prog) const
{
	for (int i = 0; i < obj_count; i++)

	{
		int h_pos, h_nor, h_tex;
		h_pos = h_nor = h_tex = -1;

		glBindVertexArray(vaoID[i]);
		// Bind position buffer
		h_pos = prog->getAttribute("vertPos");
		GLSL::enableVertexAttribArray(h_pos);
		glBindBuffer(GL_ARRAY_BUFFER, posBufID[i]);
		glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);

		// Bind normal buffer
		h_nor = prog->getAttribute("vertNor");
		if (h_nor != -1 && norBufID[i] != 0)
		{
			GLSL::enableVertexAttribArray(h_nor);
			glBindBuffer(GL_ARRAY_BUFFER, norBufID[i]);
			glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0);
		}

		if (texBufID[i] != 0)
		{
			// Bind texcoords buffer
			h_tex = prog->getAttribute("vertTex");
			if (h_tex != -1 && texBufID[i] != 0)
			{
				GLSL::enableVertexAttribArray(h_tex);
				glBindBuffer(GL_ARRAY_BUFFER, texBufID[i]);
				glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
			}
		}

		// Bind element buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID[i]);

		//texture
		

		int textureindex = materialIDs[i];
		if (textureindex >= 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textureIDs[textureindex]);
		}
		// Draw
		glDrawElements(GL_TRIANGLES, (int)eleBuf[i].size(), GL_UNSIGNED_INT, (const void *)0);

		// Disable and unbind
		if (h_tex != -1)
		{
			GLSL::disableVertexAttribArray(h_tex);
		}
		if (h_nor != -1)
		{
			GLSL::disableVertexAttribArray(h_nor);
		}
		GLSL::disableVertexAttribArray(h_pos);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

}

typedef struct
{
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
} tri_t;

static inline float max_2_float(float a, float b)
{
	if (a < b)
	{
		return b;
	}
	else
	{
		return a;
	}
}

static inline float max_3_float(float a, float b, float c)
{
	return max_2_float(max_2_float(a, b), c);
}

static inline float min_2_float(float a, float b)
{
	if (a < b)
	{
		return a;
	}
	else
	{
		return b;
	}
}

static inline float min_3_float(float a, float b, float c)
{
	return min_2_float(min_2_float(a, b), c);
}

static inline float bound_box_x_min(tri_t& tri)
{
	return min_3_float(tri.a.x, tri.b.x, tri.c.x);
}

static inline float bound_box_y_min(tri_t& tri)
{
	return min_3_float(tri.a.y, tri.b.y, tri.c.y);
}

static inline float bound_box_z_min(tri_t& tri)
{
	return min_3_float(tri.a.z, tri.b.z, tri.c.z);
}


static inline float bound_box_x_max(tri_t& tri)
{
	return max_3_float(tri.a.x, tri.b.x, tri.c.x);
}

static inline float bound_box_y_max(tri_t& tri)
{
	return max_3_float(tri.a.y, tri.b.y, tri.c.y);
}

static inline float bound_box_z_max(tri_t& tri)
{
	return max_3_float(tri.a.z, tri.b.z, tri.c.z);
}

static inline float tri_area(tri_t& tri)
{
	return 0.5 * glm::abs(glm::length(glm::cross(tri.a - tri.b, tri.a - tri.c)));
}

typedef struct
{
	bool in;
	float f_a;
	float f_b;
	float f_c;

} in_tri_result_t;

static inline void point_in_tri(in_tri_result_t& result,
	tri_t& tri,
	float x,
	float y,
	float z)
{
	tri_t a, b, c;

	a.a.x = x;
	a.a.y = y;
	a.a.z = z;
	a.b.x = tri.b.x;
	a.b.y = tri.b.y;
	a.b.z = tri.b.z;
	a.c.x = tri.c.x;
	a.c.y = tri.c.y;
	a.c.z = tri.c.z;

	b.a.x = tri.a.x;
	b.a.y = tri.a.y;
	b.a.z = tri.a.z;
	b.b.x = x;
	b.b.y = y;
	b.b.z = z;
	b.c.x = tri.c.x;
	b.c.y = tri.c.y;
	b.c.z = tri.c.z;

	c.a.x = tri.a.x;
	c.a.y = tri.a.y;
	c.a.z = tri.a.z;
	c.b.x = tri.b.x;
	c.b.y = tri.b.y;
	c.b.z = tri.b.z;
	c.c.x = x;
	c.c.y = y;
	c.c.z = z;

	float total = tri_area(tri);

	result.in = (result.f_a + result.f_b + result.f_c - total < 0.1);
}

static inline uint64_t i2hash(uint64_t a, uint64_t b)
{
	return 5 * (a*31 + b) * (a + b + 1) + b *31;
}

static inline uint64_t i3hash(uint64_t a, uint64_t b, uint64_t c)
{
	return 5 * (i2hash(a, b) * 31 + c) * (i2hash(a, b) + c + 1) + c;
}

static inline uint64_t v3hash(glm::vec3 v)
{
	uint64_t x = (v.x ) * 10000;
	uint64_t y = (v.y ) * 10000;
	uint64_t z = (v.z ) * 10000;

	return i3hash(x, y, z);
}

#define SZ 46181
bool table[SZ];

//void set_cubeposes_in_tri(std::vector<glm::vec3> &outposes, float step, tri_t tri, bool* table)
//{
//	float xmin, xmax, x;
//	float ymin, ymax, y;
//	float zmin, zmax, z;
//	in_tri_result_t result;
//	
//
//	xmin = bound_box_x_min(tri);
//	xmax = bound_box_x_max(tri);
//	ymin = bound_box_y_min(tri);
//	ymax = bound_box_y_max(tri);
//	zmin = bound_box_z_min(tri);
//	zmax = bound_box_z_max(tri);
//
//	xmin = round(xmin / step) * step;
//	xmax = round(xmin / step) * step;
//	ymin = round(ymin / step) * step;
//	ymax = round(ymax / step) * step;
//	zmin = round(zmin / step) * step;
//	zmax = round(zmax / step) * step;
//
//	for (z = zmin-step/2; z < zmax + step *2; z+=step)
//	{
//		for (y = ymin - step/2; y < ymax + step*2; y+=step)
//		{
//			for (x = xmin - step/2; x < xmax + step*2; x+=step)
//			{
//				if (table[v3hash(glm::vec3(x, y, z)) % SZ])
//					continue;
//
//				point_in_tri(result, tri, x, y, z);
//				if (result.in)
//				{
//					outposes.push_back(glm::vec3(x, y, z));
//					table[v3hash(glm::vec3(x, y, z)) % SZ] = 1;
//				}
//			}
//		}
//	}
//}

void set_cubeposes_in_tri(std::vector<glm::vec3>& outposes, float step, tri_t &tri, bool* table)
{
	float x, y, z;
	
	x = (tri.a.x + tri.b.x + tri.c.x) / 3;
	y = (tri.a.y + tri.b.y + tri.c.y) / 3;
	z = (tri.a.z + tri.b.z + tri.c.z) / 3;

	x = round(x / step) * step;
	y = round(y / step) * step;
	z = round(z/ step) * step;

				if (table[v3hash(glm::vec3(x, y, z)) % SZ])
					return;

					outposes.push_back(glm::vec3(x, y, z));
					table[v3hash(glm::vec3(x, y, z)) % SZ] = 1;
				
			
}

void Shape::cubify(std::vector<glm::vec3> &outposes, float step)
{
	

	for (int i = 0; i < SZ; i++)
	{
		table[i] = 0;
	}

	tri_t tri;
	for (unsigned int i = 0; i < eleBuf->size(); i+=3)
	{
		tri.a.x = posBuf->data()[eleBuf->data()[i] + 0];
		tri.a.y = posBuf->data()[eleBuf->data()[i] + 1];
		tri.a.z = posBuf->data()[eleBuf->data()[i] + 2];

		tri.b.x = posBuf->data()[eleBuf->data()[i+1] + 0];
		tri.b.y = posBuf->data()[eleBuf->data()[i+1] + 1];
		tri.b.z = posBuf->data()[eleBuf->data()[i+1] + 2];

		tri.c.x = posBuf->data()[eleBuf->data()[i+2] + 0];
		tri.c.y = posBuf->data()[eleBuf->data()[i+2] + 1];
		tri.c.z = posBuf->data()[eleBuf->data()[i+2] + 2];

		set_cubeposes_in_tri(outposes, step, tri, table);
	}
}


