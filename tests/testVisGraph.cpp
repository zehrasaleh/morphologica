/*
 * Visualize a graph
 */
#include <morph/Visual.h>
#include <morph/ColourMap.h>
#include <morph/GraphVisual.h>
#include <morph/Scale.h>
#include <morph/Vector.h>
#include <morph/vVector.h>
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>

int main (int argc, char** argv)
{
    int rtn = -1;

    morph::Visual v(1024, 768, "Graph", {-0.8,-0.8}, {.1,.1,.1}, 1.0f, 0.01f);
    v.zNear = 0.001;
    v.showCoordArrows = true;
    v.backgroundWhite();
    v.lightingEffects();

    bool holdVis = false;
    if (argc > 1) {
        std::string a1(argv[1]);
        holdVis = a1.empty() ? false : true;
    }
    std::cout << "NB: Provide a cmd line arg (anything) to see the graphical window for this program" << std::endl;

    try {
        morph::vVector<float> absc =  {-.5, -.4, -.3, -.2, -.1, 0, .1, .2, .3, .4, .5, .6, .7, .8};
        morph::vVector<float> data = absc.pow(3);
        morph::GraphVisual<float>* gv = new morph::GraphVisual<float> (v.shaderprog, v.tshaderprog, {0,0,0});

#if 1 // Optionally change the size of the graph and range of the axes
        gv->setsize (1.33, 1);
#endif

#if 0 // Optionally change the range of the axes
        gv->setlimits (0,1.4,0,1.4);
#endif

#if 1 // Optionally modify the features of the graph
        morph::DatasetStyle ds;
        ds.linewidth = 0.005;
        ds.linecolour = {1.0, 0.0, 0.0};
        ds.markerstyle = morph::markerstyle::triangle;
        ds.markersize = 0.02;
        ds.markercolour = {0.0, 0.0, 1.0};
        ds.markergap = 0.02;
        // For each dataset added there should be a set of 'datastyles' - linestyle, markerstyle, etc
        gv->setdata (absc, data, ds);
        ds.markerstyle = morph::markerstyle::square;
        ds.setcolour ({0.0, 1.0, 0.0});
        gv->setdata (absc, absc.pow(4), ds);
#else
        gv->setdata (absc, data);
#endif


#if 1 // Optionally set the axes up
        gv->axiscolour = {0.5, 0.5, 0.5};
        gv->axislinewidth = 0.01f;
        gv->axisstyle = morph::axisstyle::box;
        gv->setthickness (0.001f);
#endif
        gv->finalize();

        // Add the GraphVisual (as a VisualModel*)
        v.addVisualModel (static_cast<morph::VisualModel*>(gv));

        v.render();
        if (holdVis == true) {
            while (v.readyToFinish == false) {
                glfwWaitEventsTimeout (0.018);
                v.render();
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        rtn = -1;
    }

    return rtn;
}
