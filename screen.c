#include "includes.h"

static SDL_Surface *screen;

toggleFullScreen() {
  SDL_Surface *screen;

  screen = SDL_GetVideoSurface();
  SDL_WM_ToggleFullScreen(screen);
} /* toggleFullScreen */

void *doBlit (int srcX, 
	      int srcY, 
	      int srcW, 
	      int srcH, 
	      int destX, 
	      int destY, 
	      int destW, 
	      int destH,
	      SDL_Surface *bmp) {
  SDL_Rect src, dest;
  
  src.x = srcX;
  src.y = srcY;
  src.w = srcW;
  src.h = srcH;
  
  dest.x = destX;
  dest.y = destY;
  dest.w = destW;
  dest.h = destH;

  SDL_BlitSurface(bmp, &src, screen, &dest);
} /* doBlit */

/*
  Function to Load, Convert and then set color key of a bmp file.
  if RGB is 0,0,0 do not set a color key for that bmp.
*/
static SDL_Surface *fetchBitmap(char *filespec, int r, int g, int b) {
  SDL_Surface *rawBMP, *convertedBMP;

  fprintf(stderr, "loading \"%s\"\n", filespec);
  rawBMP = SDL_LoadBMP(filespec);
  if (!rawBMP) {
    fprintf (stderr, "Unable to load bitmap %s\n", filespec);
    myExit (-10);
  } /* if */

  fprintf(stderr, "converting \"%s\"\n", filespec);
  convertedBMP = SDL_DisplayFormat(rawBMP);
  if (!convertedBMP) {
    fprintf (stderr, "Unable to convert bitmap %s\n", filespec);
    myExit (-10);
  } /* if */

  if (!((r == 0) && (g == 0) && (b == 0)))
    SDL_SetColorKey(convertedBMP, SDL_SRCCOLORKEY, (SDL_MapRGB(convertedBMP->format, r, g, b)));
  
  SDL_FreeSurface(rawBMP);
  return(convertedBMP);
} /* fetchBitmap */

int ScreenThreadEntryPoint(void *data) {
  char *threadname;
  SDL_Surface *arenaBMP, *scoreboardBMP, *fontBMP;
  SDL_Surface *shotBMP[8], *zombieBMP[8], *playerBMP[8];
  SDL_Surface *wordPane, *scorePane;
  SDL_Color scoreColor;

  threadname = (char *) data;
  char buffer[1025];
  char scoreString[14];
  int i;
  zlist_t *currentZombie;

  screen =  SDL_SetVideoMode(800, 700, 16, SDL_DOUBLEBUF);
  if (screen == NULL) {
    fprintf(stderr, "Unable to set video mode: %s\n", SDL_GetError());
    myExit (-10);
  } /* if */
  
  arenaBMP      = fetchBitmap("bitmaps/arena.bmp", 0,0,0);
  scoreboardBMP = fetchBitmap("bitmaps/scoreboard.bmp", 0,0,0);
  fontBMP       = fetchBitmap("bitmaps/zombieFont.bmp", 255,255,255);

  scoreColor.r = 0;
  scoreColor.b = 70;
  scoreColor.g = 90;

  for (i=0; i<9; i++) {
    sprintf((char *) &buffer, "bitmaps/Player.%i.bmp", i);
    playerBMP[i] = fetchBitmap(buffer, 255, 255, 255);

    sprintf((char *) &buffer, "bitmaps/Zombie.%i.bmp", i);
    zombieBMP[i] = fetchBitmap(buffer, 255, 255, 255);

    sprintf((char *) &buffer, "bitmaps/Shot.%i.bmp", i);
    shotBMP[i] = fetchBitmap(buffer, 255, 255, 255);


  } /* for */

  /* loop until main() sets the flag */
  while(!exit_flag) {

    /* update the player */
    updatePlayerPosition (playerBMP[player.direction]->h, playerBMP[player.direction]->w);

    /* update the shot */
    if (shot.show)
      updateShotPosition();

    /* update the zombies */
    currentZombie = zombieList;
    while(currentZombie) {
      updateZombiePosition(currentZombie->zombie, 
			   zombieBMP[currentZombie->zombie->direction]->w, 
			   zombieBMP[currentZombie->zombie->direction]->h);
      currentZombie = currentZombie->next;
    } /* while */

    /* check for collisions */
    collisionDetect();

    /* draw background and score board */
    doBlit(0, 0, arenaBMP->w, arenaBMP->h, 0, 0, arenaBMP->w, arenaBMP->h, arenaBMP);
    doBlit(0, 0, scoreboardBMP->w, scoreboardBMP->h, 0,600, scoreboardBMP->w, scoreboardBMP->h, scoreboardBMP);

    /* draw the word */
    wordPane = TTF_RenderText_Blended(wordFont, word, wordColor);
    doBlit(0, 0, wordPane->w, wordPane->h, 130, 602, wordPane->w, wordPane->h, wordPane);

    /* draw the score */
    sprintf(scoreString, "%i", score);
    scorePane = TTF_RenderText_Blended(scoreFont, scoreString, scoreColor);
    doBlit(0, 0, scorePane->w, scorePane->h, 150, 647, scorePane->w, scorePane->h, scorePane);

    /* draw the player */
    doBlit(0, 0, playerBMP[player.direction]->w, playerBMP[player.direction]->h,
	   player.x - playerBMP[player.direction]->w / 2, player.y - playerBMP[player.direction]->h / 2,
	   playerBMP[player.direction]->w, playerBMP[player.direction]->h, playerBMP[player.direction]);

    /* draw any zombies that are on screen */
    currentZombie = zombieList;
    while(currentZombie) {
      doBlit(0, 0, zombieBMP[currentZombie->zombie->direction]->w, zombieBMP[currentZombie->zombie->direction]->h,
	     currentZombie->zombie->x - zombieBMP[currentZombie->zombie->direction]->w / 2, 
	     currentZombie->zombie->y - zombieBMP[currentZombie->zombie->direction]->h / 2,
	     zombieBMP[currentZombie->zombie->direction]->w, zombieBMP[currentZombie->zombie->direction]->h, 
	     zombieBMP[currentZombie->zombie->direction]);
      doBlit(1084 + ((currentZombie->zombie->letter - 65) * 34), 0, 30, 30, 
	     currentZombie->zombie->x - zombieBMP[currentZombie->zombie->direction]->w / 2, 
	     currentZombie->zombie->y - zombieBMP[currentZombie->zombie->direction]->h / 2,
	     30, 30, fontBMP);
      currentZombie = currentZombie->next;
    } /* while */

    /* draw the shot */
    if(shot.show)
      doBlit(0, 0, shotBMP[shot.direction]->w, shotBMP[shot.direction]->h,
	     shot.x - shotBMP[shot.direction]->w / 2, shot.y - shotBMP[shot.direction]->h / 2,
	     shotBMP[shot.direction]->w, shotBMP[shot.direction]->h, shotBMP[shot.direction]);

    /* now update the whole screen */
    SDL_UpdateRect(screen, 0, 0, 0, 0);

    SDL_Delay(20);
  } /* while */

  /* clean up and die */
  SDL_FreeSurface(arenaBMP);
  SDL_FreeSurface(scoreboardBMP);
  for (i=0; i<9; i++) {
    SDL_FreeSurface(playerBMP[i]);
    SDL_FreeSurface(shotBMP[i]);
  } /* for */
  fprintf(stderr, "%s is now exiting.\n", threadname);
  myExit(1);
} /* ScreenThreadEntryPoint */

