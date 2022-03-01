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


const int iterations = 1;
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
    v.coordArrowsInScene = true;
    v.showTitle = true;
    // Blueish background:
    v.bgcolour = {0.6f, 0.6f, 0.8f, 0.5f};
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
        morph::vVector<float> data(cubic_length*cubic_length*cubic_length);

        morph::PercolationVisual<float>* sv = new morph::PercolationVisual<float> (v.shaderprog, offset);
        sv->setDataCoords (&lattice_Array);
        sv->setScalarData (&data);
        sv->radiusFixed = 0.03f;

        auto it = particleMatrix.get_allParticles().begin(); //begin() returns iterator with a pointer to the first element, ++ gives the pointer to the next element

        size_t l = 0;
        float scaling = 0.2;

        for (int i = 0; i < iterations; i++){

            auto allPossibleBonds = particleMatrix.getPossibleBonds();
            int randomBond = rand() % allPossibleBonds.size(); // Random number from 0 to (length of allPossibleBonds)-1
            allPossibleBonds[randomBond]->isFormed = true;

            int numberFormedBonds = particleMatrix.getFormedCount(); // Keeps track of how many bonds that have been formed, used to calculate p

            double probablilityP= numberFormedBonds / (3*cubic_length*cubic_length*cubic_length);

        }


        for ( ; it != particleMatrix.get_allParticles().end(); it++)
        {   
            float x = it->position.x*scaling;
            float y = it->position.y*scaling;
            float z = it->position.z*scaling;

            lattice_Array[l] = {x, y, z};
            
            if (it->color == Color::RED)
            {
                data[l] = 0;
            }
            else if (it->color == Color::GREEN)
            {
                data[l] = 1;
            }

            if (it->up != nullptr && it->up->isFormed)
            {   
                sv->drawLine( {x,y,z}, {x,y,z+0.2});
            }

            if (it->right != nullptr && it->right->isFormed)
            {
                sv->drawLine( {x,y,z}, {x+0.2,y,z});
            }

            if (it->behind != nullptr && it->behind->isFormed)
            {
                sv->drawLine( {x,y,z}, {x,y+0.2,z});
            }
            l++;
            
        }



        //sv->colourScale = scale;

        //drawLines between right colours 
        //int possiblebondsCount = 0;
        //std::vector<float> listofBonds();
        //const std::map<int, int> listofBonds{};

        /*
        for (int i = 0; i < cubic_length; ++i) {
            for (int j = 0; j < cubic_length; ++j) {
                for (int k = 0; k < cubic_length; ++k){

                    float x = 0.2*i;
                    float y = 0.2*j;
                    float z = 0.2*k;

                    /*

                    if (isBondAbove(lattice_matrix, i, j, k))
                    {   
                        sv->drawLine( {x,y,z}, {x,y,z+0.2});
                    }


                    if (isBondRight(lattice_matrix, i, j, k))
                    {
                        sv->drawLine( {x,y,z}, {x+0.2,y,z});
                    }

                    if (isBondBehind(lattice_matrix, i, j, k))
                    {
                        sv->drawLine( {x,y,z}, {x,y+0.2,z});
                    }

                    

                }
               
            }
        }
        */

        //int random = 1+ (rand() % 100; Random number from 1 to 100 

        //sv->drawLine( {0,0,0} , {0,0,0.2} );

        sv->cm.setType (morph::ColourMapType::Rainbow);
        //sv->cm.setHueRG();
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
