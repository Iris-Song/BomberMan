#include "../../../include/utils/opengl/UIElements.h"

Shader * RectUI::_rectShader = nullptr;
GLuint RectUI::_rectVAO = 0;
GLuint RectUI::_rectVBO = 0;

const float RectUI::_rectVertices[] = {
    0.0, 0.0,
	1.0, 1.0,
	0.0, 1.0,

	0.0, 0.0,
	1.0, 0.0,
	1.0, 1.0,
};

void RectUI::init() {
    // create shader
	_rectShader = new Shader("../../resource/rect_vs.glsl", "../../resource/rect_fs.glsl");
	// enable shader
	_rectShader->use();

	// create VAO & VBO
	glGenVertexArrays(1, &_rectVAO);
	glGenBuffers(1, &_rectVBO);

	// enable vao & vbo
	glBindVertexArray(_rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(_rectVertices), _rectVertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), reinterpret_cast<void*>(0));
	glEnableVertexAttribArray(0);

	// disable vao & vbo
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// send default values to uniforms
	_rectShader->setMat4("model", glm::mat4(1.0));
    _rectShader->setVec4("masterColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    _rectShader->setVec4("secondColor", glm::vec4(1.0, 1.0, 1.0, 1.0));
    _rectShader->setFloat("colorFactor", 1);

	// disable shader
	_rectShader->unuse();
}

/**
 * @brief Construct a new Rect U I:: Rect U I object
 *
 * @param pos The position of the UI element
 * @param size The size of the UI element
 */
RectUI::RectUI(glm::vec2 pos, glm::vec2 size) {
	// set the UI to non clickable
	//_isClickableUI = false;


    
}

/**
 * @brief Destroy the Rect U I:: Rect U I object
 */
RectUI::~RectUI() {}

/**
 * @brief this is the base update function of UI objects
 */
void RectUI::_update() {
}

/**
 * @brief this is the draw function for UI
 * /!\ -> you need to draw in the reverse order (draw at first the element on the top)
 */
void RectUI::_draw() {
	glm::vec2 tmpPos;
	glm::vec2 tmpSize;

	// // get center size and position
	// tmpPos = getRealPos();
	// tmpPos.x += _borderSize;
	// tmpPos.y += _borderSize;
	// tmpSize = _size;
	// tmpSize.x -= _borderSize * 2;
	// tmpSize.y -= _borderSize * 2;
	// _drawRect(tmpPos, tmpSize, _z, _color);

	// // draw border
	// _drawBorderRect(getRealPos(), _size, _z, _borderSize, _borderColor);
}
