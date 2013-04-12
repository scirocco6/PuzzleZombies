#include <string.h>
#include "includes.h"

/* Structure for loaded sounds */
typedef struct sound_s {
  Uint8 *samples;  /* raw PCM sample data */
  Uint32 length;   /* size of sound data in bytes */
} sound_t, *sound_p;

/* Structure for a currently playing sound */
typedef struct playing_s {
  int active;       /* 1 if this sound should be played */
  sound_p sound;    /* sound data to play */
  Uint32 position;  /* current position in the sound buffer */
} playing_t, *playing_p;

/* Array for all active sound effects */
#define MAX_PLAYING_SOUNDS    10
playing_t playing[MAX_PLAYING_SOUNDS];

/* linked list of sound requests from other threads */
alist_t *audioList, *audioListEnd;


void addSound(int soundNumber) {
  alist_t *newSound;

  /* handles requests from other threads to have a sound played */
  SDL_mutexP(add_audio);
  newSound = malloc(sizeof(alist_t));
  if (!newSound)
    return;
  newSound->sampleNumber = soundNumber;

  if (!audioList) { /* special case for an empty list */
    audioList = newSound;
    audioList->next = NULL;
    audioListEnd = audioList;
  } /* if */
  else {
    audioListEnd->next = newSound;
    audioListEnd = newSound;
    audioListEnd->next = NULL;
  } /* else */

  SDL_mutexV(add_audio);
} /* add Sound */

int getSound() {
  /* returns the next requested sound number */

  alist_t *temp;
  int soundNumber;

  SDL_mutexP(add_audio);

  temp = audioList;
  soundNumber = audioList->sampleNumber;
  if (audioListEnd == audioList) /* special condition to remove the last sound */
    audioListEnd = NULL;
  audioList = audioList->next;
  SDL_mutexV(add_audio);

  free(temp);
  return(soundNumber);
} /* getSound */		  

/* 
   The higher this is, the louder each currently playing sound will be. 
   However, high values may cause distortion if too many sounds are playing.
   Experiment with this value 
*/
#define VOLUME_PER_SOUND   SDL_MIX_MAXVOLUME / 2

/*
  This function is called by the SDL whenever the soundcard needs more 
   samples to play. It might be called from a separate thread, so we should 
   be careful what we touch.
*/
void audioCallback(void *user_data, Uint8 *audio, int length) {
  int i;
  
  /* clear the audio buffer so we can mix samples into it */
  memset(audio, 0, length);
  
  /* mix in each sound */
  for (i = 0; i < MAX_PLAYING_SOUNDS; ++i) {
    if (playing[i].active) {
      Uint8 *sound_buf;
      Uint32 sound_len;
      
      /* Locate this sound's current buffer position */
      sound_buf = playing[i].sound->samples;
      sound_buf += playing[i].position;
      
      /* Determine the number of samples to mix */
      if ((playing[i].position + length) > playing[i].sound->length)
	sound_len = playing[i].sound->length - playing[i].position;
      else
	sound_len = length;
      
      /* Mix this sound into the stream */
      SDL_MixAudio(audio, sound_buf, sound_len, VOLUME_PER_SOUND);
      
      /* Update the sound buffer's position */
      playing[i].position += length;
      
      /* Have we reached the end of the sound? */
      if (playing[i].position >= playing[i].sound->length)
	playing[i].active = 0; /* mark as inactive */
    } /* if */
  } /* for */
} /* audioCallback

/* This function loads a sound with SDL_LoadWAV and converts it to the 
   specified sample format. Returns 0 on success and 1 on failure */

int LoadAndConvertSound(char *filename, SDL_AudioSpec *spec, sound_p sound) {
  SDL_AudioCVT cvt;     /* format conversion structure */
  SDL_AudioSpec loaded; /* format of loaded data */
  Uint8 *new_buf;

  /* Load the WAV file in it's original format */
  if (SDL_LoadWAV(filename, &loaded, &sound->samples, &sound->length) == NULL) {
    printf("Unable to load sound: %s\n", SDL_GetError());
    return 1;
  } /* if */

  /* 
     Build a conversion structure for converting the samples. This structure 
     contains the data SDL needs to quickly convert between sample formats 
*/
  
  if (SDL_BuildAudioCVT(&cvt, loaded.format, loaded.channels, loaded.freq,
			spec->format, spec->channels, spec->freq) < 0) {
      fprintf(stderr, "Unable to convert sound: %s\n", SDL_GetError());
      myExit(-40);
  } /* if */

  /* 
     Since converting PCM samples can result in more data (for instance,
     converting 8-bit mono to 16-bit stereo), we need to allocate a new 
     buffer for the converted data. Fortunately, SDL_BuildAudioCVT 
     supplied the necessary information 
  */
  cvt.len = sound->length;
  new_buf = (Uint8 *) malloc(cvt.len * cvt.len_mult);
  if (new_buf == NULL) {
    fprintf(stderr, "Memory allocation for conversion failed\n");
    SDL_FreeWAV(sound->samples);
    myExit(-40);
  } /* if */

  /* Copy the sound samples into the new buffer */

  memcpy(new_buf, sound->samples, sound->length);

  /* Perform the conversion on the new buffer */
  cvt.buf = new_buf;
  if (SDL_ConvertAudio(&cvt) < 0) {
      fprintf(stderr, "Audio conversion error: %s\n", SDL_GetError());
      free(new_buf);
      SDL_FreeWAV(sound->samples);
      myExit(-40);
  } /* if */

  /* Swap the converted data for the original */
  SDL_FreeWAV(sound->samples);
  sound->samples = new_buf;
  sound->length = sound->length * cvt.len_mult;
  
  /* Success! */
  
  printf("%s was loaded and converted successfully\n", filename);
} /*LoadAndConvertSound */

/* Removes all currently playing sounds */

void ClearPlayingSounds(void) {
  int i;
  for (i = 0; i < MAX_PLAYING_SOUNDS; ++i)
    playing[i].active = 0;
} /* ClearPlayingSounds */

/* 
   Adds a sound to the list of currently playing sounds. audioCallback will
   start mixing this sound into the stream the next time it is called
   (probably in a fraction of a second)
*/

int PlaySound(sound_p sound) {
  int i;

  /* Find an empty slot for this sound */
  for (i = 0; i < MAX_PLAYING_SOUNDS; ++i)
    if (playing[i].active == 0)
      break;

  /* Report failure if there are no slots */
  if (i == MAX_PLAYING_SOUNDS)
    return 1;

  /* The 'playing' structures are accessed by the audio callback, so we should 
     obtain a lock before we access them */

  SDL_LockAudio();
  playing[i].active = 1;
  playing[i].sound = sound;
  playing[i].position = 0;
  SDL_UnlockAudio();
} /* PlaySound */

int AudioThreadEntryPoint(void *data) {
  char *threadname;
  sound_t cannon, explosion, ShotHitWall;

  threadname = (char *) data;

  /* Audio format specs */
  SDL_AudioSpec desired, obtained;


  /* Open the audio device. The sound driver will try to give us the requested 
     format, but it might not succeed. The 'obtained' structure will be 
     filled in with the actual format data */
  
  desired.freq = 44100;       /* desired output sample rate */
  desired.format = AUDIO_S16; /* request signed 16 bit samples */
  desired.samples = 4096;     /* this is somewhat arbitary */
  desired.channels = 2;       /* ask for stereo */
  desired.callback = audioCallback;
  desired.userdata = NULL;    /* we don't need this */
  
  if (SDL_OpenAudio(&desired, &obtained) < 0) {
    fprintf(stderr, "Unable to open audio device: %s\n", SDL_GetError());
    myExit(-40);
  } /* if */

  if (LoadAndConvertSound("audio/cannon.wav", &obtained, &cannon) != 0) {
    fprintf(stderr, "Unable to load cannon.wav\n");
    myExit(-40);
  } /* if */

  if (LoadAndConvertSound("audio/explosion.wav", &obtained, &explosion) != 0) {
    printf("Unable to load explosion.wav\n");
    myExit(-40);
  } /* if */

  if (LoadAndConvertSound("audio/ShotHitWall.wav", &obtained, &ShotHitWall) != 0) {
    printf("Unable to load ShotHitWall.wav\n");
    myExit(-40);
  } /* if */

  ClearPlayingSounds();

  SDL_PauseAudio(0);   /* SDL's audio is initially paused, start it. */

  while(!exit_flag) {
    while(audioList) {
      switch(getSound()) {
      case 0: // SHOOT
   	PlaySound(&cannon);
   	break;
      case 1: // HIT
   	PlaySound(&explosion);
   	break;
      case 2: //ShotHitWall
	PlaySound(&ShotHitWall);
	break;
      default:
	break;
      } /* switch */
    } /* while */
    
    SDL_Delay(60);
  } /* while */
  fprintf(stderr, "%s is now exiting.\n", threadname);
  
  /* pause and lock the sound system so we can safely delete our sound data */
  SDL_PauseAudio(1);
  SDL_LockAudio();

  free(cannon.samples);
  free(explosion.samples);

  /* At this point the output is paused and we know for certain that the 
     callback is not active, so we can safely unlock the audio system */
  SDL_UnlockAudio();

  myExit(-40);
} /* AudioThreadEntryPoint */

