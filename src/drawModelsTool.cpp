#include <chrono>

#include "bomberman.h"
#include "Player.h"
#include "..\include\drawModelsTool.h"


bool DrawModelsTool::init(Camera* camera)
{
	// 读入模型
	models.resize(MODEL_NUMS, Model("resource/model/box/box.obj"));
	scaleParam.resize(MODEL_NUMS, glm::vec3(1.5f, 1.5f, 1.5f));

	models[MODEL_BOMB] = Model("resource/model/bomb/bomb.fbx");
	scaleParam[MODEL_BOMB] = glm::vec3(0.01f, 0.01f, 0.01f);

	models[MODEL_BOX] = Model("resource/model/box/box.obj");
	scaleParam[MODEL_BOX] = glm::vec3(2.0f, 2.0f, 2.0f);
	models[MODEL_PLAYER] = Model("resource/model/player/paladin.fbx");
	scaleParam[MODEL_PLAYER] = glm::vec3(0.01f, 0.01f, 0.01f);
	models[MODEL_GHOST] = Model("resource/model/redGhost/redGhost.obj");
	models[MODEL_WALL] = Model("resource/model/wall/wall.obj");

	//设置地平面
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 35.0f, -0.5f,  35.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		-35.0f, -0.5f,  35.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-35.0f, -0.5f, -35.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,

		 35.0f, -0.5f,  35.0f,  0.0f, 1.0f, 0.0f,  1.0f,  0.0f,
		-35.0f, -0.5f, -35.0f,  0.0f, 1.0f, 0.0f,   0.0f, 1.0f,
		 35.0f, -0.5f, -35.0f,  0.0f, 1.0f, 0.0f,  1.0f, 1.0f
	};
	// plane VAO
	unsigned int planeVBO;
	glGenVertexArrays(1, &_planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(_planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);

	// configure depth map FBO
	// -----------------------
	glGenFramebuffers(1, &_depthMapFBO);

	// create depth texture
	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	// attach depth texture as FBO's depth buffer
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_mapShader.use();
	_mapShader.setInt("diffuseTexture", 0);
	_mapShader.setInt("shadowMap", 1);
	_debugDepthQuad.use();
	_debugDepthQuad.setInt("depthMap", 0);

	// 设定Shader
	this->_mapShader.use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	this->_mapShader.setMat4("projection", projection);
	this->_mapShader.setMat4("view", view);


	// 设定光照
	// configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
	unsigned int VBO;
	unsigned int lightCubeVAO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);

	// note that we update the lamp's position attribute's stride to reflect the updated buffer data
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	this->_mapShader.setVec3("viewPos", camera->Position);
	this->_mapShader.setFloat("material.shininess", 32.0f);
	// directional light
	this->_mapShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	this->_mapShader.setVec3("dirLight.ambient", 0.5f, 0.5f, 0.5f);
	this->_mapShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	this->_mapShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	//pointLight
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f),
		glm::vec3(0.0f,  1.0f, -1.0f),
		glm::vec3(2.0f,  3.0f, -3.0f),
		glm::vec3(6.0f,  0.0f, 3.0f),
		glm::vec3(2.0f,  6.0f, 3.0f),
		glm::vec3(7.0f,  0.0f, 0.0f),
		glm::vec3(0.5f,  0.2f, 0.3f)
	};
	for (int i = 0; i < 10; i++) {
		addPointLight(pointLightPositions[i], &this->_mapShader, i, 0, 0);
	}
	// spotLight
	this->_mapShader.setVec3("spotLight.position", camera->Position);
	this->_mapShader.setVec3("spotLight.direction", camera->Front);
	this->_mapShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
	this->_mapShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	this->_mapShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	this->_mapShader.setFloat("spotLight.constant", 1.0f);
	this->_mapShader.setFloat("spotLight.linear", 0.09);
	this->_mapShader.setFloat("spotLight.quadratic", 0.032);
	this->_mapShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	this->_mapShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	this->_mapShader.unuse();

	// 读入地面贴图
	this->groundTexture = loadTexture("resource/skybox/ground.jpg");

	return true;

}

void DrawModelsTool::Render(Camera* camera, DrawMessage* message) {
	
	 //UpdateLightPos();

	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	float near_plane = 1.0f, far_plane = 7.5f;
	lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
	lightView = glm::lookAt(this->lightPos, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	_simpleDepthShader.use();
	_simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture);

	renderScene(_simpleDepthShader, message);
	

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// reset viewport
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 2. render scene as normal using the generated depth/shadow map  
	// --------------------------------------------------------------
	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_mapShader.use();
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	_mapShader.setMat4("projection", projection);
	_mapShader.setMat4("view", view);
	// set light uniforms
	_mapShader.setVec3("viewPos", camera->Position);
	_mapShader.setVec3("lightPos", lightPos);
	_mapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, groundTexture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _depthMap);

	renderScene(_mapShader, message);
	
}

void DrawModelsTool::renderScene(Shader& shader, DrawMessage* message) {
	// floor
	glm::mat4 model = glm::mat4(1.0f);
	shader.setMat4("model", model);
	glBindVertexArray(_planeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	// cubes
	drawAllModels(message, shader);
}

void DrawModelsTool::drawOneStaticModel(int x, int y, char type, Shader& shader)
{
	shader.use();
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f + (y - MAPSIZE / 2) * step, 0.0f, 0.0f + (x - MAPSIZE / 2) * step)); // translate it down so it's at the center of the scene
	model = glm::scale(model, this->scaleParam[idxTranslator(type)]);
	shader.setMat4("model", model);
	models[idxTranslator(type)].Draw(shader);
	shader.unuse();
	return;
}

void DrawModelsTool::drawOneMoveModel(int x, int y, char type, Shader& shader)
{
	/* 获取当前要画的元素 */
	int moving_element;
	for (int i = 0; i < MOVING_MODELS; i++) {
		if (x == mstates[i].curX && y == mstates[i].curY)
			moving_element = i;
	}

	shader.use();
	glm::mat4 model = glm::mat4(1.0f);

	// 区分角色和怪物的移动逻辑
	bool isMove;
	if (moving_element == MOVING_PLAYER)
		isMove = this->PlayerIsMoveInOneSecond;
	else
		isMove = mstates[moving_element].isMove;

	int isUpDown = isMove * (mstates[moving_element].direction == UP_PMOVE || mstates[moving_element].direction == DOWN_PMOVE);
	int isLeftRight = isMove * (mstates[moving_element].direction == LEFT_PMOVE || mstates[moving_element].direction == RIGHT_PMOVE);

	float xPos = 0.0f + (y - MAPSIZE / 2) * step + isLeftRight * (getMs().count() - mstates[moving_element].startMs.count()) / 1000.0f * speed * (mstates[moving_element].direction - DOWN_PMOVE);
	float yPos = 0.0f + glm::sin(getMs().count() * 0.005) * 0.16;
	float zPos = 0.0f + (x - MAPSIZE / 2) * step + isUpDown * (getMs().count() - mstates[moving_element].startMs.count()) / 1000.0f * speed * (mstates[moving_element].direction - LEFT_PMOVE);

	model = glm::translate(model, glm::vec3(xPos, yPos, zPos)); // translate it down so it's at the center of the scene
	float angle = glm::radians(90.0f * (mstates[moving_element].direction - DOWN_PMOVE));
	model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::scale(model, this->scaleParam[idxTranslator(type)]);
	shader.setMat4("model", model);
	models[idxTranslator(type)].Draw(shader);
	shader.unuse();
	return;
}

void DrawModelsTool::drawAllModels(DrawMessage* message, Shader& shader)
{
	updateMStates(message);
	updateBStates(message);

	bombUpdateShader();

	for (int i = 0; i < MAPSIZE; i++)
		for (int j = 0; j < MAPSIZE; j++)
			if (message->drawMap[i][j] != ' ') {
				if (JudgeStatic(message->drawMap[i][j]))
					drawOneStaticModel(i, j, message->drawMap[i][j],shader);
				else
					drawOneMoveModel(i, j, message->drawMap[i][j],shader);
			}

}

void DrawModelsTool::bombUpdateShader()
{
	for (auto it = bstates.begin(); it != bstates.end(); ++it) {
		glm::vec3 lightPos = glm::vec3(0.0f + (it->y - MAPSIZE / 2) * step, 0.0f, 0.0f + (it->x - MAPSIZE / 2) * step);
		this->_mapShader.use();
		addPointLight(lightPos, &this->_mapShader, it->lightNo, bombLightParam(it->restTime));
	}
}

float DrawModelsTool::bombLightParam(int restTime)
{
	if (restTime > 1000)
		return 0.5f;
	else if (restTime > 900)
		return 2.5 / 100 * (1020 - restTime);
	else {
		double res = 1.0;
		for (int i = 0; i < 15; i++)
			res = res * restTime / 800.0;
		return 3.0f * res;
	}
}

bool DrawModelsTool::JudgeStatic(char type)
{
	if (type == '/' || type == '*')
		return false;
	else
		return true;
}



bool DrawModelsTool::updateMStates(DrawMessage * message)
{
	// 维护Player的状态
	// 当坐标出现了记录变化时，更新起始时间
	for (int i = 0; i < MAPSIZE; i++)
		for (int j = 0; j < MAPSIZE; j++)
			if (idxTranslator(message->drawMap[i][j]) == MODEL_PLAYER) {
				if (mstates[MOVING_PLAYER].curX != i || mstates[MOVING_PLAYER].curY != j)
					mstates[MOVING_PLAYER].startMs = getMs();
				mstates[MOVING_PLAYER].curX = i;
				mstates[MOVING_PLAYER].curY = j;
			}
	mstates[MOVING_PLAYER].lastdirection = mstates[MOVING_PLAYER].direction;
	mstates[MOVING_PLAYER].direction = message->playerDir;
	mstates[MOVING_PLAYER].isMove = message->isMove;

	// 维护ENEMY的状态，按三个enemy的位置分别来
	for (int k = 1; k < MOVING_MODELS; k++) {
		if (mstates[k].curX != message->enemyPos[k - 1].x || mstates[k].curY != message->enemyPos[k - 1].y || mstates[k].isMove == false)
			mstates[k].startMs = getMs();
		mstates[k].curX = message->enemyPos[k - 1].x;
		mstates[k].curY = message->enemyPos[k - 1].y;
		mstates[k].lastdirection = mstates[k].direction;
		mstates[k].direction = message->enemyDir[k - 1];
		mstates[k].isMove = message->enemyMove[k - 1];
	}

	return true;
}


bool DrawModelsTool::updateBStates(DrawMessage* message)
{
	// 寻找所有炸弹的位置
	for (int i = 0; i < MAPSIZE; i++)
		for (int j = 0; j < MAPSIZE; j++) {
			if (message->drawMap[i][j] == '@') {
				if (this->bstates.size() == 0) {
					BombState bs;
					bs.x = i;
					bs.y = j;
					bs.lightNo = this->bombFreeStack.top();
					this->bombFreeStack.pop();
					bs.restTime = bomb_ms;	//初始化时间值
					bs.startMs = getMs();
					bstates.push_back(bs);
				}
				else {
					// 首先在States中查找有没有该炸弹
					auto it = this->bstates.begin();
					for (; it != this->bstates.end(); ++it) {
						if (it->x == i && it->y == j)
							break;	//找到了
					}

					if (it == this->bstates.end()) {// 如果没找到，则这个是新生炸弹，记录它
						BombState bs;
						bs.x = i;
						bs.y = j;
						bs.lightNo = this->bombFreeStack.top();
						this->bombFreeStack.pop();
						bs.restTime = bomb_ms;	//初始化时间值
						bs.startMs = getMs();
						bstates.push_back(bs);
					}
					else {
						it->restTime = bomb_ms - (getMs().count() - it->startMs.count());
						if (it->restTime <= 0) {
							this->bombFreeStack.push(it->lightNo);
							bstates.erase(it);
						}
					}
				}
			}
		}

	// 对所有存在的炸弹效果更新时间
	if (bstates.size() > 0) {
		auto it = bstates.begin();
		for (; it != bstates.end(); ++it) {
			it->restTime = bomb_ms - (getMs().count() - it->startMs.count());
			if (it->restTime <= 0)
				break;
		}
		if (it != bstates.end()) {
			this->bombFreeStack.push(it->lightNo);
			bstates.erase(it);
		}
	}

	return true;
}

bool DrawModelsTool::UpdateLightPos() {

	this->now_degree += delta;
	this->lightPos.y = 0.0 + cos(now_degree) * 10.0f;
	return true;
}

DrawModelsTool::DrawModelsTool() :
	_mapShader(Shader("resource/shadow_mapping.vs", "resource/shadow_mapping.fs")),
	_simpleDepthShader(Shader("resource/shadow_mapping_depth.vs", "resource/shadow_mapping_depth.fs")),
	_debugDepthQuad(Shader("resource/debug_quad.vs", "resource/debug_quad.fs"))
{
	for (int i = 0; i < MOVING_MODELS; i++) {
		mstates[i].curX = -1;
		mstates[i].curY = -1;
		mstates[i].direction = DOWN_PMOVE;
		mstates[i].lastdirection = DOWN_PMOVE;
	}

	for (int i = 0; i < 10; i++)
		this->bombFreeStack.push(i);

	this->lightPos = glm::vec3(-2.0f, 4.0f, -1.0f);//*
	this->now_degree = 0;//*
}

int DrawModelsTool::idxTranslator(char type)
{
	if (type == '$')
		return MODEL_WALL;
	else if (type == '#')
		return MODEL_BOX;
	else if (type == '/')
		return MODEL_GHOST;
	else if (type == '*')
		return MODEL_PLAYER;
	else if (type == '@')
		return MODEL_BOMB;

	return MODEL_WALL;
}

void DrawModelsTool::addPointLight(glm::vec3 position, Shader* sd, int i, float factor, float attenu) {
	/*默认factor为1,调整光强，atten调整衰减系数*/

	string str = "pointLights[" + to_string(i);

	sd->setVec3(str + "].position", position);
	sd->setVec3(str + "].ambient", factor * 0.05f, factor * 0.05f, factor * 0.05f);
	sd->setVec3(str + "].diffuse", factor * 0.8f, factor * 0.8f, factor * 0.8f);
	sd->setVec3(str + "].specular", factor * 1.0f, factor * 1.0f, factor * 1.0f);
	sd->setFloat(str + "].constant", 1.0f * attenu);
	sd->setFloat(str + "].linear", 0.09 * attenu);
	sd->setFloat(str + "].quadratic", 0.032 * attenu);
}

unsigned int DrawModelsTool::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

