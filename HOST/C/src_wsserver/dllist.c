#include <stdio.h> 
#include <stdlib.h>
#include "include/dllist.h"

//pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;


//initialize the double-linked list (dllist)
struct dllist_t* initDllist(pthread_mutex_t* mutex1) {
	struct dllist_t* list = (struct dllist_t*) malloc(sizeof(struct dllist_t));
	list->head_ref = NULL;
	list->end_ref = NULL;
	list->dllistLength = 0;
	list->mutex1 = mutex1;
	return list;
}

//inserts a new node on the front of the list
int push(struct dllist_t* dllist, void* new_data) {

	struct Node** head_ref = &dllist->head_ref;
	struct Node** end_ref = &dllist->end_ref;

	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 

	new_node->data = new_data; 

	new_node->next = (*head_ref); 
	new_node->prev = NULL; 

	if ((*head_ref) != NULL) 
		(*head_ref)->prev = new_node;

	if((*end_ref) == NULL)
		(*end_ref) = new_node;

	(*head_ref) = new_node; 

	dllist->dllistLength++;
	
	return 1;
} 

//inserts a new message in the front of the list
int pushMsg(struct dllist_t* dllist,uint8_t direct,uint8_t wsCod,uint8_t msgCod,uint8_t* msg, uint8_t length) {

	pthread_mutex_lock( dllist->mutex1 );

	uint8_t* new_msg = (uint8_t*) malloc((length + 1) * sizeof(uint8_t));
	new_msg[0] = msgCod;
	for(uint8_t i = 0; i<length; i++) new_msg[i + 1] = msg[i];

	struct dllData* new_data = (struct dllData*)malloc(sizeof(struct dllData));
	new_data->wsCod = wsCod;
	new_data->msgCod = msgCod;
	new_data->msg = new_msg;
	new_data->length = length + 1;
	new_data->direct = direct;

	int ret = push(dllist,(void*) new_data);

	pthread_mutex_unlock( dllist->mutex1 );
	return ret;
}


//appends a new node at the end
int append(struct dllist_t* dllist, void* new_data) {

	struct Node** head_ref = &dllist->head_ref;
	struct Node** end_ref = &dllist->end_ref;

	struct Node* new_node = (struct Node*)malloc(sizeof(struct Node)); 

	struct Node* last = *head_ref;

	new_node->data = new_data; 

	new_node->next = NULL; 

	if (*head_ref == NULL) { 
		new_node->prev = NULL; 
		*head_ref = new_node;
		(*end_ref) = new_node;
		dllist->dllistLength++;
		return 1; 
	} 

	while (last->next != NULL) 
		last = last->next; 

	last->next = new_node; 

	new_node->prev = last; 

	(*end_ref) = new_node;

	dllist->dllistLength++;

	return 1; 
} 

//appends a new message at the end
int appendMsg(struct dllist_t* dllist,uint8_t direct,uint8_t wsCod,uint8_t msgCod,uint8_t* msg, uint8_t length) {

	pthread_mutex_lock( dllist->mutex1 );

	uint8_t* new_msg = (uint8_t*) malloc((length + 1) * sizeof(uint8_t));
	new_msg[0] = msgCod;
	for(uint8_t i = 0; i<length; i++) new_msg[i + 1] = msg[i];

	struct dllData* new_data = (struct dllData*)malloc(sizeof(struct dllData));
	new_data->wsCod = wsCod;
	new_data->msgCod = msgCod;
	new_data->msg = new_msg;
	new_data->length = length + 1;

	int ret = append(dllist,(void*) new_data);

	pthread_mutex_unlock( dllist->mutex1 );
	return ret;
}



//delete the last message
struct dllist_t* deleteLastMsg1(struct dllist_t* dllist) {
	struct Node* last = dllist->end_ref;
	struct dllData* data;

	if(last) {
		struct Node* prev = last->prev;
		if(prev) {
			prev->next = NULL;
			dllist->end_ref = prev;
			// delete data
			data = last->data;
			free(data->msg);
			free(data);
			free(last);
		}
		else {
			dllist->head_ref = NULL;
			dllist->end_ref = NULL;
		}
		dllist->dllistLength--;
	}
	//printf("\n");
	return dllist;
}

//delete the last message
void deleteLastMsg(struct dllist_t* dllist) {
	pthread_mutex_lock( dllist->mutex1 );

	deleteLastMsg1(dllist);

	pthread_mutex_unlock( dllist->mutex1 );
}

//delete all message
void deleteAll(struct dllist_t* dllist) {

	struct dllist_t* del = dllist;
	pthread_mutex_lock( dllist->mutex1 );

	while(del->end_ref) del = deleteLastMsg1(dllist);

	pthread_mutex_unlock( dllist->mutex1 );
}

//returns the last message without deleting it
struct dllData* ritLastMsgNode(struct dllist_t* dllist) {

	struct Node* last = dllist->end_ref;
	struct dllData* data;

	pthread_mutex_lock( dllist->mutex1 );
	
	if(last) {
		data = (struct dllData*) last->data;
		pthread_mutex_unlock( dllist->mutex1 );
		return data;
	}
	pthread_mutex_unlock( dllist->mutex1 );
	return NULL;
}

