#ifndef DLLIST_H
#define DLLIST_H

#include <stdint.h>
#include <pthread.h>

// A linked list node 
struct Node { 
	void* data; 
	struct Node* next; 
	struct Node* prev; 
};

struct dllist_t {
	struct Node* head_ref;
	struct Node* end_ref;
	int dllistLength;
	pthread_mutex_t* mutex1;
};

struct dllData {
	uint8_t wsCod;
	uint8_t msgCod;
	uint8_t* msg;
	uint8_t length;
	uint8_t direct;
};

extern struct dllist_t* initDllist(pthread_mutex_t* mutex1);
extern int pushMsg(struct dllist_t* dllist,uint8_t direct,uint8_t wsCod,uint8_t msgCod,uint8_t* msg, uint8_t length);
extern int appendMsg(struct dllist_t* dllist,uint8_t direct,uint8_t wsCod,uint8_t msgCod,uint8_t* msg, uint8_t length);
extern void deleteLastMsg(struct dllist_t* dllist);
extern void deleteAll(struct dllist_t* dllist);
extern struct dllData* ritLastMsgNode(struct dllist_t* dllist);


#endif
