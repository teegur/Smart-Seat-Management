#include "device_driver.h"
#include "stm32f10x.h"
#include "udf.h"
#include <stdio.h>
#include <string.h>
#include "libemqtt.h"
#include "st7735.h"

#define printf		Uart1_Printf
#define BUF_SIZE	512
#define	SEVER_IP	"192.168.0.104"
#define PORT		1883

#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];


int socket_id;
mqtt_broker_handle_t broker;
int keepalive = 0;

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

char str[20];

void Font_Test(void)
{
        Lcd_Clr_Screen(RED);
	Lcd_Printf(8,10,BLUE,RED,2,2,"SEATED ");
	Lcd_Printf(20,40,BLUE,RED,2,2,"%s", str);
}

void main(void)
{

	int packet_length;
	uint16_t msg_id, msg_id_rcv;

	RCC_GetClocksFreq(&RCC_ClockFreq);
	Uart1_Init(115200);
	
	TIM_Config();
	Ethernet_Init();
        Lcd_Init(); // LCD 초기화
	
	mqtt_init(&broker, "Output_dev");
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

	// >>>>>> subscribe
	printf("subscribe\n");
	mqtt_subscribe(&broker, "command", &msg_id);
 	//TIM_Delay_ms(100);

	// <<<<< SUBACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		printf("Error(%d) on read packet!\n", packet_length);
		return ;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_SUBACK)
	{
		printf("SUBACK expected!\n");
		return ;
	}

	msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
	if(msg_id != msg_id_rcv)
	{
		printf("%d message id was expected, but %d message id was found!\n", msg_id, msg_id_rcv);
		return ;
	}

	printf("SUBACK received..\n");

	while(1)
	{
		// <<<<<
		packet_length = read_packet(0);
		if(packet_length == -1)
		{
			printf("Error(%d) on read packet!\n", packet_length);
			return ;
		}
		else if(packet_length > 0)
		{
			printf("Packet Header: 0x%x...\n", packet_buffer[0]);
			if(MQTTParseMessageType(packet_buffer) == MQTT_MSG_PUBLISH)
			{
				uint8_t topic[255], msg[1000];
				uint16_t len;
				len = mqtt_parse_pub_topic(packet_buffer, topic);
				topic[len] = '\0'; // for printf
				len = mqtt_parse_publish_msg(packet_buffer, msg);
				msg[len] = '\0'; // for printf
				printf("%s %s\n", topic, msg);  // msg : 받은 데이터
                                sprintf(str, "%s", msg);    // %d를 지정하여 정수를 문자열로 저장
                                Font_Test(); 
			}
		}

	}


}
 

