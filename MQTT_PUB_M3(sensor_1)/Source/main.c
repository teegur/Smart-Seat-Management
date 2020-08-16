#include "stm32f10x.h"
#include "udf.h"
#include <stdio.h>
#include <string.h>
#include "libemqtt.h"
#include "device_driver.h"

#define printf		Uart1_Printf
#define BUF_SIZE	512
#define	SEVER_IP	"192.168.0.104"
#define PORT		1883
#define RCVBUFSIZE 1024

uint8_t packet_buffer[RCVBUFSIZE];


int socket_id;
mqtt_broker_handle_t broker;
int keepalive = 300;

extern volatile int sensor[100];       // 센서 상태 확인
extern volatile int Key_Value; 
extern volatile int Sensor_num; // (추가)
extern volatile int interlock; // (추가)

extern mqtt_broker_handle_t broker;
extern CHCONFIG_TYPE_DEF Chconfig_Type_Def;

RCC_ClocksTypeDef RCC_ClockFreq;

/* Private function prototypes -----------------------------------------------*/
extern void Ethernet_Test(void);
extern void Ethernet_Init(void);

extern void loopback_tcps(char s, unsigned short port);
extern void loopback_tcpc(char s, char *str_ip,unsigned short port);
extern void loopback_udp(char s, unsigned short port);

/* Private functions ---------------------------------------------------------*/
void Board_Init(void);

int send_packet(void* socket_info, const void* buf, unsigned int count)
{
	int8_t fd = *((int8_t*)socket_info);
	return send(fd, (unsigned char*)buf, count);
}

int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	socket_id = 2;
	socket_id = socket(socket_id, SOCK_STREAM, 8522, 0x00 );
	if( socket_id  < 0)
	{
		printf("socket create error\n");
		return -1;
	}

	if( ( connect(socket_id,Chconfig_Type_Def.destip, port ) ) < 0)
		return -1;

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&socket_id;
	broker->send = send_packet;

	return 0;
}

int close_socket(mqtt_broker_handle_t* broker)
{
	int8_t fd = *((int8_t*)broker->socket_info);
	return close(fd);
}


int read_packet(int timeout)
{
	int total_bytes = 0, bytes_rcvd, packet_length;

	memset(packet_buffer, 0, sizeof(packet_buffer));

	while(total_bytes < 2) // Reading fixed header
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes

	}

	packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length
	while(total_bytes < packet_length) // Reading the packet
	{
		if((bytes_rcvd = recv(socket_id, (packet_buffer+total_bytes), RCVBUFSIZE)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}
	return packet_length;

}


void main(void)
{
	int packet_length;
        char message_sensor[10];        // 복사할 문자열 선언
        SCB->SHCSR = 7<<16;
	//char msg[10]={0};
        Key_ISR_Enable(1);
	RCC_GetClocksFreq(&RCC_ClockFreq);
	Uart1_Init(115200);
	
	TIM_Config();

	Ethernet_Init();

	mqtt_init(&broker, "sensor1_device");		//	client ID  (다수일때 다르게 해줘야 함)
	mqtt_init_auth(&broker, NULL, NULL);
	init_socket(&broker, SEVER_IP, PORT);

	// >>>>> CONNECT
	mqtt_connect(&broker);

	// <<<<< CONNACK
	packet_length = read_packet(0);

	printf("receive packet length = %d\n",packet_length);

	if(packet_length < 0)
	{
		printf("Error(%d) on read packet!\n", packet_length);
		return;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK)
	{
		printf("CONNACK expected!\n");
		return;
	}

	if(packet_buffer[3] != 0x00)
	{
		printf("CONNACK failed!\n");
		return;
	}

	printf("CONNACK received..\n");

	// >>>>> PUBLISH QoS 0   //QoS level 0 ->손대지말것
	printf("Publish: message\n");
	mqtt_publish(&broker, "sensor1", "m3 hi!", 0);  //보낸거
   	TIM_Delay_ms(10);

	for(;;) // 수정할 부분
	{
                // interrupt 발생시 key_value != 0, sensor[key_value] 값도 인터럽트 함수안에서 바뀜 void EXTI15_10_IRQHandler(void)
                if(Key_Value)       //변수 추가
                {
                      if(sensor[Key_Value] == 0)        // sensor off 일 때
                      {
                        Uart1_Printf("KEY=%d OFF\n", Key_Value);
                        sprintf(message_sensor,"!%d%d",1,Key_Value);    // 구역 1 
                        mqtt_publish(&broker,"sensor1",message_sensor,0);
                      }
                      else      // sensor on 일 때
                      {
                        Uart1_Printf("KEY=%d ON\n", Key_Value);
                        sprintf(message_sensor,"@%d%d",1,Key_Value);        // 구역 1 (구역 번호)프로토콜 추가함 
                        mqtt_publish(&broker,"sensor1",message_sensor,0);
                      }
               
                      Key_Value = 0;
                }
                      
        }
   		//TIM_Delay_ms(200);
}



	// >>>>> DISCONNECT
	/*mqtt_disconnect(&broker);
	close_socket(&broker);*/
