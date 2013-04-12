#include "includes.h"

void setupPlayer() {
  player.x         = 400;
  player.y         = 300;
  player.dx        = 0;
  player.dy        = 0;
  player.direction = 0;

  shot.show = 0;
} /* setupPlayer */ 

void fireShot() {
  fprintf(stderr, "FIRE!!!@\n");
  //  if ((shot.show) || (!player.direction)) /* player must be moving and only one shot is allowed */
  if (!player.direction) /* player must be moving to shoot */
    return;

  shot.show = 1;
  shot.direction = player.direction;
  shot.x = player.x;
  shot.y = player.y;

  switch(shot.direction) {
  case 1:
    shot.dx = 0;
    shot.dy = -7;
    break;
  case 2:
    shot.dx = 7;
    shot.dy = -7;
    break;
  case 3:
    shot.dx = 7;
    shot.dy = 0;
    break;
  case 4:
    shot.dx = 7;
    shot.dy = 7;
    break;
  case 5:
    shot.dx = 0;
    shot.dy = 7;
    break;
  case 6:
    shot.dx = -7;
    shot.dy = 7;
    break;
  case 7:
    shot.dx = -7;
    shot.dy = 0;
    break;
  case 8:
    shot.dx = -7;
    shot.dy = -7;
    break;
  } /* switch */  
  
  addSound(SHOOT);
} /* fireShot */

void movePlayer(int axis , int value) {
  /* stopped */
  if ((value > -1000) && (value < 1000))
    if (axis == 0)
      player.dx = 0;
    else 
      player.dy = 0;
  
  /* sneeking */
  else if ((value > 1000) && (value < 20000))
    if (axis == 0)
      player.dx = 1;
    else 
      player.dy = 1;
  
  else if((value < -1000) && (value > -20000))
    if (axis == 0)
      player.dx = -1;
    else 
      player.dy = -1;
  
  /* walking */
  else if ((value > 20000) && (value < 30000))
    if (axis == 0)
      player.dx = 2;
    else 
      player.dy = 2;
  
  else if((value < -20000) && (value > -30000))
    if (axis == 0)
      player.dx = -2;
    else 
      player.dy = -2;
  
  /* running */
  else if (value > 30000)
    if (axis == 0)
      player.dx = 3;
    else 
      player.dy = 3;
  
  else if(value < -30000)
    if (axis == 0)
      player.dx = -3;
    else 
      player.dy = -3;
  
  /* set player bitmap direction */
  if (player.dx > 0)
    if (player.dy < 0)
      player.direction = 2;
    else if (player.dy > 0)
      player.direction = 4;
    else
      player.direction = 3;
  
  else if (player.dx < 0)
    if (player.dy > 0)
      player.direction = 6;
    else if (player.dy < 0)
      player.direction = 8;
    else
      player.direction = 7;
  
  else
    if (player.dy > 0)
      player.direction = 5;
    else if (player.dy < 0)
      player.direction = 1;
    else
      player.direction = 0;
} /* movePlayer */

void updatePlayerPosition(int height, int width) { /* height and width are the size of the current player bitmap */
    player.x += player.dx;
    player.y += player.dy;

    if (player.x - width / 2 <= 0)
      player.x = width / 2;
    else if (player.x + width / 2 >= 800)
    player.x = 800 - width / 2;
    
    if (player.y - height / 2 <= 0)
      player.y = height / 2;
    else if (player.y + height / 2 >= 600)
      player.y = 600 - height / 2;
} /* updatePlayerPosition */

void updateShotPosition() { /* height and width are the size of the current shot bitmap */
    shot.x += shot.dx;
    shot.y += shot.dy;

    if ((shot.x  <= 0) || 
	(shot.x  >= 800) || 
	(shot.y  <= 0) ||
	(shot.y  >= 600)) {
      shot.show = 0;
      addSound(HITWALL);
    } /* if */

} /* updateShotPosition */

