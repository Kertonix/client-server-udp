// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/time.h>
#include "bufor.h"

#define PORT	 8080
#define MAXLINE 1024



// Driver code
int main() {
	int sockfd;
	unsigned char buffer[MAXLINE];
	int instrukcja_sterujaca;
	int ile_wyslac=3;
	struct sockaddr_in servaddr;
	struct timeval timeout;
	  timeout.tv_sec  = 1;
	  timeout.tv_usec = 0;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = INADDR_ANY;

//tu się zaczyna część danych

	unsigned int a = 9;

	struct PDU PDU;
	PDU.naglowek.data_type = 0;
	PDU.naglowek.operation_type = 0;
	PDU.naglowek.data_size = sizeof(PDU) - sizeof(PDU.naglowek);

	PDU.dane0.data = a;
	PDU.dane0.nr = 0;

	PDU.dane1.data = a;
	PDU.dane1.nr = 1;

	PDU.dane2.data = a;
	PDU.dane2.nr = 2;

	PDU.dane3.data = a;
	PDU.dane3.nr = 3;

	memcpy(buffer, &PDU, sizeof(PDU));

	/*	printf("Size of PDU:%d\n", sizeof(PDU));
	 printf("Typ: %d\n", buffer[0]);
	 printf("Operation type: %d\n", buffer[1]);
	 printf("Data size: %d\n", buffer[2]);
	 printf("Data 0: %d ", buffer[3]);
	 printf("Nr of data 0: %d\n", buffer[4]);
	 printf("Data 1: %d ", buffer[5]);
	 printf("Nr of data 1: %d\n", buffer[6]);
	 printf("Data 2: %d ", buffer[7]);
	 printf("Nr of data 2: %d\n", buffer[8]);
	 printf("Data 3: %d ", buffer[9]);
	 printf("Nr of data 3: %d\n", buffer[10]);
	 */
	int n, len;
	socklen_t leng;
	for (int l = 0; l < 13; l++) { //wysyłanie pakietów do serwera
		sendto(sockfd, (unsigned char*) buffer, sizeof(PDU),
		MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));
		for (int i = 0; i < sizeof(PDU); i++) {
			printf("to server: %d\n", buffer[i]);
		}
	}

	//wysyłanie instrukcji dodawania i odbiór danych
	instrukcja_sterujaca = 1;
	memset(buffer, 0, sizeof(PDU));
	buffer[0] = instrukcja_sterujaca;
	sendto(sockfd, (unsigned char*) buffer, sizeof(PDU),
	MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));
	printf("Wyslano instrukcje sumowania!\n");

	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
	n = recvfrom(sockfd, (unsigned char*) buffer, MAXLINE,
	MSG_WAITALL, (struct sockaddr*) &servaddr, &leng);
	//printf("%d",n); //co daje timeout

	if(n==-1){
		printf("timeout!\n");
	}
	else{

	unsigned int buffer1[2];
		memcpy(buffer1, buffer, sizeof(unsigned int)*2);
	printf("Wynik sumowania:	");
	if (buffer1[1]==1){//liczba ujemna
		printf("Liczba ujemna: -");
	}



	printf("%d\n", *buffer1);


	//wysyłanie instrukcji odejmowania i odbiór danych
	instrukcja_sterujaca = 2;
	memset(buffer, 0, sizeof(PDU));
	buffer[0] = instrukcja_sterujaca;
	sendto(sockfd, (unsigned char*) buffer, sizeof(PDU),
	MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));
	printf("Wyslano instrukcje odejmowania!\n");

	memset(buffer, 0, sizeof(PDU));
	  setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
	n = recvfrom(sockfd, (unsigned int*) buffer, MAXLINE,
	MSG_WAITALL, (struct sockaddr*) &servaddr, &leng);
	memcpy(buffer1, buffer, sizeof(unsigned int)*2);
	printf("Wynik odejmowania:	");
		if (buffer1[1]==1){//liczba ujemna
			printf("Liczba ujemna: -");
		}

	printf("%d\n", *buffer1);














	//wysyłanie instrukcji sortowania rosnaco i odbiór danych
	instrukcja_sterujaca = 3;
	memset(buffer, 0, sizeof(PDU));
	buffer[0] = instrukcja_sterujaca;
	buffer[1]=ile_wyslac;
	sendto(sockfd, (unsigned char*) buffer, sizeof(PDU),
	MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));
	printf("Wyslano instrukcje sortowania rosnaco!\n");





	unsigned int buffer_recv[10];
	setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timeval));
	n = recvfrom(sockfd, (unsigned int*) buffer_recv, MAXLINE,
	MSG_WAITALL, (struct sockaddr*) &servaddr, &leng);
	printf("Server to client: %d\n", *buffer_recv);
	for (int i=0;i<ile_wyslac;i++) {//wyświetlanie bufora
		printf("Server to client: %d\n", buffer_recv[i]);


	}

	//wysyłanie instrukcji sortowania malejaco i odbiór danych
	instrukcja_sterujaca = 4;
	memset(buffer, 0, sizeof(PDU));
	buffer[0] = instrukcja_sterujaca;
	sendto(sockfd, (unsigned char*) buffer, sizeof(PDU),
	MSG_CONFIRM, (const struct sockaddr*) &servaddr, sizeof(servaddr));
	printf("Wyslano instrukcje sortowania malejaco!\n");

		n = recvfrom(sockfd, (unsigned int*) buffer_recv, MAXLINE,
		MSG_WAITALL, (struct sockaddr*) &servaddr, &leng);
		printf("Server to client: %d\n", *buffer_recv);
		for (int i=0;i<ile_wyslac;i++) {//wyświetlanie bufora
			printf("Server to client: %d\n", buffer_recv[i]);
		}
	}
	/*
	 n = recvfrom(sockfd, (char*) buffer, MAXLINE,
	 MSG_WAITALL, (struct sockaddr*) &servaddr, &len);
	 buffer[n] = '\0';
	 printf("Server to client: %s\n", buffer);
	 */
	close(sockfd);
	return 0;
}
