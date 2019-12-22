#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>

#include <shader.hpp>

// Reference: https://github.com/nothings/stb/blob/master/stb_image.h#L4
// To use stb_image, add this in *one* C++ source file.
#include <stb_image.h>

struct Vertex {
	// position
	glm::vec3 Position;
	// texCoords
	glm::vec2 TexCoords;
};

// An abstract camera class that processes input and calculates the corresponding Eular Angles, Vectors and Matrices for use in OpenGL
class Heightmap
{
public:
	//heightmap attributes
	int width, height;

	// VAO for heightmap
	unsigned int VAO;

	// pointer to data - data is an array so can be accessed by data[x]. 
	//       - this is an uint8 array (so values range from 0-255)
	unsigned char* data;

	// heightmap vertices
	std::vector<Vertex> vertices;
	// indices for EBO
	std::vector<unsigned int> indices;


	// constructor
	Heightmap(const char* heightmapPath)
	{
		// load heightmap data
		load_heightmap(heightmapPath);

		// create heightmap verts from the image data - (you have to write this)
		create_heightmap();

		// free image data
		stbi_image_free(data);

		// create_indices - create the indices array (you have to write this)
		//  This is an optional step so and you can ignore this if you want to just create all the triangles rather than
		//     using this to index it.
		create_indices();

		// setup the VBO, VAO, and EBO and send the information to OpenGL (you need to write this)
		setup_heightmap();
	}



	// render the heightmap mesh (you need to write this)
	void Draw(Shader shader, unsigned int textureID)
	{
		// You must:
		// -  active proper texture unit before binding
		// -  bind the texture
		// -  draw mesh (using GL_TRIANGLES is the most reliable way)


		glActiveTexture(GL_TEXTURE0);//activate proper texture & bind the texture
		glBindTexture(GL_TEXTURE_2D, 6);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);


		shader.use();

		// crate heightmap model 

		glm::mat4 heightmap_model;

		heightmap_model = glm::translate(heightmap_model, glm::vec3(-150.0f, -10.0f, 100.0f));//translate the heightmap to the correct position
		heightmap_model = glm::rotate(heightmap_model, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));//rotate the height map for 180 degrees to make it appears on the screen
		heightmap_model = glm::scale(heightmap_model, glm::vec3(100.0f, 20.0f, 100.0f));// scale the height map larger
		shader.setMat4("model", heightmap_model);


		// draw mesh
		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(GL_TRIANGLES, 0, 6 * (height - 1) * (width - 1)); //Only need height*width verts and then 6*(height-1)*(width-1) indices




		// always good practice to set everything back to defaults once configured.
		glActiveTexture(GL_TEXTURE0);
	}

private:

	unsigned int VBO, EBO;

	// Load the image data
	void load_heightmap(const char* heightmapPath)
	{
		int nrChannels;
		data = stbi_load(heightmapPath, &width, &height, &nrChannels, 0);
		if (!data)
		{
			std::cout << "Failed to load heightmap" << std::endl;
		}

	}


	// Make Vertex:  take x and y position return a new vertex for that position which includes 
	//  the position and the texture coordinates
	//     The data is in a char c-array and can be access via  
	//           float(data[x*height + y]) / 255.0f 
	//      where x and y are varables between 0 and width or height  (just use a black and white image for simplicity)


	Vertex make_vertex(int x, int y)

	{
		Vertex v;
		//XYZ coords
		v.Position.x = 2.0f * (float(x) / width); // set the x coords
		v.Position.y = float(data[x * height + y]) / 255.0f;// set the x coords
		v.Position.z = 2.0f * (float(y) / height);// set the x coords

		//Texture Coords
		v.TexCoords.x = float(x) / width;
		v.TexCoords.y = float(y) / height;

		return v;
	}


	// convert heightmap to floats, set position and texture vertices using the subfunction make_vertex
	void create_heightmap()
	{
		for (int x = 0; x < width; x++) {
			for (int y = 0; y < height; y++) {
				vertices.push_back(make_vertex(x, y));// set two triangles to render the height map
				vertices.push_back(make_vertex(x + 1, y));
				vertices.push_back(make_vertex(x, y + 1));
				vertices.push_back(make_vertex(x + 1, y));
				vertices.push_back(make_vertex(x, y + 1));
				vertices.push_back(make_vertex(x + 1, y + 1));
			}
		}

	}


	// create the indicies array for the EBO (so what indicies correspond with triangles to for rendering)
	//  This is an optional step so and you can ignore this if you want to just create all the triangles rather than
	//     using this to index it.
	void create_indices()
	{



	}


	// create buffers/arrays for the VAO, VBO, and EBO 
	// Notes
	//  -  sizeof(Vertex) returns the size of the vertex
	//  -  to get the pointer the underlying array, you can use "&vertices[0]"
	void setup_heightmap()
	{
		//unsigned int VBO, VAO, EBO;
		//This block of code is coming from the tutorial 1


		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
		glBindVertexArray(VAO);

		// Copy our vertices array in a vertex buffer for OpenGL to use
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		// Position attribute for the 3D Position Coordinates and link to position 0
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		glEnableVertexAttribArray(0);
		// TexCoord attribute for the 2d Texture Coordinates and link to position 2
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);



	}

};
#endif