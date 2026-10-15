#ifndef FASTANIMAL_H
#define FASTANIMAL_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the base class
typedef struct FastAnimal FastAnimal;

// The Base Class structure
struct FastAnimal {
    void (*sound)(const FastAnimal* self);
    char* (*returnSound)(const FastAnimal* self);
};

FastAnimal* fastAnimal_create();

// Base class interface macros/functions for clean syntax
inline void fastAnimal_sound(const FastAnimal* self) { self->sound(self); }
inline char* fastAnimal_returnSound(const FastAnimal* self) { return self->returnSound(self); }

//Pig class
typedef struct {
    FastAnimal baseFastAnimal; // MUST be the first member
} FastPig;

FastAnimal* fastPig_create();

//Dog class
typedef struct {
    FastAnimal baseFastAnimal; // MUST be the first member
} FastDog;

FastAnimal* fastDog_create();

// Color Animal class
typedef struct FastColorAnimal FastColorAnimal;

struct FastColorAnimal {
    FastAnimal baseFastAnimal;
	char* (*color)(const FastColorAnimal* self); 
    char animalColor;
};

inline char* fastColorAnimal_color(const FastColorAnimal* self) { return self->color(self); } 

// Color Pig Class
typedef struct {
    FastColorAnimal baseFastColorAnimal; // MUST be the first member
} FastColorPig;


FastAnimal* fastColorPig_create(char inputColor);

// Color Dog Class
typedef struct {
    FastColorAnimal baseFastColorAnimal; // MUST be the first member
} FastColorDog;

FastAnimal* fastColorDog_create(char inputColor);


#ifdef __cplusplus
}
#endif

#endif
