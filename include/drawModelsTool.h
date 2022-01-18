#pragma once

#include"usingOpengl.h"
#include "model.h"
#include "camera.h"
#include <vector>
#include <stack>
#include <chrono>

// ����ģ������
#define MODEL_NUMS 5

// ����ģ��index
#define MODEL_BOMB 0
#define MODEL_BOX 1
#define MODEL_PLAYER 2
#define MODEL_GHOST 3
#define MODEL_WALL 4

// ����ᶯ��ģ�������ͱ��
#define MOVING_MODELS 4
#define MOVING_PLAYER 0
#define MOVING_ENEMY1 1
#define MOVING_ENEMY2 2
#define MOVING_ENEMY3 3


// ����ģ�ͼ��
const float step = 2.0f;
const float speed = 2.0f;

// ���屬ըЧ������
const int bomb_ms = 3000;

//��Ӱ��С
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
//ʱ��������
const float delta = 0.003;

// ����˵����Ϊÿһ���ᶯ��ģ�Ͷ���һ����ǰ���ڵ�λ��(curX, curY)
// ���������Ķ�ģ�Ͳ���(x, y)λ�����¼��curX, curY��ͬʱ��ʹ��(��ǰʱ��-startMs)*�ٶ� + pos(�������)
// ���������Ķ�ģ�Ͳ���(x, y)λ���롭����������������ͬʱ������������������ģ�ͻ����µ�x, y��

struct DrawMessage;

struct modelState {
	int curX, curY;
	std::chrono::milliseconds	startMs;

	int direction, lastdirection;
	bool isMove;
};

struct BombState {
	int x, y;		// ��¼ը����λ��
	int restTime;	// ��ը����ʧ�󣬼�¼�����ʣ��ʱ�䣬���﹫ʽ�Ժ�����ʽ����
	int lightNo;	// ը����ţ�Ĭ��һ��10��������Shader���޸�
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
	void Render(Camera* camera,DrawMessage* message);//��SceneGame���б����ã�ȡ��ԭ�ȵ�drawAllModels
	
protected:
	// ���ⲿ�����ģ�����࣬ת��Ϊģ��index
	int idxTranslator(char type);
	void drawOneStaticModel(int x, int y, char type, Shader& shader);//*����shader����
	void drawOneMoveModel(int x, int y, char type, Shader& shader);//**����shader����
	void drawAllModels(DrawMessage* message, Shader& shader);//����shader��������renderScene����
	void renderScene(Shader& shader, DrawMessage* message);//������������Render����
	
	void bombUpdateShader();
	float bombLightParam(int restTime);

	bool JudgeStatic(char type);
	bool updateMStates(DrawMessage* message);
	bool updateBStates(DrawMessage* message);
	bool UpdateLightPos();//*�������ı�lightPos��������ʱ��仯��Դλ��

	// ���ߺ���������ȡ������ʹ��
	void addPointLight(glm::vec3 position, Shader* sd, int i, float factor = 1, float attenu = 1);
	unsigned int loadTexture(char const* path);
};

//ɾ����������ֱ࣬����drawModelTool�л�����