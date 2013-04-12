#include "includes.h"

int zombieDirection(int dx, int dy) {
  if ((dx > 0) && (dy >0))
    return(4);
  else if ((dx > 0) && (dy == 0))
    return(3);
  else if ((dx > 0) && (dy < 0))
    return(2);
  else if ((dx < 0) && (dy >0))
    return(6);
  else if ((dx < 0) && (dy == 0))
    return(7);
  else if ((dx < 0) && (dy < 0))
    return(8);
  else if ((dx == 0) && (dy < 0))
    return(1);
  else if ((dx == 0) && (dy > 0))
    return(5);
  else
    return(0);
} /* zombieDirection */


zombie_t *makeZombie() {
  zombie_t *zombie;

  zombie = malloc (sizeof(zombie_t));
  if(!zombie) {
    fprintf(stderr, "Unable to create a zombie!!!@\n");
    myExit(-50);
  } /* if */

  /*  zombie->letter = (rand() % 26) + 65;*/
  if (*enteringWord == ':')
    enteringWord = randomWord();
  
  zombie->letter = toupper(*enteringWord);
  enteringWord++;

  fprintf (stderr, "%i is for %c is for ", zombie->letter, zombie->letter);

  zombie->x = rand() % WIDTH;
  zombie->y = rand() % HEIGHT;

  zombie->dx = (rand() % (MAXSPEED *2)) - MAXSPEED;
  zombie->dy = (rand() % (MAXSPEED *2)) - MAXSPEED;
  zombie->direction = zombieDirection(zombie->dx, zombie->dy);

  return(zombie);
} /* makeZombie */

void newZombie() {
  zlist_t *newListEntry;
  zombie_t *newZombie;

  SDL_mutexP(zombieLock); /*no one else can play with the zombies during the creation ritual */
  newListEntry = malloc(sizeof(alist_t));
  if(!newListEntry) {
    fprintf(stderr, "Unable to allocate a new zombie!!!@\n");
    myExit(-50);
  } /* if */
  newListEntry->zombie = makeZombie();
    
  if (!zombieList) { /* special case for an empty list */
    zombieList = newListEntry;
    zombieList->next = NULL;
    zombieList->prev = NULL;
    zombieListEnd = zombieList;
  } /* if */
  else {
    zombieListEnd->next = newListEntry;
    newListEntry->prev = zombieListEnd;
    zombieListEnd = newListEntry;
    zombieListEnd->next = NULL;
  } /* else */
  totalZombies++;
  SDL_mutexV(zombieLock);
  fprintf(stderr, "BRAINS!!!@\n");
} /* newZombie */

void deleteZombie(zlist_t *zombie) {
  SDL_mutexP(zombieLock); /* no one else can play with the zombies during death */
 
  if (!zombie->prev) { // first zombie
    if (!zombie->next) // AND last zombie
      zombieList = zombieListEnd = NULL;
    else {
      zombieList = zombie->next;
      zombie->next->prev = NULL;
    } /* else */
  } /* if */
  else if (!zombie->next) {
    zombieListEnd = zombie->prev;
    zombie->prev->next = NULL;
  } /* if */
  else {
    zombie->next->prev = zombie->prev;
    zombie->prev->next = zombie->next;
  } /* else */
  totalZombies--;
  SDL_mutexV(zombieLock);
  free(zombie);
} /* deleteZombie */

void updateZombiePosition(zombie_t *zombie, int width, int height) {
  int wiggle = 0;

  zombie->x += zombie->dx;
  zombie->y += zombie->dy;

  if (zombie->x - width / 2 <= 0) {
    zombie->x = width / 2;
    wiggle = 1;
  } /* if */
  else if (zombie->x + width / 2 >= 800) {
    zombie->x = 800 - width / 2;
    wiggle = 1;
  } /* else */

  if (zombie->y - height / 2 <= 0) {
    zombie->y = height / 2;
    wiggle = 1;
  } /* if */
  else if (zombie->y + height / 2 >= 600) {
    zombie->y = 600 - height / 2;
    wiggle = 1;
  } /* if */

  if (wiggle) {
    zombie->dx = (rand() % (MAXSPEED *2)) - MAXSPEED;    
    zombie->dy = (rand() % (MAXSPEED *2)) - MAXSPEED;    
    zombie->direction = zombieDirection(zombie->dx, zombie->dy);
  } /* wiggle */
} /* updateZombiePosition */

