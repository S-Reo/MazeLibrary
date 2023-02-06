/*
 * test.h
 *
 *  Created on: 2022/09/03
 *      Author: leopi
 */

#ifndef INC_TEST_H_
#define INC_TEST_H_

/* マクロワード */
#define DEBUG_ON    0
#define SIMULATION  1

/* マクロ関数 */
// #define IS_GOAL(next_x, next_y) (( (GOAL_X <= next_x && next_x < (GOAL_X+GOAL_SIZE_X) )) && (GOAL_Y <= next_y && next_y < (GOAL_Y+GOAL_SIZE_Y)) )

/* 構造体 */
typedef enum {
    TASK_ADACHI_AND_DFS,
    TASK_TEST_ACTION_GENERATION,
    TASK_FASTEST_RUN
}simulation_task;

#endif /* INC_TEST_H_ */
