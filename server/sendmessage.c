/**
 * sendmessage.c
 * Josh Armstrong
 *
 * send_message sets up message according to 
 * http://tools.ietf.org/html/rfc6455, section 5.2. The main logic is in 
 * setting the message length. The length is specified different ways based
 * on the it's size, with a maximum size of a 64-bit unsigned integer. The
 * data is then copied after the length. 
 *
 */

#include "mongoose.h"
#include <stdlib.h>

/** 
 * Sends a message to the client through the websocket connection. This function
 * sets up the message data according to the websocket specification. 
 */
void send_message(struct mg_connection *conn, char *msg, size_t msg_len) {
  unsigned char *reply;
  int i;
  int offset = 0;

  reply = (unsigned char *)malloc(sizeof(unsigned char) * msg_len);

  // The flags, see the spec for details
  reply[0] = 0x81;  // text and FIN flags set

  // Set the length
  if (msg_len <= 125) {
    reply[1] = msg_len;
    offset = 2;
  } else if (msg_len <= 65535) {
    reply[1] = 126;
    reply[2] = (msg_len & 0xff00) >> 8;
    reply[3] = msg_len & 0x00ff;
    offset = 4;
  } else if (msg_len <= 0xffffffffffffffff) {
    reply[1] = 127;
    unsigned long long mask =  0xff00000000000000;
    for (i = 0; i < 8; i++) {
      reply[i+2] = (msg_len & mask) >> ((7-i) * 8);
      mask = mask >> 16;
    }
    offset = 10;
  } else {
    printf("Error: message is too large to send.\n");
    //tl_send_error("Error sending message: payload too large", conn);
  }

  // Set the actual data
  for (i = 0; i < msg_len; i++) {
    reply[i+offset] = msg[i];
  }

  // Send it off
  mg_write(conn, reply, offset + msg_len);
}


