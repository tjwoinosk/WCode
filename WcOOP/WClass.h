#ifndef ANIMAL_H
#define ANIMAL_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declaration of the base class
typedef struct Animal Animal;

// The Virtual Table structure
typedef struct {
    void (*sound)(const Animal* self);
    char* (*returnSound)(const Animal* self);
} AnimalVTable;

// The Base Class structure
struct Animal {
    const AnimalVTable* vtable; // Pointer to the vtable
};

Animal* animal_create();

// Base class interface macros/functions for clean syntax
inline void animal_sound(const Animal* self) { self->vtable->sound(self); }
inline char* animal_returnSound(const Animal* self) { return self->vtable->returnSound(self); }

//Pig class
typedef struct {
    Animal base; // MUST be the first member
} Pig;

Animal* pig_create();

//Dog class
typedef struct {
    Animal base; // MUST be the first member
} Dog;

Animal* dog_create();

//Color Animal class
typedef struct ColorAnimal ColorAnimal;

// The Virtual Table structure
typedef struct {
    AnimalVTable base_vtable;
    char* (*color)(const ColorAnimal* self);
} ColorAnimalVTable;

struct ColorAnimal {
    Animal baseAnimal;
    char animalColor;
};

inline char* colorAnimal_color(const ColorAnimal* self) { 
    // Fetch the extended vtable pointer from the base layout
    const ColorAnimalVTable* vtable = (const ColorAnimalVTable*)self->baseAnimal.vtable;
    return vtable->color(self);
}

//Color Pig Class
typedef struct {
    ColorAnimal baseColorAnimal; // MUST be the first member
} ColorPig;


Animal* colorPig_create(char inputColor);

//Color Dog Class
typedef struct {
    ColorAnimal baseColorAnimal; // MUST be the first member
} ColorDog;

Animal* colorDog_create(char inputColor);


#ifdef __cplusplus
}
#endif

#endif
