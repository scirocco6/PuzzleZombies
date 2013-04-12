#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>
#include <SDL/SDL_ttf.h>

#define WIDTH 800
#define HEIGHT 600

#define MAXSPEED 5
#define MAXLETTERS 7 /* words are no more than this many letters long */

#define soundFire 0
#define soundBlocked 1

char *randomWord();

typedef struct player_s {
  int x,y;
  int dx, dy; /* movement vector */
  int direction; /* which direction bitmap to show */
} player_t;

typedef struct shot_s {
  int show;
  int x,y;
  int dx, dy; /* movement vector */
  int direction; /* which direction bitmap to show */
} shot_t;  

typedef struct zombie_s {
  char letter;
  int x,y;
  int dx, dy; /* movement vector */
  int direction; /* which direction bitmap to show */
} zombie_t;

typedef struct zlist_s {
  zombie_t *zombie;
  struct zlist_s *prev, *next;
} zlist_t;

zlist_t *zombieList, *zombieListEnd;

typedef struct alist_s {
  int sampleNumber;
  struct alist_s *next;
} alist_t;

player_t player;
shot_t shot;
int level, exit_flag, score, currentLetter, totalZombies;
SDL_mutex *add_audio, *zombieLock;
TTF_Font *wordFont, *scoreFont;
char word[MAXLETTERS * 2];
SDL_Color wordColor;
char *enteringWord;

enum sounds {SHOOT, HITZOMBIE, HITWALL};








