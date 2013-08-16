
#include "mongoose.h"
#include <jansson.h>

extern char rootdir[512];

extern void tl_init();
extern void tl_destroy();

extern void tl_send_error(const char *errormsg, struct mg_connection *conn);
extern void tl_send_animationlist(struct mg_connection *conn);
extern void tl_play(int animationindex, struct mg_connection *conn);
extern void tl_stop(struct mg_connection *conn);
extern void tl_load(int animationindex, struct mg_connection *conn);
extern void tl_send_animationlist(struct mg_connection *conn);
extern void tl_send_pong(struct mg_connection *conn);
extern void tl_send_error(const char *errormsg, struct mg_connection *conn);

extern void tl_handle_message(json_t *msg, struct mg_connection *conn); 
