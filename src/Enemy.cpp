#include "../include/Enemy.h"
Enemy::Enemy(){}
void Enemy::InitEnemy(POS p){
    this->pos=p;
    this->lastPos = p;
    this->secondLastPos = p;
    this->walkback = 0;
    this->lastMoveTime = 0;
}
bool Enemy::JudgeDeath(POS bomPos){
    for(int i=0;i<4;i++){
        if(this->pos.equal(bomPos.x+BomSpan[i][0],bomPos.y+BomSpan[i][1])){
            return true;
        }
    }
    return false;
}
//��ĳ����������������ֵ
int Enemy::calculateF(POS src,POS des,int g) {
    if (src.equal(-1,-1))//���
        return  abs(des.x - src.x) + abs(des.y - src.y);
    else
        return g + 1 + abs(des.x - src.x) + abs(des.y - src.y);
}
int findDes(vector<AStar> table,POS des) {
    for (int i = 0; i < table.size(); i++) {
        if (table[i].pos == des)
            return i;
    }
    return -1;
}
//����A*�㷨Ϊ����Ѱ������·��
bool Enemy::findPath(POS des,int m[MAPSIZE][MAPSIZE]) {
    this->path.clear();//���ԭ����·��
    this->open.clear();
    this->close.clear();
    AStar* start = new AStar(this->pos, abs(des.x - this->pos.x) + abs(des.y - this->pos.y), 0);
    this->open.push_back(*start);
    int p = -1;
    while (!this->open.empty()) {
        if ((p = findDes(open, des)) != -1)
            break;
        int min = min_element(this->open.begin(), this->open.end()) - this->open.begin();//open����f��С��Ԫ�ص��±�
        AStar* now = new AStar(this->open[min].parent, this->open[min].pos, this->open[min].f, this->open[min].g);
        this->open.erase(this->open.begin() + min);//��open�����ó�
        this->close.push_back(*now);
        for (int i = 0; i < 4; i++) {
            POS next = now->pos.move(BomSpan[i][0], BomSpan[i][1]);
            if (!m[next.x][next.y]||des.equal(next.x,next.y)) {//���ǵ�ͼ���ը�����õ�λ�òſ�����Ϊ·��
                AStar* child = new AStar(now, next);
                child->f = calculateF(child->pos, des, now->g);
                child->g = now->g + 1;
                int j;
                if (findDes(this->close, child->pos) != -1) {
                    continue;
                }
                else if ((j = findDes(this->open, child->pos)) != -1) {
                    if (this->open[j].f > child->f) {//open���е�����ڵ��fֵ�������滻
                        this->open[j].f = child->f;
                        this->open[j].parent = child->parent;
                    }
                }
                else {//����open�б��У��������
                    this->open.push_back(*child);
                }
            }
        }
    }
    if (p != -1) {
        //����Ѱ�ҵ���·��
        this->path.push_back(open[p].pos);
        AStar* temp = open[p].parent;
        while (temp != NULL) {
            this->path.push_back(temp->pos);
            temp = temp->parent;
        }
        reverse(this->path.begin(), this->path.end());
        this->path.erase(this->path.begin() + 0);
        return true;
    }
    else {//˵���Ҳ���ͨ����ҵ�·����ʱ���Զ�ը��Ϊ��
        return false;
    }
}
void Enemy::escapeBomb(vector<BOMB>bPos, int m[MAPSIZE][MAPSIZE]) {
    int size = bPos.size();
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < 4; j++) {
            if (this->pos.equal(bPos[i].pos.x + BomSpan[j][0], bPos[i].pos.y + BomSpan[j][1])) {
                //���ɶ�����ը����·��
                for (int k = 0; k < 4; k++) {
                    if (!bPos[i].pos.equal(this->pos.x + BomSpan[j][0], this->pos.y + BomSpan[j][1]) && !m[this->pos.x + BomSpan[j][0]][this->pos.y + BomSpan[j][1]]) {
                        POS p;
                        p.set(this->pos.x + BomSpan[j][0], this->pos.y + BomSpan[j][1]);
                        this->path.push_back(p);
                        return;
                    }
                }
            }
        }
    }
}
/*void Enemy::Walk(POS des) {
    if (clock_t nowTime = clock() - this->lastFindTime > 5000 || this->path.size() <= 1) {
        this->findPath(des);
        this->lastFindTime = clock();
    }
    if(this->path[0])
    this->lastPos = this->pos;
    this->secondLastPos = this->lastPos;
    this->pos = this->path[0];
    this->path.erase(this->path.begin() + 0);
}*/
