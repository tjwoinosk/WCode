
namespace basic {
// Base class
class Animal {
    std::string sound{"The animal makes a sound"};
    public:
        std::string returnSound() {return sound;}
        void animalSound() {
            std::cout << sound;
        }
};

// Derived class
class Pig : public Animal {
    std::string sound{"The pig says: wee wee"};
    public:
        std::string returnSound() {return sound;}
        void animalSound() {
            std::cout << sound;
        }
};

// Derived class
class Dog : public Animal {
    std::string sound{"The dog says: bow wow"};
    public:
        std::string returnSound() {return sound;}
        void animalSound() {
            std::cout << sound;
        }
};
}

namespace poly {
// Base class
class Animal {
    std::string sound{"The animal makes a sound"};
    public:
        virtual std::string returnSound() {return sound;}
        virtual void animalSound() {
            std::cout << sound;
        }
};

// Derived class
class Pig : public Animal {
    std::string sound{"The pig says: wee wee"};
    public:
        std::string returnSound() override {return sound;}
        void animalSound() override {
            std::cout << sound;
        }
};

// Derived class
class Dog : public Animal {
    std::string sound{"The dog says: bow wow"};
    public:
        std::string returnSound() override {return sound;}
        void animalSound() override {
            std::cout << sound;
        }
};
}