#ifndef INC_FASTRUN_H_
#define INC_FASTRUN_H_
#include "MazeLib.h"
#include "Searching.h"
// 最短走行関連の処理を担う
//最短走行用のアクションに番号を振る
typedef enum {
	START,
	ACC_DEC,
	ACC_DEC_45,
	ACC_DEC_90,
	L_90_SEARCH,
	R_90_SEARCH,
	L_90_FAST,
	R_90_FAST,
    //前距離と後距離は同じで設定しておく
    L_180_FAST,
    R_180_FAST,
    L_90_FAST_DIAGONAL,
    R_90_FAST_DIAGONAL,

    // 前距離と後距離を入れ替える必要がある
    L_45_FAST,
    R_45_FAST,
    L_135_FAST,
    R_135_FAST,
    L_45_FAST_REVERSE,
    R_45_FAST_REVERSE,
    L_135_FAST_REVERSE,
    R_135_FAST_REVERSE
}Action;

typedef struct {
	state path_state;
	Action path_action;
	//斜めで壁を使うにはどうするか. 今回斜めまで入れられない...
		//前壁を見るセンサを少し強めに傾けておいて見る
	_Bool path_ahead;
}Path;

extern Path FastPath[16*16];
extern int Num_Nodes;




void getPathNode(maze_node *maze, profile *mouse);
void getPathAction();
void printPathAction();
int getPathActionDiagonal();

cardinal shiftCardinalByTurn(cardinal focus_car, Action action);

void getRouteFastRun(state *log_st, state *now_st, int n);
void printRoute(state *route, int n);
#endif //INC_FASTRUN_H_