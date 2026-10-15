#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WClassFast.h"

// Animal Class
static const char fastAnimal_speek[] = "The animal says: Animals make many different sounds";
 
static void default_fastAnimal_sound(const FastAnimal* self) {
    printf("%s\n", fastAnimal_speek);
}

static char* default_fastAnimal_returnSound(const FastAnimal* self) {
    const int len = strlen(fastAnimal_speek);
    char* returnStr = malloc(sizeof(char)*(len+1));
    memcpy(returnStr, fastAnimal_speek, len+1);
    return returnStr;
}

FastAnimal* fastAnimal_create() {
    FastAnimal* a = malloc(sizeof(FastAnimal));
    if(!a) return NULL;
    a->sound = default_fastAnimal_sound;
    a->returnSound = default_fastAnimal_returnSound;
    return a;
}

// Pig class
static const char fastPig_speek[] = "The pig says: wee wee";

static void fastPig_sound(const FastAnimal* self) {
    printf("%s\n", fastPig_speek);
}

static char* fastPig_returnSound(const FastAnimal* self) {
    const int len = strlen(fastPig_speek);
    char* returnStr = malloc(sizeof(char)*(len+1));
    memcpy(returnStr, fastPig_speek, len+1);
    return returnStr;
}

FastAnimal* fastPig_create() {
    FastPig* a = malloc(sizeof(FastPig));
    if(!a) return NULL;
	a->baseFastAnimal.sound = fastPig_sound;
    a->baseFastAnimal.returnSound = fastPig_returnSound;
    return (FastAnimal*)a;
}

// Dog class
static const char fastDog_speek[] = "The dog says: bow wow";

static void fastDog_sound(const FastAnimal* self) {
    printf("%s\n", fastDog_speek);
}

static char* fastDog_returnSound(const FastAnimal* self) {
    const int len = strlen(fastDog_speek);
    char* returnStr = malloc(sizeof(char)*(len+1));
    memcpy(returnStr, fastDog_speek, len+1);
    return returnStr;
}

FastAnimal* fastDog_create() {
    FastDog* a = malloc(sizeof(FastDog));
    if(!a) return NULL;
	a->baseFastAnimal.sound = fastDog_sound;
    a->baseFastAnimal.returnSound = fastDog_returnSound;
    return (FastAnimal*)a;
}

//ColorPig Class
static const char fastColorPigStr[] = "The pigs color is";

static char* fastColorPig_color(const FastColorAnimal* self) {
    int len = strlen(fastColorPigStr);
    char* returnStr = malloc(sizeof(char)*(len+4));
    memcpy(returnStr, fastColorPigStr, len);
	returnStr[len+3] = '\0';
    returnStr[len+2] = self->animalColor;
    returnStr[len+1] = ' ';
    returnStr[len] = ':';
    return returnStr;
}

FastAnimal* fastColorPig_create(char input) {
    FastColorPig* a = malloc(sizeof(FastColorPig));
    if(!a) return NULL;
	a->baseFastColorAnimal.baseFastAnimal.sound = fastPig_sound;
	a->baseFastColorAnimal.baseFastAnimal.returnSound = fastPig_returnSound;
    a->baseFastColorAnimal.color = fastColorPig_color;
    a->baseFastColorAnimal.animalColor = input;
    return (FastAnimal*)a;
}

//ColorDog Class
static const char fastColorDogStr[] = "The dogs color is";

static char* fastColorDog_color(const FastColorAnimal* self) {
    int len = strlen(fastColorDogStr);
    char* returnStr = malloc(sizeof(char)*(len+4));
    memcpy(returnStr, fastColorDogStr, len);
	returnStr[len+3] = '\0';
    returnStr[len+2] = self->animalColor;
    returnStr[len+1] = ' ';
    returnStr[len] = ':';
    return returnStr;
}

FastAnimal* fastColorDog_create(char input) {
    FastColorDog* a = malloc(sizeof(FastColorDog));
    if(!a) return NULL;
	a->baseFastColorAnimal.baseFastAnimal.sound = fastDog_sound;
	a->baseFastColorAnimal.baseFastAnimal.returnSound = fastDog_returnSound;
    a->baseFastColorAnimal.color = fastColorDog_color;
    a->baseFastColorAnimal.animalColor = input;
    return (FastAnimal*)a;
}