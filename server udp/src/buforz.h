/*
 * bufor.h
 *
 *  Created on: Nov 7, 2019
 *      Author: root
 */

#ifndef BUFORZ_H_
#define BUFORZ_H_

struct naglowek_prot{
	unsigned char data_type:8;
	unsigned char operation_type:8;
	unsigned char data_size:8;
};

struct dane{
	unsigned char data:8;
	unsigned char nr:1;
};

struct PDU{
	struct naglowek_prot naglowek;
	struct dane dane0;
	struct dane dane1;
	struct dane dane2;
	struct dane dane3;

};

struct odebrane{//to używamy do przechowywania zsumowanych danych
	unsigned int odebrane0;
};

struct bufor {//lista wiązana
	struct bufor *pierwszy;  	//wskażnik na pierwszy element listy
	struct bufor *poprzedni;	//wskaźnik na poprzeni element listy
	struct bufor *nastepny;			//wskaźnik na następny element listy
	unsigned int pakiet;	//kontener na dane (w przypadku przenoszenia poważnych danych)
};



#endif /* BUFORZ_H_ */
