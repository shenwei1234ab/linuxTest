/*
 * pub.h
 *
 *  Created on: Jan 28, 2016
 *      Author: shenwei
 */

#ifndef PUB_H_
#define PUB_H_

typedef int SOCKET;
int send_work(const char *phost,int port,const char *pfilename );
int recv_work(int port);
#endif /* PUB_H_ */
