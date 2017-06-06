#include "iodefine.h"
#include "global.h"

CAR_STATUS cs;

unsigned char motor_used_list[200];
unsigned short motor_elapsed_time_list[200];
unsigned char motor_used_index = 0;

void initCar(void){
	//初期設定
	cs.posX = 0.0f;
	cs.posY = 0.0f;
	cs.angle = 0.0f;
	
	cs.targetPosX = 0.0f;
	cs.targetPosY = 0.0f;
	
	cs.motor_mode = Motor_STOP;
	cs.pre_motor_mode = Motor_STOP;
	
	initCMT0();
	initMotorController();
	initUltrasonicController();
}

void runCar(void){
	cs.pre_motor_mode = cs.motor_mode;
	switch(runUltrasonicController()){
		case Ultra_Front:
			mcTurnLeft();
			cs.motor_mode = Motor_TURNLEFT;
			break;
		case Ultra_Right:
			mcGoStraight();
			cs.motor_mode = Motor_GOFORWARD;
			break;
		case Ultra_All:
			mcTurnLeft();
			cs.motor_mode = Motor_TURNLEFT;
			break;
		case Ultra_Nothing://障害物なし：（目標 - 設定された目的地に指定ルートないし最短ルートで移動する）
			mcGoStraight();
			cs.motor_mode = Motor_GOFORWARD;
			break;
		default:
			break;
	}
	if(cs.motor_mode != cs.pre_motor_mode){
		//機器の移動方向が変化した。
		//タイマを止めて、現在までの移動方向と経過時間を記録する。
		stopCMT0Ir();
		
		//配列に格納する（暫定)
		motor_used_list[motor_used_index] = cs.pre_motor_mode;
		motor_elapsed_time_list[motor_used_index] = getCMT0Counter();
		motor_used_index += 1;
		
		//タイマをリセットして再スタートする
		clearCMT0Counter();
		startCMT0Ir();
	}
}