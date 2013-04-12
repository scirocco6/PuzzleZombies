#include "includes.h"

void collisionDetect() {
  zlist_t *currentZombie;

  currentZombie = zombieList;
  while(shot.show && currentZombie) {
    fprintf(stderr, "shot x,y: %i, %i   Zombie x,y: %i, %i\n", shot.x, shot.y, 
	    currentZombie->zombie->x, currentZombie->zombie->y);
    if(((shot.x - currentZombie->zombie->x > -16) && (shot.x - currentZombie->zombie->x < 16)) &&
       ((shot.y - currentZombie->zombie->y > -16) && (shot.y - currentZombie->zombie->y < 16))) {
      shot.show = 0; /* remove the shot from the screen */
      score ++;
      addLetter(&currentZombie->zombie->letter);
      addSound(1); /* make some sound */
      deleteZombie(currentZombie);
      break;
    } /* if */
    currentZombie = currentZombie->next;
  } /* while */
} /* collisionDetect */


