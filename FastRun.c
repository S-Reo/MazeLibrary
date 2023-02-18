#include "FastRun.h"

#include "MazeLib.h"
// マップから、通るノードを決めて配列へ
// 通るノードの配列から、アクションコマンドを生成
// 生成したアクションコマンドを確認

int Num_Nodes = 0;


Path FastPath[16*16]={0};
void printPathAction(){
	for(int i=0; i < 100; i++)
		printf("%d : %d\r\n",i ,FastPath[i].path_action);
}
//最短走行用の経路配列作成 hogehoge
void getPathNode(maze_node *maze, profile *mouse)
{
	//ノード情報は既にある前提
	for(int i=0; i < 16*16; i++)
		FastPath[i].path_ahead = false;

	static int path_num=0;
	//仮想マップを参照して最短経路を走行（0x03マスクによる重みマップを進む）し、その経路を状態配列とする関数
	getNowWallVirtual(maze, mouse, mouse->now.pos.x, mouse->now.pos.y); //仮想壁を現在のマスの壁情報に入れる
	
    getNextWallVirtual(maze, mouse, mouse->next.pos.x, mouse->next.pos.y);
	FastPath[path_num].path_state = mouse->now;
	FastPath[path_num].path_ahead = true;
		printState(&(mouse->now));
	shiftState(mouse);
//		printState(&(my_mouse.next));
	//一度データ上で最短走行する
	//ゴールなら減速.　なのでwhile文
    printf("ここまでOK\r\n");
	while(! ( ((mouse->target_pos.x <= mouse->now.pos.x) && (mouse->now.pos.x < mouse->target_pos.x+mouse->target_size.x) ) && ((mouse->target_pos.y <= mouse->now.pos.y) && (mouse->now.pos.y < (mouse->target_pos.y+mouse->target_size.y)) ))  ) //nextがゴール到達するまでループ
	{
        // printState(&(mouse->now));
		//0,1。前方。
//		getNowWallVirtual(my_mouse.now.pos.x, my_mouse.now.pos.y);
		mouse->next.node = getNextNode(maze, mouse->now.car, mouse->now.node, 0x03);
		getNextState(&(mouse->now),&(mouse->next), mouse->next.node);
		getNextWallVirtual(maze, mouse, mouse->next.pos.x, mouse->next.pos.y);
			// printf("now\r\n");
			// printState(&(mouse->now));
		path_num ++;
		//次の方向はこの時点で入れる.nextstateがわかった時点で入れたい
		FastPath[path_num].path_state = mouse->now; //next.dir
		shiftState(mouse);
	}
    printf("完了?\r\n");
	path_num ++;
	FastPath[path_num].path_state = mouse->next;
    // printState(&(mouse->next));
    // printState(&(FastPath[path_num+1].path_state));
	Num_Nodes = path_num;

}
#define _CHECK_AT_GOAL_(n) ( (GOAL_X <= FastPath[n].path_state.pos.x) &&  (FastPath[n].path_state.pos.x < (GOAL_X +GOAL_SIZE_X) ) && ( GOAL_Y <= FastPath[n].path_state.pos.y) &&  (FastPath[n].path_state.pos.y < (GOAL_Y +GOAL_SIZE_Y))  )

// #define _EQUAL_CHECK_RC_(n1,n2) (FastPath[focus+n1].path_state.node->rc == FastPath[focus+n2].path_state.node->rc)
static _Bool judgeEqualRC(Path *focus_path, Path *ahead_path){

    if(focus_path->path_state.node->rc == ahead_path->path_state.node->rc)
        return true;
    else
        return false;
    
}
// 2つのノードの位置関係. スタートノードにおける方角に応じてxyの判定を変える
// 東西南北: 0個目と4個目の位置関係
static _Bool judgeTurn180_L(node *focus_nd, node *ahead_nd, cardinal focus_car){

    position focus_pos = focus_nd->pos;
    position ahead_pos = ahead_nd->pos;
    switch(focus_car){
        case north:
            if(focus_pos.y == ahead_pos.y)
                if (focus_pos.x == (ahead_pos.x+1) )
                    return true;
            break;
        case south:
            if(focus_pos.y == ahead_pos.y)
                if ( (focus_pos.x+1) == ahead_pos.x )
                    return true;
            break;          
        case east:
            if(focus_pos.x == ahead_pos.x)
                if ( (focus_pos.y+1) == ahead_pos.y )
                    return true;
            break;
        case west:
            if(focus_pos.x == ahead_pos.x)
                if ( focus_pos.y == (ahead_pos.y+1) )
                    return true;
            break;
        default :
            break;
    }
    return false;
}
static _Bool judgeTurn180_R(node *focus_nd, node *ahead_nd, cardinal focus_car){

    position focus_pos = focus_nd->pos;
    position ahead_pos = ahead_nd->pos;
    switch(focus_car){
        case north:
            if(focus_pos.y == ahead_pos.y)
                if ( (focus_pos.x+1) == ahead_pos.x )
                    return true;
            break;
        case south:
            if(focus_pos.y == ahead_pos.y)
                if ( focus_pos.x == (ahead_pos.x+1) )
                    return true;        
            break;                                
        case east:
            if(focus_pos.x == ahead_pos.x)
                if ( focus_pos.y == (ahead_pos.y+1) )
                    return true;
            break;
        case west:
            if(focus_pos.x == ahead_pos.x)
                if ( (focus_pos.y+1) == ahead_pos.y )
                    return true;
            break;
        default :
            break;
    }
    return false;
}

// 東西南北: 0個目と3個目の位置関係
static _Bool judgeTurn45_L(node *focus_nd, node *ahead_nd, cardinal focus_car){
    position focus_pos = focus_nd->pos;
    position ahead_pos = ahead_nd->pos;
    
    _Bool equal_rc = (focus_nd->rc == ahead_nd->rc);
    if(equal_rc == false)
        return false;
    // 相対的条件から、絶対的位置関係での判断に変える = 0と3個先での位置関係
    switch(focus_car){
        case north:
            if( (focus_pos.y+2) == ahead_pos.y){
                if ( focus_pos.x == (ahead_pos.x+1) )
                    return true;//L
            }
            break;
        case south:
            if( focus_pos.y == (ahead_pos.y+2) ){
                if ( (focus_pos.x+1) == ahead_pos.x )
                    return true;//L
            }                                
            break;                                
        case east:
            if( (focus_pos.x+2) == ahead_pos.x ){
                if ( (focus_pos.y+1) == ahead_pos.y )
                    return true;//L
            }
            break;
        case west:
            if( focus_pos.x == (ahead_pos.x+2) ){
                if ( focus_pos.y == (ahead_pos.y+1) )
                    return true;//L
            }
            break;
        default :
            break;
    }
    return false;
}
static _Bool judgeTurn45_R(node *focus_nd, node *ahead_nd, cardinal focus_car){
    position focus_pos = focus_nd->pos;
    position ahead_pos = ahead_nd->pos;
    _Bool equal_rc = (focus_nd->rc == ahead_nd->rc);
    if(equal_rc == false)
        return false;

    switch(focus_car){
        case north:
            if( (focus_pos.y+2) == ahead_pos.y){
                if ( (focus_pos.x+1) == ahead_pos.x )
                    return true;//R
            }
            break;
        case south:
            if( focus_pos.y == (ahead_pos.y+2) ){
                if ( focus_pos.x == (ahead_pos.x+1) )
                    return true;//R
            }                                
            break;                                
        case east:
            if( (focus_pos.x+2) == ahead_pos.x ){
                if ( focus_pos.y == (ahead_pos.y+1) )
                    return true;//R
            }
            break;
        case west:
            if( focus_pos.x == (ahead_pos.x+2) ){
                if ( (focus_pos.y+1) == ahead_pos.y )
                    return true;//R
            }
            break;
        default :
            break;
    }
    return false;
}

// 斜めの直進: 0と2で決められる
static _Bool judgeACC_DEC_DIAGONAL(node *focus_nd, node *ahead_nd, cardinal focus_car){

    position focus_pos = focus_nd->pos;
    position ahead_pos = ahead_nd->pos;

    switch(focus_car){
        case ne:
            if(focus_pos.x+1 == ahead_pos.x && focus_pos.y+1 == ahead_pos.y)
                return true;
            break;
        case se:
            if(focus_pos.x+1 == ahead_pos.x && focus_pos.y == ahead_pos.y+1)
                return true;
            break;                                
        case sw:
            if(focus_pos.x == ahead_pos.x+1 && focus_pos.y == ahead_pos.y+1)
                return true;
            break;
        case nw:
            if(focus_pos.x == ahead_pos.x+1 && focus_pos.y+1 == ahead_pos.y)
                return true;
            break;
        default :
            break;
    }
    return false;
}

// ゴール時のフィニッシュを決める動作パターンを生成

int getFinishActionStraight90(cardinal car, Path *focus_path){
    int straight_num = 0;
    switch (car)
    {
    case east:
    case west:
        for(int i=0; i < GOAL_SIZE_X; i++){
            focus_path->path_action = ACC_DEC_90;
            focus_path++;
            straight_num++;
            // printf("%d,%p,%d\r\n", focus_path->path_action, focus_path, straight_num);

        }
        break;
    case north:
    case south:
        for(int i=0; i < GOAL_SIZE_Y; i++){
            focus_path->path_action = ACC_DEC_90;
            focus_path++;
            straight_num++;
        }
        break;
    
    default:
        printf("tag b error.\r\n");
        break;
    }
    return straight_num;
}

cardinal shiftCardinalByTurn(cardinal focus_car, Action action){

    switch (action)
    {
    case START:
    case ACC_DEC_90:
    case ACC_DEC_45:
        return focus_car;
    case L_45_FAST:
    case L_45_FAST_REVERSE:
        focus_car = focus_car-1;
        return focus_car%8;
    case R_45_FAST:
    case R_45_FAST_REVERSE:
        focus_car = (focus_car+1);
        return focus_car%8;
    case L_90_SEARCH:
    case L_90_FAST:
    case L_90_FAST_DIAGONAL:
        focus_car = (focus_car -2);
        return focus_car%8;
    case R_90_SEARCH:
    case R_90_FAST:
    case R_90_FAST_DIAGONAL:
        focus_car = focus_car+2;
        return (focus_car%8);
    case L_135_FAST:
    case L_135_FAST_REVERSE:
        focus_car = focus_car-3;
        return (focus_car%8);
    case R_135_FAST:
    case R_135_FAST_REVERSE:
        focus_car = (focus_car+3);
        return (focus_car%8);
    case L_180_FAST:
        focus_car = (focus_car-4);
        return (focus_car%8);
    case R_180_FAST:
        focus_car = (focus_car+4);
        return (focus_car%8);

    default:
        printf("Action matching error in shiftCardinalByTurn()!");
        return (focus_car);
    }
}

int getPathActionDiagonal(){

    int focus=0, action_num=0;

	if(_CHECK_AT_GOAL_(0)) {
        //動かない
    }
    else if(_CHECK_AT_GOAL_(1)){
        FastPath[action_num].path_action = ACC_DEC_90;//加減速一回で終わり
        action_num++;
        //61.5+45mm
    }
    else {
        //それ以外は同じアルゴリズムで回していく
		if(FastPath[2].path_state.node->rc == 1){
			FastPath[action_num].path_action = START;	 //初手ターン用の加速
			// FastPath[1].path_action = R_90_SEARCH;
            focus = 0;
            action_num++;
		}
		else{
			//1マスとちょっと直進
			FastPath[action_num].path_action = ACC_DEC_90; //90+(61.5-45)
			// FastPath[1].path_action = ACC_DEC;
            focus = 1;
            action_num++;
		}
        cardinal car = north;

        while(1){ 
            
            // printf("action_num : %d, action: %d, car: %d\r\n", action_num-1, FastPath[action_num-1].path_action, car);
            // printf("%d, %u, %u\r\n", focus,FastPath[focus].path_state.pos.x, FastPath[focus].path_state.pos.y);
            //4方角のとき
            if(car == north || car == south || car == east || car == west){
                // printf("東西南北\r\n");
                if(judgeEqualRC(&FastPath[focus], &FastPath[focus+2]) == true){
                    //行から行または列から列
                    //actonに直線90を追加し、action_numを1進める
                    FastPath[action_num].path_action = ACC_DEC_90;
                    action_num++;
                    if(_CHECK_AT_GOAL_(focus+2)){
                        // 2個先がゴールなら
                        action_num += getFinishActionStraight90(car, &FastPath[action_num]); // ゴールサイズの数だけ90mm加減速. 中点でトップスピード.終端速度0、 tag b
                        break;
                    }
                    else{
                        printState(&(FastPath[focus].path_state)); //focus 12, 列の5,3にいて、直進を選んでいるのはなぜ？
                        // printState(&(FastPath[focus+1].path_state));
                        focus += 1;
                        continue;
                    }
                }
                else { //行から列または列から行
                // 2つ先、3つ先、4つ先、の順に確認
                    if(_CHECK_AT_GOAL_(focus+2)){
                        // 90度ターン LかRかジャッジ. 1と2
                        position plus1 = FastPath[focus+1].path_state.node->pos;
                        position plus2 = FastPath[focus+2].path_state.node->pos;
                        switch (car)
                        {
                        case north:
                            if(__RAW_TO_COLUMN_NE__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = R_90_FAST;
                            else if(__RAW_TO_COLUMN_NW__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = L_90_FAST;
                            break;
                        case south:
                            if(__RAW_TO_COLUMN_SW__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = R_90_FAST;
                            else if(__RAW_TO_COLUMN_SE__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = L_90_FAST;
                            break;
                        case east:
                            if(__COLUMN_TO_RAW_SE__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = R_90_FAST;
                            else if(__COLUMN_TO_RAW_NE__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = L_90_FAST;
                            break;
                        case west:
                            if(__COLUMN_TO_RAW_NW__(plus1.x, plus1.y, plus2.x, plus2.y) == true)
                                FastPath[action_num].path_action = R_90_FAST;
                            else if(__COLUMN_TO_RAW_SW__(plus1.x, plus1.y, plus2.x, plus2.y) == true){
                                FastPath[action_num].path_action = L_90_FAST;
                                printState(&(FastPath[focus].path_state));
                            }
                            break;
                        default:
                            break;
                        }
                        Action selected_action = FastPath[action_num].path_action;
                        car = shiftCardinalByTurn(car, selected_action);
                        break;
                    }
                    else {
                        //45か135
                        node *focus_node = FastPath[focus].path_state.node;
                        node *ahead_node = FastPath[focus+3].path_state.node;

                        // 3個先との位置関係で45度ターン判定 // 2個先を見て行列異なる、かつゴールなら90度ターン+緊急ストップ
                        if(judgeTurn45_L(focus_node, ahead_node, car) == true){
                            FastPath[action_num].path_action = L_45_FAST;
                        }
                        else if(judgeTurn45_R(focus_node, ahead_node, car) == true){
                            FastPath[action_num].path_action = R_45_FAST;
                        }
                        
                        // 90度の判定
                        else if(! (judgeEqualRC(&FastPath[focus], &FastPath[focus+3]) == true) ){
                    
                            position pre = FastPath[focus+2].path_state.node->pos;
                            position fol = FastPath[focus+3].path_state.node->pos;
                            switch (car) // focus=0の向き
                            {
                            case north:
                                if(__COLUMN_TO_COLUMN_WEST__(pre.x, pre.y, fol.x, fol.y) == true)
                                    FastPath[action_num].path_action = L_90_FAST;
                                else
                                    FastPath[action_num].path_action = R_90_FAST;
                                break;
                            case south:
                                if(__COLUMN_TO_COLUMN_WEST__(pre.x, pre.y, fol.x, fol.y) == true)
                                    FastPath[action_num].path_action = R_90_FAST;
                                else
                                    FastPath[action_num].path_action = L_90_FAST;
                                break;
                            case east:
                                if(__RAW_TO_RAW_NORTH__(pre.x, pre.y, fol.x, fol.y) == true)
                                    FastPath[action_num].path_action = L_90_FAST;
                                else
                                    FastPath[action_num].path_action = R_90_FAST;
                                break;
                            case west:
                                if(__RAW_TO_RAW_NORTH__(pre.x, pre.y, fol.x, fol.y) == true)
                                    FastPath[action_num].path_action = R_90_FAST;
                                else
                                    FastPath[action_num].path_action = L_90_FAST;
                                break;
                            default: //一応例外はない予定
                                
                                break;
                            }
                        }
                        else {
                            // 135度か180度の判定
                            if(_CHECK_AT_GOAL_(focus+3)){
                                    //0と3のxyで左右の判定
                                position focus_pos = FastPath[focus].path_state.node->pos;
                                position ahead_pos = FastPath[focus+3].path_state.node->pos;
                                switch (car)
                                {
                                case north:
                                    if(focus_pos.x < ahead_pos.x)
                                        FastPath[action_num].path_action = R_180_FAST;
                                    if(focus_pos.x > ahead_pos.x)
                                        FastPath[action_num].path_action = L_180_FAST;
                                    break;
                                case south:
                                    if(focus_pos.x < ahead_pos.x)
                                        FastPath[action_num].path_action = L_180_FAST;
                                    if(focus_pos.x > ahead_pos.x)
                                        FastPath[action_num].path_action = R_180_FAST;
                                    break;
                                case east:
                                    if(focus_pos.y < ahead_pos.y)
                                        FastPath[action_num].path_action = L_180_FAST;
                                    if(focus_pos.y > ahead_pos.y)
                                        FastPath[action_num].path_action = R_180_FAST;
                                    break;
                                case west:
                                    if(focus_pos.y < ahead_pos.y)
                                        FastPath[action_num].path_action = R_180_FAST;
                                    if(focus_pos.y > ahead_pos.y)
                                        FastPath[action_num].path_action = L_180_FAST;
                                    break;
                                
                                default:
                                    printf("方角エラー. 135度か180度の判定.\r\n");
                                    break;
                                }
                                Action selected_action = FastPath[action_num].path_action;
                                car = shiftCardinalByTurn(car, selected_action);
                                break;//while
                            }
                            else {
                                //4つ先を見る
                                // Signal(1);
                                if(judgeEqualRC(&FastPath[focus], &FastPath[focus+4]) == true){ //

                                    node *focus_node = FastPath[focus].path_state.node;
                                    node *ahead_node = FastPath[focus+4].path_state.node;

                                    if(judgeTurn180_L(focus_node, ahead_node, car) == true){
                                        FastPath[action_num].path_action = L_180_FAST;
                                    }
                                    else if(judgeTurn180_R(focus_node, ahead_node, car) == true){
                                        FastPath[action_num].path_action = R_180_FAST;
                                        // 90mm加減速で終端速度0、トップスピード真ん中にする. ゴールサイズの数だけ追加 tag b
                                    }
                                    Action selected_action = FastPath[action_num].path_action;
                                    car = shiftCardinalByTurn(car, selected_action); //180度分

                                    action_num++;
                                    if(_CHECK_AT_GOAL_(focus+4)){
                                        action_num += getFinishActionStraight90(car, &FastPath[action_num]); // ゴールサイズの数だけ90mm加減速. 中点でトップスピード.終端速度0、 tag b
                                        action_num --;
                                        // focus += 4 - 1;
                                        break;
                                    }
                                    else{
                                        focus += 4 - 1;
                                        continue;
                                    }
                                }
                                else {
                                    // 135の判定
                                    // 03が行行、列列の前提
                                    position plus_1 = FastPath[focus+1].path_state.node->pos;
                                    position plus_3 = FastPath[focus+3].path_state.node->pos;
                                    
                                    switch (car)
                                    {
                                    case north:
                                        if(plus_1.y == plus_3.y){
                                            if(plus_1.x > plus_3.x)
                                                FastPath[action_num].path_action = L_135_FAST;
                                            if(plus_1.x < plus_3.x)
                                                FastPath[action_num].path_action = R_135_FAST;
                                        }
                                        break;
                                    case south:
                                        if(plus_1.y == plus_3.y){
                                            if(plus_1.x > plus_3.x)
                                                FastPath[action_num].path_action = R_135_FAST;
                                            if(plus_1.x < plus_3.x)
                                                FastPath[action_num].path_action = L_135_FAST;
                                        }
                                        break;
                                    case east:
                                        if(plus_1.x == plus_3.x){
                                            if(plus_1.y > plus_3.y)
                                                FastPath[action_num].path_action = R_135_FAST;
                                            if(plus_1.y < plus_3.y)
                                                FastPath[action_num].path_action = L_135_FAST;
                                        }
                                        break;
                                    case west:
                                        if(plus_1.x == plus_3.x){
                                            if(plus_1.y > plus_3.y)
                                                FastPath[action_num].path_action = L_135_FAST;
                                            if(plus_1.y < plus_3.y)
                                                FastPath[action_num].path_action = R_135_FAST;
                                        }
                                        break;
                                    default:
                                        break;
                                    }
                                    Action selected_action = FastPath[action_num].path_action;
                                    car = shiftCardinalByTurn(car, selected_action);
                                    action_num++;
                                    // リバース
                                    _Bool focus_rc = FastPath[focus].path_state.node->rc;

                                    if(_CHECK_AT_GOAL_(focus+4)){
                                        switch (car)
                                        {
                                        case ne:
                                            if(focus_rc == 1)
                                                FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                            if(focus_rc == 0)
                                                FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                        case sw:
                                            if(focus_rc == 1)
                                                FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                            if(focus_rc == 0)
                                                FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                            break;
                                        case se:
                                            if(focus_rc == 1)
                                                FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                            if(focus_rc == 0)
                                                FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                        case nw:
                                            if(focus_rc == 1)
                                                FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                            if(focus_rc == 0)
                                                FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                            break;
                                        
                                        default:
                                            break;
                                        }
                                        Action selected_action = FastPath[action_num].path_action;
                                        car = shiftCardinalByTurn(car, selected_action);
                                        //focus
                                        break; //while
                                    }
                                    else{
                                        focus += 4 - 1; //どっち. 4ポイ
                                        // focus += 3 - 1;
                                        continue;
                                    }
                                    
                                    
                                }
                            }
                        }
                        
                        //90か45
                        // printf("which,car: %d, FastPath[action_num].path_action: %d\r\n", car, FastPath[action_num].path_action);
                        Action selected_action = FastPath[action_num].path_action;
                        car = shiftCardinalByTurn(car, selected_action); //左回転90度分
                        // printf("car: %d\r\n", car);
                        if(_CHECK_AT_GOAL_(focus+3)){
                            if(selected_action == L_45_FAST || selected_action == R_45_FAST){
                                break;
                            }
                            else if(selected_action == L_90_FAST || selected_action == R_90_FAST){
                                // 90mm加減速で終端速度0、トップスピード真ん中にする. ゴールサイズの数だけ追加 tag b
                                action_num++;
                                action_num += getFinishActionStraight90(car, &FastPath[action_num]); // ゴールサイズの数だけ90mm加減速追加. 中点でトップスピード.終端速度0、 tag b
                                action_num --;
                                break; //while
                            }
                        }
                        else{
                            action_num++;
                            focus += 3 - 1;
                            continue;
                        }
                    }
                    
                }
            }
            //間の方角 //135はリバース
            if(car == ne || car == se || car == sw || car == nw) {
                // printf("間の方角\r\n");
                _Bool focus_rc = FastPath[focus].path_state.node->rc;
                if( judgeEqualRC(&FastPath[focus], &FastPath[focus+2]) == true){ //2個先も行同士、列同士
                    
                    node *focus_node = FastPath[focus].path_state.node;
                    node *ahead_node = FastPath[focus+2].path_state.node;
                    
                    if( judgeACC_DEC_DIAGONAL(focus_node, ahead_node, car) == true){ //斜め直進
                        // printf("斜め直進\r\n");
                        FastPath[action_num].path_action = ACC_DEC_45;
                        action_num += 1;
                        // ゴールエリアのどのノードにどの方角で入射するかで分岐. 斜め直進に何を追加するか
                        if(_CHECK_AT_GOAL_(focus+2)){
                            // 方角に合わせて45度ターン左右どちらか
                                // 緊急ストップ（ゴール一マス目の中央でターンが終わるため）（ゴールサイズに応じて90mm追加）
                            switch (car)
                            {
                            case ne:
                                if(focus_rc == 1)
                                    FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                if(focus_rc == 0)
                                    FastPath[action_num].path_action = L_45_FAST_REVERSE;
                            case sw:
                                if(focus_rc == 1)
                                    FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                if(focus_rc == 0)
                                    FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                break;
                            case se:
                                if(focus_rc == 1)
                                    FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                if(focus_rc == 0)
                                    FastPath[action_num].path_action = R_45_FAST_REVERSE;
                            case nw:
                                if(focus_rc == 1)
                                    FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                if(focus_rc == 0)
                                    FastPath[action_num].path_action = R_45_FAST_REVERSE;
                                break;
                            
                            default:
                                break;
                            }
                            Action selected_action = FastPath[action_num].path_action;
                            car = shiftCardinalByTurn(car, selected_action);
                            //focus
                            break; //while
                        }
                        else {
                            focus += 1;
                            // printf("acc_dec_diagonal\r\n");
                            continue;
                        }
                    }
                    else {
                        //90,135左
                        //2,3で行か列か変われば90
                        // 2個先がゴールなら、ゴールエリアのサイズとノードの位置に応じて90度か135度
                        // ゴールで無いなら3個先を見て90度か135度
                        if(_CHECK_AT_GOAL_(focus+2)){
                            // 1マスなら: L135リバース+緊急停止(neの例)
                            // 2,3マスなら: L135リバース+(90mm×（ノード数-1）） (neの例)
                            switch (car){  // ゴールエリアのどのノードにどの方角で入射するかで分岐. エリアが1マスならリバースの135に変更
                                case ne:
                                case sw:
                                    if(focus_rc == 1)
                                        FastPath[action_num].path_action = L_135_FAST_REVERSE;
                                    
                                    if(focus_rc == 0)
                                        FastPath[action_num].path_action = R_135_FAST_REVERSE;
                                    
                                    break;
                                case se:
                                case nw:
                                    if(focus_rc == 1)
                                        FastPath[action_num].path_action = R_135_FAST_REVERSE;
                                    
                                    if(focus_rc == 0)
                                        FastPath[action_num].path_action = L_135_FAST_REVERSE;
                                    
                                    break;
                                default:
                                    break;
                            }
                            Action selected_action = FastPath[action_num].path_action;
                            car = shiftCardinalByTurn(car, selected_action);
                            //focus
                            break; //while
                        }
                        else { // 2個先がゴールでない
                            //90度か135度
                            _Bool equal_rc = judgeEqualRC(&FastPath[focus+2], &FastPath[focus+3]);
                            switch (car){  // ゴールエリアのどのノードにどの方角で入射するかで分岐. エリアが1マスならリバースの135に変更
                                case ne:
                                case sw:
                                    if(focus_rc == 1){
                                        if(equal_rc == true)
                                            FastPath[action_num].path_action = L_135_FAST_REVERSE;
                                        if(equal_rc == false)
                                            FastPath[action_num].path_action = L_90_FAST_DIAGONAL;
                                    }
                                    if(focus_rc == 0){
                                        if(equal_rc == true)
                                            FastPath[action_num].path_action = R_135_FAST_REVERSE;
                                        if(equal_rc == false)
                                            FastPath[action_num].path_action = R_90_FAST_DIAGONAL;
                                    }
                                    break;
                                case se:
                                case nw:
                                    if(focus_rc == 1){
                                        if(equal_rc == true)   
                                            FastPath[action_num].path_action = R_135_FAST_REVERSE;
                                        if(equal_rc == false)   
                                            FastPath[action_num].path_action = R_90_FAST_DIAGONAL;
                                    }
                                    if(focus_rc == 0){
                                        if(equal_rc == true)
                                            FastPath[action_num].path_action = L_135_FAST_REVERSE;
                                        if(equal_rc == false)
                                            FastPath[action_num].path_action = L_90_FAST_DIAGONAL;
                                    }
                                    break;
                                default:
                                    break;
                            }
                            Action selected_action = FastPath[action_num].path_action;
                            car = shiftCardinalByTurn(car, selected_action);
                            action_num += 1;

                            if(_CHECK_AT_GOAL_(focus+3)){ //3個先がゴール
                            // ゴール1マス: L90度+R45リバース（緊急停止）か、L135リバース+90mm×ノード数 : 方角分け、ゴールサイズ分け
                            // ゴール2,3マス: L90度+(斜め直進orR45リバース)か、L135+90mm×ノード数: 斜め直進要らない
                                if(selected_action == L_90_FAST_DIAGONAL){
                                    FastPath[action_num].path_action = R_45_FAST_REVERSE; //2マス以上あれば通常の減速を追加
                                    // 緊急ストップ（アクションの読み出し側で、区画中央で終わるターン（加減速以外）は緊急停止とする）（ゴールサイズに合わせて変える）
                                    // FastPath[action_num+1].path_action = ACC_DEC_90;
                                    break;
                                }
                                else if(selected_action == R_90_FAST_DIAGONAL){
                                    FastPath[action_num].path_action = L_45_FAST_REVERSE;
                                    // 緊急ストップ（上に同じ）
                                    // FastPath[action_num+1].path_action = ACC_DEC_90;
                                    break;
                                }
                                else if( (selected_action == L_135_FAST_REVERSE) || (selected_action == R_135_FAST_REVERSE) ){
                                    int add_num=0;
                                    add_num = getFinishActionStraight90(car, &FastPath[action_num]);
                                    action_num += add_num;
                                    // printf("breakのはず. %d\r\n", action_num);
                                    break;
                                }
                            }
                            else{
                                focus += 3-1;
                                continue;
                            }
                            
                        }
                    }
                }
                else { // 2個先が行列異なる
                    // printf("%d, %u, %u\r\n", focus,FastPath[focus].path_state.pos.x, FastPath[focus].path_state.pos.y);
                     //45度ターンのリバース
                    switch (car){
                        case ne:
                        case sw:
                            if(focus_rc == 1)
                                FastPath[action_num].path_action = L_45_FAST_REVERSE; //LRともにリバース
                            if(focus_rc == 0)
                                FastPath[action_num].path_action = R_45_FAST_REVERSE; //LRともにリバース
                            
                            break;
                        case se:
                        case nw:
                            if(focus_rc == 1)
                                FastPath[action_num].path_action = R_45_FAST_REVERSE;
                            if(focus_rc == 0)
                                FastPath[action_num].path_action = L_45_FAST_REVERSE;
                            break;
                        default :
                            break;
                    }
                    car = shiftCardinalByTurn(car, FastPath[action_num].path_action);
                    action_num += 1;
                    
                    // 2個先がゴールである
                    // ゴールサイズ分だけ90mm進む
                    if(_CHECK_AT_GOAL_(focus+2)){
                        action_num += getFinishActionStraight90(car, &FastPath[action_num]);
                        action_num --;
                        break; //while
                    }
                    else {
                        focus += 2-1;
                        continue;
                    }
                    
                }
                //2個先が行列同じならxy1ずつずらしたノードなら45度直進を1追加し、注目ノードを1進める. 
                //行列が異なれば45度ターン、3個先を見て行列が異なれば斜め90、xy1ずつずらしていれば135. 
                //n個先を見たら、n-1個足したノードから開始。
            }
        }
        //最後のアクションは、ゴールエリアのサイズと侵入パターンに応じて決める
        printf("アクション数: %d\r\n", action_num);
        // num_nodesではなく、action_numでジャッジ
        
    }

    return action_num;
}
//90度と直進だけの組み合わせ
void getPathAction()
{
	//Pathからアクション計画を立てる
	int count = 0;
	if( _CHECK_AT_GOAL_(count)){
		//ゴールノード
		//終端速度の変数 0
		//動いていなければ動かない
		if(count == 0){ //初手ノードなら、加減速一回で終わり
		}
		else if(count == 1){ //一歩目がゴールなら（ありえない）
			FastPath[0].path_action = ACC_DEC;//加減速一回で終わり
			//61.5+45mm
		}
	}
	else {
        
		if(FastPath[2].path_state.node->rc == 1){
			FastPath[0].path_action = START;	 //初手ターン用の加速
			FastPath[1].path_action = R_90_SEARCH;
		}
		else{
			//2マス以上直進
			FastPath[0].path_action = ACC_DEC;
			FastPath[1].path_action = ACC_DEC;
		}
		count = 2; //==2
		//以降はゴールまで同じ流れで決定
		while( !_CHECK_AT_GOAL_(count) )
		{
            if(FastPath[count].path_state.node->rc == FastPath[count+1].path_state.node->rc){
                FastPath[count].path_action = ACC_DEC;
            }
            else{
                uint8_t now_x = FastPath[count].path_state.node->pos.x, now_y = FastPath[count].path_state.node->pos.y;
                uint8_t next_x = FastPath[count+1].path_state.node->pos.x, next_y = FastPath[count+1].path_state.node->pos.y;
                //ターン. 傾きで選ぶ. マクロ
                //行から列.左右のどちらか判断.あとで後ろも候補にあげる
                if(FastPath[count].path_state.node->rc == 0)
                {
                    //右旋回
                    //北向きから北東
                    if( __RAW_TO_COLUMN_NE__(now_x, now_y, next_x, next_y) || __RAW_TO_COLUMN_SW__(now_x, now_y, next_x, next_y) )
                    {
                        FastPath[count].path_action = R_90_SEARCH;
                    }
                    //左旋回
                    //北向きから北西 or //南向きから南東
                    if( __RAW_TO_COLUMN_NW__(now_x, now_y, next_x, next_y) || __RAW_TO_COLUMN_SE__(now_x, now_y, next_x, next_y) )
                    {
                        FastPath[count].path_action = L_90_SEARCH;
                    }
                }
                else if(FastPath[count].path_state.node->rc == 1)
                {
                    //右旋回
                    //東向きから南東
                    if( __COLUMN_TO_RAW_SE__(now_x, now_y, next_x, next_y) || __COLUMN_TO_RAW_NW__(now_x, now_y, next_x, next_y) )
                    {
                        FastPath[count].path_action = R_90_SEARCH;
                    }
                    //左旋回
                    //東向きから北東 or //西向きから南西
                    if( __COLUMN_TO_RAW_NE__(now_x, now_y, next_x, next_y) || __COLUMN_TO_RAW_SW__(now_x, now_y, next_x, next_y) )
                    {
                        FastPath[count].path_action = L_90_SEARCH;
                    }
                }
            }



			// printf("%d, %d, %u, %u\r\n", count, FastPath[count].path_action, FastPath[count].path_state.pos.x,  FastPath[count].path_state.pos.y);
			count ++;
		}
	}
	//ゴールノード
	//終端速度の変数 0
	//前回がターンなら加減速を選択して、加速の割合を0として減速
	//前回までが直進なら、加減速を選択して、ゴールラインを駆け抜ける処理を入れる
		//一つ前のpath_actionによって変える
		switch(FastPath[count-1].path_action)
		{
			case START:
				//初手90°ターン用なので来ない
				break;
			case L_90_SEARCH: //LRで同じ
			case R_90_SEARCH:
				FastPath[count].path_action = ACC_DEC; //ただの減速.
				//加速の割合を0にする
				break;
			case ACC_DEC:
				//前回ACC_DECということはそのまま継続でひとまとめにする
				FastPath[count].path_action = ACC_DEC; //前のアクションとひとまとめ（countを利用）
				//ゴール通過時に速度を高くしておきたいので、通過後に壁が無ければ駆け抜ける仕様にする
				break;
			default :
				printf("missng action !! in getPathAction.\r\n");
				break;
		}
}


void getRouteFastRun(state *log_st, state *now_st, int n)
{
    // printf("呼び出し: %p, %d\r\n", now_st, now_st->node->rc);
    // printf("呼び出し: %p, %p\r\n", log_st, log_st[n].node);
    //最短走行時に通ったノードのxyとrawかcolumnかの情報を配列に格納する
    log_st[n].node = now_st->node;

    log_st[n].car = now_st->car;
    log_st[n].pos.x = now_st->pos.x;
    log_st[n].pos.y = now_st->pos.y;

    log_st[n].wall.north = now_st->wall.north;
    log_st[n].wall.east = now_st->wall.east;
    log_st[n].wall.south = now_st->wall.south;
    log_st[n].wall.west = now_st->wall.west;
}
void printRoute(state *route, int n)
{
    //ノードをチェック.
    for(int k=0; k < n; k++)
    {
        if(route[k].node->rc == 0)
        {
            printf("行ノード: ");
        }
        if(route[k].node->rc == 1)
        {
            printf("列ノード: ");
        }
        printf("x:%u, y:%u, 重み: %u\r\n", route[k].node->pos.x , route[k].node->pos.y, route[k].node->weight);
        printf("座標: x:%u, y:%u, 方角: %u\r\n", route[k].pos.x, route[k].pos.y, route[k].car); //方角はわかりやすく表示したい
        printf("[北 東 南 西] = [%u %u %u %u]\r\n", route[k].wall.north, route[k].wall.east, route[k].wall.south, route[k].wall.west);
        printf("\r\n");
    }
}
