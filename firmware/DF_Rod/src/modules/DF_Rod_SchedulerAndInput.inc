// Unity implementation module. Included only by DF_Rod_Application.cpp.
void t100ms_Process()
{

	// Power On Process
	if(rodPowerOnFlag)
	{
		rodPowerOnFlag = 0;	// CLR , Exec Once Only
		sendInfo_boardType();
	}
	
	//=== INFO REQ STEP
	if(recvInfoSeqStep)	// Req SEQ (not 0, Step Process)
	{
		sendRodInfo_forStep();
	}
	//===

// == IMU AGING TEST( IMU data출력)

//== ROD등록 "L"길게 누름 CHECK
	#define L_BTN_LONG_TIME	2500	// 2.5초
//== ROD L버튼 누른 시간 Check
	if(btn_L_DownCnt)
	{
		btn_L_DownCnt++;
		if((L_BTN_LONG_TIME/100) < btn_L_DownCnt)	// 2.5초 Over
		{
			
			rodRegistMode = 55;		// ROD REGIST ENTRY

			btn_L_DownCnt = 0;
			

			// 2) BC로 주소 송부
			eNow.addPeerAddress(broad_cast_addr);

			String rodAddr = eNow.getMyAddress();
			//eNow.write(STR_PID_CTRL_NUM, "$0600000016%");		// L버튼 LONG DOWN
			eNow.writeBC(DF_Protocol_RodToMain_RodAddress, rodAddr); 	// L버튼 LONG DOWN , ROD=>MAIN

			// TBD_, 메인에서 명령이 안오는 경우는 CANCEL처리 필요.(20초?)
			rodRegistToStart();
			
			// LED Blink SET
			AddrWrite_LedBlink = 1;	// LED Blink Set

			// TBD, Peer DisConnection?

			//----
		}
	}

//== ROD등록 LED점멸 Control
	#define ROD_REGI_LED_BLINK_TIME	1000	// 1 SEC
	// led Blink
	if(AddrWrite_LedBlink)
	{
		led1Grn_OutSts = !led1Grn_OutSts;
		digitalWrite(LED1_GRN_PIN, led1Grn_OutSts); 	// NORMAL : Green(스위치부) LED Blink 1SEC
		AddrWrite_LedBlink++;
 		if((ROD_REGI_LED_BLINK_TIME/100) < AddrWrite_LedBlink)
		{
			// 1) STOP BLINK
			AddrWrite_LedBlink = 0;
		}		// BLINK STOP (1초간만 Blinking)

	}

//== ROD등록 T/O Control
	if(rodRegistMode)
	{
		rodRegistToControl();
	}
	
}

/*---------------------------------------------------------
//
----------------------------------------------------------*/
void t500ms_Process()
{

	// 1) AD Read, Batt Level
	//unsigned bat_chk_us = micros();
	
	battery.scanAvgInteg();

	//LogPrintlnus(" LG] BatRead Time us:" + String(micros()-bat_chk_us));

}

/*---------------------------------------------------------
//
----------------------------------------------------------*/
void t1sec_Process()
{
	// 1 - Local 1sec Count
  /*
	local_10secCnt++;
	if(9 < local_10secCnt) { local_10secCnt = 0; }		// Count  1 ~ 10
  */

	// 2) ROD_ALIVE_CHK Recved Check
	rodAliveRecvTOCheck();


	// 4) IMU Connection Polling
	imuConnectCheck();

	// 5) Send BAT LEVEL (each 1 SEC)
	battChangeCheckSend();

	// 6) LED Display OUT
	rodLed1SecControl();

}

//-----------------------------------------------------
//  낚시대 LED 제어
//	Call 1 SEC
void rodLed1SecControl()
{
	if(rodAliveRcv_LedBlinkFlag)
	{
		if(!AddrWrite_LedBlink)
		{
			led1Grn_OutSts = !led1Grn_OutSts;
			digitalWrite(LED1_GRN_PIN, led1Grn_OutSts); 	// NORMAL : Green(스위치부) LED Blink 1SEC
		}
		led2Red_OutSts = !led2Red_OutSts;
		digitalWrite(LED2_RED_PIN, led2Red_OutSts); 	// NORMAL : Red(CPU보드부) LED Blink 1SEC
	}
	else
	{
		if(!AddrWrite_LedBlink)
		{
			led1Grn_OutSts = HIGH;
			digitalWrite(LED1_GRN_PIN, led1Grn_OutSts); 	// MAIN Not OP :  GREEN ON
		}
		led2Red_OutSts = LOW;
		digitalWrite(LED2_RED_PIN, led2Red_OutSts);	// MAIN Not OP :  BLUE OFF
	}
}

/*
=====================================
              Running Timer
=====================================
 */
unsigned int system_delay_cnt = 0;

void run_timer()
{
	unsigned int add_tick = 0;

	curr_ms_tick = millis();
	if(oldcurr_ms_tick != curr_ms_tick)
	{
		// TBD System Run TImer & System Count(1ms,10ms,100ms,1sec )
		if(curr_ms_tick > oldcurr_ms_tick)
		{
			add_tick = (int)(curr_ms_tick - oldcurr_ms_tick);
		}
		else
		{
			add_tick = (int)((0xFFFFFFFF - oldcurr_ms_tick) + 1 + curr_ms_tick);
		}

		sys_1ms_cnt += add_tick;		// system 1ms ++

		run_time_1ms += add_tick;
		
gotoSystemTimer:
	
		if( 9 < run_time_1ms)	// 10ms
		{
			//run_time_1ms = (run_time_1ms - 10);
			run_time_1ms -= 10;
			
			sys_10ms_cnt++;

			run_time_10ms++;
			if( 9 < run_time_10ms)	// 100ms
			{
				run_time_10ms -= 10;
				
				sys_100ms_cnt++;

				run_time_100ms++;

				if(!(run_time_100ms % 5))		// 500ms
				{
					sys_500ms_cnt++;
				}

				if( 9 < run_time_100ms)	// 1000ms
				{
					run_time_100ms -= 10;
					
					sys_1sec_cnt++;		// 1 SEC
					// run_time_1sec++;
				}
				
			}
		}

		if(9 < run_time_1ms)	// 
		{
			goto gotoSystemTimer;
		}
		
		//if(10 < add_tick)
		//if(5 < add_tick)
		if(2 < add_tick)
		{
			// TBD
			// error = sys delay = not good performance !!!
			system_delay_cnt++;
			LogPrintln(" LG] SYSTIK ROD Delay " + String(add_tick) + "ms ,cnt:" + String(system_delay_cnt));
		}

		oldcurr_ms_tick = curr_ms_tick;

	}

}

//------------------------------------------------------------------
//
//
//------------------------------------------------------------------
void ioPinSetting()
{

	//== SET Pin Mode (INPUT)

	//--Setup Mode Switch
	pinMode(SETUP_MODE_PIN, INPUT);

	//=== SET Pin Mode ( OUT )
	// BRD LED
	pinMode(LED1_GRN_PIN, OUTPUT);
	pinMode(LED2_RED_PIN, OUTPUT);

	// BTN LED
	pinMode(BTN_LED_LF_RED_PIN, OUTPUT);
	pinMode(BTN_LED_RT_BLUE_PIN, OUTPUT);

	pinMode(VRT_MOT_ON_PIN, OUTPUT);
	digitalWrite(VRT_MOT_ON_PIN, VRT_MOT_OFF);

}

//---------------------------------------------------------------
// File System Mount Check
//---------------------------------------------------------------
void checkFileSystem()
{
	// FILE System INIT with FORMAT & Reset
	if (!SPIFFS.begin())		// without FORMAT
	{
			LogPrintln(" LG] FS Format Start");
		
    	SPIFFS.format();	// About 30sec
		
		
		//delay(2000);
		delay(10);		// Format후 Reset 안정화(?) 시간
		
    	//ESP.restart();
		ESP_Soft_Reset();
	}

	// OK시만 처리됨
}


/*------------------------------------------------------------------
   SETUP Arduino
------------------------------------------------------------------*/
void DF_Rod_Application_Setup()
{

	//==
	curr_ms_tick = millis();

	//== SET Serial USB CDC UART (PC-Slave)
	Serial.begin(115200);
	Serial.setTimeout(10);
	//Serial.println();

	DbgSerial.begin(115200, SERIAL_8N1, DEBUG_RX_PIN, DEBUG_TX_PIN);		// TXD_0 / RXD_0(Vr1.0.1.0)
	DbgSerial.setTimeout(10);		  // wait 10ms, Default 1SEC

	//== 보드 Type판별
	settingIOtype();	// SET Type for EACH IO by BOARD Type

	//== 1)핀 입출력 모드 설정
	// 보드 TYPE설정 포함
	ioPinSetting();

	//== SETUP MODE 읽기
	setSlaveMode();		// Slave Mode SET
	
	//== 3) Power ON, LED깜빡 제어
	// LED3 Binking 3 times ( ON/OFF Toggle 6 times each 50ms For 300ms )
	int cnt = 0;
	while (cnt < 6)		// 300ms = 50ms * 6 times
	{
	  cnt++;
	  led1Grn_OutSts = !led1Grn_OutSts;
	  digitalWrite(LED1_GRN_PIN, led1Grn_OutSts);
	  led2Red_OutSts = !led2Red_OutSts;
	  digitalWrite(LED2_RED_PIN, led2Red_OutSts);
	  delay(50);	// 50ms마다 깜빡
	}
	
	//== LED2,3 All OFF
	// 보드 LED 출력
	led1Grn_OutSts = LOW;
	digitalWrite(LED1_GRN_PIN, led1Grn_OutSts);
	led2Red_OutSts = LOW;
	digitalWrite(LED2_RED_PIN, led2Red_OutSts);

	//== 버튼 LED출력
	btnLFLedRed_OutSts = BTN_LED_OFF;		// Not Active
	digitalWrite(BTN_LED_LF_RED_PIN, btnLFLedRed_OutSts);
	btnRTLedBlue_OutSts = BTN_LED_OFF;		// Not Active
	digitalWrite(BTN_LED_RT_BLUE_PIN, btnRTLedBlue_OutSts);
	

	LogPrintln(" LG] VER:" + String(rodVer));

	//== File System Mount Check
  	checkFileSystem();		// Format후 Reset
  	
	//== GET Target Mac Address, Addr NG시 Deafult SET : FF~FF
	eNow.bindTargetAddress();	// eNow Target Addr Load, TBD-Error처리
	
	//== IMU SERAIL_1 SET
	imuSerial.begin(115200, SERIAL_8N1, IMU_RX, IMU_TX);	// Return Thing

	//== imu AutoOutSet
	if(0 != imu.checkSetupFileVal())	// 0-OK, else-NG
	{
		// imu Out Setup
			LogPrintln(" LG] IMU OUT AUTO_SET Start");
		imu.autoSetup();
		// File "100"저장
		short len = imu.saveInterval(IMU_INTV_DEFAULT_TIME);	//// Write Byte수, Intv=100ms
		//delay(1000);
		//ESP_Soft_Reset();
	}
	else
	{
	}

	//== @@ Sleep & Wakeup
	  	
	//=== INIT OBJ
	sBtn.init();	// PIN설정, Type결정
	imu.init();		// IMU SERIAL 1 설정

	//==== MODE SELECTION  : NORMAL/SETUP
	if(0) { /* dummy*/ }
	
	//==Setup Mode ?
	//if (!digitalRead(SETUP_MODE_PIN))	// Normal Mode
	if (isSlaveSetupMode())			// SetUp Mode?
	{
		// IMU Stop Stepup Mode
		imu.setStartStop(IMU_CONF_STOP);			// IMU STOP
  		flagIMUSTOP = 1;
	
		// Output LED3 ON
		led2Red_OutSts = HIGH;
		digitalWrite(LED2_RED_PIN, led2Red_OutSts);		// SETUP MODE : RED ON
	}
	
	//==Normal Mode?
	else
	{
		// INIT enow,Encoder,Motor Only Normal Mode
		eNow.init(recv_cb_esp_now_msg);
		if( ESP_OK != esp_now_register_send_cb(sent_cb_esp_now_sts))
		{
			LogPrintln(" LG] Error NOW SENT_CB_REGIST NG");
		}
		
		encoder.init();
		battery.init();
		//    imu.init();
		
		// Set Callback Funtion encoder/Btn/imu, Only Norma Mode
		encoder.setRotateCallback(rotateChangeCallback);
		sBtn.setSwitchCallback(btnChangeCallback);
		imu.setStateCallback(imuDataCallback);
	
		imu.setStartStop(IMU_CONF_START);			// IMU START
	
		led1Grn_OutSts = HIGH;
		digitalWrite(LED1_GRN_PIN, led1Grn_OutSts);
		
	}

	//== FLAG & VAR INIT
	rodPowerOnFlag = 1;		// PowerOn후 MAIN에 ROD정보 송부용 Flag
	// set Not Recv Now Msg(pid)
	now_rcv_id = PID_NOT_RECVED;
	//system Delay Count CLR
	system_delay_cnt = 0;

	LogPrintln(" LG] PWRON_ time:" + String(millis()-curr_ms_tick));

	// System Timer CLR
	curr_ms_tick = millis();
	oldcurr_ms_tick = curr_ms_tick;

}


int pwrOn1st_loop = 0;
int resp_imu = 0;
/*------------------------------------------------------------------------
  Arduino Loop
------------------------------------------------------------------------*/
void DF_Rod_Application_Loop()
{

	//==== System Run Timer & Counter
	run_timer();			// Freerun Timer & System Counter() 1ms, 10ms, 100ms, 1 sec)

	//===== MODE SELECTION

	//-- SETUP
	//Setup Mode (Mac Address & Imu Config Setup)
	//if (digitalRead(SETUP_MODE_PIN))
	if (isSlaveSetupMode())
	{

  		if(0 < sys_1ms_cnt)
		{
			sys_1ms_cnt--;
			
 			if(0 < sys_10ms_cnt)
			{ 
				sys_10ms_cnt--; 

				//==10ms마다 처리, SETUP은
				// recv PC
				uartPcHandler_Setup();			// Recv CMD from PC exxcute & Response
				// recv IMU
				resp_imu = imu.uartRecv_Setup(); 		// Slave-IMU
				// TBD : LOG
			}
			if(0 < sys_100ms_cnt)	{ sys_100ms_cnt--; }
			if(0 < sys_500ms_cnt)	{ sys_500ms_cnt--; }
			if(0 < sys_1sec_cnt)	{ sys_1sec_cnt--; }
  		}

	}

	//-- Normal Mode
	else			
	{
		// 1 ms처리 , NORMAL
  		if(0 < sys_1ms_cnt)
  		{
			chk_ms_tick = millis(); 	// Read Current Tick
			
  			sys_1ms_cnt--;		// 1ms마다 실행.

			// TBD, 1ms INput READ(Button)
			// TBD, 10ms/100ms INPUT READ

			// recv MAIN
			nowRecvHandler();			//1ms eNow(MAIN->SLAVE)
			// recv PC : uartHandlerNormal();			// Recv CMD from PC exxcute & Response
			uartPcHandler_NormalMode();

			//battery.scan();	// Move to 100ms Process
			resp_imu = imu.uartRecv_Normal();			// 1ms IMU Data Read 5ms
			
		  
			//
			encoder.rotate();		// 1ms Handle Encoder
			//
			sBtn.toggle();			// 1ms Button Toggle READ

			//=================================
			//-- Timer 1ms,10ms,100ms,1sec EXEC-----
			//t1ms_Process();	// NA

			if(0 < sys_10ms_cnt)
			{
				sys_10ms_cnt--;
				t10ms_Process();	// NA			
			}
		
			if(0 < sys_100ms_cnt)
			{
				sys_100ms_cnt--;
				t100ms_Process();	// 
			}

			if(0 < sys_500ms_cnt)
			{
				sys_500ms_cnt--;
				t500ms_Process();	// 
			}
		
			if(0 < sys_1sec_cnt)
			{
				sys_1sec_cnt--;
				t1sec_Process();
				// Alive, Sleep Check
				// batt Level Change Check
				// imu Connect Check
				//LogPrintln(" LG] SyT_1s " + String(sys_100ms_cnt) + "," + String(sys_1sec_cnt) + "[100ms,1sec]");
			}

			// Check Delay Time
				if(1 < (millis()-chk_ms_tick))
				{
					LogPrintln(" LG] 1msExe Delay:" + String(millis()-chk_ms_tick)+" ms");
				}
		}
		// 1ms Call in NORMAL
		//delay(1);
	}
}
