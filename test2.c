//タイマで移動時間を測りログをとる
//速度変化・向き変化（回転）などごとに計測する
#include "global.h"

void main(void);

void main(void){
	initCar();
	
	while(1){
		runCar();
	}
}