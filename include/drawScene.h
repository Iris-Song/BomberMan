#pragma once
#include "usingOpengl.h"
#include <string>

class DrawSceneTool 
{
public:
	DrawSceneTool();
	bool init(const std::string &filepath);
	void draw();

protected:
	Shader _shader;
	unsigned int _VAO;
	unsigned int _texture;
};