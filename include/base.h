#pragma once
#define A_pos 65
#define W_pos 87
#define S_pos 83
#define D_pos 68
#define BackSpace 13
#include <fstream>
#include <iostream>
#include <conio.h>
#include <ctime>
#include <stdlib.h>
#include <string>
#include <Windows.h>
#include <vector>
#include <algorithm>
#include <queue>
#include <cmath>

#define UP_PMOVE 1
#define DOWN_PMOVE 3
#define LEFT_PMOVE 2
#define RIGHT_PMOVE 4

using namespace std;
const int MAPSIZE=12;
const int BomSpan[4][2]={{0,-1},{-1,0},{0,1},{1,0}};
struct POS{
    int x;
    int y;
    bool operator !=(POS a){
        return this->x!= a.x||this->y!=a.y;
    }
    bool operator ==(POS a) {
        return this->x == a.x && this->y == a.y;
    }
    bool equal(int i,int j){
        return x==i&&y==j;
    }
    void set(int i,int j){
        x=i;
        y=j;
    }
    POS move(int dx, int dy) {
        POS p;
        p.set(this->x + dx, this->y + dy);
        return p;
    }
	int JudgeDir(POS a) {
		if (a.y == this->y && a.x < this->x) {
			return UP_PMOVE;//ÉÏ±ß
		}
		if (a.y == this->y && a.x > this->x) {
			return DOWN_PMOVE;//ÏÂ±ß
		}
		if (a.x == this->x && a.y < this->y) {
			return LEFT_PMOVE;//×ó±ß
		}
		if (a.x == this->x && a.y > this->y) {
			return RIGHT_PMOVE;//ÓÒ±ß
		}
        if (a.x == this->x && a.y == this->y) {
            return -1;
        }
	}
};
struct BOMB {
    POS pos;
    clock_t startTime;
    BOMB(POS p, clock_t t) {
        pos = p;
        startTime = t;
    }
    bool operator ==(POS a) {
        return pos == a;
    }
    bool operator !=(BOMB a)
    {
        return pos != a.pos || startTime != a.startTime;
    }
};