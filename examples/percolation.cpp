/*
 * Visualize a test surface
 */

/*!
 * \file
 *
 * \author Zehra Saleh
 * \date 2022
 */

#include <morph/Visual.h>
#include <morph/ColourMap.h>
#include <morph/PercolationVisual.h>
#include <morph/Scale.h>
#include <morph/Vector.h>
#include <morph/vVector.h>
#include <morph/percolationClasses.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>

bool Bond::isBondPossible()               //If bond is to a particle of the same color, this is true.
{              
    if (a != nullptr && b != nullptr)
    {
        if (a->color == Color::NONE || b->color == Color::NONE)
        {
            return false;  
        }
        return a->color == b->color;    // "." needs object, "->" dereferences the pointer (goes to object in adress) can also use (*b).color
    }
    return false;
};

/* void drawFormedBonds(int scaling, morph::vVector<morph::Vector<float, 3>> *lattice_Array, morph::vVector<float> *colorData, Matrix *particleMatrix, morph::PercolationVisual<float>* sv) 
{
    size_t l = 0;
    auto it = particleMatrix->get_allParticles().begin(); //begin() returns iterator with a pointer to the first element, ++ gives the pointer to the next element

    // Draw the bonds between right colours that have been formed 
    for ( ; it != particleMatrix->get_allParticles().end(); it++)
    {   
        float x = it->position.x*scaling;
        float y = it->position.y*scaling;
        float z = it->position.z*scaling;

        &lattice_Array[l] = {x, y, z};
        
        if (it->color == Color::RED)
        {
            &colorData[l] = 0;
        }
        else if (it->color == Color::GREEN)
        {
            &colorData[l] = 1;
        }

        if (it->up != nullptr && it->up->isFormed)
        {   
            sv->drawLine( {x,y,z}, {x,y,z+scaling});
        }

        if (it->right != nullptr && it->right->isFormed)
        {
            sv->drawLine( {x,y,z}, {x+scaling,y,z});
        }

        if (it->behind != nullptr && it->behind->isFormed)
        {
            sv->drawLine( {x,y,z}, {x,y+scaling,z});
        }
        l++;
        
    }
};
*/

template<class Element>             //Get pointer to a random element in array
Element* getRandomElement(std::vector<Element*> array){
    
    int randomElement = rand() % array.size();
    
    return array[randomElement];
};

//Find a neighbor that particle can swap with, in other words find neighbor that has different color, if none -> return nullptr
Particle* findSwapableNeighbor(Particle *particle)
{

    if (particle->color != particle->up->b->color){
        return particle->up->b;
    }
    else if (particle->color != particle->down->b->color){
        return particle->down->b;
    }
    else if (particle->color != particle->right->b->color){
        return particle->right->b;
    }
    else if (particle->color != particle->left->b->color){
        return particle->left->b;
    }
    else if (particle->color != particle->behind->b->color){
        return particle->behind->b;
    }
     else if (particle->color != particle->front->b->color){
        return particle->front->b;
    }
};
const int iterations = 4;
const int cubic_length = 5;
// Above, Right, Behind
// Z is above direction
// X is to the right direction
// Y is behind


int main (int argc, char** argv)
{
    Matrix particleMatrix(cubic_length);
    
    int rtn = -1;

    morph::Visual v(1024, 768, "morph::PercolationVisual", {0,0}, {1,1,1}, 1.0f, 0.05f);
    v.zNear = 0.001;
    v.showCoordArrows = false;
    v.coordArrowsInScene = false;
    v.showTitle = true;
    // Blueish background:
    //v.bgcolour = {0.6f, 0.6f, 0.8f, 0.5f};

    v.bgcolour = {0.4f, 0.4f, 1.0f, 0.8f};
    v.lightingEffects();


    // Matrix consists of elements with colors RED, GREEN or NONE 
    // Lattice Array consists of x,y,z positions for every point that needs to be drawn; further clarification: that means elements that have colors RED or GREEN


    try {
        morph::Vector<float, 3> offset = { 0.0, 0.0, 0.0 };
        morph::Scale<float> scale;
        scale.setParams (1.0, 0.0);

        // Note use of morph::vVectors here, which can be passed into
        // VisualDataModel::setDataCoords(std::vector<Vector<float>>* _coords)
        // and setScalarData(const std::vector<T>* _data)
        // This is possible because morph::vVector derives from std::vector.
        morph::vVector<morph::Vector<float, 3>> lattice_Array(cubic_length*cubic_length*cubic_length);
        morph::vVector<float> colorData(cubic_length*cubic_length*cubic_length);

        morph::PercolationVisual<float>* sv = new morph::PercolationVisual<float> (v.shaderprog, offset);
        sv->setDataCoords (&lattice_Array);
        sv->setScalarData (&colorData);
        sv->radiusFixed = 0.03f;
        sv->cm.setType (morph::ColourMapType::Rainbow);
        //sv->cm.setHueRG();
        
        
        float scaling = 0.2;

        for (int i = 0; i < iterations; i++){

            //Form Random Bond, TO DO: Flytt til egen funksjon 
            auto allPossibleBonds = particleMatrix.getPossibleBonds();
            Bond *randomBond = getRandomElement(allPossibleBonds); // Random
            randomBond->isFormed = true;
            randomBond->a->isBonded = true;
            randomBond->b->isBonded = true;

            // //Swap two particles of different color
            // auto allUnboundParticles = particleMatrix.getUnboundParticles();
            // Particle *randomParticle = getRandomElement(allUnboundParticles);
            // bool swapped;
            // while (!swapped){
            //     if (randomParticle.color != randomParticle.up.color)
            // }

            //Calculate probability P using two different methods
            int numberFormedBonds = particleMatrix.getFormedCount(); // Keeps track of how many bonds that have been formed, used to calculate p

            double probablilityP = numberFormedBonds / (3*cubic_length*cubic_length*cubic_length);
            double probabilityOtherP = numberFormedBonds / allPossibleBonds.size();    
        
        }

        //drawFormedBonds(scaling, &lattice_Array, &colorData, &particleMatrix, sv); 

        size_t l = 0;
        auto it = particleMatrix.get_allParticles().begin(); //begin() returns iterator with a pointer to the first element, ++ gives the pointer to the next element

        // Draw the bonds between right colours that have been formed 
        for ( ; it != particleMatrix.get_allParticles().end(); it++)
        {   
            float x = it->position.x*scaling;
            float y = it->position.y*scaling;
            float z = it->position.z*scaling;

            lattice_Array[l] = {x, y, z};
            
            if (it->color == Color::RED)
            {
                colorData[l] = 0;
            }
            else if (it->color == Color::GREEN)
            {
                colorData[l] = 1;
            }

            if (it->up != nullptr && it->up->isFormed)
            {   
                sv->drawLine( {x,y,z}, {x,y,z+scaling});
            }

            if (it->right != nullptr && it->right->isFormed)
            {
                sv->drawLine( {x,y,z}, {x+scaling,y,z});
            }

            if (it->behind != nullptr && it->behind->isFormed)
            {
                sv->drawLine( {x,y,z}, {x,y+scaling,z});
            }
            l++;
            
        }
        sv->finalize();
        unsigned int visId = v.addVisualModel (sv);
        std::cout << "Added Visual with visId " << visId << std::endl;

        v.render();
        while (v.readyToFinish == false) {
            glfwWaitEventsTimeout (0.018);
            v.render();
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        rtn = -1;
    }

    return rtn;
}
