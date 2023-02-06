/*
 * Search.h
 *
 *  Created on: 2022/10/28
 *      Author: leopi
 */

#ifndef INC_SEARCHING_H_
#define INC_SEARCHING_H_

#include "MazeLib.h"
// #include "Searching.h"
// #include "MicroMouse.h"
// #include "Action.h"
//データ構造
/* ----- 探索者データ管理 ここから----- */
#define IS_GOAL(next_x, next_y) (( (GOAL_X <= next_x && next_x < (GOAL_X+GOAL_SIZE_X) )) && (GOAL_Y <= next_y && next_y < (GOAL_Y+GOAL_SIZE_Y)) )

typedef enum{
    north,
    ne,
    east,
    se,
    south,
    sw,
    west,
    nw
}cardinal;
typedef struct{
    uint8_t north:2;
    uint8_t east:2;
    uint8_t south:2;
    uint8_t west:2;
}wall_existence;

typedef enum{
    front,
	frontright,
    right,
	backright,		//Uターン+旋回のとき、まとめて動作させる
    back,
	backleft,
    left,
	frontleft
}direction;

typedef struct
{
    position pos;
    cardinal car;
    direction dir; //方向はどうせnowしかないので個別で指定: 保留
    //多分アクションも（コマンド？）
    wall_existence wall; //壁の有無
    node *node;
}state;

typedef struct
{
    // 最終的なゴール
    // position goal_lesser;
    // position goal_larger;
    position target_pos; // 今現在、重みを0とする座標
    position target_size; // その目標エリアのサイズ（positionだと不適）
    // state target;
    state now;
    state next;
}profile;

void setNextPosition(state *st);
void setPosition(position *pos,uint8_t x, uint8_t y);
void setGoal(profile *prof);
//void setWall(state *st, wall_state *w_st);
void setWallExistence(wall_existence *existence, wall_state *state);

void initProfile(profile *prof, maze_node *maze);
void shiftState(profile *prof);
void printState(state *st);
void printGoal(profile *prof);
void printProfile(profile *prof);
void initState(state *log_st, int n, node *nd);

void updateRealSearch(maze_node *maze, profile *mouse);
void getNextDirection(maze_node *my_maze, profile *Mouse, char turn_mode, int mask);
void setSearchTurnParam(int8_t mode);


node *getNodeInfo(maze_node *maze, uint8_t x, uint8_t y, cardinal car);
node *getNextNode(maze_node *maze, cardinal car, node *my_node, int mask);
state *getNextState(state *now_state, state *next_state, node *next_node);


_Bool getWallNow(state *st, wall_state *wall_st);
void getNowWallVirtual(maze_node *, profile *, uint8_t now_x, uint8_t now_y);
void getNextWallVirtual(maze_node *, profile *, uint8_t next_x, uint8_t next_y);
_Bool judgeAccelorNot(maze_node *maze, cardinal car, node *now_node);


//ノードの更新
void updateNodeThree(maze_node *maze, wall_existence *wall, uint8_t x, uint8_t y);
void updateNodeDraw(maze_node *maze, uint8_t x, uint8_t y);

void FindUnwantedSquares(maze_node *maze);

void initSearchData(maze_node *my_maze, profile *Mouse);
/* ----- 探索者データ管理 ここまで ----- */


#endif /* INC_SEARCHING_H_ */
