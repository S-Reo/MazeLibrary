#if SIMULATION
//全面探索のシミュレーション
    #if 1
static void AutoAllSearch(maze_node *maze, profile *mouse){
    //外から迷路を取得

    initSearchData(maze, mouse); //探索用の迷路データとプロフィールを初期化
    shiftState(mouse);
    wall_state wall[4]={0};
    //配列と現在情報から、東西南北の配列を求めて渡す
    convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]); //壁の検出
    getWallNow(&(mouse->now), &wall[0]);    //前後左右のデータを現在壁情報に反映
    updateNodeThree(maze, &(mouse->now), mouse->now.pos.x, mouse->now.pos.y); //未知であれば壁の存在を書き込む
    updateAllNodeWeight(maze, mouse->goal_lesser.x, mouse->goal_lesser.y, GOAL_SIZE_X, GOAL_SIZE_Y, 0x01); //重みマップを更新

    //探索開始
    int count=0;
    while( ! IS_GOAL(mouse->goal_lesser.x, mouse->goal_lesser.y, mouse->goal_larger.x, mouse->goal_larger.y, mouse->now.pos.x, mouse->now.pos.y) )
    {
		/*（シミュレーションと実環境走行を切り替え）*/
        
        //ここから肝心のアルゴリズム : 進行方向の決定.
        mouse->now.node = getNodeInfo(maze,mouse->now.pos.x,mouse->now.pos.y, mouse->now.car);
        mouse->next.node = getNextNode(maze,mouse->now.car,mouse->now.node,0x01); //周囲ノードを見て重み最小を選択
        getNextState(&(mouse->now),&(mouse->next), mouse->next.node); //
				#if SIMULATION
				    printProfile(mouse);
				    //getNextState(&(mouse->now),&(mouse->next), mouse->next.node);
				    printf("次のノードの重み:%x, 侵入方角:%d, x:%d, y:%d, ノードxy:%u,%u\r\n\r\n",mouse->next.node->weight, mouse->next.car, mouse->next.pos.x,mouse->next.pos.y, mouse->next.node->pos.x,mouse->next.node->pos.y);
				#endif
        shiftState(mouse);
        //配列と現在情報から、東西南北の配列を求めて渡す
        convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]); //壁の検出
        getWallNow(&(mouse->now), &wall[0]);    //前後左右のデータを現在壁情報に反映
        updateNodeThree(maze, &(mouse->now), mouse->now.pos.x, mouse->now.pos.y); //未知であれば壁の存在を書き込む
        updateAllNodeWeight(maze, mouse->goal_lesser.x, mouse->goal_lesser.y, GOAL_SIZE_X, GOAL_SIZE_Y, 0x01); //重みマップを更新
    count ++;
    }
        //出来上がった迷路を出力
        printf("探索に要した歩数 : %u, スタートノードの重み : %u\r\n", count, maze->RawNode[0][1].weight);
        printf("得られた迷路\r\n");
        printAllNode((maze));
        printAllWeight((maze), &(mouse->now.pos));
        printMatrix16ValueFromNode((maze)); //自分の迷路を更新していなかった
        outputDataToFile(maze);
    printf("終了\r\n");
    
    //一回目のゴール
    //減速、停止、フラッシュ保存
    //ゴール時点の状態を表示
    
    //通れるマスと通れないマスを色分けしてprint
    //全面探索開始（終了条件に注意）
    
    // while(1)
    // {
    // 	//
	// }
    //停止状態 → マップだけ更新
    updateAllNodeWeight(maze, 0,0, 1,1, 0x03);
    mouse->next.node = getNextNode(maze,mouse->now.car,mouse->now.node,0x03); //周囲ノードを見て重み最小を選択
    getNextState(&(mouse->now),&(mouse->next), mouse->next.node); //
    //現在の状態と次の状態を比較し、方向転換
    
    //加速し、状態更新
    shiftState(mouse);
    //配列と現在情報から、東西南北の配列を求めて渡す
    convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]); //壁の検出
    getWallNow(&(mouse->now), &wall[0]);    //前後左右のデータを現在壁情報に反映
    updateNodeThree(maze, &(mouse->now), mouse->now.pos.x, mouse->now.pos.y); //未知であれば壁の存在を書き込む
    updateAllNodeWeight(maze, 0,0, 1,1, 0x03); //重みマップを更新

    //スタートへ（最短経路）
    while(! IS_GOAL(0,0,0,0, mouse->now.pos.x, mouse->now.pos.y)){
        mouse->now.node = getNodeInfo(maze,mouse->now.pos.x,mouse->now.pos.y, mouse->now.car);
        mouse->next.node = getNextNode(maze,mouse->now.car,mouse->now.node,0x03); //周囲ノードを見て重み最小を選択
        getNextState(&(mouse->now),&(mouse->next), mouse->next.node); //
				#if SIMULATION
				    printProfile(mouse);
				    //getNextState(&(mouse->now),&(mouse->next), mouse->next.node);
				    printf("次のノードの重み:%x, 侵入方角:%d, x:%d, y:%d, ノードxy:%u,%u\r\n\r\n",mouse->next.node->weight, mouse->next.car, mouse->next.pos.x,mouse->next.pos.y, mouse->next.node->pos.x,mouse->next.node->pos.y);
				#endif
        shiftState(mouse);
        //配列と現在情報から、東西南北の配列を求めて渡す
        convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]); //壁の検出
        getWallNow(&(mouse->now), &wall[0]);    //前後左右のデータを現在壁情報に反映
        updateNodeThree(maze, &(mouse->now), mouse->now.pos.x, mouse->now.pos.y); //未知であれば壁の存在を書き込む
        updateAllNodeWeight(maze, 0,0, 1,1, 0x03); //重みマップを更新
    }
    //終了
//        printf("探索関数\r\n");
//    maze_node maze;
// //    Search(&maze, &mouse);
//     AutoAllSearch(&maze, &mouse);
}
    #endif

#if 1
void Search(maze_node *maze, profile *mouse){   
    initSearchData(maze, mouse);//名前良くない
//        printf("自分の迷路の初期化の確認");
//        printMatrix16ValueFromNode(maze);
//        printf("%ld\r\n",sizeof(*maze));
//        printAllNode(maze);
//        printAllWeight(maze, &(mouse->now.pos));
        //printProfile(&mouse);
    //printf("2\r\n");

     //ハードウェア処理

    //走らせる
    #if SIMULATION 
        //最初の加速
    #else 
        //実環境走行 : //最初の61.5mmの加速コマンドを発行
    #endif

//        printAllWeight(maze, &(mouse->now.pos));
//        printProfile(mouse);
//        printf("0,0の南ノードの重み（スタート）:%x\r\n",mouse->now.node->weight);
//        printf("0,0から見た北のノードの重み（スタート）:%x\r\n",mouse->next.node->weight);
        int count=0;
        while( ! IS_GOAL(mouse->goal_lesser.x, mouse->goal_lesser.y, mouse->goal_larger.x, mouse->goal_larger.y, mouse->now.pos.x, mouse->now.pos.y))
        {
            //printf("%d, %d, %d, %d, %d, %d\r\n",mouse->goal_lesser.x, mouse->goal_lesser.y, mouse->goal_larger.x, mouse->goal_larger.y, mouse->now.pos.x, mouse->now.pos.y);
            
            #if ADACHI_SEARCH
                if(mouse->next.pos.x == 0 && mouse->next.pos.y == 0)
                {
                    printf("break; 一周にかかった歩数 : %d\r\n",count);
                    break;
                }
            #endif
                shiftState(mouse);
#if DEBUG_ON
                //新しいノードに入ったことになっている
                printf("現在ノードの重み:%x\r\n",mouse->now.node->weight);
                printf("次のノードの重み:%x\r\n",mouse->next.node->weight);//nowをネクストに入れたばかりなのでnow==next
                //printf("4\r\n");
#endif
            //2. 壁の更新
                //今向いている方向に応じて、前右左をとる（後ろはかならず無し.）かならず013前右左, 3後ろ. 値は01のどちらかしかない
                wall_state wall[4]={0};
                /*（シミュレーションと実環境走行を切り替え）*/
                #if SIMULATION 
                    //配列と現在情報から、東西南北の配列を求めて渡す
                    #if DEBUG_ON
                        printf("壁の状態0 %d, %d, %d, %d\r\n", wall[0], wall[1], wall[2], wall[3]);
                        printf("チェック: %d\r\n",convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]));
                        printf("壁の状態1 %d, %d, %d, %d\r\n", wall[0], wall[1], wall[2], wall[3]);
                    #else
                        convert16ValueToWallDirection_Simulation(&test, &(mouse->now), &wall[0]);
                    #endif
                #else 
                    //実環境走行 : センサデータを持ってきて、閾値で判断したものをwallに代入

                    //Photo[]を得るためのWallDetectライブラリを使う
                    //コマンドキュー手法でない間は、センサデータを比較する処理をここで入れる

                #endif

                //コマンドキュー手法を使わないときは、以下をアクションの中で呼ぶ
                //前右左の情報を配列に入れて持ってくる
                getWallNow(&(mouse->now), &wall[0]);    //前後左右のデータを自分の現在壁情報に反映
                #if DEBUG_ON
                    printf("壁の状態2 %d, %d, %d, %d\r\n", wall[0], wall[1], wall[2], wall[3]);
                #endif
                //2. 現在壁情報を、Mazeに反映
                updateNodeThree(maze, &(mouse->now), mouse->now.pos.x, mouse->now.pos.y);
            #if SIMULATION 
                    //機体から出力するためにデータをセットする処理を呼ぶ
                    //flagじゃなくて、drawに入れる
                    //updateNodeDraw(maze, mouse->now.pos.x, mouse->now.pos.y);
            #endif

            #if DEBUG_ON
                printf("壁の状態3 %d, %d, %d, %d\r\n", wall[0], wall[1], wall[2], wall[3]);
            #endif
            //ここから肝心のアルゴリズム : 進行方向の決定
            

            #if ADACHI_SEARCH


#if SIMULATION
                                updateAllNodeWeight(maze, mouse->goal_lesser.x, mouse->goal_lesser.y, GOAL_SIZE_X, GOAL_SIZE_Y, 0x01);

                                                mouse->now.node = getNodeInfo(maze,mouse->now.pos.x,mouse->now.pos.y, mouse->now.car);
                                #if DEBUG_ON
                                                printf("現在ノードの重み:%x, 侵入方角:%d, x:%d, y:%d, ノードのxy:%u, %u, rawなら0.columnなら1:%d\r\n",mouse->now.node->weight, mouse->now.car, mouse->now.pos.x,mouse->now.pos.y, mouse->now.node->pos.x, mouse->now.node->pos.y, mouse->now.node->rc);
                                #endif
                                                //updateAllNodeWeight(maze, mouse->goal_lesser.x, mouse->goal_lesser.y, GOAL_SIZE_X, GOAL_SIZE_Y, 0x01);
                                                mouse->next.node = getNextNode(maze,mouse->now.car,mouse->now.node,0x01);
                                                getNextState(&(mouse->now),&(mouse->next), mouse->next.node);
#else
                //実環境での現状態から次状態への進行方向を得る
                //実環境での進行方向ごとの動作

                //ループの先頭で状態の更新
#endif
                //ノードではマップ上のノードのアドレスを見ているが、自分の状態を示すのには値を使用している
                //次のノードを決定
                //最大3つのノードを比較。行けるところがなければUターン
                //現在ノードから行けるノードのうち、もっとも重みが小さいノードのx,yを返す
                //現ノードと次ノードの情報から、進行方向を決定

                //斜めを考えるのはここ.
                //細かい動作生成のための情報は抜きにして、4方角だけを考えると、方角と座標さえ更新できればいい.
                //現在ノードから

                //コマンドの発行まで
            
            #endif


            #if DEPTH_SEARCH

                //深さ優先探索
                printf("深さ優先探索");

            #endif

            #if WIDTH_SEARCH
                //幅優先探索 
            #endif
        
        #if DEBUG_ON
            printf("x,%u, y,%u\r\n",mouse->goal_lesser.x, mouse->goal_lesser.y);
            printf("アップデート完了\r\n");
            printf("8\r\n");
        #endif
        
        
        #if SIMULATION
            printProfile(mouse);
            //getNextState(&(mouse->now),&(mouse->next), mouse->next.node);
            printf("次のノードの重み:%x, 侵入方角:%d, x:%d, y:%d, ノードxy:%u,%u\r\n\r\n",mouse->next.node->weight, mouse->next.car, mouse->next.pos.x,mouse->next.pos.y, mouse->next.node->pos.x,mouse->next.node->pos.y);            
        #endif
        count ++;
        }

    #if SIMULATION 
        //出来上がった迷路を出力する
        //
        printf("探索に要した歩数 : %u, スタートノードの重み : %u\r\n", count, maze->RawNode[0][1].weight);
        printf("得られた迷路\r\n");
        printAllNode((maze));
        printAllWeight((maze), &(mouse->now.pos));
        //printf("9\r\n");
        printMatrix16ValueFromNode((maze)); //自分の迷路を更新していなかった
        outputDataToFile(maze);
        //printf("10\r\n");
    #else 
        //実環境走行 : //最初の61.5mmの加速コマンドを発行
        //フラッシュ
        //合図

        //待機
        //迷路出力
        printMatrix16ValueFromNode(maze);
    #endif
    printf("終了\r\n");
    //break;
    //return true;
}
#endif