#ifndef _RX631_AUTODRIVER_GLOBAL_H_
#define _RX631_AUTODRIVER_GLOBAL_H_

enum UltrasonicState {
	Ultra_Nothing = 0,
	Ultra_Front = 1,
	Ultra_Right = 2,
	Ultra_All = 3
};

enum Motor_Mode{
	Motor_GOFORWARD,
	Motor_GOTOBACK,
	Motor_TURNRIGHT,
	Motor_TURNLEFT,
	Motor_STOP
};

//車両
typedef struct car_status {
	float posX;
	float posY;
	float angle;//度数表記
	
	float targetPosX;
	float targetPosY;
	
	enum Motor_Mode motor_mode;
	enum Motor_Mode pre_motor_mode;
} CAR_STATUS;

//hファイルにはCファイルに実態のあるグローバル変数にextern指定子をつけて宣言する
extern unsigned char SCI12_RXI12_INTERRUPT;
extern unsigned char MANUAL_SPEED_FLAG;
extern unsigned char manual_motor_speed;

#endif