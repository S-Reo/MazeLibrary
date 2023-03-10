#ifndef MAZESIMULATION_H_
#define MAZESIMULATION_H_

#include "MazeLib.h"

#include "Searching.h"

#include <stdint-gcc.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
//シミュレーション用の構造体. 実装時は入れないので別ファイル
typedef struct {
    maze_node virtual_maze;
    uint8_t value_16[NUMBER_OF_SQUARES_X][NUMBER_OF_SQUARES_Y]; //外から読み込んだ0~15配列
    char filename[200];
    FILE *fp;
}simulation;

_Bool getFileData(simulation *simu);

void getNodeFrom16Value_Simulation(simulation *simu);

void recordSearchHistory(position now_pos, position target_pos);
void outputSearchHistory();
#endif /* MAZESIMULATION_H_ */