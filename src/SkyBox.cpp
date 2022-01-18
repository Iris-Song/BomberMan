#include "SkyBox.h"


const float Skybox::_vertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f

};

Skybox::Skybox() :
	_shader(SHADER_SKYBOX_VS, SHADER_SKYBOX_FS) {
	std::vector<std::string> skyboxFaces = {
		std::string(SKYBOX_PATH) + SKYBOX_NAME_RIGHT,  // right
		std::string(SKYBOX_PATH) + SKYBOX_NAME_LEFT,  // left
		std::string(SKYBOX_PATH) + SKYBOX_NAME_TOP,  // up
		std::string(SKYBOX_PATH) + SKYBOX_NAME_BOTTOM,  // down
		std::string(SKYBOX_PATH) + SKYBOX_NAME_FRONT,  // front
		std::string(SKYBOX_PATH) + SKYBOX_NAME_BACK,  // back
	};
	_shader.use();
	load(skyboxFaces);
	_shader.setInt("skybox", 0);
	_vao = 0;
	_vbo = 0;
	glGenVertexArrays(1, &_vao);
	glGenBuffers(1, &_vbo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_vertices), &_vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
	
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	_shader.unuse();
}


Skybox::Skybox(Skybox const& src) :
	_shader(src.getShader()) {
	_textureID = getTextureID();
	_vao = src._vao;
	_vbo = src._vbo;
}


Skybox::~Skybox() {
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteVertexArrays(1, &_vao);
	glDeleteBuffers(1, &_vbo);
	if (_textureID != 0) {
		glDeleteTextures(1, &_textureID);
	}
	_shader.unuse();
}


void Skybox::setProjection(glm::mat4& projection)
{
	_shader.use();
	_shader.setMat4("projection",projection);
	_shader.unuse();
}

void Skybox::load(std::vector<std::string>& faces) {
	glGenTextures(1, &_textureID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);

	int width;
	int height;
	int nrChannels;
	for (unsigned int i = 0; i < faces.size(); ++i) {
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		else {
			std::cout << "Failed to load texture at path: " << faces[i] << std::endl;
		}
		stbi_image_free(data);
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}


void Skybox::draw(glm::mat4 view, float nightProgress) {
	glm::mat4 skyView = view;
	skyView[3][0] = 0;  // remove translation for the skybox
	skyView[3][1] = 0;
	skyView[3][2] = 0;
	//skyView = glm::mat4(glm::mat3(view));
	_shader.use();
	_shader.setMat4("view", skyView);
	glDepthFunc(GL_LEQUAL);
	glBindVertexArray(_vao);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _textureID);
	_shader.setFloat("nightProgress", nightProgress);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(_vertices) / sizeof(_vertices[0]));
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDepthFunc(GL_LESS);
	glBindVertexArray(0);
	_shader.unuse();
}

Shader& Skybox::getShader() { return _shader; }

Shader const& Skybox::getShader() const { return _shader; }

uint32_t Skybox::getTextureID() const { return _textureID; }
