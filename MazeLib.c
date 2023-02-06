#include "MazeLib.h"

#include "string.h"
//迷路データを管理する機能を担う(探索者ファイルと分ける必要がある)
// 初期化
// 表示
// 取得
// 更新

int WALL_MASK;

//初期化処理
void initWeight(maze_node *maze)
{
    for(int i=0; i < NUMBER_OF_SQUARES_X; i++)
    {
        for(int j=0; j < NUMBER_OF_SQUARES_Y+1; j++)
        {
            maze->RawNode[i][j].weight = MAX_WEIGHT;  
        }
    }
    for(int i=0; i < NUMBER_OF_SQUARES_X+1; i++)
    {
        for(int j=0; j < NUMBER_OF_SQUARES_Y; j++)
        {
            maze->ColumnNode[i][j].weight = MAX_WEIGHT;
        }
    }
}
void initMaze(maze_node *maze) //重みは別で初期化
{
    //まず未探索状態にする
    for(int i=0; i < NUMBER_OF_SQUARES_X; i++)
    {
        for(int j=1; j < NUMBER_OF_SQUARES_Y; j++)
        {
            maze->RawNode[i][j].existence = UNKNOWN;
            maze->RawNode[i][j].draw = false;//未知壁は描画のときに無いものとする
            maze->RawNode[i][j].rc = 0;
            maze->RawNode[i][j].pos.x = i;
            maze->RawNode[i][j].pos.y = j;
//            maze->RawNode[i][j].visit = false;
        }
    }
    for(int i=0; i < NUMBER_OF_SQUARES_X; i++) //外壁
    {
            maze->RawNode[i][0].existence = WALL;
            maze->RawNode[i][0].draw = true;//未知壁は描画のときに無いものとする
            maze->RawNode[i][0].rc = 0;
            maze->RawNode[i][0].pos.x = i;
            maze->RawNode[i][0].pos.y = 0;
//            maze->RawNode[i][0].visit = true; //便宜的に訪問済みとする
            maze->RawNode[i][NUMBER_OF_SQUARES_Y].existence = WALL;
            maze->RawNode[i][NUMBER_OF_SQUARES_Y].draw = true;//未知壁は描画のときに無いものとする
            maze->RawNode[i][NUMBER_OF_SQUARES_Y].rc = 0;
            maze->RawNode[i][NUMBER_OF_SQUARES_Y].pos.x = i;
            maze->RawNode[i][NUMBER_OF_SQUARES_Y].pos.y = NUMBER_OF_SQUARES_Y;
//            maze->RawNode[i][NUMBER_OF_SQUARES_Y].visit = true; //便宜的に訪問済みとする
    }
    for(int i=1; i < NUMBER_OF_SQUARES_X; i++)
    {
        for(int j=0; j < NUMBER_OF_SQUARES_Y; j++)
        {
            maze->ColumnNode[i][j].existence = UNKNOWN;
            maze->ColumnNode[i][j].draw = false;
            maze->ColumnNode[i][j].rc = 1;
            maze->ColumnNode[i][j].pos.x = i;
            maze->ColumnNode[i][j].pos.y = j;
//            maze->ColumnNode[i][j].visit = false;
        }
    }
    for(int j=0; j < NUMBER_OF_SQUARES_Y; j++)
    {
            maze->ColumnNode[0][j].existence = WALL;
            maze->ColumnNode[0][j].draw = true;
            maze->ColumnNode[0][j].rc = 1;
            maze->ColumnNode[0][j].pos.x = 0;
            maze->ColumnNode[0][j].pos.y = j;
//            maze->ColumnNode[0][j].visit = true;
            maze->ColumnNode[NUMBER_OF_SQUARES_X][j].existence = WALL;
            maze->ColumnNode[NUMBER_OF_SQUARES_X][j].draw = true;
            maze->ColumnNode[NUMBER_OF_SQUARES_X][j].rc = 1;
            maze->ColumnNode[NUMBER_OF_SQUARES_X][j].pos.x = NUMBER_OF_SQUARES_X;
            maze->ColumnNode[NUMBER_OF_SQUARES_X][j].pos.y = j;
//            maze->ColumnNode[NUMBER_OF_SQUARES_X][j].visit = true;
    }
    maze->ColumnNode[1][0].existence = WALL;    //東1
    maze->RawNode[0][1].existence = NOWALL;     //北0

    maze->ColumnNode[1][0].draw = true;    //東1
    maze->RawNode[0][1].draw = false;     //北0

    //visitは、壁があるところと訪問済みのところを1、まだのところを0
//    maze->ColumnNode[1][0].visit = true;    //東1
//	maze->RawNode[0][1].visit = false;     //北0
}
void initTargetAreaWeight(maze_node *maze, position *target_pos, position *target_size)
{
    uint8_t x = target_pos->x;
    uint8_t y = target_pos->y;
    //ゴールエリアの外堀と中のノードは全て0、壁があればMAX。
    for(int i=0; i < target_size->x; i++)
    {
        for(int j=0; j < target_size->y; j++)
        {
            // maze->RawNode[x+i][y+1+j].weight = (maze->RawNode[x+i][y+1+j].weight == MAX_WEIGHT) ? MAX_WEIGHT : 0;       //北
            // maze->ColumnNode[x+1+i][y+j].weight = (maze->ColumnNode[x+1+i][y+j].weight == MAX_WEIGHT) ? MAX_WEIGHT : 0; //東
            // maze->RawNode[x+i][y+j].weight = (maze->RawNode[x+i][y+j].weight == MAX_WEIGHT) ? MAX_WEIGHT : 0;           //南
            // maze->ColumnNode[x+i][y+j].weight = (maze->ColumnNode[x+i][y+j].weight == MAX_WEIGHT) ? MAX_WEIGHT : 0;     //西

        	//なぜdraw == trueでやっていたかわからない
            maze->RawNode[x+i][y+1+j].weight = (maze->RawNode[x+i][y+1+j].existence == WALL) ? MAX_WEIGHT : 0;       //北
            maze->ColumnNode[x+1+i][y+j].weight = (maze->ColumnNode[x+1+i][y+j].existence == WALL) ? MAX_WEIGHT : 0; //東
            maze->RawNode[x+i][y+j].weight = (maze->RawNode[x+i][y+j].existence == WALL) ? MAX_WEIGHT : 0;           //南
            maze->ColumnNode[x+i][y+j].weight = (maze->ColumnNode[x+i][y+j].existence == WALL) ? MAX_WEIGHT : 0;     //西
        }
    }
}

/* 重みマップを含む、ノードの更新処理 */
#define WEIGHT_NANAME   1
#define WEIGHT_STRAIGHT 3

void updateAllNodeWeight(maze_node *maze,position *target_pos, position *target_size, int mask)
{
	//全体に154/20ms = 7.7ms
    //新しい区画に入ったときに、更新

    initWeight(maze); //3/20ms

    initTargetAreaWeight(maze, target_pos, target_size);

    int i,j; //コンパイラオプションで-Ofastを付ければ、register修飾子は要らなかった。
    _Bool change_flag;
    int skip_raw=0, skip_column=0;

	do //(6,9)(7,10)に対して、7,11がおかしい。
	{
		change_flag = false;
        
        //列
        for(i = 1; i < NUMBER_OF_SQUARES_X; i++)
		{
			for( j = 0; j < NUMBER_OF_SQUARES_Y; j++)
			{
                if(maze->ColumnNode[i][j].weight == MAX_WEIGHT)		//MAXの場合は次へ
				{
                	skip_column++;
					continue;
				}
                // printf("continueはクリア. Column[%d][%d]\r\n",i,j);

                //東側ノード
				if(i < NUMBER_OF_SQUARES_X-1)					//範囲チェック
				{
                    // printf("列東%d,mask: %d, result: %d\r\n",maze->ColumnNode[i+1][j].existence, mask,((maze->ColumnNode[i+1][j].existence) & mask));
					if( ((maze->ColumnNode[i+1][j].existence & mask) == NOWALL) && (maze->ColumnNode[i+1][j].weight == MAX_WEIGHT))	//壁がなければ(maskの意味はstatic_parametersを参照)
					{
						maze->ColumnNode[i+1][j].weight = maze->ColumnNode[i][j].weight + WEIGHT_STRAIGHT;	//値を代入
						change_flag = true;		//値が更新されたことを示す
					}
                }
                //西側ノード
				if(1 < i)						//範囲チェック
				{
					if( ((maze->ColumnNode[i-1][j].existence & mask) == NOWALL) && (maze->ColumnNode[i-1][j].weight == MAX_WEIGHT))	//壁がなければ
					{
						maze->ColumnNode[i-1][j].weight = maze->ColumnNode[i][j].weight + WEIGHT_STRAIGHT;	//値を代入
						change_flag = true;		//値が更新されたことを示す
					}
				}
                //北側に斜めが2方向
				if(j < NUMBER_OF_SQUARES_Y-1)					//範囲チェック
				{
                    //北東
					if(i < NUMBER_OF_SQUARES_X)
					{
						if( ((maze->RawNode[i][j+1].existence & mask) == NOWALL) && (maze->RawNode[i][j+1].weight == MAX_WEIGHT))		//壁がなければ
						{
							maze->RawNode[i][j+1].weight = maze->ColumnNode[i][j].weight + WEIGHT_NANAME;	//値を代入
							change_flag = true;		//値が更新されたことを示す
						}
					}
                    //北西
					if(0 < i)
					{
						if( ((maze->RawNode[i-1][j+1].existence & mask) == NOWALL) && (maze->RawNode[i-1][j+1].weight == MAX_WEIGHT))		//壁がなければ
						{
							maze->RawNode[i-1][j+1].weight = maze->ColumnNode[i][j].weight + WEIGHT_NANAME;	//値を代入
							change_flag = true;		//値が更新されたことを示す
						}
					}
				}

                //南側に斜めが2方向
				if(0 < j)
				{
					if( i < NUMBER_OF_SQUARES_X )//j > 0)						//範囲チェック
					{
						//南東
						if( ((maze->RawNode[i][j].existence & mask) == NOWALL) && (maze->RawNode[i][j].weight == MAX_WEIGHT)	)		//壁がなければ
						{
							maze->RawNode[i][j].weight = maze->ColumnNode[i][j].weight + WEIGHT_NANAME;	//値を代入
							change_flag = true;		//値が更新されたことを示す
						}
					}
					if(0 < i ){
						//南西
						if( ((maze->RawNode[i-1][j].existence & mask) == NOWALL) && (maze->RawNode[i-1][j].weight == MAX_WEIGHT))		//壁がなければ
						{
								maze->RawNode[i-1][j].weight = maze->ColumnNode[i][j].weight + WEIGHT_NANAME;	//値を代入
								change_flag = true;		//値が更新されたことを示す
						}
					}
				}
            }
        }
        //行と列でわけて、一周
         //行から
 		for( i = 0; i < NUMBER_OF_SQUARES_X; i++)			//迷路の大きさ分ループ(x座標)
 		{
 			for(j = 1; j < NUMBER_OF_SQUARES_Y; j++)		//迷路の大きさ分ループ(y座標)
 			{
                 //1ノードずつ見る.そこから加算対象が最大6個
                 //端を見ないので、一番上の列からスタート j=N; j >= 0, xを1からN-1まで
                 //次に行 j=N-1から1まで xを0からN-1まで
 				if(maze->RawNode[i][j].weight == MAX_WEIGHT)		//MAXの場合は次へ
 				{
 					skip_raw ++;
 					continue;
 				}
 				// printf("continueはクリア. Raw[%d][%d]\r\n",i,j);
                 //北側ノード
 				if(j < NUMBER_OF_SQUARES_Y-1)   //範囲チェック. 座標のxyではなく、ノードのxy
 				{
 					if( ((maze->RawNode[i][j+1].existence & mask) == NOWALL) && (maze->RawNode[i][j+1].weight == MAX_WEIGHT) )	//壁がなければ(maskの意味はstatic_parametersを参照)
 					{
 						maze->RawNode[i][j+1].weight = maze->RawNode[i][j].weight + WEIGHT_STRAIGHT;	//値を代入
 						change_flag = true;		//値が更新されたことを示す
 					}
                 }
                 //南側ノード
 				if(j > 1)						//範囲チェック.ミスってた
 				{
 					if( ((maze->RawNode[i][j-1].existence & mask) == NOWALL) && (maze->RawNode[i][j-1].weight == MAX_WEIGHT) )	//壁がなければ
 					{
 						maze->RawNode[i][j-1].weight = maze->RawNode[i][j].weight + WEIGHT_STRAIGHT;	//値を代入
 						change_flag = true;		//値が更新されたことを示す
 					}
 				}
                 //東側に斜めが2方向
 				if(i < NUMBER_OF_SQUARES_X-1)					//範囲チェック
 				{
                     //y方向の制限は？
                     //北東
 					if( j < NUMBER_OF_SQUARES_Y )
 					{
 						if( ((maze->ColumnNode[i+1][j].existence & mask) == NOWALL) && (maze->ColumnNode[i+1][j].weight == MAX_WEIGHT))		//壁がなければ
 						{
 							maze->ColumnNode[i+1][j].weight = maze->RawNode[i][j].weight + WEIGHT_NANAME;	//値を代入
 							change_flag = true;		//値が更新されたことを示す
 						}
 					}
 					if( 0 < j )
 					{
 						//南東
 						if( ((maze->ColumnNode[i+1][j-1].existence & mask) == NOWALL) && (maze->ColumnNode[i+1][j-1].weight == MAX_WEIGHT)	)		//壁がなければ
 						{
 							maze->ColumnNode[i+1][j-1].weight = maze->RawNode[i][j].weight + WEIGHT_NANAME;	//値を代入
 							change_flag = true;		//値が更新されたことを示す
 						}
 					}
 				}

                 //西側に斜めが2方向
 				if(i > 0)						//範囲チェック
 				{
                     //北西
 					if( j < NUMBER_OF_SQUARES_Y )
 					{
 						if( ((maze->ColumnNode[i][j].existence & mask) == NOWALL)  && (maze->ColumnNode[i][j].weight == MAX_WEIGHT) )		//壁がなければ
 						{
 								maze->ColumnNode[i][j].weight = maze->RawNode[i][j].weight + WEIGHT_NANAME;	//値を代入
 								change_flag = true;		//値が更新されたことを示す

 						}
 					}
 					if( 0 < j )
 					{
 						//南西
 						if( ((maze->ColumnNode[i][j-1].existence & mask) == NOWALL) && (maze->ColumnNode[i][j-1].weight == MAX_WEIGHT))		//壁がなければ
 						{
 							maze->ColumnNode[i][j-1].weight = maze->RawNode[i][j].weight + WEIGHT_NANAME;	//値を代入
 							change_flag = true;		//値が更新されたことを示す
 						}
 					}

 				}
 			}
 		}
        //printf("重みの更新\r\n");//一回しか呼ばれていない
        //cnt++;
	}while(change_flag == true);	//全体を作り終わるまで待つ
//    t = 0;
//	HAL_TIM_Base_Stop_IT(&htim8);
//	printf("%d/20ms, %d, %d\r\n\r\n",timer8, skip_raw, skip_column);
}


/* ノードの中身確認 */
static _Bool judgeRawNodeGoal(maze_node *maze, uint8_t x, uint8_t y)
{
    //重みが0かどうか
    if(maze->RawNode[x][y].weight == 0)
    {
        //ゴールノードであるかどうか:マクロ作った

        if ( __JUDGE_GOAL__ (x,y) || __JUDGE_GOAL__(x,y-1) )
            return true;

        return false;
    }
    else
    {
        return false;
    }
}
static _Bool judgeColumnNodeGoal(maze_node *maze, uint8_t x, uint8_t y)
{
    //重みが0かどうか
    if(maze->ColumnNode[x][y].weight == 0)
    {
        //ゴールノードであるかどうか:マクロ作った

        if ( __JUDGE_GOAL__ (x,y) || __JUDGE_GOAL__(x-1,y) )
            return true;

        return false;
    }
    else
    {
        return false;
    }
}

void printAllWeight(maze_node *maze, position *pos)
{
    //見やすい形に成型して表示する
    //全出力を3桁にそろえればよさそう
    //重みが0かつゴールエリア内の座標なら赤色で出力 31;1m
    //行から表示して、
    //列を表示
    //交互に
    printf("全ノードの重み\r\n");

    for(int y=NUMBER_OF_SQUARES_Y; y > 0; y--)
    {
        //行
        printf("  +  ");
        for(int x=0; x < NUMBER_OF_SQUARES_X; x++)
        {
            if(judgeRawNodeGoal(maze, x,y) == true || ((pos->x == x) && (pos->y == y)))//辿った経路を赤で表示
            {
                printf(" \x1B[31;1m%3x\x1B[37;m ",maze->RawNode[x][y].weight);
            }
            else
            {
                printf(" %3x ",maze->RawNode[x][y].weight);
            }
            if(x < NUMBER_OF_SQUARES_X-1)
                 printf("  +  ");
        }
        printf("\r\n");

        //列
        for(int x=0; x < NUMBER_OF_SQUARES_X+1; x++)
        {
            if(judgeColumnNodeGoal(maze, x,y-1) == true || ((pos->x == x) && (pos->y == y)))
            {
                printf(" \x1B[31;1m%3x\x1B[37;m ",maze->ColumnNode[x][y-1].weight);
            }
            else
            {
                printf(" %3x ",maze->ColumnNode[x][y-1].weight);
            }
            if(x < NUMBER_OF_SQUARES_X)
                printf("     ");
        }
        printf("\r\n");
    }
    //y が0のときの行だけ表示
    printf("  +  ");
    for(int x=0; x < NUMBER_OF_SQUARES_X; x++)
    {
        printf(" %3x ",maze->RawNode[x][0].weight);
        if(x < NUMBER_OF_SQUARES_X-1)
                printf("  +  ");
    }
    printf("\r\n");


}
void printSingleNode(maze_node *mn, uint8_t x, uint8_t y)
{
    printf("行ノード %d, %d : 壁 %u, 重み %u, draw %u\r\n", x,y, mn->RawNode[x][y].existence,mn->RawNode[x][y].weight,mn->RawNode[x][y].draw);
    printf("列ノード %d, %d : 壁 %u, 重み %u, draw %u\r\n", x,y, mn->ColumnNode[x][y].existence,mn->ColumnNode[x][y].weight,mn->ColumnNode[x][y].draw);
}

void printAllNode(maze_node *mn)
{
    printf("全ノード\r\n");
    //間違ってるかも
    //MATLABで保存するときと同じパターンで出力する
    //Raw
    //Column
    //Rawを1列出力し、改行せずColumnの1列出力。
    //行を増やして同じ処理
    for(int i=0; i < NUMBER_OF_SQUARES_X; i++)
    {
        for(int j=1; j < NUMBER_OF_SQUARES_Y+1; j++)
        {
            printf("%u,",mn->RawNode[i][j].draw);
        }
        for(int j=0; j < NUMBER_OF_SQUARES_Y; j++)
        {
            printf("%u",mn->ColumnNode[i+1][j].draw);
            if(j < NUMBER_OF_SQUARES_Y-1)
                printf(",");
        }
        printf("\r\n");
    }
    printf("\r\n");
}
void printAllNodeExistence(maze_node *mn)
{
    printf("全ノードの壁の存在\r\n");
    //間違ってるかも
    //MATLABで保存するときと同じパターンで出力する
    //Raw
    //Column
    //Rawを1列出力し、改行せずColumnの1列出力。
    //行を増やして同じ処理
    for(int i=0; i < NUMBER_OF_SQUARES_X; i++)
    {
        for(int j=1; j < NUMBER_OF_SQUARES_Y+1; j++)
        {
            printf("%u,",mn->RawNode[i][j].existence);
        }
        for(int j=0; j < NUMBER_OF_SQUARES_Y; j++)
        {
            printf("%u",mn->ColumnNode[i+1][j].existence);
            if(j < NUMBER_OF_SQUARES_Y-1)
                printf(",");
        }
        printf("\r\n");
    }
    printf("\r\n");
}

_Bool outputDataToFile(maze_node *maze, char *name)
{
    char weight_file[20]={0};
    strcpy_s(&weight_file[0], 20,name);
    FILE*fp;
    fp = fopen(weight_file,"w");
    if(fp == NULL) {
		printf("%s file not open!\n", weight_file);
		return false;
	} else {
		printf("%s file opened!\n", weight_file);
	}
    
    for(int y=NUMBER_OF_SQUARES_Y; y > 0; y--)
    {
        //行
        fprintf(fp,"     ");
        for(int x=0; x < NUMBER_OF_SQUARES_X; x++)
        {
            if(judgeRawNodeGoal(maze, x,y) == true)
            {
                fprintf(fp," GGG ");
            }
            else
            {
                fprintf(fp," %3x ",maze->RawNode[x][y].weight);
            }
            if(x < NUMBER_OF_SQUARES_X-1)
                fprintf(fp,"     ");
        }
        fprintf(fp,"\r\n");
                
        //列
        for(int x=0; x < NUMBER_OF_SQUARES_X+1; x++)
        {
            if(judgeColumnNodeGoal(maze, x,y-1) == true)
            {
                fprintf(fp," GGG ");
            }
            else
            {
                fprintf(fp," %3x ",maze->ColumnNode[x][y-1].weight);
            }
            if(x < NUMBER_OF_SQUARES_X)
                fprintf(fp,"     ");
        }
        fprintf(fp,"\r\n");
    }
    //y が0のときの行だけ表示
    fprintf(fp,"     ");
    for(int x=0; x < NUMBER_OF_SQUARES_X; x++)
    {
        fprintf(fp," %3x ",maze->RawNode[x][0].weight);
        if(x < NUMBER_OF_SQUARES_X-1)
                fprintf(fp,"     ");
    }
    fprintf(fp,"\r\n");
    fclose(fp);
    return true;

}
static uint8_t convertNodeTo16Value(maze_node *maze, int x, int y)
{
    //xy座標を入力
    //各マスの16進数を出力。
    uint8_t val=0;
    val += 1 * maze->RawNode[x][y+1].draw;      //北
    val += 2 * maze->ColumnNode[x+1][y].draw;   //東
    val += 4 * maze->RawNode[x][y].draw;        //南
    val += 8 * maze->ColumnNode[x][y].draw;     //西
    return val;
}
//機体からTeraTermで出力するための関数
void printMatrix16ValueFromNode(maze_node *maze)
{
    printf("機体からTeraTermへの出力用\r\n");
    for(int j=NUMBER_OF_SQUARES_Y-1; j >= 0; j--)
    {
        for(int i=0; i < NUMBER_OF_SQUARES_X; i++)
        {
            printf("%u",convertNodeTo16Value(maze, i,j));
            if(i < NUMBER_OF_SQUARES_X-1)
                printf(",");
        }
        printf("\r\n");
    }
    printf("\r\n");

}

/* ノードの中身確認 */