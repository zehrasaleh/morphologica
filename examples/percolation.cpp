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
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>

enum Color {
	RED = 0, 
	GREEN = 1,
	NONE = 2
};


struct Particle {
	Color color = Color::NONE;
};

const int cubic_length = 5;
// Above, Right, Behind
//Z is above direction
//X is to the right direction
//Y is behind

bool isBondAbove(Particle latice_matrix[cubic_length][cubic_length][cubic_length], int x, int y, int z){

    if (z == cubic_length - 1) { 
        return false;
    }

    Color site_state = latice_matrix[x][y][z].color;
    Color site_state_below = latice_matrix[x][y][z+1].color;

    if (site_state == Color::NONE)
    {
        return false;
    }

    return site_state == site_state_below;
}


bool isBondRight(Particle latice_matrix[cubic_length][cubic_length][cubic_length], int x, int y, int z){

    if (x == cubic_length - 1) { 
        return false;
    }

    Color site_state = latice_matrix[x][y][z].color;
    Color site_state_below = latice_matrix[x+1][y][z].color;

    if (site_state == Color::NONE)
    {
        return false;
    }

    return site_state == site_state_below;
}

bool isBondBehind(Particle latice_matrix[cubic_length][cubic_length][cubic_length], int x, int y, int z){

    if (y == cubic_length - 1) { 
        return false;
    }

    Color site_state = latice_matrix[x][y][z].color;
    Color site_state_below = latice_matrix[x][y+1][z].color;

    if (site_state == Color::NONE)
    {
        return false;
    }

    return site_state == site_state_below;
}

/*
int getPosition(int* lattice_matrix, int x, int y, int z){
    return lattice_matrix[((x * cubic_Length) + y) * cubic_Length + z];
}
*/


int main (int argc, char** argv)
{
    int rtn = -1;

    morph::Visual v(1024, 768, "morph::PercolationVisual", {0,0}, {1,1,1}, 1.0f, 0.05f);
    v.zNear = 0.001;
    v.showCoordArrows = false;
    v.coordArrowsInScene = true;
    v.showTitle = true;
    // Blueish background:
    v.bgcolour = {0.6f, 0.6f, 0.8f, 0.5f};
    v.lightingEffects();
    

    Particle lattice_matrix[cubic_length][cubic_length][cubic_length];

    // Lattice Matrix consists of elements with colors RED, GREEN or NONE 
    // Lattice Array consists of x,y,z positions for every point that needs to be drawn; further clarification: that means elements that have colors RED or GREEN

    for (int x = 0; x < cubic_length; ++x) {
        for (int y = 0; y < cubic_length; ++y) {
            for (int z = 0; z < cubic_length; ++z) {
                lattice_matrix[x][y][z] = Particle();
                int a = rand() % 3; //0 , 1 or 2
                if (a == 0){
                    lattice_matrix[x][y][z].color = Color::RED;
                }
                else if (a == 1){
                    lattice_matrix[x][y][z].color = Color::GREEN;
                }
			}
        }
    }

   

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
       size_t l = 0;
        for (int i = 0; i < cubic_length; ++i) {
            for (int j = 0; j < cubic_length; ++j) {
                for (int k = 0; k < cubic_length; ++k){

                    float x = 0.2*i;
                    float y = 0.2*j;
                    float z = 0.2*k;
                    
                    if (lattice_matrix[i][j][k].color == Color::RED){
                        lattice_Array[l] = {x, y, z};
                        data[l] = 0;
                    }
                    else if (lattice_matrix[i][j][k].color == Color::GREEN){
                        lattice_Array[l] = {x, y, z};
                        data[l] = 1;
                    } 
                    l++;

                }
               
            }
        }

        morph::PercolationVisual<float>* sv = new morph::PercolationVisual<float> (v.shaderprog, offset);

       

        sv->setDataCoords (&lattice_Array);
        sv->setScalarData (&data);
        sv->radiusFixed = 0.03f;
        //sv->colourScale = scale;

        //drawLines between right colours 
    
        for (int i = 0; i < cubic_length; ++i) {
            for (int j = 0; j < cubic_length; ++j) {
                for (int k = 0; k < cubic_length; ++k){

                    float x = 0.2*i;
                    float y = 0.2*j;
                    float z = 0.2*k;

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

        sv->drawLine( {0,0,0} , {0,0,0.2} );

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
