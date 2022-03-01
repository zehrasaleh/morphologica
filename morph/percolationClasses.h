/*
 * Classes and structs needed, as well as information about what needs to be done
 */

/*!
 * \file
 *
 * \author Zehra Saleh
 * \date 2022
 */

#include <iostream>
#include <fstream>
#include <cmath>
#include <array>

// Looked at, waiting  1) Periodic Boundary conditions on X,Y dir.  Phyiscally there exists particles on all the edges except for Z dir.---> This means that latticeArrray[x=cubicLength] = latticeArray[x=0], wrap around, no need for if (on edge) checks...
// Done  2) Open BC in Z dir. --> Means that there are no particles above z= z.max or under z = 0.  (The way it is already implemented)
// Done  3) Put all possible bindings in a list, choose randomly 1 of these. 
// Done  4) Calculate probability p.  p = Number of bonds formed / 3* number of sites . Remember particles on the edges, will have to make up for those when cubicLength gets large. (Info for myself: The problem we are looking at is called Bonding Percolation).
//   5) Switch places between two neighbors or random particles of different color that do not have formed bonds. 
//   6) First: Do  3) and 5) once, then try to do it for certain time steps.
//   Implement Burning Method - to check for percolation
//   Implement Hoshen–Kopelman Algorithm - gather information about the cluster sizes.
//   If time allows: Bind Arrows to display only red clusters or only green clusters


enum Color {

	RED = 0, 
	GREEN = 1,
	NONE = 2

};

struct Particle;

struct Bond {

    bool isFormed;                      //If bond is created betweeen particle of the same color, this is true. //Struct default public, Class default private
    bool isBondPossible();              //If bond is to a particle of the same color, this is true.
    
    Particle *a;
    Particle *b;

};

struct Position {
    int x;
    int y;
    int z;
};

struct Particle {
    
	Color color = Color::NONE;
    Position position;
    bool isBonded(){
        if ( up && up->isFormed ){
            return true;
        }
        else if ( down && down->isFormed ){
            return true;
        }
        else if ( right && right->isFormed ){
            return true;
        }
        else if ( left && left->isFormed ){
            return true;
        }
        else if ( behind && behind->isFormed ){
            return true;
        }
        else if ( front && front->isFormed ){
            return true;
        }
        return false;
    }


    Bond *up;                        // Information about the bonds in pos. z direction & neigihbor
    Bond *down;                      // Information about the bonds in neg. z direction & neighbor    
    Bond *right;                     // Information about the bonds in pos. x direction & neighbor  
    Bond *left;                      // Information about the bonds in neg. x direction & neighbor  
    Bond *behind;                    // Information about the bonds in pos. y direction & neighbor 
    Bond *front;                     // Information about the bonds in neg. y direction & neighbor 

};

class Matrix {
public:
    Matrix(int dim): dim(dim), allParticles(dim*dim*dim), allBonds(dim*dim*dim*10) 
    {
        int posn= 0;
        for (int x = 0; x < dim; ++x) {
            for (int y = 0; y < dim; ++y) {
                for (int z = 0; z < dim; ++z){
                    
                    // Initialising all particles and setting the respective colors for each of them
                    auto *me = &allParticles[getPosition(x,y,z)];
                    int randomNumb = rand() % 2; //0 , 1 or 2

                    if (randomNumb == 0){
                        me->color = Color::RED;
                    }
                    else if (randomNumb == 1){
                        me->color = Color::GREEN;
                    }

                    // Set the particle positions for every particle in allParticles
                    me->position.x = x;
                    me->position.y = y;
                    me->position.z = z;

                    // Initialising all bonds between neighboring particles in allBonds
                    // Both neighbors me & __neighbor gain information about the bonds, and this information is saved in their Particle struct. 
                    if (z < dim-1){

                        auto *upNeighbor = &allParticles[getPosition(x,y,z+1)];
                        auto *upBond = &allBonds[posn];

                        me->up = upBond;
                        upNeighbor->down = upBond;

                        upBond->a = me;
                        upBond->b = upNeighbor;
                    }

                    if (x < dim-1){

                        auto *rightNeighbor = &allParticles[getPosition(x+1,y,z)];
                        auto *rightBond = &allBonds[posn+1];

                        me->right = rightBond;
                        rightNeighbor->left = rightBond;

                        rightBond->a = me;
                        rightBond->b = rightNeighbor;
                    }
                    
                    if (y < dim-1){

                        auto *behindNeighbor = &allParticles[getPosition(x,y+1,z)];    
                        auto *behindBond = &allBonds[posn+2];
                        
                        me->behind = behindBond;
                        behindNeighbor->front = behindBond;
                                                
                        behindBond->a = me;
                        behindBond->b = behindNeighbor;
                    }
                    posn += 3;
                }
            }
        }

    };

    std::vector<Bond*> getPossibleBonds()  //Returns a list of pointers to all the bonds that are possible to be formed
    {
        std::vector<Bond*> allPossibleBonds;
        auto it = allBonds.begin(); //begin() returnerer iterator med peker til første element, ++ gir peker til neste element

        for ( ; it != allBonds.end(); it++)
        {  
            if (it->isBondPossible() && !it->isFormed)
            {
                allPossibleBonds.push_back(&*it); 
            }
           
        }
        return allPossibleBonds;
    };

    const std::vector<Particle>& get_allParticles() //Returns a list of pointers to all the particles
    {
        return allParticles;
    };

    int getFormedCount()  //Returns the number of bonds that have been formed
    {
        int count = 0;
        auto it = allBonds.begin(); //begin() returnerer iterator med peker til første element, ++ gir peker til neste element

        for ( ; it != allBonds.end(); it++)
        {  
            if (it->isFormed)
            {
                count++;
            }
           
        }
        return count;
    };

private:
    int dim;
    int getPosition(int x, int y, int z){
        return ((x * dim) + y) * dim + z;
    };
    std::vector<Particle> allParticles;
    std::vector<Bond> allBonds;

};
