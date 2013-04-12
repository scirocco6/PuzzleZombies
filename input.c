#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include "includes.h"

int InputThreadEntryPoint(void *data) {
  SDL_Event event;
  SDL_Joystick *js;
  int num_js, i;

  char *threadname;

  threadname = (char *) data;


  /* by default keyboard repeat is disabled in SDL 
     use SDL_EnableKeyrepeat(delay, rate) to enable it */
  /* SDL_EnableKeyRepeat(350, 30); */
  
  num_js = SDL_NumJoysticks();
  if (num_js == 0) {
    fprintf(stderr, "No joysticks were detected.\n");
    myExit (-20);
  } /* if */

  js = SDL_JoystickOpen(0);
  if (js == NULL) {
    fprintf(stderr, "Unable to open joystick: %s\n.", SDL_GetError());
    myExit (-20);
  } /* if */

  while((!exit_flag) && (SDL_WaitEvent(&event) != 0)) {
    SDL_keysym keysym;
    
    /* SDL_WaitEvent has filled in our event structure with the next event.
       We check its type field to find out what has happened */
    
    switch(event.type) {
    case SDL_JOYAXISMOTION:
      if ((event.jaxis.which == 0) && (event.jaxis.axis <2))
	movePlayer(event.jaxis.axis, event.jaxis.value);
      break;
      
    case SDL_JOYBUTTONUP:
      break;

    case SDL_JOYBUTTONDOWN:
      if (event.jbutton.which == 0)
	switch(event.jbutton.button) {
	case 0:
	  delLetter();
	  break;
	case 2:
	  fireShot();
	  break;
	case 3:
	  scoreWord();
	  break;
	} /* switch */
      break;

    case SDL_KEYDOWN:
      keysym = event.key.keysym;

      /* toggle full screen mode */
      if (keysym.sym == SDLK_f) {
	fprintf(stderr, "'F' pressed, toggling fullscreen.\n");
	toggleFullScreen();
      } /* if */

      /* would you like a zombie? */
      if (keysym.sym == SDLK_z) {
	fprintf(stderr, "'Z' pressed, sending in a zombie.\n");
	newZombie();
      } /* if */
      
      /* did the user press the Q key? */
      if (keysym.sym == SDLK_q) {
	fprintf(stderr, "'Q' pressed, exiting.\n");
	myExit(0);
      } /* if */
      break;
      
    case SDL_KEYUP:
      break;
      
    case SDL_QUIT:
      fprintf(stderr, "Quit event. Bye.\n");
      exit_flag = 1;
      break;
    } /* switch */
  } /* while */
  SDL_JoystickClose(js);
  fprintf(stderr, "%s is now exiting.\n", threadname);
  
  myExit(1);
} /* InputThreadEntryPoint */
