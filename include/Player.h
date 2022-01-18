
#ifdef OLD
#pragma once
#include "Wall.h"
#include "Enemy.h"
class Player{
    POS pos;//玩家位置
    vector<BOMB> bPos;//玩家放置的炸弹位置
    Wall stoneMap;//地图对象
    vector<Enemy> enemy;//敌人对象
    clock_t bomStartTime;//玩家放置炸弹的起始时间
    bool win;//玩家胜利还是失败
public:
    Player(string filepath);//初始化玩家位置
    void Operate();//更新玩家位置
    void PutBomb();//放置炸弹
    void GetMap(int tempMap[MAPSIZE][MAPSIZE]);//获取当前地图上所有被占用的格子
    void Walk();//敌人走动
    bool JudgeDeath(POS bomPos);//判断游戏是否结束/玩家是否被炸死
    bool Update();//对游戏状态等进行更新、地图更新、返回游戏是否结束
    void Display();//显示函数
    void Begin();
	bool IsWin();
	void OperateBySDL();
};
#else
#pragma once
#include "Wall.h"
#include "Enemy.h"
#define ENEMT_NUM 3



struct DrawMessage {
	char drawMap[MAPSIZE][MAPSIZE];
	int playerDir;
	int enemyDir[ENEMT_NUM];
	bool isMove;
	bool enemyMove[ENEMT_NUM];
	POS enemyPos[ENEMT_NUM];
	DrawMessage() {
		for (int i = 0; i < ENEMT_NUM; i++) {
			enemyDir[i] = UP_PMOVE;
		}
	}
	void display() {
		for (int i = 0; i < MAPSIZE; i++) {
			for (int j = 0; j < MAPSIZE; j++) {
				cout << drawMap[i][j];
			}
			cout << endl;
		}
		cout << "playerdir:" << playerDir << endl;
		cout << "enemydir:";
		for (int i = 0; i < ENEMT_NUM; i++) {
			cout << enemyDir[i] << " ";
		}
		cout << endl;
		cout << "isMove:" << this->isMove << endl;
		cout << "enemyMove:";
		for (int i = 0; i < ENEMT_NUM; i++) {
			cout << enemyMove[i] << " ";
		}
		cout << endl;
		for (int i = 0; i < ENEMT_NUM; i++) {
			cout << "enemy" << i + 1 << ":" << enemyPos[i].x << "," << enemyPos[i].y << "  " << endl;
		}
	}

};

class Player {
	POS pos;//玩家位置
	vector<BOMB> bPos;//玩家和敌人放置的炸弹位置
	Wall stoneMap;//地图对象
	vector<Enemy> enemy;//敌人对象
	clock_t bomStartTime;//玩家放置炸弹的起始时间
	bool win;//玩家胜利还是失败
	int dir;
	bool isMove;
public:

	Player(string filepath);//初始化玩家位置
	void Operate();//更新玩家位置
	void PutBomb();//放置炸弹
	void GetMap(int tempMap[MAPSIZE][MAPSIZE]);//获取当前地图上所有被占用的格子
	void Walk();//敌人走动
	bool JudgeDeath(POS bomPos);//判断游戏是否结束/玩家是否被炸死
	bool Update();//对游戏状态等进行更新、地图更新、返回游戏是否结束
	void Display();//显示函数
	void Begin();
	bool IsWin();
	void OperateBySDL();
	void GetDrawMessage(DrawMessage *drawMessage);
};

#endif