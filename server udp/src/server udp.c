// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "buforz.h"

#define PORT	 8080
#define MAXLINE 1024
#define MAX_BUFF_SIZE 10; //10 pakietow ograniczenie listy

// Driver code
int main() {
	int sockfd;
	struct bufor *adres_zakotwiczenia = NULL; //zmienne pozwalające na powiązanie elementów listy
	struct bufor *obecny_adres = NULL;			//(punkt 3.)
	struct bufor *ostatni = NULL;
	struct bufor *temp;
	int max_bufor = MAX_BUFF_SIZE
	;

	unsigned char buffer[MAXLINE];
	struct sockaddr_in servaddr, cliaddr;

	// Creating socket file descriptor
	if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	memset(&cliaddr, 0, sizeof(cliaddr));

	// Filling server information
	servaddr.sin_family = AF_INET; // IPv4
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port = htons(PORT);

	// Bind the socket with the server address
	if (bind(sockfd, (const struct sockaddr*) &servaddr, sizeof(servaddr))
			< 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

	int len, n;	//zmienne pomocnicze

	int ile_elem = 0;
	while (1) {	//robimy nieskończoną pętlę działania programu
		n = recvfrom(sockfd, (unsigned char*) buffer, MAXLINE,//odbieranie od klienta, musi być w pętli
				MSG_WAITALL, (struct sockaddr*) &cliaddr, &len);

		if (buffer[0] == 0) { //typ operacji 0 to przesyłanie danych

			if (ile_elem >= 10) { //jeśli lista przepełniona to skasuj pierwszy element
				usun_element(&adres_zakotwiczenia, 0);
			} //skasowano, karuzela jedzie dalej

			int poczatek_danych = 3;
			struct odebrane odebrane_dane;
			odebrane_dane.odebrane0 = 0;
			for (int i = 0; i < buffer[2] / 2; i++) { //tu robimy sobie dodawanie danych
				odebrane_dane.odebrane0 = odebrane_dane.odebrane0
						+ buffer[poczatek_danych + (2 * i)];
			}
			//printf("Suma zapisana w serwerze: %d\n", odebrane_dane.odebrane0); //wyswietlanie sumy zapisanej w serwerze

			//zapisywanie sumy do listy wiązanej

			//czy jeszcze nie utworzono listy, wtedy musimy nadać pierwszy adres(adres zakotwiczenia listy)
			if (adres_zakotwiczenia == NULL) {
				adres_zakotwiczenia = (struct bufor*) malloc(
						sizeof(struct bufor)); //adres zakotwiczenia
				obecny_adres = adres_zakotwiczenia;
				obecny_adres->poprzedni = NULL;	//nie ma poprzedniego elementu :)
			}

			else {
				ostatni = adres_zakotwiczenia;//zabawę zaczynamy od początku listy

				for (n = 0; n < max_bufor; n++) {//no i teraz skoro wiemy że jest lista to musimy znaleźć jej ostatni element
					if (ostatni->nastepny != NULL) { //tu sprawdzamy czy w bloku listy odpowiedzialnym za przechowywanie adresu do następnego elementu coś jest
						ostatni = ostatni->nastepny; //jeśli coś tam jest to przechodzimy do następnego elementu
					} else
						break; //jeśli nic już nie ma to mamy ustawiony adres ostatniego elementu na zmiennej o nazwie ostatni
				}

				obecny_adres = (struct bufor*) malloc(sizeof(struct bufor)); //zarezerwowaliśmy pamięć dla naszego elementu listy
				ostatni->nastepny = obecny_adres; //podajemy poprzedniemu elementowi adres nowego elementu
				obecny_adres->poprzedni = ostatni;

			}
			obecny_adres->pierwszy = adres_zakotwiczenia; //w każdym elemencie musi być informacja o pierwszym
			obecny_adres->nastepny = NULL; //w każdym przypadku nieznamy jaki będzie adres następnego "Przekorny los"~Zenon Martyniuk
			memcpy(&obecny_adres->pakiet, &odebrane_dane.odebrane0, 4); //tu mamy wpisywanie danych do elementu <------ trzeba zmienić

			printf("Zawartość elementu: %d \n", obecny_adres->pakiet);

		}

		//tu jedziemy dalej z typami operacji
		else if (buffer[0] == 1) { //1 oznacza sumę danych z listy wiązanej i wysłanie do kilenta
			unsigned int suma = 0;
			unsigned int temp_buffer[10];		//bufor pomocniczy
			temp_buffer[1] = 0; //flaga określająca że liczba jest dodatnia (tak wstępnie)
			for (temp = adres_zakotwiczenia; temp != NULL; temp =
					temp->nastepny) {
				suma = suma + temp->pakiet;
			}
			if (suma < 0) { //jeśli liczba ujemna to do bufora wpisać dodatnią i dostawić flage że ujemna
				suma = abs(suma);
				temp_buffer[1] = 1; //flaga określająca że liczba jest ujemna
			}

			memcpy(temp_buffer, &suma, sizeof(int));
			sendto(sockfd, (unsigned int*) temp_buffer,
					sizeof(unsigned int) * 2,
					MSG_CONFIRM, (const struct sockaddr*) &cliaddr, len);
			printf("Suma wyslana!\n");

		}

		else if (buffer[0] == 2) { //2 oznacza odejmowanie danych z listy wiązanej i wysłanie do kilenta
			int suma = 0;
			unsigned int temp_buffer[10];
			temp_buffer[1] = 0; //flaga określająca że liczba jest dodatnia (tak wstępnie)
			for (temp = adres_zakotwiczenia; temp != NULL; temp =
					temp->nastepny) {
				suma = suma - temp->pakiet;
			}
			if (suma < 0) { //jeśli liczba ujemna to do bufora wpisać dodatnią i dostawić flage że ujemna
				suma = abs(suma);
				temp_buffer[1] = 1; //flaga określająca że liczba jest ujemna
			}
			memcpy(temp_buffer, &suma, sizeof(int));
			sendto(sockfd, (unsigned int*) temp_buffer, sizeof(int) * 2,
			MSG_CONFIRM, (const struct sockaddr*) &cliaddr, len);
			printf("Roznica wyslana!\n");

		} else if (buffer[0] == 3) { //sortowanie rosnąco
			int ile_wyslac = buffer[1];	//mamy wysłać tyle ile wynosi podany numer
			unsigned int temp_buffer[10];		//bufor pomocniczy
			int s = 0;		//zmienna pomocnicza
			for (temp = adres_zakotwiczenia; temp != NULL; temp =
					temp->nastepny) {
				temp_buffer[s] = temp->pakiet;
				s++;
			}

			for (int i = 0; i < n; i++)            //Loop for ascending ordering
					{
				for (int j = 0; j < n; j++)    //Loop for comparing other values
						{
					if (temp_buffer[j] > temp_buffer[i]) //Comparing other array elements
							{
						int tmp = temp_buffer[i]; //Using temporary variable for storing last value
						temp_buffer[i] = temp_buffer[j];       //replacing value
						temp_buffer[j] = tmp;             //storing last value
					}
				}
			}
			sendto(sockfd, (unsigned int*) temp_buffer,
					sizeof(int) * ile_wyslac,
					MSG_CONFIRM, (const struct sockaddr*) &cliaddr, len);
			printf("Wyslano posortowane rosnaco dane!\n");

		}

		else if (buffer[0] == 4) { //sortowanie malejąco
			int ile_wyslac = buffer[1];
			int temp_buffer[10];
			int s = 0;
			for (temp = adres_zakotwiczenia; temp != NULL; temp =
					temp->nastepny) {
				temp_buffer[s] = temp->pakiet;
				s++;
			}
			for (int i = 0; i < n; i++)           //Loop for descending ordering
					{
				for (int j = 0; j < n; j++)    //Loop for comparing other values
						{
					if (temp_buffer[j] < temp_buffer[i]) //Comparing other array elements
							{
						int tmp = temp_buffer[i]; //Using temporary variable for storing last value
						temp_buffer[i] = temp_buffer[j];       //replacing value
						temp_buffer[j] = tmp;             //storing last value
					}
				}
			}

			sendto(sockfd, (unsigned int*) temp_buffer,
					sizeof(int) * ile_wyslac,
					MSG_CONFIRM, (const struct sockaddr*) &cliaddr, len);
			printf("Wyslano posortowane malejaco dane!\n");

			//koncowe usuwanie listy
			for (int u = 0; u <= 11; u++) {
				if (adres_zakotwiczenia != NULL){
				printf("Usunieto %d element!\n", u);

				struct bufor *temp;
				struct bufor *obecny_adres;

				obecny_adres = adres_zakotwiczenia;
				if(obecny_adres->nastepny!=NULL){//bez tego wywala błąd bo próbujemy się dostać do elementu listy którego nie ma
				obecny_adres->nastepny->poprzedni = NULL; //powiedzenie drugiemu elementowi że teraz już nic przed nim nie ma
				}
				temp = obecny_adres->nastepny;
				for (temp; temp != NULL; temp = temp->nastepny) { //przypisujemy do wszystkich elementów nowy adres zakotwiczenia
					temp->pierwszy = obecny_adres->nastepny;
				}
				adres_zakotwiczenia = obecny_adres->nastepny;
				free(obecny_adres);
				}
				else {
					ile_elem=0;
					break;

				}
			}
		}

		else {
			printf("Bledny typ operacji");
		}

		/*sendto(sockfd, (unsigned char *)buffer, sizeof(buffer),
		 MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
		 len);
		 printf("message sent.\n");
		 */
		ile_elem++;
	}

	return 0;
}

int usun_element(struct bufor **adres_zakotwiczenia,
		unsigned int numer_elementu) {
	struct bufor *obecny_adres;
	///Funckcja do usuwania elementu listy, zwraca wskaznik do skopiowanych danych
	if (*adres_zakotwiczenia != NULL) { //sprawdzamy czy jest cokolwiek
		obecny_adres = *adres_zakotwiczenia;

		for (int i = 0; i <= numer_elementu; i++) { //iterujemy żeby znaleźć element który chcemy usunąć
			if (numer_elementu == 0) {
				printf("Usuwanie %d elementu\n\n\n", i);
				usun_pierwszy(adres_zakotwiczenia);
				break;
			} else if (i == numer_elementu) {
				//jeśli nie jest pierwszy to:
				if (obecny_adres->nastepny == NULL) { //sprawdź czy ostatni
					//usun ostatni
					printf("Usuwanie %d-ostatniego elementu\n\n\n", i);
					obecny_adres->poprzedni->nastepny = NULL;
					free(obecny_adres);
					break;
				} else {						//jeśli nie ostatni to środkowy
												//usun n-ty
					printf("Usuwanie %d elementu\n\n\n", i);
					obecny_adres->poprzedni->nastepny = obecny_adres->nastepny;
					obecny_adres->nastepny->poprzedni = obecny_adres->poprzedni;
					free(obecny_adres);
					break;
				}
			}
			if (obecny_adres->nastepny == NULL) {//zabezpieczenie przed podaniem zbyt dużego argumentu
				printf("Nie ma takiego elementu\n\n\n\n");
				break;
			} else
				//jeśli git to idziemy dalej
				obecny_adres = obecny_adres->nastepny;
		}
	} else {
		//nie mam nic do skasowania
		return (-1);
	}
	return (1);
}

int usun_pierwszy(struct bufor **adres_zakotwiczenia) {
	struct bufor *temp;
	struct bufor *obecny_adres;

	obecny_adres = *adres_zakotwiczenia;
	obecny_adres->nastepny->poprzedni = NULL; //powiedzenie drugiemu elementowi że teraz już nic przed nim nie ma
	temp = obecny_adres->nastepny;
	for (temp; temp != NULL; temp = temp->nastepny) { //przypisujemy do wszystkich elementów nowy adres zakotwiczenia
		temp->pierwszy = obecny_adres->nastepny;
	}
	*adres_zakotwiczenia = obecny_adres->nastepny;
	free(obecny_adres);

	return (1);
}
