/**
 * driver.c
 *
 */


#include "gamma.h"
#include <wiringPi.h>
#include <pthread.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define DS  11
#define OE  12
#define ST_CP 13
#define SH_CP 9
#define MR  15

#define LATCHPIN ST_CP
#define CLOCKPIN SH_CP
#define DATAPIN DS

#define BCM_PERIOD 10


//static int animation[framecount][10][4][3];
static unsigned int (*animation)[10][4][3];
//static unsigned int framedeltas[framecount];
static unsigned int *framedeltas;
static int framecount = 0;
static int animationloaded = 0;

static pthread_t animationthread;
static pthread_mutex_t stoplock;
static int stop = 0;

/**
 * Selects a specific red, green or blue group.
 */
static void leds_select(uint32_t group) {
  // This function works by shifting out select group  

  int i;
  digitalWrite(LATCHPIN, LOW);
  digitalWrite(CLOCKPIN, LOW);
  //delayMicroseconds(100);
  for (i = 0; i < 24; i++) {
    digitalWrite(DATAPIN, group & 0x000001);
    //delayMicroseconds(100);
    digitalWrite(CLOCKPIN, LOW);
    //delayMicroseconds(100);
    digitalWrite(CLOCKPIN, HIGH);
    //delayMicroseconds(100);
    digitalWrite(CLOCKPIN, LOW);
    //delayMicroseconds(100);
    group = group >> 1;
  }
  digitalWrite(LATCHPIN, HIGH);
  //delayMicroseconds(100);
}


/** 
 * Initializes the wiring pi library and sets up the hardware. 
 */
void leds_setup() {
  wiringPiSetup();

  pinMode(DS, OUTPUT);
  pinMode(SH_CP, OUTPUT);
  pinMode(ST_CP, OUTPUT);
  pinMode(OE, OUTPUT);
  pinMode(MR, OUTPUT);

  digitalWrite(DS, LOW);
  digitalWrite(SH_CP, LOW);
  digitalWrite(ST_CP, LOW);
  digitalWrite(OE, LOW);
  digitalWrite(MR, HIGH);

  leds_select(0x000000);

  pthread_mutex_init(&stoplock, NULL);
}

/**
 * Outputs a frame to the leds. Does one cycle of bcm. Should take
 * BCM_PERIOD * 8 microseconds to run
 */
static void outputFrame(unsigned int data[10][4][3]) {
  uint8_t redoutput = 0;
  uint8_t blueoutput = 0;
  uint8_t greenoutput = 0;
  uint8_t currentbit = 0;

  int multiplier = 0;

  for (multiplier = 0; multiplier < 8; multiplier++) {
    currentbit = 1 << multiplier;

    int row, col;
    for (row = 0; row < 10; row += 2) {
      redoutput = 0;
      greenoutput = 0;
      blueoutput = 0;

      for (col = 0; col < 4; col++){ 
        redoutput   |= (((gamma_table[data[row]  [col][0]] & currentbit) == currentbit) << col);
        redoutput   |= (((gamma_table[data[row+1][col][0]] & currentbit) == currentbit) << (col+4));
        greenoutput |= (((gamma_table[data[row]  [col][1]] & currentbit) == currentbit) << col);
        greenoutput |= (((gamma_table[data[row+1][col][1]] & currentbit) == currentbit) << (col+4));
        blueoutput  |= (((gamma_table[data[row]  [col][2]] & currentbit) == currentbit) << col);
        blueoutput  |= (((gamma_table[data[row+1][col][2]] & currentbit) == currentbit) << (col+4));
      }
     
      leds_select(redoutput << 16);
      leds_select((redoutput << 16) | (1 << (((row/2)*3) + 0)));

      leds_select(greenoutput << 16);
      leds_select((greenoutput << 16) | (1 << (((row/2)*3) + 1)));

      leds_select(blueoutput << 16);
      leds_select((blueoutput << 16) | (1 << (((row/2)*3) + 2)));
    }
      
    delayMicroseconds(BCM_PERIOD << multiplier);
  }
}



/** 
 * Reads a .tan file and saves the contents to memory. It can
 * then be played with the play() function.
 */
int leds_openAnimation(char *filename) {
  animationloaded = 0;
  free(animation);
  FILE *file = fopen(filename, "r");

  if (file == NULL) {
    printf("Error: Could not open \"%s\".\n", filename);
    return 1;
  }

  char buffer[1024];
  int i;
  int version;

  // Read in the version 
  if (fgets(buffer, 1024, file) == NULL) {
    printf("Error: Couldn't read file\n");
    return 2;
  } else {
    double v = strtod(buffer, NULL);
    if (fabs(v - 0.2) < 0.0001) {
      version = 2;
    } else if (fabs(v - 0.3) < 0.0001) {
      version = 3;
    } else {
      fprintf(stderr, "File version is not supported\n");
      return 4;
    }
  }

  if (version == 3) {
    // Read in the first four lines of pallette information
    for (i = 0; i < 3; i++) {
      if (fgets(buffer, 1024, file) == NULL) {
        printf("Error: Couldn't read file\n");
        return 2;
      }
    }
  }
  
  // Read in the animation dimensions
  int width, height;
  if (fscanf(file, "%i %i %i", &framecount, &height, &width) == EOF) {
    printf("Error: Couldn't read file\n");
    return 2;
  }
  
  if (width != 4 || height != 10) {
    printf("Error: Invalid frame sizes (%i, %i)\n", width, height);
    return 3;
  }
  printf("Framecount: %i, Width: %i, height: %i\n", framecount, width, height);
  animation = malloc(sizeof(unsigned int[10][4][3]) * framecount); 
  framedeltas = malloc(sizeof(unsigned int) * framecount);
 
  int row, col;
  int frameindex;
  int min;
  float sec;
  int time;
  int currenttime = 0;
  memset(animation, 0, sizeof(unsigned int[10][4][3]) * framecount);
  for (frameindex = 0; frameindex < framecount; frameindex++) {
    if (version == 2) {
      if (fscanf(file, "%f", &sec) != 1) {
        printf("Failed to read time\n");
      }
      time = currenttime + (int)(sec * 1000);
      currenttime = time;
    } else if (version == 3) {
      if (fscanf(file, "%i:%f", &min, &sec) != 2) {
        printf("Failed to read time\n");
      }
      time = min * 60 * 1000 + (int)(sec * 1000);
    }

    framedeltas[frameindex] = time;

    for (row = 0; row < height; row++) {
      for (col = 0; col < width; col++) {
        if (version == 2) {
          unsigned int value;
          fscanf(file, "%u", &value);
          value = value == 1 ? 255 : 0;
          animation[frameindex][row][col][0] = value;
          animation[frameindex][row][col][1] = value;
          animation[frameindex][row][col][2] = value;
        } else if (version == 3) {
          fscanf(file, "%u %u %u", &(animation[frameindex][row][col][0]), &(animation[frameindex][row][col][1]), &(animation[frameindex][row][col][2]));
        }
      }
    }
  }

  for (frameindex = 0; frameindex < 3; frameindex++) {
    printf("Frame %i:\n", frameindex);
    for (row = 0; row < 10; row++) {
      for (col = 0; col < 4; col++) {
        printf("<%i,%i,%i> ", animation[frameindex][row][col][0], animation[frameindex][row][col][0], animation[frameindex][row][col][0]);
      }
      printf("\n");
    }
  }

  printf("Animation successfully loaded\n");
  animationloaded = 1;
  return 0;
}

/**
 * Stops a playing animation, if any.
 */
void leds_stop() {
  pthread_mutex_lock(&stoplock);
  stop = 1;
  pthread_mutex_unlock(&stoplock);  
}

static void *leds_play_helper(void *param) {
  unsigned int starttime = millis();
  int frameindex = 0;

  while (frameindex < framecount-1) {
    pthread_mutex_lock(&stoplock);
    if (stop == 1) {
      stop = 0;
      pthread_mutex_unlock(&stoplock);
      break;
    }
    pthread_mutex_unlock(&stoplock);

    outputFrame(animation[frameindex]);

    if (millis() - starttime > framedeltas[frameindex+1]) {
      frameindex++;
    }
  }

  outputFrame(animation[frameindex]);
    
  printf("animation done playing\n");
  leds_select(0x000000);
  leds_select(0x00ffff);
  leds_select(0x000000);

  pthread_exit(NULL);
}

/**
 * Plays the currently loaded animation.
 */
void leds_play() {
  pthread_mutex_lock(&stoplock);
  stop = 0;
  pthread_mutex_unlock(&stoplock);

  if (pthread_create(&animationthread, NULL, leds_play_helper, NULL) != 0) {
    // couldn't create the thread
    fprintf(stderr, "Couldn't create the animation thread\n");
  }
}

