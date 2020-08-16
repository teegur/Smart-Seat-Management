#pragma comment(lib,"ws2_32")
#pragma warning(disable: 4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <conio.h>

#include "libemqtt.h"


#define BUF_SIZE	512
#define	SEVER_IP	"192.168.0.104"	// broker ip 
#define PORT		1883


//	�����Լ� ������� �� ����
void Error_Exit(char* str)
{
	printf("%s Error!! Exit...\n",str);
	_getch();
	exit(1);
}


#define RCVBUFSIZE 1024
uint8_t packet_buffer[RCVBUFSIZE];

int socket_id;
int sensor_state[10][10];	// �� ������ �¼����¸� ������ �迭 (�ϴ��� 10x10 �迭�� �۰� ������)
int sensor_count;	// ������ �¼� ���� ��û�� �� ���
SOCKET hSocket;



int send_packet(void* socket_info, const void* buf, unsigned int count)
{
	SOCKET fd = *((SOCKET*)socket_info);
	return send(fd, buf, count, 0);
}

int init_socket(mqtt_broker_handle_t* broker, const char* hostname, short port)
{
	int flag = 1;
	int keepalive = 300; // Seconds

	WSADATA wsaData;
	SOCKADDR_IN servAdr;

	//	���� �ʱ�ȭ
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)!=0)
	{
		Error_Exit("WSAStartup()"); 
	}

	//	���� ����: socket()
	hSocket=socket(PF_INET, SOCK_STREAM, 0);   
	if(hSocket==INVALID_SOCKET)
	{
		Error_Exit("socket()");
	}

	//	������ �ʱ�ȭ �� connect()
	printf("try to connect to the server...");
	memset(&servAdr, 0, sizeof(servAdr));
	servAdr.sin_family=AF_INET;
	servAdr.sin_addr.s_addr=inet_addr(hostname);
	servAdr.sin_port=htons(port);
	
	if(connect(hSocket, (SOCKADDR*)&servAdr, sizeof(servAdr))==SOCKET_ERROR)
	{
		Error_Exit("connect()");
	}
	printf("\nserver connected...\n");
	printf("[send]: ");

	// MQTT stuffs
	mqtt_set_alive(broker, keepalive);
	broker->socket_info = (void*)&hSocket;
	broker->send = send_packet;

	return 0;
}

int close_socket(mqtt_broker_handle_t* broker)
{
	SOCKET fd = *((SOCKET*)broker->socket_info);
	closesocket(fd);
	return WSACleanup();
}


int read_packet(int timeout)
{
	int total_bytes = 0, bytes_rcvd, packet_length;

	memset(packet_buffer, 0, sizeof(packet_buffer));

	while(total_bytes < 2) // Reading fixed header
	{
		if((bytes_rcvd = recv(hSocket, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes

	}

	packet_length = packet_buffer[1] + 2; // Remaining length + fixed header length
	while(total_bytes < packet_length) // Reading the packet
	{
		if((bytes_rcvd = recv(hSocket, (packet_buffer+total_bytes), RCVBUFSIZE, 0)) <= 0)
			return -1;
		total_bytes += bytes_rcvd; // Keep tally of total bytes
	}
	return packet_length;
}

/*
int min_area(void)
{
	int i;
	int index;
	int min = 100;	// �ӽü���

	for (i = 1; i < 10; i++)
	{
		if (sensor_count[i] < min)
		{
			min = sensor_count[i];
			index = i;
		}
	}
	return index;	// ���� ���� ����� �ɾ� �ִ� ���� return
}
*/

int main(void)
{
	int packet_length,i,j;
	int tmp;	//
	int cnt = 0;	// On�� sensor�� ���� �ľ�
	// sensor_state[][] ���������� ������
	char str[10];
	uint16_t msg_id, msg_id_rcv;
	mqtt_broker_handle_t broker;

	mqtt_init(&broker, "iot_server");
	mqtt_init_auth(&broker, NULL, NULL);
	init_socket(&broker, SEVER_IP, PORT);

	// >>>>> CONNECT
	mqtt_connect(&broker);

	// <<<<< CONNACK
	packet_length = read_packet(0);

	printf("receive packet length = %d\n",packet_length);

	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
		return -1;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_CONNACK)
	{
		fprintf(stderr, "CONNACK expected!\n");
		return -2;
	}

	if(packet_buffer[3] != 0x00)
	{
		fprintf(stderr, "CONNACK failed!\n");
		return -2;
	}

	printf("CONNACK received..\n");

	// >>>>> PUBLISH QoS 0
	printf("subscribe\n");
	mqtt_subscribe(&broker, "sensor1", &msg_id);

	// <<<<< SUBACK
	packet_length = read_packet(1);
	if(packet_length < 0)
	{
		fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
		return -1;
	}

	if(MQTTParseMessageType(packet_buffer) != MQTT_MSG_SUBACK)
	{
		fprintf(stderr, "SUBACK expected!\n");
		return -2;
	}

	msg_id_rcv = mqtt_parse_msg_id(packet_buffer);
	if(msg_id != msg_id_rcv)
	{
		fprintf(stderr, "%d message id was expected, but %d message id was found!\n", msg_id, msg_id_rcv);
		return -3;
	}

	while(1)
	{
		//_sleep(500);          // 0.5�� (�ð������� �ʿ������ ������ �� ����ø� recv�ǰ� ���� �Ѱ����⿡ ������ �������� ��Ȯ�� ������)
		//int���� ���������� �ٲ㼭 pub
		cnt = 0;
		for (i = 0; i < 10; i++)
		{
			for (j = 0; j < 10; j++)
			{
				if (sensor_state[i][j] == 1)	cnt++;	// �ŷڼ��� ������ ���� �������� �ѹ� �� ������
			}
		}

		sprintf(str, "%d", cnt);
		mqtt_publish(&broker, "command", str, 0);
		// <<<<<
		packet_length = read_packet(0);
		if(packet_length == -1)
		{
			fprintf(stderr, "Error(%d) on read packet!\n", packet_length);
			return -1;
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
				printf("%s %s\n", topic, msg);	// ������ �޴� �κ�
				if (msg[0] == '@')	// sensor on msg	// ���⵵ �������� char�� int������ �ٲ����(�ƽ�Ű �ڵ� ���� -48)
				{
					//cnt++;
					//sensor_count[msg[1]-48]++;	// �� ������ sensor ī��Ʈ
					sensor_state[msg[1]-48][msg[2]-48] = 1;	// sensor_state ����
				}
				else if (msg[0] == '!') // '!' sensor off msg
				{
					//cnt--;
					//sensor_count[msg[1]-48]--;
					sensor_state[msg[1]-48][msg[2]-48] = 0; // sensor_state ����
				}
				else if (msg[0] == '#') // '#' sensor ���� ����
				{
					mqtt_publish(&broker, "command", "disconnected", 0);
				}
				else if (msg[0] == '$')	// ���ϴ� ������ �¼� �� ���
				{
					// msg[1] char���� int������ �ٲ����
					sensor_count = 0;
					for (i = 0; i < 10; i++)
					{
						if (sensor_state[msg[1] - 48][i] == 1) sensor_count++;
					}
					sprintf(str, "%c area seated: %d", msg[1], sensor_count);
					mqtt_publish(&broker, "area_state", str, 0);
				}
				//printf("%d\n", cnt);
			}
		}

		//sprintf(str, "recommend area : %d", min_area());
		//mqtt_publish(&broker, "current_seat",str, 0);	// ����� ���� ���� ������ ��ȣ�� current_seat �������� pub
	}
	return 0;
}
