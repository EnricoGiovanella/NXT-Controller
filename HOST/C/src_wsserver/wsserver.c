#include <stdio.h>
#include <websock/websock.h>
#include <pthread.h>
#include "include/message.h"
#include "include/wsserver.h"
#include "include/usbrs232.h"
#include "include/timertx.h"

int users = 0;
// number of users connected with the web socket

libwebsock_context *wctx = NULL;

int sendBinaryAllUsers(unsigned char *in_data, size_t len_data) {
	unsigned int count = 0;
	int flags =  WS_FRAGMENT_FIN | WS_OPCODE_BINARY;
	libwebsock_client_state *current;
	if (wctx->clients_HEAD == NULL) {
		return 0;
	}
	for (current = wctx->clients_HEAD; current != NULL; current = current->next) {
		libwebsock_send_fragment(current,  in_data, len_data, flags);
		count++;
	}

	return count;
}


int onMessage(libwebsock_client_state *state, libwebsock_message *msg) {
	switch (msg->opcode) {
		case WS_OPCODE_TEXT:
			libwebsock_send_text_with_length(state, msg->payload, msg->payload_len);
			fprintf(stderr, "opcode: %d\n", msg->opcode);
		break;
		case WS_OPCODE_BINARY:
			fprintf(stderr, "\nmessaggio arrivato opcode: %u length %llu\n\n", msg->opcode , msg->payload_len);
			for(int i=0; i< msg->payload_len; i++)
				fprintf(stderr, "%u, ",msg->payload[i]);
			fprintf(stderr, "\n");
			receiveBinaryWsMsg(msg->payload,msg->payload_len);
		break;
		default:
			fprintf(stderr, "Unknown opcode: %d\n", msg->opcode);
		break;
	}
	return 0;
}

int onOpen(libwebsock_client_state *state) {
	users++;
	fprintf(stderr, "onopen: %d users %d\n", state->sockfd,users);
	return 0;
}

int onClose(libwebsock_client_state *state) {
	users--;
	if(users <= 0) closeRs232();
  fprintf(stderr, "onclose: %d users %d\n",state->sockfd,users);
  return 0;
}


int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <port to listen on>\n\nNote: You must be root to bind to port below 1024\nuse .\\wsservec 5000\n", argv[0]);
		exit(0);
	}

	startSerialThread();


	wctx = libwebsock_init();
	if (wctx == NULL ) {
		fprintf(stderr, "Error during libwebsock_init.\n");
		exit(EXIT_FAILURE);
	}
	libwebsock_bind(wctx, "0.0.0.0", argv[1]);
	fprintf(stderr, "libwebsock listening on port %s\n", argv[1]);
	wctx->onmessage = onMessage;
	wctx->onopen = onOpen;
	wctx->onclose = onClose;

	libwebsock_wait(wctx);
	stopSerialThread();

	fprintf(stderr, "Exiting.\n");
	exit(EXIT_SUCCESS);
}
