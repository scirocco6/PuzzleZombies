#include "includes.h"

int ScreenThreadEntryPoint(void *);
int InputThreadEntryPoint(void *);
int AudioThreadEntryPoint(void *);

void myExit(int reason) {
  exit_flag = 1;  /* signal the threads to exit */
  SDL_Delay(3500);  /* Give all threads time to notice the flag and exit. */
  SDL_Quit;
  SDL_CloseAudio;
  SDL_DestroyMutex(add_audio);
  exit(reason);
} /* myExit */

int main() {
  SDL_Thread *screenThread, *inputThread, *audioThread;
  time_t now, lastZombie;
  

  add_audio = SDL_CreateMutex();
  zombieLock = SDL_CreateMutex();

  srand(time(NULL));
  setupPlayer();
  exit_flag = score = totalZombies = 0;
  level = 1;
  loadWordList();
  newWord();
  enteringWord = randomWord();
  lastZombie = time(NULL);

  /* initialize SDL's video system and check for errors */
  if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
    fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
    myExit(-1);
  } /* if */

  /* initialize font schtuff */
  if(TTF_Init()) {
    fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
    myExit(-1);
  } /* if */

  wordFont=TTF_OpenFont("FreeSerif.ttf", 36);
  scoreFont=TTF_OpenFont("FreeSerif.ttf", 36);
  
  if(!wordFont || !scoreFont) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    myExit(-1);
  } /* if */
  
  SDL_WM_SetCaption("Puzzle Zombies", "Puzzle Zombies");

  screenThread = SDL_CreateThread(ScreenThreadEntryPoint, "screenThread");
  if (!screenThread) {
    fprintf(stderr, "failed to create screenThread\n");
    myExit(-1);
  } /* if */

  inputThread  = SDL_CreateThread(InputThreadEntryPoint, "inputThread");
  if (!inputThread) {
    fprintf(stderr, "failed to create inputThread\n");
    myExit(-1);
  } /* if */

  audioThread  = SDL_CreateThread(AudioThreadEntryPoint, "audioThread");
  if (!audioThread) {
    fprintf(stderr, "failed to create audioThread\n");
    myExit(-1);
  } /* if */

  while(!exit_flag) {
    now = time(NULL);
    if (((totalZombies < level * 2 +5) && (now - lastZombie > 3)) ||
	(now - lastZombie > 120 - level)) {
      newZombie();
      lastZombie = now;
    } /* if */
    SDL_Delay(100);
  } /* while */
    myExit(0);
} /* main */
