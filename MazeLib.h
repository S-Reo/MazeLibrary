
/* マイクロマウスの迷路管理ライブラリ */
// 独立させる。ほかのライブラリと疎結合。
// 仕様
    // 管理方法は変わっても、データが示すものは一緒

// 重みづけ処理の管理
    //複数パターン用意する
        //マスの中央に評価値を置くパターン
        //エッジに評価値を置くパターン

#ifndef MAZELIB_H_
#define MAZELIB_H_

#include <stdint-gcc.h>
#include <stdbool.h>
#include <stdio.h>

#define NUMBER_OF_SQUARES_X 32
#define NUMBER_OF_SQUARES_Y 32


#define GOAL_SIZE_X 2
#define GOAL_SIZE_Y 2

#define GOAL_X 2 //14
#define GOAL_Y 2 //14

#define __JUDGE_GOAL__(x,y) (( (GOAL_X <= x) && (x < GOAL_X + GOAL_SIZE_X)) && ((GOAL_Y <= y) && (y < GOAL_Y + GOAL_SIZE_Y)) )

#define MAX_WEIGHT 4095

extern int WALL_MASK;
//隣り合うノード間の移動において、ノードの位置関係と移動方向（ベクトル）を判定するマクロ
    // 例: 行ノードから行ノード、向きは北であるかどうか（入力は2つのノードのxy）
#define __RAW_TO_COLUMN_NE__(x_r,y_r, x_c,y_c)   (x_r+1 == x_c && y_r == y_c)   //北東 
#define __COLUMN_TO_RAW_SW__(x_c,y_c, x_r,y_r)   (x_c-1 == x_r && y_c == y_r)   //南西

#define __RAW_TO_COLUMN_SE__(x_r,y_r, x_c,y_c)   (x_r+1 == x_c && y_r-1 == y_c) //南東
#define __COLUMN_TO_RAW_NW__(x_c,y_c, x_r,y_r)   (x_c-1 == x_r && y_c+1 == y_r) //北西

#define __RAW_TO_COLUMN_SW__(x_r,y_r, x_c,y_c)   (x_r == x_c && y_r-1 == y_c)   //南西
#define __COLUMN_TO_RAW_NE__(x_c,y_c, x_r,y_r)   (x_c == x_r && y_c+1 == y_r)   //北東 

#define __RAW_TO_COLUMN_NW__(x_r,y_r, x_c,y_c)   (x_r == x_c && y_r == y_c)     //北西
#define __COLUMN_TO_RAW_SE__(x_c,y_c, x_r,y_r)   (x_c == x_r && y_c == y_r)     //南東

//直進
#define __RAW_TO_RAW_NORTH__(x_1,y_1, x_2,y_2)   (x_1 == x_2 && y_1+1 == y_2)
#define __RAW_TO_RAW_SOUTH__(x_1,y_1, x_2,y_2)   (x_1 == x_2 && y_1-1 == y_2)

#define __COLUMN_TO_COLUMN_EAST__(x_1,y_1, x_2,y_2)   (x_1+1 == x_2 && y_1 == y_2)
#define __COLUMN_TO_COLUMN_WEST__(x_1,y_1, x_2,y_2)   (x_1-1 == x_2 && y_1 == y_2)


// 上記のほかにも、斜め走行のためには位置関係をジャッジするマクロが必要になる

typedef enum{
    NOWALL  = 0,
    WALL    = 1,
    UNKNOWN = 2,
    VIRTUAL = 3,   //壁が無いが、あると仮定したいときに使う
    
}wall_state;
typedef struct
{
    uint8_t x;
    uint8_t y;
}position;
typedef struct {
	uint8_t existence;
	uint16_t weight; //64×64対応
	_Bool draw;
    _Bool rc; //行か列かを見たい
    position pos;
//    _Bool visit;
}node;

typedef struct {
    node RawNode[NUMBER_OF_SQUARES_X][NUMBER_OF_SQUARES_Y+1];
    node ColumnNode[NUMBER_OF_SQUARES_X+1][NUMBER_OF_SQUARES_Y];
}maze_node;
//迷路の初期化
void initMaze(maze_node *maze);
void initWeight(maze_node *maze);
//迷路データの更新用の関数
void updateAllNodeWeight(maze_node *maze, position *target_pos, position *target_size, int mask);

void printAllWeight(maze_node *maze, position *pos);
//ノードの壁の有無
void printSingleNode(maze_node *mn, uint8_t x, uint8_t y);
void printAllNode(maze_node *mn);//外堀だけprintfせず、そのまま描画用データに
void printAllNodeExistence(maze_node *mn);
_Bool outputDataToFile(maze_node *maze, char *name);

//迷路の状態確認
void printMatrix16ValueFromNode(maze_node *maze);

/* ----- 迷路データ管理 ここまで----- */


#endif /* MAZELIB_H_ */
