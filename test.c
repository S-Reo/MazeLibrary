/* 自身のヘッダ */
#include "test.h"

/* ライブラリ */
#include <stdio.h>
#include <stdbool.h>
#include <stdint-gcc.h>

/* 自作ヘッダ */
#include "MazeLib.h"
#include "Searching.h"
#include "FastRun.h"
#include "dfs.h"


/* シミュレーション時のみ有効化 */
#if SIMULATION == true
    #include "MazeSimulation.h"
    simulation test;
#endif
// _Bool runTask(simulation_task task);
/* メイン */

/* タスクに必要な処理 */

/* 仮想迷路の取得 */
static _Bool getVirtualMaze(){
    
    initMaze(&(test.virtual_maze));
    initWeight(&(test.virtual_maze));
        
            #if DEBUG_ON
                    printMatrix16ValueFromNode(&(test.virtual_maze));//OK
            #endif

    if(getFileData(&test) == true){ //迷路の取得
            printf("ファイル読み込みに成功しました\r\n");
    }
    else{
            printf("ファイル読み込みに失敗しました\r\n");
            return false;
    }
    //参照用の仮想迷路データに変換
    getNodeFrom16Value_Simulation(&test);
    // マップサイズの確認が必要
           
            #if DEBUG_ON == 0
                printf("仮想迷路の");
                printAllNode(&(test.virtual_maze));
                printf("仮想迷路の");
                printMatrix16ValueFromNode(&(test.virtual_maze));
                position goal_pos = {GOAL_X, GOAL_Y};
                position goal_size = {GOAL_SIZE_X, GOAL_SIZE_Y};
                updateAllNodeWeight(&(test.virtual_maze),&goal_pos, &goal_size,0x03); //最短経路用の初期化
                printf("仮想迷路の");
                
            #endif
        position a ={1,1};
        // printAllWeight(&(test.virtual_maze),&a);
        updateAllNodeWeight(&(test.virtual_maze), &goal_pos, &goal_size, 0x03);
        char filename[] = "weight_virtual.txt";
        outputDataToFile(&(test.virtual_maze), &filename[0]);
    return true;
}

    
/* タスク群 */
void searchAdachi_and_DFS_Core(maze_node *maze, profile *mouse, int mask){ // 深さ優先探索のアルゴリズムのコア
    int WALL_MASK = 0x01; // 
    // 手前に初期化、加速の処理を挟んだ探索関数でSimulation
    //!IS_GOAL(mouse->goal_lesser.x, mouse->goal_lesser.y, mouse->goal_larger.x, mouse->goal_larger.y,mouse->next.pos.x, mouse->next.pos.y)
    position start_pos = {0,0};
    int walk_count = 1;
    // LowStackFlag();
    HighStackFlag();
    LowDFSFlag();
    while(1){ //ゴールエリアでない間
        recordSearchHistory(mouse->now.pos, mouse->target_pos);
        mouse->next.node = getNextNode(maze, mouse->now.car, mouse->now.node, mask); // 次のノードを選択
        getNextState(&(mouse->now), &(mouse->next), mouse->next.node); // 現ノードと次ノードをもとに、次の状態を取得（更新はしない）
        switch (mouse->now.dir)
        {
        case back:
        case backleft:
        case backright:
            FindUnwantedSquares(maze);
            break;
        
        default:
            break;
        }
        // printState(&(mouse->now));
        // printState(&(mouse->next));
        // 次の方向dirを使ってアクションを呼び出す
        // アクションの終盤で壁の取得
        // シミュレーションではアクションのコマンドを発行
            // 付随して、既知区間加速かどうかを知らせるフラグを操作
        

        /* 実環境ではActionで行う処理 */
        shiftState(mouse); //区画進入直前なので、更新予定の方角と座標がNextに入っている
        VisitedMass(mouse->now.pos); //訪問したマスを訪問済み配列に登録
        walk_count++;
        // printf("歩数: %d\r\n", walk_count);
        // printAllWeight(maze, &(mouse->target_pos));
        // printVisited();
        if(mouse->now.pos.x == start_pos.x && mouse->now.pos.y == start_pos.y)
            break;
        #if 1 //センサ値の更新
            wall_state wall[4]={0};
            convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]); //前右左
            getWallNow(&(mouse->now), &wall[0]);
        #else
        // この関数をアクションの終盤で呼ぶ
        //getWallState(&mouse,&Photo); //4方角の壁の有無を取得. リアル走行用.（Nextにすればシンプルになるのでは？） ハードウェア依存 or シミュレーション用データ
        #endif
        updateNodeThree(maze, &(mouse->now.wall), mouse->now.pos.x, mouse->now.pos.y); // ノードに反映
        // ゴールマス（ひとマス）に到達したら（進入）
        // if(IS_GOAL(mouse->now.pos.x, mouse->now.pos.y)){
        //     break;
        // }
        // printf("x:%u, y:%u\r\n", mouse->target_pos.x, mouse->target_pos.y);
        if(IS_GOAL(mouse->now.pos.x, mouse->now.pos.y) == true) {
            HighDFSFlag();
            // HighStackFlag();
            
        }
        // _sleep(250);
        if( (GetStackFlag() == true) ) {//ComparePosition(&(mouse->target_pos), &(mouse->now.pos)) || ComparePosition(&(mouse->target_pos), &(start_pos)) ){//帰ってくるときも一応スタックチェック
            // HighStackFlag();
            position target_size = {1,1};
            mouse->target_size = target_size;
            int n = GetStackNum();
            if(!(mouse->now.pos.x == mouse->target_pos.x && mouse->now.pos.y == mouse->target_pos.y) ){ //取り出したスタックに到達していなければ
                n++;
                SetStackNum(n);
            }
            _Bool stacked_one_or_more = StackMass(maze, &(mouse->now)); //何も積んでいないかどうかの情報が必要
            // if(stacked_one_or_more == 0) printf("スタックが無い\r\n");//ChangeLED(7);
            // else printf("スタックが何かしらある\r\n");//ChangeLED(0);

            n = GetStackNum();
            
            // _sleep(500);
            //0なら
            if(GetDFSFlag() == true){
                
                if(n == 0){
                    WALL_MASK = 0x01;
                    mouse->target_pos = GetStackMass(); //カウントは減らさない n = 0のまま
                    SetStackNum(n);
                }//0以外なら通常通り
                else{
                    WALL_MASK = 0x01;
                    position pos;
                    _Bool is_first = false;
                    while( 1 ){
                        pos = GetStackMass();
                        is_first = GetVisited(&(pos)); //0なら未訪問
                        if(n == 0){
                            mouse->target_pos = pos;
                            // printf("未訪問\r\n"); //コード読む気が失せる。何やってるかわからない
                            //ChangeLED(7);
                            break;
                        }
                        else if(is_first == false){

                            mouse->target_pos =pos;
                            --n;
                            SetStackNum(n);
                            break;
                        } //0,0座標にぶつかったら、trueなので次に行ってしまう. 0なら別ルート
                        else if(is_first == true){
                            --n;
                            SetStackNum(n); //0になったら
                        }
                        //訪問済みであれば更に下を読む
                    }
                }
            }

        }//到達していなければ、そのまま最短でtarget.posに向かう
    
        //壁の存在を基に重みマップを更新
        updateAllNodeWeight(maze, &(mouse->target_pos), &(mouse->target_size), WALL_MASK);
    }

    printf("帰ってきました\r\n全探索して帰ってくるまでにかかった歩数は、%d\r\n", walk_count);
}
static _Bool searchAdachi_and_DFS(){
    // 1. 仮想迷路の取得
    // initMaze(&(test.virtual_maze));
    // initWeight(&(test.virtual_maze));
    if(getVirtualMaze() == false){
        printf("In searchAdachi_and_DFS(), Maze Loading Error!\r\n");
        return false;
    }

    // 2. 自分のデータの初期化
    maze_node maze;
    profile mouse;
    initSearchData(&maze, &mouse); // maskを可変にしたい
    printVisited();
    // printState(&(mouse.now));

    // 3. スタート（次の姿勢はわかっている）
    #if 1 //センサ値の更新
        wall_state wall[4]={0};
        convert16ValueToWallDirection_Simulation(&test, &(mouse.next), &wall[0]); //前右左
        getWallNow(&(mouse.next), &wall[0]);
    #else
        // この関数をアクションの終盤で呼ぶ
        //getWallState(&mouse,&Photo); //4方角の壁の有無を取得. リアル走行用.（Nextにすればシンプルになるのでは？） ハードウェア依存 or シミュレーション用データ
    #endif
    // アクション内で呼ぶ処理
    shiftState(&mouse); //区画進入直前なので、更新予定の方角と座標がNextに入っている
    VisitedMass((mouse.now.pos)); //訪問したマスを訪問済み配列に登録
    updateNodeThree(&maze, &(mouse.now.wall), mouse.now.pos.x, mouse.now.pos.y); // ノードに反映
    _Bool stacked_one_or_more = StackMass(&maze, &(mouse.now));
    updateAllNodeWeight(&maze, &(mouse.target_pos), &(mouse.target_size), 0x01);
    // printVisited();
    // printState(&(mouse.now));
    
    // 4. 探索のコア
    searchAdachi_and_DFS_Core(&maze, &mouse, 0x01);

    // 5. 終了
    printf("Finish searchAdachi_and_DFS.\r\n");

    // printAllWeight(&maze, &(mouse.target_pos));
    printVisited();
    position goal = {GOAL_X, GOAL_Y};
    position goal_size = {GOAL_SIZE_X, GOAL_SIZE_Y};
    updateAllNodeWeight(&maze, &goal, &goal_size, 0x03);
    char filename[] = "weight.txt";
    outputDataToFile(&maze, &filename[0]);
    outputSearchHistory();
    printMatrix16ValueFromNode(&maze);
#if 0
    initProfile(&mouse, &maze);
    updateAllNodeWeight(&maze, &(mouse.target_pos), &(mouse.target_size), 0x03);
    
    getPathNode(&(maze), &mouse);
    // return false;
    // 3. 状態配列をもとに、最短走行のためのアクションを生成
	// getPathAction();
	getPathActionDiagonal();
    printPathAction();
    
    // 4. 生成結果を確認
    printf("パスアクション配列\r\n");
	printPathAction();
#endif
    return true;

}

void Fastest_Run_Core(maze_node *maze, profile *mouse, state *route_log)
{
    updateAllNodeWeight(maze,&(mouse->target_pos), &(mouse->target_size),0x03); //壁情報の入った迷路を取得
   
    initProfile(mouse, maze);
    //理想は、ゴールまで重み更新なしで、コマンドによるモータ制御のみ
    //シミュレーションの1stステップとしては、重み更新無しでノード選択しながら、stateの更新だけする
    //最初の加速コマンド
    int cnt=0;
    while(! ((GOAL_X <= mouse->next.pos.x && mouse->next.pos.x < (GOAL_X+GOAL_SIZE_X) ) && (GOAL_Y <= mouse->next.pos.y && mouse->next.pos.y < (GOAL_Y+GOAL_SIZE_Y))))
    {
        shiftState(mouse);
        updateAllNodeWeight(maze, &(mouse->target_pos), &(mouse->target_size), 0x03);
        mouse->now.node = getNodeInfo(maze,mouse->now.pos.x,mouse->now.pos.y, mouse->now.car);
        //選んだノードと、迷路上のノードの、アドレスが一致していればOK. 
        char r[]="行";
        char c[]="列";
        printf("現ノード    重み:%x\r\n            %s x:%u, y:%u\r\n            侵入方角:%d, x:%d, y:%d\r\n",mouse->now.node->weight, (mouse->now.node->rc == 1) ? c:r, mouse->now.node->pos.x, mouse->now.node->pos.y, mouse->now.car, mouse->now.pos.x,mouse->now.pos.y);
        //updateAllNodeWeight(&maze, mouse->goal_lesser.x, mouse->goal_lesser.y, GOAL_SIZE_X, GOAL_SIZE_Y, 0x01);
        mouse->next.node = getNextNode(maze, mouse->now.car, mouse->now.node, 0x03);//これらの引数のどれかがいけない. 迷路、方角、ノードポインタ. 一発目の、ノードの重みがfffなのはなぜ？
        //char rcnext[2] = (mouse->next.node->rc == 1) ? "列" : "行";
        printf("次ノード    重み:%x\r\n            %s x:%u, y:%u\r\n            ", mouse->next.node->weight, (mouse->next.node->rc == 1) ? c:r , mouse->next.node->pos.x, mouse->next.node->pos.y);
        
        getNextState(&(mouse->now),&(mouse->next),mouse->next.node);
        printf("侵入方角:%d, x:%d, y:%d\r\n\r\n",mouse->next.car, mouse->next.pos.x,mouse->next.pos.y);
        //デバッグ用
        //route_log[cnt].node = mouse->now.node;
        //printf("あ: %p\r\n", route_log[cnt].node);
        getRouteFastRun( route_log, &(mouse->now), cnt);
        cnt++;
        // if(cnt == 5) break;
    }
    printAllWeight(maze, &(mouse->now.pos));
    char filename[] = "weight.txt";
    outputDataToFile(maze, &filename[0]);
    printf("最短走行終了: かかった歩数: %d, スタートノードの重み: %d\r\n",cnt, maze->RawNode[0][1].weight);
    #if 0
       //全面探索
//       // //最短走行
//    state route_log[100]={0}; //要素。メモリが足りてない?
//    printf("最短走行: 確保済みログデータサイズ: %ld\r\n", sizeof(route_log));
//    //これやると、バグる. ログ用の配列の、ノード用のポインタにスタートノードのアドレスを入れただけ. ログ側のポインタ変数はいじっていない➡ //initState(&route_log[0], 6, &(maze.RawNode[0][1]));

//    Fastest_Run(&maze,&mouse, &route_log[0]);

//        printRoute(&route_log[0], 100);
#define LOG_SIZE 300
   state route_log[LOG_SIZE]={0}; //要素。メモリが足りてない?
   printf("最短走行: 確保済みログデータサイズ: %ld\r\n", sizeof(route_log));

   Fastest_Run(&test.virtual_maze,&mouse, &route_log[0]);

       printRoute(&route_log[0], LOG_SIZE);
#endif
}

static _Bool TestActionGeneration(){ //アクション生成のテスト
    
    profile mouse;
    // maze_node maze;
    initSearchData(&(test.virtual_maze), &mouse);
    // 1. 仮想迷路の準備（MATLABで生成したもの）
    if(getVirtualMaze() == false){
        printf("In TestActionGeneration task, Map Loading Error!\r\n");
        return false;
    }
    updateAllNodeWeight(&(test.virtual_maze),&(mouse.target_pos), &(mouse.target_size),0x03);
    
    // 2. 仮想マップをデータ上で最短走行し、状態配列を取得
    // updateAllNodeWeight(&maze,&(mouse.target_pos), &(mouse.target_size),0x03);
    
	getPathNode(&(test.virtual_maze), &mouse);
    // return false;
    // 3. 状態配列をもとに、最短走行のためのアクションを生成
	// getPathAction();
	getPathActionDiagonal();
    printPathAction();
    
    // 4. 生成結果を確認
    printf("パスアクション配列\r\n");
	printPathAction();

    printf("Finish TestActionGeneration task.\r\n");
    return true;
}
/* タスク群 */

/* BOSSモジュール */
static _Bool runTask(simulation_task task){
    
    switch (task)
    {
    case TASK_ADACHI_AND_DFS:
        return searchAdachi_and_DFS();
        break;
    case TASK_TEST_ACTION_GENERATION:
        return TestActionGeneration();
    case TASK_FASTEST_RUN:
        // Fastest_Run();
        break;
    default:
        break;
    }
}
int main()
{
    //外部から迷路をインポートして走らせる
    if(runTask(TASK_ADACHI_AND_DFS) == false)
        printf("タスク遂行に失敗しました\r\n");
    
    else
        printf("タスクを完了しました\r\n");

    return 0;
}
