//大体一周回転するように時間を調整
//時間はCMTタイマで正確なはず


unsigned char _mspeed;

void test1(void){
	initCMT0();
	initMotorController();
	mcSetSpeed(4);
	while(1){
		mcTurnLeft();
		cmt0timer(3800);
		mcTurnRight();
		cmt0timer(4600);
		if(_mspeed < 10) _mspeed++;
		else _mspeed = 3;
		mcSetSpeed(_mspeed);
	}
}