/**
 *
 */

#include "towerlights.h"
#include "sendmessage.h"
#include "driver.h"
#include "mongoose.h"
#include <string.h>
#include <dirent.h>

typedef struct {
  char *tanfile;
  char *musicfile;
  char *title;
} animation_t;

static animation_t *animations = NULL;
static int animationcount = 0;

char rootdir[512] = "www/animations/";

void tl_init() {

}

void tl_destroy()
{
  free(animations);
}

void tl_play(int animationindex, struct mg_connection *conn) {
  if (animationindex >= animationcount) {
    printf("Animation index %i out of bounds!\n", animationindex);
    return;
  }

  leds_play();
}

void tl_stop(struct mg_connection *conn) {
  leds_stop();
}

/**
 * Loads the specified animation into memory
 */
void tl_load(int animationindex, struct mg_connection *conn) {
  if (animationcount == 0) {
    printf("Error: can't load an animation if the animation list hasn't been enumerated\n");
  } 

  int i;
  for (i = 0; i < animationcount; i++) {
    printf("%s\n  wav:\"%s\"\n  tan:\"%s\"\n", animations[i].title, animations[i].musicfile, animations[i].tanfile);
  }

  char filename[1024];
  snprintf(filename, 1024, "www/%s", animations[animationindex].tanfile);
  if (leds_openAnimation(filename) != 0) {
    tl_send_error("Error opening animation file", conn);
    printf("Error opening animation file \"%s\" (%s)\n", filename, animations[animationindex].tanfile);
    return;
  }

  json_t *root = json_pack("{s:s}", "type", "doneloading");
  char *msg = json_dumps(root, 0);

  send_message(conn, msg, strlen(msg));
  json_decref(root); 
}

/**
 * Rebuilds the animation list and sends it to the client.
 */
void tl_send_animationlist(struct mg_connection *conn) {
  json_t *root;
  json_t *array;
  char *msg;

  root = json_object();
  json_object_set_new(root, "type", json_string("animationlist"));
  array = json_array(); 
  //free(animations);

  // Goal: Each folder in the specified animations folder holds an animation and it's music. 
  // Enumerate all the animations their music files
  DIR *dirp = opendir(rootdir);
  struct dirent *dp;

  // Check if the directory exists
  if (dirp == NULL) {
    printf("Could not animation directory '%s'.\n", rootdir);
    tl_send_error("Could not open root animation directory", conn);
    return;
  }

  // Iterate over the directories
  while ((dp = readdir(dirp)) != NULL) {
    if (dp->d_type == DT_DIR) {
      if (strncmp(dp->d_name, ".", 1) != 0 &&
          strncmp(dp->d_name, "..", 2) != 0) {
        animationcount++;
        animations = realloc(animations, animationcount * sizeof(animation_t));
        if (animations == NULL) {
          printf("Realloc failed while initializing %i length array\n", animationcount);
          return;
        }

        json_t *animation = json_object();       
        json_object_set_new(animation, "title", json_string(dp->d_name));
  
        printf("Animation title: %s\n", dp->d_name); 
        animations[animationcount-1].title = strndup(dp->d_name, 1024);

        char newdir[100]; 
        strncpy(newdir, rootdir, 100);
        strcat(strcat(newdir, "/"), dp->d_name);

        DIR *dirp2 = opendir(newdir);
        struct dirent *dp2;
        int foundtan = 0;
        int foundwav = 0;

        // Look for one wav file and one tan file
        while ((dp2 = readdir(dirp2)) != NULL) {
          if (dp2->d_type == DT_REG) {
            if (strncmp((dp2->d_name + strlen(dp2->d_name) - 4), ".wav", 4) == 0) {
              if (foundwav == 1) {
                printf("  Error: multiple wav files in a directory\n");
              } else {
                // printf("  wav file: %s\n", dp2->d_name);
                char musicfilename[1024];
                snprintf(musicfilename, 1024, "animations/%s/%s", dp->d_name, dp2->d_name);
                json_object_set_new(animation, "music", json_string(musicfilename));
                animations[animationcount-1].musicfile = strndup(musicfilename, 1024);
              }
              foundwav = 1;
            } else if (strncmp((dp2->d_name + strlen(dp2->d_name) - 4), ".tan", 4) == 0) {
              if (foundtan == 1) {
                printf("  Error: multiple tan files in a directory\n");
              } else {
                // printf("  tan file: %s\n", dp2->d_name);
                char tanfilename[1024];
                snprintf(tanfilename, 1024, "animations/%s/%s", dp->d_name, dp2->d_name);
                json_object_set_new(animation, "tan", json_string(tanfilename));
                animations[animationcount-1].tanfile = strndup(tanfilename, 1024);
              }
              foundtan = 1;
            }
          }
        }

        json_array_append_new(array, animation);
        closedir(dirp2);
      }

    }
  }

  closedir(dirp);

  json_object_set(root, "animations", array);

  msg = json_dumps(root, 0);
  if (msg == NULL) {
    printf("An error occured while dumping the json string\n");
  }

  send_message(conn, msg, strlen(msg));

  json_decref(root); 
}

/**
 * Send a pong message to the the client. 
 */
void tl_send_pong(struct mg_connection *conn) {
  json_t *root;
  char *msg;

  root = json_pack("{s:s}", "type", "pong");
  msg = json_dumps(root, 0);

  send_message(conn, msg, strlen(msg));
 
  json_decref(root); 
}

/** 
 * Constructs a JSON object representing an error object and sends it to the client.
 */
void tl_send_error(const char *errormsg, struct mg_connection *conn) {
  json_t *root;
  char *msg;

  root = json_pack("{s:s, s:s}", "type", "error", "message", errormsg);
  msg = json_dumps(root, 0);

  send_message(conn, msg, strlen(msg));
 
  json_decref(root); 
}


