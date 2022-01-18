#ifdef OLD_PLAYER

#include "../include/Player.h"
#include "SDL2//SDL.h"

//初始化玩家位置
Player::Player(string filepath){
    //POS p;
    //pos=this->stoneMap.GetFirstSpace();
    //p=this->stoneMap.GetLastSpace();
    this->stoneMap.InitWall(filepath);
    this->pos = this->stoneMap.p_pos;//玩家位置读入
    for (int i = 0; i < this->stoneMap.e_pos.size(); i++) {//敌人位置读入
        Enemy singleEnemy;
        singleEnemy.InitEnemy(this->stoneMap.e_pos[i]);
        this->enemy.push_back(singleEnemy);
    }
}
//更新玩家位置
void Player::Operate(){
    if (_kbhit()){//如果有按键按下，则_kbhit()函数返回真
        char ch = _getch();//使用_getch()函数获取按下的键值
        POS moveTo=this->pos;
        if(ch==A_pos){
            moveTo.y--;
        }
        else if(ch==W_pos){
            moveTo.x--;
        }
        else if(ch==S_pos){
            moveTo.x++;
        }
        else if(ch==D_pos){
            moveTo.y++;
        }
        else if(ch==BackSpace){
            this->PutBomb();
        }
        int tempMap[MAPSIZE][MAPSIZE];//存入当前地图上所有被占用的格子
        this->GetMap(tempMap);
        if(!tempMap[moveTo.x][moveTo.y]){//格子没有被占用则移动成功
            this->pos = moveTo;
        }
    }
}
void Player::OperateBySDL() {
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		if (SDL_KEYDOWN == ev.type) // SDL_KEYUP
		{
			POS moveTo = this->pos;
			if (SDLK_DOWN == ev.key.keysym.sym)
			{
				moveTo.x++;

			}
			else if (SDLK_UP == ev.key.keysym.sym)
			{
				moveTo.x--;

			}
			else if (SDLK_LEFT == ev.key.keysym.sym)
			{
				moveTo.y--;

			}
			else if (SDLK_RIGHT == ev.key.keysym.sym)
			{
				moveTo.y++;
			}
			int tempMap[MAPSIZE][MAPSIZE];//存入当前地图上所有被占用的格子
			this->GetMap(tempMap);
			if (!tempMap[moveTo.x][moveTo.y]) {//格子没有被占用则移动成功
				this->pos = moveTo;
			}
		}
	}
}

void Player::GetMap(int tempMap[MAPSIZE][MAPSIZE]) {
    for (int i = 0; i < MAPSIZE; i++) {//加入格子位置
        memcpy(tempMap[i], this->stoneMap.walls[i], sizeof(int) * MAPSIZE);
    }
    tempMap[this->pos.x][this->pos.y] = 1;//加入玩家位置
    for (int i = 0; i < this->bPos.size(); i++) {
        tempMap[this->bPos[i].pos.x][this->bPos[i].pos.y] = 1;//加入玩家放置的炸弹的位置
    }
    for (int e = 0; e < this->enemy.size(); e++) {
        tempMap[this->enemy[e].pos.x][this->enemy[e].pos.y] = 1;//加入敌人位置
        for (int i = 0; i < this->enemy[e].bPos.size(); i++) {
            tempMap[this->enemy[e].bPos[i].pos.x][this->enemy[e].bPos[i].pos.y] = 1;//加入敌人放置的炸弹的位置
        }
    }
    return;
}
void Player::Walk() {
    for (int e = 0; e < this->enemy.size(); e++) {
        if (this->enemy[e].path.size() == 1) {
            BOMB b(this->enemy[e].path[0], clock());
            this->enemy[e].bPos.push_back(b);
            this->enemy[e].path.insert(this->enemy[e].path.begin() + 0, this->enemy[e].pos);//多走两步
            this->enemy[e].path.insert(this->enemy[e].path.begin() + 0, this->enemy[e].lastPos);
        }
        int tempMap[MAPSIZE][MAPSIZE];//存入当前地图上所有被占用的格子
        this->GetMap(tempMap);
        if (clock() - this->enemy[e].lastFindTime > 5000 || this->enemy[e].path.size() <= 1/* || tempMap[this->enemy[e].path[0].x][this->enemy[e].path[0].y]*/) {
            this->enemy[e].findPath(this->pos, tempMap);
            this->enemy[e].lastFindTime = clock();
        }
        if (clock() - this->enemy[e].lastMoveTime >= 1000) {
            if (this->enemy[e].path.size()) {
                if (tempMap[this->enemy[e].path[0].x][this->enemy[e].path[0].y]) {
                    this->enemy[e].pos = this->enemy[e].lastPos;
                    this->enemy[e].findPath(this->pos, tempMap);
                    this->enemy[e].lastFindTime = clock();
                }
                else {
                    this->enemy[e].lastPos = this->enemy[e].pos;
                    this->enemy[e].pos = this->enemy[e].path[0];
                    this->enemy[e].path.erase(this->enemy[e].path.begin() + 0);
                }
            }
            this->enemy[e].lastMoveTime = clock();
        }
    }
}
//放置炸弹,需要在主函数中不断获取时间减去bomStartTime，若在2-3秒之间，则调用map的更新函数，并且将starttime重置为0
void Player::PutBomb(){
    clock_t bomStartTime=clock();
    BOMB bomb(this->pos, bomStartTime);
    this->bPos.push_back(bomb);
}
//判断游戏是否结束/玩家是否被炸死，在炸弹爆炸更新地图时调用，判断玩家是否在爆炸范围内
bool Player::JudgeDeath(POS bomPos){
    if (this->pos == bomPos) {
        return true;
    }
    for(int i=0;i<4;i++){
        if(this->pos.x==bomPos.x+BomSpan[i][0]&&this->pos.y==bomPos.y+BomSpan[i][1]){
            return true;
        }
    }
    return false;
}
//对游戏状态等进行更新、地图更新，返回游戏是否结束
bool Player::Update(){
    
	/* Cancan : 更改输入窗口 */
	//this->Operate();//抓取玩家操作，玩家移动或防止炸弹
	this->OperateBySDL();
	
	this->Walk();//敌人自动移动并根据情况放置炸弹
    clock_t nowTime=clock();
    for (int i = 0; i < this->bPos.size(); i++) {
        if (nowTime - this->bPos[i].startTime >= 2000) {

            this->stoneMap.Update(this->bPos[i].pos);

            if (this->JudgeDeath(this->bPos[i].pos)) {//玩家被自己放置的炸弹炸死
                this->win = false;//游戏失败
                return true;
            }
            for (int e = 0; e < this->enemy.size(); e++) {
                if (this->enemy[e].JudgeDeath(this->bPos[i].pos)) {//敌人被玩家的炸弹炸死
                    this->enemy.erase(this->enemy.begin() + e);//删除这个敌人
                    
                }
            }
            if (this->enemy.empty()) {//敌人全部被杀死
                this->win = true;//游戏成功
                return true;
            }
            this->bPos.erase(this->bPos.begin()+i);//删除此炸弹
        }
    }
    for (int e = 0; e < this->enemy.size(); e++) {
        for (int i = 0; i < this->enemy[e].bPos.size(); i++) {
            if (nowTime - this->enemy[e].bPos[i].startTime >= 2000) {
                this->stoneMap.Update(this->enemy[e].bPos[i].pos);
                if (this->JudgeDeath(this->enemy[e].bPos[i].pos)) {//玩家被敌人放置的炸弹炸死
                    this->win = false;//游戏失败
                    return true;
                }
                this->enemy[e].bPos.erase(this->enemy[e].bPos.begin() + i);//删除此炸弹
            }
        }
    }
    //敌人能够在放置炸弹的基础上保证不会被自己放置的炸弹炸死(但有可能被同伴炸死....)
    return false;
}
//显示函数
void Player::Display(){
    for(int i=0;i<MAPSIZE;i++){
        for(int j=0;j<MAPSIZE;j++){
            if (this->stoneMap.walls[i][j]) {
                cout << "#";
            }
            else if (this->pos.equal(i, j)) {
                cout << "*";
            }
            else {
                bool isOccupied = false;
                for (int k = 0; k < this->bPos.size(); k++) {
                    if (this->bPos[k].pos.equal(i, j)) {
                        isOccupied = true;
                        cout << "@";
                        break;
                    }
                }
                for (int e = 0; e < this->enemy.size(); e++) {
                    if (this->enemy[e].pos.equal(i, j)) {
                        isOccupied = true;
                        cout << "/";
                        break;
                    }
                    for (int k = 0; k < this->enemy[e].bPos.size(); k++) {
                        if (this->enemy[e].bPos[k].pos.equal(i, j)) {
                            isOccupied = true;
                            cout << "@";
                            break;
                        }
                    }
                }
                if(!isOccupied)
                    cout << " ";
            }
        }
        cout<<endl;
    }
}
void gotoxy(int x, int y) {
    COORD pos = { (SHORT)x,(SHORT)y };
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}
void Player::Begin(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    GetConsoleCursorInfo(hOut, &info);
    info.bVisible = false;
    SetConsoleCursorInfo(hOut, &info);
    while(1){
        this->Operate();//抓取玩家操作，玩家移动或防止炸弹
        this->Walk();//敌人自动移动并根据情况放置炸弹
        bool gameOver=this->Update();//更新，判断炸弹是否爆炸，游戏是否结束
        //system("cls");
        gotoxy(0, 0);
        this->Display();
        if(gameOver){
            break;
        }
    }
    if(this->win){
        cout<<"YOU WIN"<<endl;
    }
    else{
        cout<<"YOU LOSE"<<endl;
    }
}

bool Player::IsWin()
{
	return this->win;
}

#else

#include "../include/Player.h"
#include "SDL2//SDL.h"

//初始化玩家位置
Player::Player(string filepath) {
	//POS p;
	//pos=this->stoneMap.GetFirstSpace();
	//p=this->stoneMap.GetLastSpace();
	this->stoneMap.InitWall(filepath);
	this->pos = this->stoneMap.p_pos;//玩家位置读入
	for (int i = 0; i < this->stoneMap.e_pos.size(); i++) {//敌人位置读入
		Enemy singleEnemy;
		singleEnemy.InitEnemy(this->stoneMap.e_pos[i]);
		this->enemy.push_back(singleEnemy);
	}
	this->dir = RIGHT_PMOVE;
	this->isMove = false;
}
//更新玩家位置
void Player::Operate() {
	if (_kbhit()) {//如果有按键按下，则_kbhit()函数返回真
		char ch = _getch();//使用_getch()函数获取按下的键值
		POS moveTo = this->pos;
		if (ch == A_pos) {
			moveTo.y--;
			this->isMove = true;
		}
		else if (ch == W_pos) {
			moveTo.x--;
			this->isMove = true;
		}
		else if (ch == S_pos) {
			moveTo.x++;
			this->isMove = true;
		}
		else if (ch == D_pos) {
			moveTo.y++;
			this->isMove = true;
		}
		else if (ch == BackSpace) {
			this->PutBomb();
		}
		int tempMap[MAPSIZE][MAPSIZE];//存入当前地图上所有被占用的格子
		this->GetMap(tempMap);
		if (!tempMap[moveTo.x][moveTo.y]) {//格子没有被占用则移动成功
			this->pos = moveTo;
		}
	}
}
void Player::OperateBySDL() {
	SDL_Event ev;

	while (SDL_PollEvent(&ev))
	{
		if (SDL_KEYDOWN == ev.type) // SDL_KEYUP
		{
			POS moveTo = this->pos;
			if (SDLK_DOWN == ev.key.keysym.sym)
			{
				moveTo.x++;
				dir = DOWN_PMOVE;

			}
			else if (SDLK_UP == ev.key.keysym.sym)
			{
				moveTo.x--;
				dir = UP_PMOVE;
			}
			else if (SDLK_LEFT == ev.key.keysym.sym)
			{
				moveTo.y--;
				dir = LEFT_PMOVE;
			}
			else if (SDLK_RIGHT == ev.key.keysym.sym)
			{
				moveTo.y++;
				dir = RIGHT_PMOVE;
			}
			else if (SDLK_SPACE == ev.key.keysym.sym)
			{
				this->PutBomb();
			}
			int tempMap[MAPSIZE][MAPSIZE];//存入当前地图上所有被占用的格子
			this->GetMap(tempMap);
			if (!tempMap[moveTo.x][moveTo.y]) {//格子没有被占用则移动成功
				this->pos = moveTo;
			}
		}
	}
}

void Player::GetMap(int tempMap[MAPSIZE][MAPSIZE]) {
	for (int i = 0; i < MAPSIZE; i++) {//加入格子位置
		memcpy(tempMap[i], this->stoneMap.walls[i], sizeof(int) * MAPSIZE);
	}
	tempMap[this->pos.x][this->pos.y] = 1;//加入玩家位置
	for (int i = 0; i < this->bPos.size(); i++) {
		tempMap[this->bPos[i].pos.x][this->bPos[i].pos.y] = 1;//加入玩炸弹的位置
	}
	for (int e = 0; e < this->enemy.size(); e++) {
		tempMap[this->enemy[e].pos.x][this->enemy[e].pos.y] = 1;//加入敌人位置
	}
	return;
}
void Player::Walk() {
	for (int e = 0; e < this->enemy.size(); e++) {
		if (this->enemy[e].path.size() == 2) {
			BOMB b(this->enemy[e].path[0], clock());
			this->bPos.push_back(b);
		}
		int tempMap[MAPSIZE][MAPSIZE];//存入当前地图上所有被占用的格子
		this->GetMap(tempMap);
		if (clock() - this->enemy[e].lastFindTime > 3000 ||  this->enemy[e].path.size() == 0 || tempMap[this->enemy[e].path[0].x][this->enemy[e].path[0].y]|| this->enemy[e].path[0] == this->pos) {
			if (this->enemy[e].path.size()&&this->enemy[e].path[0] == this->pos) {
				this->enemy[e].path.clear();
				this->enemy[e].path.push_back(this->enemy[e].lastPos);
			}
			else if (!this->enemy[e].findPath(this->pos, tempMap)) {
				this->enemy[e].escapeBomb(this->bPos, tempMap);
			}
			if (this->enemy[e].path.size() == 0) {
				this->enemy[e].path.push_back(this->enemy[e].pos);
			}
			this->enemy[e].lastFindTime = clock();
		}
		if (clock() - this->enemy[e].lastMoveTime >= 1000) {
			this->enemy[e].lastPos = this->enemy[e].pos;
			tempMap[this->enemy[e].pos.x][this->enemy[e].pos.y] = 0;
			this->enemy[e].pos = this->enemy[e].path[0];
			tempMap[this->enemy[e].pos.x][this->enemy[e].pos.y] = 1;
			this->enemy[e].path.erase(this->enemy[e].path.begin() + 0);
			this->enemy[e].lastMoveTime = clock();
		}
	}
}
//放置炸弹,需要在主函数中不断获取时间减去bomStartTime，若在2-3秒之间，则调用map的更新函数，并且将starttime重置为0
void Player::PutBomb() {
	clock_t bomStartTime = clock();
	BOMB bomb(this->pos, bomStartTime);
	this->bPos.push_back(bomb);
}
//判断游戏是否结束/玩家是否被炸死，在炸弹爆炸更新地图时调用，判断玩家是否在爆炸范围内
bool Player::JudgeDeath(POS bomPos) {
	if (this->pos == bomPos) {
		return true;
	}
	for (int i = 0; i < 4; i++) {
		if (this->pos.x == bomPos.x + BomSpan[i][0] && this->pos.y == bomPos.y + BomSpan[i][1]) {
			return true;
		}
	}
	return false;
}
//对游戏状态等进行更新、地图更新，返回游戏是否结束
bool Player::Update() {

	/* Cancan : 更改输入窗口 */
	//this->Operate();//抓取玩家操作，玩家移动或防止炸弹
	this->OperateBySDL();

	this->Walk();//敌人自动移动并根据情况放置炸弹
	clock_t nowTime = clock();
	for (int i = 0; i < this->bPos.size(); i++) {
		if (nowTime - this->bPos[i].startTime >= 2000) {

			this->stoneMap.Update(this->bPos[i].pos);

			if (this->JudgeDeath(this->bPos[i].pos)) {//玩家被炸弹炸死
				this->win = false;//游戏失败
				return true;
			}
			for (int e = 0; e < this->enemy.size(); e++) {
				if (this->enemy[e].JudgeDeath(this->bPos[i].pos)) {//敌人被炸弹炸死
					this->enemy.erase(this->enemy.begin() + e);//删除这个敌人
				}
			}
			if (this->enemy.empty()) {//敌人全部被杀死
				this->win = true;//游戏成功
				return true;
			}
			this->bPos.erase(this->bPos.begin() + i);//删除此炸弹
		}
	}
		for (int i = 0; i < this->bPos.size(); i++) {
			if (nowTime - this->bPos[i].startTime >= 2000) {
				this->stoneMap.Update(this->bPos[i].pos);
				if (this->JudgeDeath(this->bPos[i].pos)) {//玩家被敌人放置的炸弹炸死
					this->win = false;//游戏失败
					return true;
				}
				for (int en = 0; en < this->enemy.size(); en++) {
					if (this->enemy[en].JudgeDeath(this->bPos[i].pos)) {//敌人被炸弹炸死
						this->enemy.erase(this->enemy.begin() + en);//删除这个敌人
					}
				}
				if (this->enemy.empty()) {//敌人全部被杀死
					this->win = true;//游戏成功
					return true;
				}
				this->bPos.erase(this->bPos.begin() + i);//删除此炸弹
			}
		}
	return false;
}
//显示函数
void Player::Display() {
	for (int i = 0; i < MAPSIZE; i++) {
		for (int j = 0; j < MAPSIZE; j++) {
			if (this->stoneMap.walls[i][j]) {
				cout << "#";
			}
			else if (this->pos.equal(i, j)) {
				cout << "*";
			}
			else {
				bool isOccupied = false;
				for (int k = 0; k < this->bPos.size(); k++) {
					if (this->bPos[k].pos.equal(i, j)) {
						isOccupied = true;
						cout << "@";
						break;
					}
				}
				for (int e = 0; e < this->enemy.size(); e++) {
					if (this->enemy[e].pos.equal(i, j)) {
						isOccupied = true;
						cout << "/";
						break;
					}
				}
				if (!isOccupied)
					cout << " ";
			}
		}
		cout << endl;
	}
	this->isMove = false;
}
void gotoxy(int x, int y) {
	COORD pos = { (SHORT)x,(SHORT)y };
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleCursorPosition(hOut, pos);
}
void Player::Begin() {
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(hOut, &info);
	info.bVisible = false;
	SetConsoleCursorInfo(hOut, &info);
	while (1) {
		this->Operate();//抓取玩家操作，玩家移动或防止炸弹
		this->Walk();//敌人自动移动并根据情况放置炸弹
		bool gameOver = this->Update();//更新，判断炸弹是否爆炸，游戏是否结束
		//system("cls");
		gotoxy(0, 0);
		//this->Display();
		if (gameOver) {
			break;
		}
	}
	if (this->win) {
		cout << "YOU WIN" << endl;
	}
	else {
		cout << "YOU LOSE" << endl;
	}
}

bool Player::IsWin()
{
	return this->win;
}

void Player::GetDrawMessage(DrawMessage* drawMessage) {
	drawMessage->isMove = this->isMove;
	for (int i = 0; i < MAPSIZE; i++) {
		for (int j = 0; j < MAPSIZE; j++) {
			if (i == 0 || j == 0 || i == MAPSIZE - 1 || j == MAPSIZE - 1) {
				drawMessage->drawMap[i][j] = '$';
			}
			else if (this->stoneMap.walls[i][j]) {
				drawMessage->drawMap[i][j] = '#';
			}
			else if (this->pos.equal(i, j)) {
				drawMessage->drawMap[i][j] = '*';
			}
			else
				drawMessage->drawMap[i][j] = ' ';
		}
	}
	for (int k = 0; k < this->bPos.size(); k++) {
		drawMessage->drawMap[this->bPos[k].pos.x][this->bPos[k].pos.y] = '@';
	}
	drawMessage->playerDir = this->dir;
	POS enemypos0[ENEMT_NUM];
	for (int i = 0; i < this->enemy.size(); i++) {
		drawMessage->drawMap[this->enemy[i].pos.x][this->enemy[i].pos.y] = '/';
		drawMessage->enemyPos[i] = this->enemy[i].pos;
		if (this->enemy[i].path.size()) {
				int dir = this->enemy[i].pos.JudgeDir(this->enemy[i].path[0]);
				drawMessage->enemyDir[i] = (dir == -1) ? drawMessage->enemyDir[i] : dir;
				drawMessage->enemyMove[i] = (dir == -1) ? false : true;
				for (int k = 0; k < i; k++) {
					if (this->enemy[k].path.size() && (this->enemy[k].path[0] == this->enemy[i].path[0])) {
						drawMessage->enemyMove[i] = false;
						break;
					}
				}
		}
	}
	for (int j = 0; j < this->bPos.size(); j++) {
		drawMessage->drawMap[this->bPos[j].pos.x][this->bPos[j].pos.y] = '@';
	}
}
#endif