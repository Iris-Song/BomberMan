#pragma once

#include"usingOpengl.h"
#include "model.h"
#include "camera.h"
#include <vector>
#include <stack>
#include <chrono>

// 定义模型数量
#define MODEL_NUMS 5

// 定义模型index
#define MODEL_BOMB 0
#define MODEL_BOX 1
#define MODEL_PLAYER 2
#define MODEL_GHOST 3
#define MODEL_WALL 4

// 定义会动的模型数量和标号
#define MOVING_MODELS 4
#define MOVING_PLAYER 0
#define MOVING_ENEMY1 1
#define MOVING_ENEMY2 2
#define MOVING_ENEMY3 3


// 定义模型间隔
const float step = 2.0f;
const float speed = 2.0f;

// 定义爆炸效果参数
const int bomb_ms = 3000;

//阴影大小
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//时间递增间隔
const float delta = 0.003;

// 功能说明：为每一个会动的模型定义一个当前所在的位置(curX, curY)
// 当传过来的动模型参数(x, y)位置与记录的curX, curY相同时，使用(当前时间-startMs)*速度 + pos(按方向加)
// 当传过来的东模型参数(x, y)位置与……………………不同时，重置三个参数，将模型画在新的x, y上

struct DrawMessage;

struct modelState {
	int curX, curY;
	std::chrono::milliseconds	startMs;

	int direction, lastdirection;
	bool isMove;
};

struct BombState {
	int x, y;		// 记录炸弹的位置
	int restTime;	// 当炸弹消失后，记录演绎的剩余时间，演绎公式以函数形式给出
	int lightNo;	// 炸弹标号，默认一共10个，可在Shader中修改
	std::chrono::milliseconds	startMs;
};


class DrawModelsTool {
protected:
	std::vector<Model> models;
	std::vector<glm::vec3> scaleParam;

	std::vector<BombState> bstates;

	std::stack<int> bombFreeStack;

	modelState mstates[MOVING_MODELS];

	bool PlayerIsMoveInOneSecond;
	std::chrono::milliseconds	startMs;

	Shader _mapShader;
	Shader _simpleDepthShader;
	Shader _debugDepthQuad;

	unsigned int _planeVAO;
	unsigned int _depthMapFBO;
	unsigned int _depthMap;

	unsigned int groundTexture;

	glm::vec3 lightPos;
	float now_degree;

public:
	bool init(Camera* camera);
	DrawModelsTool();
	void Render(Camera* camera,DrawMessage* message);//在SceneGame类中被调用，取代原先的drawAllModels
	
protected:
	// 将外部定义的模型种类，转化为模型index
	int idxTranslator(char type);
	void drawOneStaticModel(int x, int y, char type, Shader& shader);//*增加shader参数
	void drawOneMoveModel(int x, int y, char type, Shader& shader);//**增加shader参数
	void drawAllModels(DrawMessage* message, Shader& shader);//增加shader参数，由renderScene调用
	void renderScene(Shader& shader, DrawMessage* message);//新增函数，由Render调用
	
	void bombUpdateShader();
	float bombLightParam(int restTime);

	bool JudgeStatic(char type);
	bool updateMStates(DrawMessage* message);
	bool updateBStates(DrawMessage* message);
	bool UpdateLightPos();//*新增，改变lightPos，可以随时间变化光源位置

	// 工具函数，可提取到类外使用
	void addPointLight(glm::vec3 position, Shader* sd, int i, float factor = 1, float attenu = 1);
	unsigned int loadTexture(char const* path);
};

//删除画地面的类，直接在drawModelTool中画地面