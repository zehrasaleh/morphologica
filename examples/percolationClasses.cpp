/*
 * Classes and structs needed, as well as information about what needs to be done
 */

/*!
 * \file
 *
 * \author Zehra Saleh
 * \date 2022
 */


#include <morph/percolationClasses.h>

//   1) Periodic Boundary conditions on X,Y dir.  Phyiscally there exists particles on all the edges except for Z dir.---> This means that latticeArrray[x=cubicLength] = latticeArray[x=0], wrap around, no need for if (on edge) checks...
//   2) Open BC in Z dir. --> Means that there are no particles above z= z.max or under z = 0.  (The way it is already implemented)
//   3) Put all possible bindings in a list, choose randomly 1 of these. Calculate probability p.  p = Number of bonds formed / 3* number of sites . Remember particles on the edges, will have to make up for those when cubicLength gets large. (Info for myself: The problem we are looking at is called Bonding Percolation).
//   4) Switch places between two neighbors or random particles of different color that do not have formed bonds. 
//   5) First: Do  3) and 4) once, then try to do it for certain time steps.
//   Implement Burning Method - to check for percolation
//   Implement Hoshen–Kopelman Algorithm - gather information about the cluster sizes.
//   If time allows: Bind Arrows to display only red clusters or only green clusters



