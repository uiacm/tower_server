/**
 * sendmessage.h
 * Josh Armstrong
 *
 * This file includes a function that sends a message according to the protocol
 * at http://tools.ietf.org/html/rfc6455, section 5.2. 
 *
 */

#include "mongoose.h"
#include <stdlib.h>

extern void send_message(struct mg_connection *conn, char *msg, size_t msg_len);

