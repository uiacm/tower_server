/**
 * towerserver.c
 * Josh Armstrong
 *
 */

#include "sendmessage.h"
#include "towerlights.h"
#include "mongoose.h"
#include "driver.h"
#include <jansson.h>
#include <stdio.h>
#include <string.h>

/**
 * Callback function from mongoose when a websocket has data. This function will 
 * interpret the message and take appropriate actions. The message must be a JSON 
 * formatted object and have a type property. 
 * 
 * Arguments:
 *   flags: first byte of websocket frame, see websocket RFC,
 *          http://tools.ietf.org/html/rfc6455, section 5.2
 *   data, data_len: payload data. Mask, if any, is already applied.
 */
static int websocket_data_handler(struct mg_connection *conn, int flags, char *data, size_t data_len) {
  printf("Recieved: [%.*s]\n", (int) data_len, data);
  
  // A variable to hold any errors from jansson
  json_error_t error;
  // The root node in the decoded JSON object
  json_t *root;

  // Decode the recieved text
  root = json_loadb(data, data_len, 0, &error);

  // Check if the decoding went ok...
  if (root == NULL) {
    fprintf(stderr, "Error decoding JSON: on line %d, column %d: %s\n", error.line, error.column, error.text);
    send_message(conn, "Error decoding JSON string", 25);
  } else {
    // Now we have a decoded JSON object.
    // Check if the root is an object
    if (json_is_object(root) == 0) {
      fprintf(stderr, "Websocket message was not a JSON object\n");
      send_message(conn, "Message must be a JSON object", 30);
    } else {
      // The object should have type property
      json_t *type = json_object_get(root, "type");
      if (json_is_string(type) == 0) {
        fprintf(stderr, "Message type missing");
        send_message(conn, "Message type missing", 21);
      } else {
        // Now handle the message!
        const char *msgtype = json_string_value(type);

        if (strncmp(msgtype, "ping", 5) == 0) {
          printf("Responding to ping\n");
          tl_send_pong(conn);
        } else if (strncmp(msgtype, "getanimations", 13) == 0) {
          printf("Building and sending animation list\n");
          tl_send_animationlist(conn);
        } else if (strncmp(msgtype, "play", 5) == 0) {
          int animationindex = json_integer_value(json_object_get(root, "index"));
          printf("Playing animation at index %i\n", animationindex);
          tl_play(animationindex, conn);
        } else if (strncmp(msgtype, "stop", 5) == 0) {
          printf("Stopping animation\n");
          tl_stop(conn);
        } else if (strncmp(msgtype, "load", 5) == 0) {
          int animationindex = json_integer_value(json_object_get(root, "index"));
          printf("Loading animation at index %i\n", animationindex);
          tl_load(animationindex, conn);
        } else {
          tl_send_error("Unrecognized Command", conn);
        }
      }
      
      json_decref(root);
    }
  }

  // Returning zero means stoping websocket conversation.
  return 1;
}

int main(void) {
  struct mg_context *ctx;
  struct mg_callbacks callbacks;
  const char *options[] = {
    "listening_ports", "8080",
    "document_root", "www",
    NULL
  };

  // Initialize callbacks to null
  memset(&callbacks, 0, sizeof(callbacks));
  // Handle the ready callback
  //callbacks.websocket_ready = websocket_ready_handler;
  // Handle the data callback
  callbacks.websocket_data = websocket_data_handler;
  // Set up the hardware driver
  leds_setup();
  // Set up the tower lights logic
  tl_init();
  // Start the server
  ctx = mg_start(&callbacks, NULL, options);
  // Wait until user hits "enter"
  printf("Enter a character to exit:\n");
  getchar();  
  // Stop the server
  mg_stop(ctx);
  // Allow the tower lights to free memory
  //tl_destroy();

  return 0;
}

