/********************************************************************
 * test_m8266wifi.c
 * .Description
 *     Source file of M8266WIFI testing application 
 * .Copyright(c) Anylinkin Technology 2015.5-
 *     IoT@anylinkin.com
 *     http://www.anylinkin.com
 *     http://anylinkin.taobao.com
 *  Author
 *     wzuo
 *  Date
 *  Version
 ********************************************************************/
 
#include "stdio.h"
#include "brd_cfg.h"
#include "M8266WIFIDrv.h"
#include "M8266HostIf.h"
#include "M8266WIFI_ops.h"
#include "led.h"
#include "stdint.h"
#include "stm32f4xx_hal.h"
#include "string.h"

u16 SPI_TX_BUFFER[35] =   {CONVERT0,CONVERT1,CONVERT2,CONVERT3,CONVERT4,CONVERT5,CONVERT6,CONVERT7,
					                      CONVERT8,CONVERT9,CONVERT10,CONVERT11,CONVERT12,CONVERT13,CONVERT14,CONVERT15,
					                      CONVERT16,CONVERT17,CONVERT18,CONVERT19,CONVERT20,CONVERT21,CONVERT22,CONVERT23,CONVERT24,
				                        CONVERT25,CONVERT26,CONVERT27,CONVERT28,CONVERT29,CONVERT30,CONVERT31,CONVERT32,CONVERT33,CONVERT34};

uint16_t SPI_TX_intan[5] = {0xe800, 0xe900, 0xea00, 0xeb00, 0xec00}; //AScii intan

														
extern uint32_t x;

extern SPI_HandleTypeDef hspi2;

#define SPI_RX_BUFFER_SIZE 2048

void M8266WIFI_Test(void)
{
	 uint16_t status =0;
	 uint8_t  link_no=0;

#ifdef USE_LED_AND_KEY_FOR_TEST	
	for(i=0; i<3; i++)   // Flash the Two LEDs 4 times in the Main Board to indicate we start test. not mandatory.
	{                    // Chinese: 单片机主板上的LED灯闪烁4次，用于提示代码执行到这里的方便调试。和模组通信无关，非必须，可略。若没有定义宏USE_LED_AND_KEY_FOR_TEST，此处不会被编译进来。
      	 LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
	 }
	 LED_set(0, 1); LED_set(1, 1);
#endif	 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Macro Defitions SETUP  SOCKET CONNECTIONS  (Chinese: 创建套接字的一些宏)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
   /////////////
   ////	 Macro for Test Type(Chinese：测试类型宏定义)
	 #define TEST_M8266WIFI_TYPE    3	     //           1 = Repeative Sending, 2 = Repeative Reception, 3 = Echo  4 = multi-clients transimission test
	                                       // (Chinese: 1=模组向外不停地发送数据 2=模组不停地接收数据 3= 模组将接收到的数据发送给发送方 4=多客户端测试) 

	 /////////////
   ////	 Macros for Socket Connection Type (Chinese：套接字类型的宏定义) 
	 #define TEST_CONNECTION_TYPE   2	    //           0=WIFI module as UDP, 1=WIFI module as TCP Client, 2=WIFI module as TCP Server
	                                      // (Chinese: 0=WIFI模组做UDP, 1=WIFI模组做TCP客户端, 2=WIFI模组做TCP服务器
   /////////////
   ////	 Macros for Soket ip:port pairs  (Chinese：套接字的本地端口和目标地址目标端口的宏定义) 
   //local port	(Chinese：套接字的本地端口)
#if (TEST_CONNECTION_TYPE==1)         //// if module as TCP Client (Chinese:如果模组作为TCP客户端)
	 #define TEST_LOCAL_PORT  			0			//           local port=0 will generate a random local port each time fo connection. To avoid the rejection by TCP server due to repeative connection with the same ip:port
   	                                    // (Chinese: 当local port传递的参数为0时，本地端口会随机产生。这一点对于模组做客户端反复连接服务器时很有用。因为随机产生的端口每次会不一样，从而避免连续两次采用同样的地址和端口链接时被服务器拒绝。
#elif (TEST_CONNECTION_TYPE==0) || (TEST_CONNECTION_TYPE==2) //// if module as UDP or TCP Server (Chinese:如果模组作为UDP或TCP服务器)
   #define TEST_LOCAL_PORT  			4321  //           a local port should be specified //(Chinese:如果模组作为UDP或TCP服务器，则需要指定该套接字的本地端口)
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif                                  // (Chinese: 如果模组作为TCP服务器或UDP，那么必须指定本地端口

   //local port	(Chinese：套接字的目标地址和目标端口)
#if (TEST_CONNECTION_TYPE==0)        //// if module as UDP (Chinese:如果模组作为UDP，则可以指定目标地址和端口，也可以随便填充，在发送数据时再设置或更改)
   #define TEST_REMOTE_ADDR    		"192.168.4.2"
   #define TEST_REMOTE_PORT  	    1234
#elif (TEST_CONNECTION_TYPE==1)      //// if module as TCP Client (Chinese:如果模组作为TCP客户端，当然必须指定目标地址和目标端口，即模组所要去连接的TCP服务器的地址和端口)
   #define TEST_REMOTE_ADDR    	 	"192.168.43.233"  // "www.baidu.com"
   #define TEST_REMOTE_PORT  	    1234						// 80
#elif (TEST_CONNECTION_TYPE==2)     //// if module as TCP Server (Chinese:如果模组作为TCP服务器)
		#define TEST_REMOTE_ADDR      "1.1.1.1" // no need remote ip and port upon setup connection. below values just for filling and any value would be ok. 
    #define TEST_REMOTE_PORT  	  1234  	 //(Chinese: 创建TCP服务器时，不需要指定目标地址和端口，这里的数据只是一个格式填充，随便填写。
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif			 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Setup Connection and Config connection upon neccessary (Chinese: 创建套接字，以及必要时对套接字的一些配置)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////
//step 0: config tcp windows number (Chinese: 步骤0：如果是TCP类型的套接字，可以配置调整窗口参数）
#if ( 0 && ((TEST_CONNECTION_TYPE==1)||(TEST_CONNECTION_TYPE==2)) ) //If you hope to change TCP Windows, please change '0' to '1' in the #if clause before setup the connection
																																		//(Chinese: 如果想改变套接字的窗口数，可以将#if语句中的0改成1，这个配置需要在创建套接字之前执行)
// uint8_t M8266WIFI_SPI_Config_Tcp_Window_num(uint8_t link_no, uint8_t tcp_wnd_num, u16* status)
  if(M8266WIFI_SPI_Config_Tcp_Window_num(link_no, 4, &status)==0)
  {
		 while(1)
		 {
     #ifdef USE_LED_AND_KEY_FOR_TEST	 // led flash in 1Hz uppon errors
	      LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(100);
	      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(100);
     #endif
	   }
	}
#endif

///////	
//step 1: setup connection (Chinese: 步骤1：创建套接字连接）

  //  uint8_t M8266WIFI_SPI_Setup_Connection(uint8_t tcp_udp, u16 local_port, char remote_addr, u16 remote_port, uint8_t link_no, uint8_t timeout_in_s, u16* status);
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &status)==0)
	{		
		 while(1)
		 {
     #ifdef USE_LED_AND_KEY_FOR_TEST	 // led flash in 1Hz uppon errors (Chinese: 如果创建套接字失败，则进入死循环和闪烁主板上的灯）
	      LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
	      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
     #endif
		 }
	}
	else  // else: setup connection successfully, we could config it (Chinese: 创建套接字成功，就可以配置套接字）
	{
#if (1 && (TEST_CONNECTION_TYPE == 0) )  		//// If UDP, then the module could join a multicust group. If you hope to use multicust, Change first '0' to '1'  in the #if clause
		                                        //   (Chinese: 如果这个套接字是UDP，那么可以配置成组播模式。如果需要配置成组播，可以将#if语句中的第一个'0'改成'1')
			//uint8_t M8266WIFI_SPI_Set_Multicuast_Group(uint8_t join_not_leave, char multicust_group_ip[15+1], u16* status)
		 if(M8266WIFI_SPI_Op_Multicuast_Group(0, "224.6.6.6", &status)==0)
     {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST   // led flash in 1Hz uppon errors (Chinese: 如果失败，则进入死循环和闪烁主板上的灯）
				  LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif				 
		   }
     }
     else

#elif (TEST_CONNECTION_TYPE == 2)          //// If TCP server, then tcp server auto disconnection timeout, and max clients allowed could be set	
		                                        //  (Chinese: 如果TCP服务器，那么可以设置这个TCP服务器(因长时间无通信而)断开客户端的超时时间)
#if 1
		 //uint8_t M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(uint8_t link_no, u16 timeout_in_s, u16* status)	
		 if( M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(link_no, 120, &status) == 0)
     {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST	// led flash in 1Hz when error
	        LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif		 
		   }
     }
#endif		 
#if 0
  		   //uint8_t M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(uint8_t server_link_no, uint8_t max_allowed, u16* status);
     else if( M8266WIFI_SPI_Config_Max_Clients_Allowed_To_A_Tcp_Server(link_no, 5, &status)==0)
		 {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST	// led flash in 1Hz when error
	        LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif		 
		   }
		 }
#endif		 
		 else
#endif
		 //Setup Connection successfully (Chinese: 设置套接字链接成功)
     {
     #ifdef USE_LED_AND_KEY_FOR_TEST			 
		   // led 1 flash 4 times upon success 
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
		   LED_set(1, 0); M8266WIFI_Module_delay_ms(50); LED_set(1, 1); M8266WIFI_Module_delay_ms(50);
     #endif			 
		 }
	}


#if (TEST_M8266WIFI_TYPE==3)  // Echo test: to receive data from remote and then echo back to remote (Chinese: 收发测试，模组将接收到的数据立刻返回给发送方)
{
   uint16_t SPI_RX_BUFFER[SPI_RX_BUFFER_SIZE]; //定义接收数据的缓冲区
	 uint16_t sent;
   uint8_t data[2]={0,0}; //定义两个字节的空间来存放指令  初始化为0，0，所以wifi接收到的就是xx 00   
	 uint8_t send_data[5]= {0x10,0x11,0x12,0x13,0xFF};
	 uint16_t test[1]={0x5555};
	 uint16_t FFzhiling[1]={0xFFFF};
	 uint16_t fasong;
	 uint8_t chucun[2];
while(1)
{
	 if(M8266WIFI_SPI_Has_DataReceived())
	{	
    sent = M8266WIFI_SPI_RecvData(data, 1, 10,&link_no, &status);
		if(sent!=0)
			{
				 
				 if (data[0] == 0x33 )
				 {
					 x=0x00000000U;		
	         SPI2_Init(x);
					 M8266WIFI_SPI_Send_Data(&send_data[0],1,link_no, &status);
				 }
				 if (data[0] == 0x34 )
				 {
					 x=0x00000008U;		
	         SPI2_Init(x);
					 M8266WIFI_SPI_Send_Data(&send_data[1],1,link_no, &status);
				 }
				 if (data[0] == 0x35 )
				 {
					 x=0x00000020U;		
	         SPI2_Init(x);
					 M8266WIFI_SPI_Send_Data(&send_data[2],1,link_no, &status);
				 }
				 if (data[0] == 0x36 )
				 {
					 x=0x00000038U;		
	         SPI2_Init(x);
					 M8266WIFI_SPI_Send_Data(&send_data[3],1,link_no, &status);
				 }
				 if(data[0] == 0x37)  
				 { 
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[0]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[1]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[0];
           chucun[0] = (uint8_t)(fasong >> 8); //取高八
           chucun[1] = (uint8_t)fasong; //取低八
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);	
				 }
				  if(data[0] == 0x38)  
				 {	
           SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[1]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[1]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[1];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);	 
				 }
				 if(data[0] == 0x39)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[2]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[2]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[2];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x40)  
				 {
           SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[3]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[3]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[3];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x41)  
				 {					
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[4]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[4]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[4];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x42)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[5]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[5]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[5];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x43)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[6]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[6]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[6];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x44)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[7]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[7]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[7];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x45)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[8]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[8]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[8];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x46)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[9]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[9]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[9];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x47)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[10]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[10]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[10];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x48)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[11]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[11]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[11];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x49)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[12]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[12]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[12];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x50)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[13]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[13]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[13];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x51)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[14]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[14]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[14];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x52)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[15]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[15]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[15];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x53)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[16]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[16]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[16];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x54)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[17]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[17]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[17];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x55)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[18]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[18]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[18];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x56)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[19]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[19]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[19];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x57)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[20]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[20]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[20];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x58)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[21]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[21]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[21];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x59)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[22]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[22]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[22];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x60)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[23]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[23]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[23];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x61)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[24]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[24]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[24];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x62)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[25]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[25]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[25];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x63)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[26]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[26]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[26];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x64)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[27]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[27]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[27];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x65)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[28]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[28]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[28];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x66)  
				 {
					SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[29]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[29]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[29];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x67)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[30]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[30]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[30];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x68)  
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[31]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[31]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[31];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
				 if(data[0] == 0x76)  //采样32通道
				 {
					 int i;
					 for (i=0;i<32;i++)
		          {		
               SPI2_CS_LOW();	
			         SPI_RX_BUFFER[i+40]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[i]);
               SPI2_CS_HIGH();								
							 fasong = SPI_RX_BUFFER[i+40];
					     chucun[0] = (uint8_t)(fasong >> 8); 
               chucun[1] = (uint8_t)fasong; 
               M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);	
								M8266WIFI_Module_delay_ms(1);	
					     
		           }
			    if(i==32)
		         { 
			         M8266WIFI_SPI_Send_Data(&send_data[4], 2, link_no, &status);
			         i=0;
				     }
				}
				 if(data[0] == 0x77)  //采样35通道
				 {
					while(1)
					{
					 int m;
					for (m=0;m<35;m++)
		          {
							 SPI2_CS_LOW();
					     SPI_RX_BUFFER[m+80]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[m]);
			         SPI2_CS_HIGH();
							 fasong = SPI_RX_BUFFER[m+80];
					     chucun[0] = (uint8_t)(fasong >> 8); 
               chucun[1] = (uint8_t)fasong; 	      
           if(m==32||m==33||m==34)
					 {
						 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
						 M8266WIFI_Module_delay_ms(1000);	
					 }			
           else
					 {						 
                M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);	
					      M8266WIFI_Module_delay_ms(10);		
					 }						 
					 	}
					 }
				 }
				 if(data[0] == 0x78)  //采样电压通道
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[180]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[34]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[180];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }				 	 		
 ///**测试功能
				 if(data[0] == 0x91)  //采样电压通道
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[181]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[32]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[181];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }
 if(data[0] == 0x92)  //采样电压通道
				 {
					 SPI2_CS_LOW();						 
			     SPI_RX_BUFFER[182]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[33]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[182];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }			//**/	 
				 if(data[0] == 0x80)  //ascii码intan
				 {
					int j;
			    for(j = 0; j < 5; j++)
			      {			
            SPI2_CS_LOW();				
				    SPI_SendHalfWord(&hspi2,SPI_TX_intan[j]);
				    SPI2_CS_HIGH();									
						fasong = SPI_TX_intan[j];
    		    chucun[0] = (uint8_t)(fasong >> 8); 
            chucun[1] = (uint8_t)fasong;    					        
            M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);	
					  M8266WIFI_Module_delay_ms(100);	
			       }
				 }
				 if(data[0] == 90)
				 {
					 M8266WIFI_SPI_Send_Data((uint8_t *)&test[0], 2, link_no, &status);
				 }
			 }//if 0
		 } //while end of
	}
	 }
}// end of M8266WIFI_Test





/*	
volatile long int block_num=100; uint16_t tmpbuf1[4096];	 int cnt=0;		 
if(data[0] == 0x79)  //存SD卡
				 {
					 for (i=0;i<35;i++)
		           {
								SPI2_CS_LOW();
          			SPI_RX_BUFFER[i+120]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[i]);
			          SPI2_CS_HIGH();
			          tmpbuf1[cnt] = SPI_TX_BUFFER[i];
                cnt++;
			          if(cnt == 4096)
			              {
				              SD_WriteDisk((uint8_t*)tmpbuf1,block_num,16);
				              block_num = block_num+16;			
				              cnt=0;
			              }
		            }
		      }        
		    else
			     cnt = 0; //avoid store last cycle data after another call sign9
	 }
	   
*/
	
#endif


