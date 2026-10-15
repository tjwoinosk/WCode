#include <gtest/gtest.h>

#include "WClass.h"
#include "WClassFast.h"
#include "test_classes.h"

//polymorphic behavior test
TEST(polyTest, usingRealClass) {
    poly::Animal* myAnimal = new poly::Animal();
    poly::Animal* myPig = new poly::Pig();
    poly::Animal* myDog = new poly::Dog();

    EXPECT_EQ(myAnimal->returnSound(), "The animal makes a sound");
    EXPECT_EQ(myPig->returnSound(), "The pig says: wee wee");
    EXPECT_EQ(myDog->returnSound(), "The dog says: bow wow");
}

TEST(polyTest, usingClassV1) {
    //Animal -> Pig
    //Animal -> Dog
    Animal* myAnimal = animal_create();
    Animal* myPig = pig_create();
    Animal* myDog = dog_create();
    
	printf("Test Start\n");
	animal_sound(myAnimal);
	printf("Test Ends\n");
	
    EXPECT_EQ(std::string(animal_returnSound(myAnimal)), "The animal says: Animals make many different sounds");
    EXPECT_EQ(std::string(animal_returnSound(myPig)), "The pig says: wee wee");
    EXPECT_EQ(std::string(animal_returnSound(myDog)), "The dog says: bow wow");
}

TEST(polyTest, usingClassV1_twoLevels) {
    //Animal -> ColorAnimal -> ColorPig
    //Animal -> ColorAnimal -> ColorDog
    Animal* myColorPig = colorPig_create('b');
    Animal* myColorDog = colorDog_create('r');
    
    EXPECT_EQ(std::string(animal_returnSound(myColorPig)), "The pig says: wee wee"); 
    EXPECT_EQ(std::string(colorAnimal_color((ColorAnimal*)myColorPig)), "The pigs color is: b");
	
	EXPECT_EQ(std::string(animal_returnSound(myColorDog)), "The dog says: bow wow"); 
    EXPECT_EQ(std::string(colorAnimal_color((ColorAnimal*)myColorDog)), "The dogs color is: r");
}

//Animal -> Pig
//Animal -> Dog
TEST(polyTest, usingClassV2) {
    FastAnimal* myAnimal = fastAnimal_create();
    FastAnimal* myPig = fastPig_create();
    FastAnimal* myDog = fastDog_create();
    
    EXPECT_EQ(std::string(fastAnimal_returnSound(myAnimal)), "The animal says: Animals make many different sounds");
    EXPECT_EQ(std::string(fastAnimal_returnSound(myPig)), "The pig says: wee wee");
    EXPECT_EQ(std::string(fastAnimal_returnSound(myDog)), "The dog says: bow wow");
}

//Animal -> ColorAnimal -> ColorPig
//Animal -> ColorAnimal -> ColorDog
TEST(polyTest, usingClassV2_twoLevels) {

    FastAnimal* myColorPig = fastColorPig_create('b');
    FastAnimal* myColorDog = fastColorDog_create('r');
    
    EXPECT_EQ(std::string(fastAnimal_returnSound(myColorPig)), "The pig says: wee wee"); 
    EXPECT_EQ(std::string(fastColorAnimal_color((FastColorAnimal*)myColorPig)), "The pigs color is: b");
	
	EXPECT_EQ(std::string(fastAnimal_returnSound(myColorDog)), "The dog says: bow wow"); 
    EXPECT_EQ(std::string(fastColorAnimal_color((FastColorAnimal*)myColorDog)), "The dogs color is: r");
}

//No polymorphic behavior test
TEST(nonPolyTest, usingRealClass) {
    basic::Animal* myAnimal = new basic::Animal();
    basic::Animal* myPig = new basic::Pig();
    basic::Animal* myDog = new basic::Dog();

    EXPECT_EQ(myAnimal->returnSound(), "The animal makes a sound");
    EXPECT_EQ(myPig->returnSound(), "The animal makes a sound");
    EXPECT_EQ(myDog->returnSound(), "The animal makes a sound");
    
    basic::Pig* newMyPig = static_cast<basic::Pig*>(myPig);
    basic::Dog* newMyDog = static_cast<basic::Dog*>(myDog);
    EXPECT_EQ(newMyPig->returnSound(), "The pig says: wee wee");
    EXPECT_EQ(newMyDog->returnSound(), "The dog says: bow wow");
}