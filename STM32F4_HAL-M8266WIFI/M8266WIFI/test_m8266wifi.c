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
uint16_t SPI_TX_BUFFER_2[32] = {CONVERT2,CONVERT3,CONVERT4,CONVERT5,CONVERT6,CONVERT7,
					                      CONVERT8,CONVERT9,CONVERT10,CONVERT11,CONVERT12,CONVERT13,CONVERT14,CONVERT15,
					                      CONVERT16,CONVERT17,CONVERT18,CONVERT19,CONVERT20,CONVERT21,CONVERT22,CONVERT23,CONVERT24,
				                        CONVERT25,CONVERT26,CONVERT27,CONVERT28,CONVERT29,CONVERT30,CONVERT31,CONVERT0,CONVERT1};
uint16_t SPI_TX_intan[5] = {0xe800, 0xe900, 0xea00, 0xeb00, 0xec00}; //AScii intan

														
extern uint32_t x;

extern SPI_HandleTypeDef hspi2;
uint8_t first_acquire_circle = 0;

void M8266WIFI_Test(void)
{
	 extern uint16_t status;
	 extern uint8_t link_no;

#ifdef USE_LED_AND_KEY_FOR_TEST	
	for(i=0; i<3; i++)   // Flash the Two LEDs 4 times in the Main Board to indicate we start test. not mandatory.
	{                    // Chinese: ��Ƭ�������ϵ�LED����˸4�Σ�������ʾ����ִ�е�����ķ�����ԡ���ģ��ͨ���޹أ��Ǳ��룬���ԡ���û�ж����USE_LED_AND_KEY_FOR_TEST���˴����ᱻ���������
      	 LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 0); LED_set(1, 1);  M8266WIFI_Module_delay_ms(100);
		     LED_set(0, 1); LED_set(1, 0);  M8266WIFI_Module_delay_ms(100);
	 }
	 LED_set(0, 1); LED_set(1, 1);
#endif	 

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Macro Defitions SETUP  SOCKET CONNECTIONS  (Chinese: �����׽��ֵ�һЩ��)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 
   /////////////
   ////	 Macro for Test Type(Chinese���������ͺ궨��)
	 #define TEST_M8266WIFI_TYPE    3	     //           1 = Repeative Sending, 2 = Repeative Reception, 3 = Echo  4 = multi-clients transimission test
	                                       // (Chinese: 1=ģ�����ⲻͣ�ط������� 2=ģ�鲻ͣ�ؽ������� 3= ģ�齫���յ������ݷ��͸����ͷ� 4=��ͻ��˲���) 

	 /////////////
   ////	 Macros for Socket Connection Type (Chinese���׽������͵ĺ궨��) 
	 #define TEST_CONNECTION_TYPE   2	    //           0=WIFI module as UDP, 1=WIFI module as TCP Client, 2=WIFI module as TCP Server
	                                      // (Chinese: 0=WIFIģ����UDP, 1=WIFIģ����TCP�ͻ���, 2=WIFIģ����TCP������
   /////////////
   ////	 Macros for Soket ip:port pairs  (Chinese���׽��ֵı��ض˿ں�Ŀ���ַĿ��˿ڵĺ궨��) 
   //local port	(Chinese���׽��ֵı��ض˿�)
#if (TEST_CONNECTION_TYPE==1)         //// if module as TCP Client (Chinese:���ģ����ΪTCP�ͻ���)
	 #define TEST_LOCAL_PORT  			0			//           local port=0 will generate a random local port each time fo connection. To avoid the rejection by TCP server due to repeative connection with the same ip:port
   	                                    // (Chinese: ��local port���ݵĲ���Ϊ0ʱ�����ض˿ڻ������������һ�����ģ�����ͻ��˷������ӷ�����ʱ�����á���Ϊ��������Ķ˿�ÿ�λ᲻һ�����Ӷ������������β���ͬ���ĵ�ַ�Ͷ˿�����ʱ���������ܾ���
#elif (TEST_CONNECTION_TYPE==0) || (TEST_CONNECTION_TYPE==2) //// if module as UDP or TCP Server (Chinese:���ģ����ΪUDP��TCP������)
   #define TEST_LOCAL_PORT  			4321  //           a local port should be specified //(Chinese:���ģ����ΪUDP��TCP������������Ҫָ�����׽��ֵı��ض˿�)
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif                                  // (Chinese: ���ģ����ΪTCP��������UDP����ô����ָ�����ض˿�

   //local port	(Chinese���׽��ֵ�Ŀ���ַ��Ŀ��˿�)
#if (TEST_CONNECTION_TYPE==0)        //// if module as UDP (Chinese:���ģ����ΪUDP�������ָ��Ŀ���ַ�Ͷ˿ڣ�Ҳ���������䣬�ڷ�������ʱ�����û����)
   #define TEST_REMOTE_ADDR    		"192.168.4.2"
   #define TEST_REMOTE_PORT  	    1234
#elif (TEST_CONNECTION_TYPE==1)      //// if module as TCP Client (Chinese:���ģ����ΪTCP�ͻ��ˣ���Ȼ����ָ��Ŀ���ַ��Ŀ��˿ڣ���ģ����Ҫȥ���ӵ�TCP�������ĵ�ַ�Ͷ˿�)
   #define TEST_REMOTE_ADDR    	 	"192.168.43.233"  // "www.baidu.com"
   #define TEST_REMOTE_PORT  	    1234						// 80
#elif (TEST_CONNECTION_TYPE==2)     //// if module as TCP Server (Chinese:���ģ����ΪTCP������)
		#define TEST_REMOTE_ADDR      "1.1.1.1" // no need remote ip and port upon setup connection. below values just for filling and any value would be ok. 
    #define TEST_REMOTE_PORT  	  1234  	 //(Chinese: ����TCP������ʱ������Ҫָ��Ŀ���ַ�Ͷ˿ڣ����������ֻ��һ����ʽ��䣬�����д��
#else
#error WRONG TEST_CONNECTION_TYPE defined !
#endif			 


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	 Setup Connection and Config connection upon neccessary (Chinese: �����׽��֣��Լ���Ҫʱ���׽��ֵ�һЩ����)
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////
//step 0: config tcp windows number (Chinese: ����0�������TCP���͵��׽��֣��������õ������ڲ�����
#if ( 0 && ((TEST_CONNECTION_TYPE==1)||(TEST_CONNECTION_TYPE==2)) ) //If you hope to change TCP Windows, please change '0' to '1' in the #if clause before setup the connection
																																		//(Chinese: �����ı��׽��ֵĴ����������Խ�#if����е�0�ĳ�1�����������Ҫ�ڴ����׽���֮ǰִ��)
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
//step 1: setup connection (Chinese: ����1�������׽������ӣ�

  //  uint8_t M8266WIFI_SPI_Setup_Connection(uint8_t tcp_udp, u16 local_port, char remote_addr, u16 remote_port, uint8_t link_no, uint8_t timeout_in_s, u16* status);
	if(M8266WIFI_SPI_Setup_Connection(TEST_CONNECTION_TYPE, TEST_LOCAL_PORT, TEST_REMOTE_ADDR, TEST_REMOTE_PORT, link_no, 20, &status)==0)
	{		
		 while(1)
		 {
     #ifdef USE_LED_AND_KEY_FOR_TEST	 // led flash in 1Hz uppon errors (Chinese: ��������׽���ʧ�ܣ��������ѭ������˸�����ϵĵƣ�
	      LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(500);
	      LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(500);
     #endif
		 }
	}
	else  // else: setup connection successfully, we could config it (Chinese: �����׽��ֳɹ����Ϳ��������׽��֣�
	{
#if (1 && (TEST_CONNECTION_TYPE == 0) )  		//// If UDP, then the module could join a multicust group. If you hope to use multicust, Change first '0' to '1'  in the #if clause
		                                        //   (Chinese: �������׽�����UDP����ô�������ó��鲥ģʽ�������Ҫ���ó��鲥�����Խ�#if����еĵ�һ��'0'�ĳ�'1')
			//uint8_t M8266WIFI_SPI_Set_Multicuast_Group(uint8_t join_not_leave, char multicust_group_ip[15+1], u16* status)
		 if(M8266WIFI_SPI_Op_Multicuast_Group(0, "224.6.6.6", &status)==0)
     {
		   while(1)
		   {
       #ifdef USE_LED_AND_KEY_FOR_TEST   // led flash in 1Hz uppon errors (Chinese: ���ʧ�ܣ��������ѭ������˸�����ϵĵƣ�
				  LED_set(0, 0); LED_set(1, 0); M8266WIFI_Module_delay_ms(1000);
	        LED_set(0, 1); LED_set(1, 1); M8266WIFI_Module_delay_ms(1000);
       #endif				 
		   }
     }
     else

#elif (TEST_CONNECTION_TYPE == 2)          //// If TCP server, then tcp server auto disconnection timeout, and max clients allowed could be set	
		                                        //  (Chinese: ���TCP����������ô�����������TCP������(��ʱ����ͨ�Ŷ�)�Ͽ��ͻ��˵ĳ�ʱʱ��)
#if 1
		 //uint8_t M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(uint8_t link_no, u16 timeout_in_s, u16* status)	
		 if( M8266WIFI_SPI_Set_TcpServer_Auto_Discon_Timeout(link_no, 7200, &status) == 0)
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
		 //Setup Connection successfully (Chinese: �����׽������ӳɹ�)
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


#if (TEST_M8266WIFI_TYPE==3)  // Echo test: to receive data from remote and then echo back to remote (Chinese: �շ����ԣ�ģ�齫���յ����������̷��ظ����ͷ�)
{
	 extern uint8_t receive_sign;
   uint8_t SPI_RX_BUFFER[1452]; //����������ݵĻ�����  < 1460  (66*22=1452)
	 uint8_t send_data[5]= {0x10,0x11,0x12,0x13,0xFF};
	 uint16_t test[1]={0x5555};
	 uint16_t FFzhiling[1]={0xFFFF};
	 uint16_t fasong;
	 uint8_t chucun[2];
	 while(1)
	 {
				 if(receive_sign == 1)  //����32ͨ��
				 {
					 
					 int i,j;
					 if(first_acquire_circle)  // discard the first two words 
					 {
						  uint8_t r_tem[2];
							SPI2_CS_LOW();
							SPI_SendHalfWord(&hspi2,CONVERT0,&r_tem[0]);
							SPI2_CS_HIGH();
							SPI2_CS_LOW();
							SPI_SendHalfWord(&hspi2,CONVERT1,&r_tem[0]);
							SPI2_CS_HIGH();
							first_acquire_circle = 0;
					 }
					 
					 for(j=0;j<22;j++)
					 {
						 for (i=0;i<32;i++)
							{		
								 SPI2_CS_LOW();	
								 SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER_2[i],&SPI_RX_BUFFER[66*j+2*i]);
								 SPI2_CS_HIGH();								
												
							}
							SPI_RX_BUFFER[66*j+64] = 0x12;
							SPI_RX_BUFFER[66*j+65] = 0x34;
							
				    }
					  M8266WIFI_SPI_Send_Data((uint8_t *)&SPI_RX_BUFFER, 1452, link_no, &status);	
						//M8266WIFI_Module_delay_ms(1);	
				}
						
				 if(receive_sign == 2)  //����35ͨ��
				 {
					while(1)
					{
					 int m;
					for (m=0;m<35;m++)
		          {
							 SPI2_CS_LOW();
					     //SPI_RX_BUFFER[m+80]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[m]);
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
				 if(receive_sign == 3)  //������ѹͨ��
				 {
					 SPI2_CS_LOW();						 
			     //SPI_RX_BUFFER[180]=SPI_SendHalfWord(&hspi2,SPI_TX_BUFFER[34]);		
           SPI2_CS_HIGH();					
					 fasong = SPI_RX_BUFFER[180];
           chucun[0] = (uint8_t)(fasong >> 8); 
           chucun[1] = (uint8_t)fasong; 
					 M8266WIFI_SPI_Send_Data((uint8_t *)&chucun, 2, link_no, &status);
				 }				 	 		
 
				 if(receive_sign == 4)
				 {
					 M8266WIFI_SPI_Send_Data((uint8_t *)&test[0], 2, link_no, &status);
				 }
		}//if 0
} //while end of
}





/*	
volatile long int block_num=100; uint16_t tmpbuf1[4096];	 int cnt=0;		 
if(data[0] == 0x79)  //��SD��
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


