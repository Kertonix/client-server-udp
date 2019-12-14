/*
 * bufor.h
 *
 *  Created on: Nov 7, 2019
 *      Author: root
 */

#ifndef BUFOR_H_
#define BUFOR_H_

struct naglowek_prot{
	unsigned char data_type:8;
	unsigned char operation_type:8;
	unsigned char data_size:8;
};

struct dane{
	unsigned char data:8;
	unsigned char nr:8;
};

struct PDU{
	struct naglowek_prot naglowek;
	struct dane dane0;
	struct dane dane1;
	struct dane dane2;
	struct dane dane3;

};

#endif /* BUFOR_H_ */
