#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "WClass.h"

// Animal Class
static const char animal_speek[] = "The animal says: Animals make many different sounds";
 
static void default_animal_sound(const Animal* self) {
	
	char* str = "hello";
	str[0] = 'H';
	printf("%s\n",str);
	
    printf("%s\n", animal_speek);
}

static char* default_animal_returnSound(const Animal* self) {
    const int len = strlen(animal_speek);
    char* returnStr = malloc(sizeof(char)*(len+1));
    memcpy(returnStr, animal_speek, len+1);
    return returnStr;
}

const AnimalVTable animal_vtable = {
    .sound = default_animal_sound,
    .returnSound = default_animal_returnSound
};

Animal* animal_create() {
    Animal* a = malloc(sizeof(Animal));
    if(!a) return NULL;
    a->vtable = &animal_vtable;
    return a;
}

// Pig class
static const char pig_speek[] = "The pig says: wee wee";

static void pig_sound(const Animal* self) {
    printf("%s\n", pig_speek);
}

static char* pig_returnSound(const Animal* self) {
    const int len = strlen(pig_speek);
    char* returnStr = malloc(sizeof(char)*(len+1));
    memcpy(returnStr, pig_speek, len+1);
    return returnStr;
}

const AnimalVTable pig_vtable = {
    .sound = pig_sound,
    .returnSound = pig_returnSound
};

Animal* pig_create() {
    Pig* a = malloc(sizeof(Pig));
    if(!a) return NULL;
    a->base.vtable = &pig_vtable;
    return (Animal*)a;
}

// Dog class
static const char dog_speek[] = "The dog says: bow wow";

static void dog_sound(const Animal* self) {
    printf("%s\n", dog_speek);
}

static char* dog_returnSound(const Animal* self) {
    const int len = strlen(dog_speek);
    char* returnStr = malloc(sizeof(char)*(len+1));
    memcpy(returnStr, dog_speek, len+1);
    return returnStr;
}

const AnimalVTable dog_vtable = {
    .sound = dog_sound,
    .returnSound = dog_returnSound
};

Animal* dog_create() {
    Dog* a = malloc(sizeof(Dog));
    if(!a) return NULL;
    a->base.vtable = &dog_vtable;
    return (Animal*)a;
}

//ColorPig Class
static const char colorPigStr[] = "The pigs color is";

static char* colorPig_color(const ColorAnimal* self) {
    int len = strlen(colorPigStr);
    char* returnStr = malloc(sizeof(char)*(len+4));
    memcpy(returnStr, colorPigStr, len);
    returnStr[len+3] = '\0';
    returnStr[len+2] = self->animalColor;
    returnStr[len+1] = ' ';
    returnStr[len] = ':';
    return returnStr;
}

const ColorAnimalVTable colorPig_vtable = {
    .base_vtable = {
        .sound = pig_sound,
        .returnSound = pig_returnSound
    },
    .color = colorPig_color
};

Animal* colorPig_create(char input) {
    ColorPig* a = malloc(sizeof(ColorPig));
    if(!a) return NULL;
    a->baseColorAnimal.baseAnimal.vtable = (const AnimalVTable*)&colorPig_vtable;
    a->baseColorAnimal.animalColor = input;
    return (Animal*)a;
}

//ColorDog Class
static const char colorDogStr[] = "The dogs color is";

static char* colorDog_color(const ColorAnimal* self) {
    int len = strlen(colorDogStr);
    char* returnStr = malloc(sizeof(char)*(len+4));
    memcpy(returnStr, colorDogStr, len);
	returnStr[len+3] = '\0';
    returnStr[len+2] = self->animalColor;
    returnStr[len+1] = ' ';
    returnStr[len] = ':';
    return returnStr;
}

const ColorAnimalVTable colorDog_vtable = {
    .base_vtable = {
        .sound = dog_sound,
        .returnSound = dog_returnSound
    },
    .color = colorDog_color
};

Animal* colorDog_create(char input) {
    ColorDog* a = malloc(sizeof(ColorDog));
    if(!a) return NULL;
    a->baseColorAnimal.baseAnimal.vtable = (const AnimalVTable*)&colorDog_vtable;
    a->baseColorAnimal.animalColor = input;
    return (Animal*)a;
}