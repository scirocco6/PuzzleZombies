#include <SDL/SDL_ttf.h>
#include "includes.h"

TTF_Font *font;

setupFonts() {
  if(TTF_Init()) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    myExit(-60);
  } /* if */

  font=TTF_OpenFont("FreeSerif.ttf", 16);
  if(!font) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    myExit(-60);
  } /* if */
} /* setupFonts */

scribeSurface(SDL_Surface *surface, char *message, SDL_Color color) {
  if(!(surface = TTF_RenderText_Solid(font, message, color)))
    fprintf(stderr, "TTF_RenderText_Solid: %s\n", TTF_GetError());
} /* scribeSurface */
