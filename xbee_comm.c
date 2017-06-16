/* 
 * test6.c 
 * test3.c派生（test3.cはXBeeのATモード利用)
 * XBeeのAPIモード利用
 */
#include "iodefine.h"
#include "global.h"
//グローバル変数はCファイルに書く（http://d.hatena.ne.jp/saitodevel01/20110321/1300685958）
unsigned char SCI12_RXI12_INTERRUPT;//SCI12割込みフラグ
unsigned char MANUAL_SPEED_FLAG;//速度調整自動・手動切り替えフラグ
unsigned char MANUAL_MOTOR_SPEED = 0;//手動移動時速度変数
unsigned char AUTOR_CONTROL_MODE = AUTOR_RETURN;
unsigned char AUTOR_CONTROL_MODE_PRE = 100;
unsigned char XBEE_FRAME_START = 0;
unsigned char XBEE_FRAME_STRING[50];
unsigned char XBEE_FRAME_LENGTH = 51;
unsigned char XBEE_FRAME_COUNT = 0;
unsigned char XBEE_FRAME_TYPE = 0;
unsigned char XBEE_DB_DATA;
unsigned char XBEE_DIGITALDATA[2];
unsigned char XBEE_PRE_DIGITALDATA[2];
unsigned short XBEE_ANALOGDATA[4];
unsigned char XBEE_DB_DATA_NOW = 0;
unsigned char XBEE_DB_DATA_PREV = 0;
unsigned char XBEE_DB_DATA_DIRECTION_COMPLETE = 0;
unsigned char XBEE_DB_DATA_GOAL = 0;
unsigned char XBEE_RETURN_COUNT = 0;
unsigned char XBEE_RETURN_DATA[8];
unsigned char roAddr[] = { 0x00, 0x13, 0xA2, 0x00, 0x40, 0xE7, 0x46, 0xBD };
unsigned char coAddr[] = { 0x00, 0x13, 0xA2, 0x00, 0x40, 0xD8, 0xD2, 0x68 };
/* PE3:Front - PE4:Front-Left(旧Left) - PC5:Front-Right - PC6:Back(旧Right)*/
void main(void){
  unsigned char angle_count;
  initUltrasonicController();
  initCMT0();
  initCMT1();
  initMotorController();
  initSCI12();
  mcSetSpeed(4);
  mcStop();
  
  while(1){
    if(XBEE_DIGITALDATA[1] != XBEE_PRE_DIGITALDATA[1]){
      switch(XBEE_DIGITALDATA[1]){
        case 0xf0:
          break;
        case 0x70:
          AUTOR_CONTROL_MODE = AUTOR_MANUAL;
          break;
        case 0xe0:     AUTOR_CONTROL_MODE = AUTOR_AUTO;
          break;
        case 0xd0:
          mcSpeedUp();
          break;
        case 0xb0:
          mcSpeedDown();
          break;
        case 0x90:
          if(MANUAL_SPEED_FLAG > 5) MANUAL_SPEED_FLAG = 0;
          else MANUAL_SPEED_FLAG++;
          break;
        default:
          break;
      }   
      XBEE_PRE_DIGITALDATA[1] = XBEE_DIGITALDATA[1];
    }   
    if(SCI12.SSR.BIT.ORER == 1 || SCI12.SSR.BIT.FER == 1 || SCI12.SSR.BIT.PER == 1){
      clearSCI12ErrorFlag();//フラグを解除し、RDRを読み捨てる
      enableSCI12();
    }
    
    //通信中以外に処理する工程
    if(!XBEE_FRAME_START){
      unsigned short yAxis;
      unsigned short xAxis;
      switch(AUTOR_CONTROL_MODE){
        case AUTOR_AUTO:
          if(AUTOR_CONTROL_MODE != AUTOR_CONTROL_MODE_PRE){
            AUTOR_CONTROL_MODE_PRE = AUTOR_CONTROL_MODE;
            stopCMT1Ir();
          }
          if(PORTE.PIDR.BIT.B3 == 0){
            if(!MANUAL_SPEED_FLAG) mcSetSpeed(4);
            mcGoToBack(); 
            cmt0timer(500);
            mcTurnLeft();
            cmt0timer(500);
          }else if(PORTC.PIDR.BIT.B5 == 0){
            if(!MANUAL_SPEED_FLAG) mcSetSpeed(6);
            mcTurnLeft();
            cmt0timer(500);
          }else if(PORTC.PIDR.BIT.B6 == 0){
            if(!MANUAL_SPEED_FLAG) mcSetSpeed(4);
            mcTurnLeft();
            cmt0timer(500);
          }else if(PORTE.PIDR.BIT.B4 == 0){
            if(!MANUAL_SPEED_FLAG) mcSetSpeed(4);
            mcTurnRight();
            cmt0timer(500);
          }else{ 
            if(!MANUAL_SPEED_FLAG) mcSetSpeed(10);
            mcGoStraight();
          } 
          break;    
        case AUTOR_MANUAL: 
          if(AUTOR_CONTROL_MODE != AUTOR_CONTROL_MODE_PRE){ 
            AUTOR_CONTROL_MODE_PRE = AUTOR_CONTROL_MODE;
            stopCMT1Ir();
          }
          //IOデータによるリモコン操作     //とりあえず、リモコンのパッド一つで操作
          yAxis = XBEE_ANALOGDATA[0];
          xAxis = XBEE_ANALOGDATA[1];
          if(yAxis > 800){
            mcGoToBack();
          }else if(yAxis < 200){
            if(PORTE.PIDR.BIT.B3 != 0 && PORTE.PIDR.BIT.B4 != 0 && PORTC.PIDR.BIT.B5 != 0) mcGoStraight();
          }else if(xAxis < 200){
            mcTurnLeft();
          }else if(xAxis > 800){
            mcTurnRight();
          }else{
            mcStop();
          }
          break;
        case AUTOR_RETURN:
          if(AUTOR_CONTROL_MODE != AUTOR_CONTROL_MODE_PRE){
            AUTOR_CONTROL_MODE_PRE = AUTOR_CONTROL_MODE;
            startCMT1Ir();
            mcSetSpeed(5);
          }
          //電波強度XBEE_FRAME_STRING[18]
          //データに基づき行動する
          #if 1
          if(XBEE_FRAME_TYPE == 0x97){
          //if(XBEE_DB_DATA != XBEE_DB_DATA_PREV){
            if(XBEE_RETURN_COUNT <= 6){
              XBEE_RETURN_DATA[XBEE_RETURN_COUNT] = XBEE_DB_DATA;
              mcTurnRight();
              cmt0timer(250);//45度回転する時間
              mcStop();
              cmt0timer(500);
              XBEE_RETURN_COUNT++;
            }else if(XBEE_RETURN_COUNT == 7){
              unsigned char xi;
              unsigned char minRD = 255;
              unsigned char minXI = 0;
              if(!XBEE_DB_DATA_DIRECTION_COMPLETE){
                XBEE_RETURN_DATA[XBEE_RETURN_COUNT] = XBEE_DB_DATA;
                //電波強度が最小の方向を探す
                for(xi=0; xi<8; xi++){
                  if(minRD > XBEE_RETURN_DATA[xi]){
                    minRD = XBEE_RETURN_DATA[xi];
                    minXI = xi;
                  }
                }  
                mcTurnLeft();
                cmt0timer(250*(8-minXI));
                mcStop();
                cmt0timer(500);
                mcSetSpeed(8);
                mcGoStraight();
                XBEE_DB_DATA_DIRECTION_COMPLETE = 1;
              }else{ 
                if(XBEE_DB_DATA < 30){ 
                  mcStop();
                  AUTOR_CONTROL_MODE = AUTOR_MANUAL;
                  mcTurnRight();
                  cmt0timer(2000);
                  mcStop();
                  XBEE_RETURN_COUNT = 0;
                }   
              }
              //XBEE_RETURN_COUNT = 0;
            }
            //}
            //XBEE_DB_DATA_PREV = XBEE_DB_DATA;
            XBEE_FRAME_TYPE = 0;
          }
          #endif
          #if 0 
          xbee_get_db(roAddr); 
          if(XBEE_FRAME_TYPE == 0x97){
            //縦方向距離計測
            XBEE_DB_DATA_NOW = XBEE_DB_DATA;
            if(XBEE_DB_DATA_PREV == 0){ 
              mcGoStraight(); 
              cmt0timer(250);
            }else{ 
              if(XBEE_DB_DATA_NOW < XBEE_DB_DATA_PREV){
                mcGoStraight(); 
                cmt0timer(250);
              }else if(XBEE_DB_DATA_NOW > XBEE_DB_DATA_PREV){ 
                mcGoToBack(); 
                cmt0timer(250); 
              }else{  
                if(!XBEE_DB_DATA_GOAL){ 
                  mcTurnRight();//90度回転
                  cmt0timer(250);
                }else{  
                  mcStop(); 
                  AUTOR_CONTROL_MODE = AUTOR_MANUAL;  
                } 
              } 
            }
            XBEE_DB_DATA_PREV = XBEE_DB_DATA_NOW; 
            XBEE_FRAME_TYPE = 0; 
            if((0x1c - 0x02) < XBEE_DB_DATA && XBEE_DB_DATA < (0x1c + 0x02)) XBEE_DB_DATA_GOAL = 1;
            //xbee_get_db(roAddr);
          } 
          #endif 
          break;    
        default:    
          break;
      }
    }
  }
}
