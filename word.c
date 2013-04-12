#include "includes.h"
static char *words;
char currentWord[MAXLETTERS];

char *randomWord() {
  char *w;

  w = (rand() % strlen(words)) + words;
  while (*(w - 1) != ':')
    w--;
  return(w);
} /* randomWord */

void setWordColor() {
  if (isaWord()) {
    wordColor.r = 0;
    wordColor.b = 0;
    wordColor.g = 200;
  } /* if */
  else {
    wordColor.r = 200;
    wordColor.b = 0;
    wordColor.g = 0;
  } /* else */
} /* setWordColor */

void newWord() {
  *word = *currentWord = 0;
  strcpy(word, "_ _ _ _ _ _ _");
  currentLetter = -1;
  setWordColor();
} /* newWord */

void addLetter(char *letter) {
  currentLetter++;
  if (currentLetter == 7)
    return;
  strncpy(&word[currentLetter*2], letter, 1);
  currentWord[currentLetter] = tolower(*letter);
  currentWord[currentLetter + 1] = 0;
  fprintf(stderr, "The current word is \"%s\"\n", currentWord);
  setWordColor();
} /* addLetter */

void delLetter() {
  if (currentLetter == -1)
    return;
  strncpy(&word[currentLetter*2], "_", 1);
  currentWord[currentLetter] = 0;
  currentLetter--;
  if(score)
    score--;
  setWordColor();
} /* delLetter */

int isaWord() {
  char testWord[MAXLETTERS+3];

  sprintf(testWord, ":%s:", currentWord);
  if (strstr(words, testWord))
    return 1;
  return 0;
} /* isaWord */

void scoreWord() {
  static int letterValue[26] = {13,3,2,1,4,2,4,1,8,5,1,3,1,1,3,10,1,1,1,4,4,8,4,10};
  int i;

  if (!isaWord())
    return;

  for(i=0; i <= currentLetter; i++) {
    fprintf(stderr, "adding %i to score\n", letterValue[currentWord[i] - 97]);
    score += letterValue[currentWord[i] - 97];
  } /* for */
  newWord();


} /* scoreWord */

char *loadWordList() {
  FILE *fp;
  char *line = NULL, *nl;
  size_t len = 0, size = 10240;
  ssize_t read;

  size = 10240;
  
  words = malloc(size);
  *words = 0;
  strcat(words, ":");

  fp = fopen("WordList.dat", "r");
  if (!fp)
    myExit(-60);
  while ((read = getline(&line, &len, fp)) != -1) {
    if (strlen(words) + read + 2 >= size) {
      size += 10240;
      words = realloc(words, size);
    } /* if */
    if(nl = strstr(line, "\n"))
      *nl = 0;
    strcat(words, line);
    strcat(words, ":"); /* why yes this IS remarkably lazy */
  } /* while */
  if (line)
    free(line);
  return EXIT_SUCCESS;
} /* loadWordList */
