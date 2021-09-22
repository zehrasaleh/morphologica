/*
 * Visualize a graph. Minimal example showing how a default graph appears
 */
#include <morph/Visual.h>
#include <morph/GraphVisual.h>
#include <morph/vVector.h>
#include <iostream>

int main (int argc, char** argv)
{
    // Set up a morph::Visual 'scene environment'.
    morph::Visual v(1024, 768, "Made with morph::GraphVisual");
    // Create a new GraphVisual with offset within the scene of 0,0,0
    morph::GraphVisual<float>* gv = new morph::GraphVisual<float> (v.shaderprog, v.tshaderprog, {0,0,0});
    // Create some data for the x axis:
    morph::vVector<float> x;
    // This works like numpy's linspace() (the 3 args are "start", "end" and "num"):
    x.linspace (-0.5, 0.8, 14);
    // Set a graph up of y = x^3
    gv->setdata (x, x.pow(3));
    // Complete the setup
    gv->finalize();
    // Add the GraphVisual to the Visual scene
    v.addVisualModel (gv);
    // Render the scene on the screen until user quits with 'x'
    v.keepOpen();

    return 0;
}
