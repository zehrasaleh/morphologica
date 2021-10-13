/*!
 * \file
 *
 * \author Seb James
 * \date 2019
 */
#pragma once

#ifdef __OSX__
# include <OpenGL/gl3.h>
#else
# include <GL3/gl3.h>
#endif
#include <morph/tools.h>
#include <morph/VisualDataModel.h>
#include <morph/Scale.h>
#include <morph/Vector.h>
#include <iostream>
#include <vector>
#include <array>

namespace morph {

    //! The template argument Flt is the type of the data which this ScatterVisual
    //! will visualize.
    template <typename Flt>
    class ScatterVisual : public VisualDataModel<Flt>
    {
    public:
        //! Simplest constructor. Use this in all new code!
        ScatterVisual(GLuint sp, const Vector<float> _offset)
        {
            this->shaderprog = sp;
            this->mv_offset = _offset;
            this->viewmatrix.translate (this->mv_offset);
            this->zScale.setParams (1, 0);
            this->colourScale.do_autoscale = true;
        }

#define USE_DEPRECATED_CONSTRUCTORS 1
#ifdef USE_DEPRECATED_CONSTRUCTORS
        ScatterVisual(GLuint sp,
                      std::vector<Vector<float,3>>* _coords,
                      const Vector<float, 3> _offset,
                      const std::vector<Flt>* _data,
                      const Scale<Flt>& _scale,
                      ColourMapType _cmt,
                      const float _hue = 0.0f) {
            // Set up...
            this->shaderprog = sp;
            this->mv_offset = _offset;
            this->viewmatrix.translate (this->mv_offset);
            this->colourScale = _scale;
            this->dataCoords = _coords;
            this->scalarData = _data;

            this->cm.setHue (_hue);
            this->cm.setType (_cmt);

            this->initializeVertices();
            this->postVertexInit();
        }

        //! This constructor allows for setting the fixed radius,
        //! ScatterVisual::radiusFixed.
        ScatterVisual(GLuint sp,
                      std::vector<Vector<float,3>>* _coords,
                      const Vector<float, 3> _offset,
                      const std::vector<Flt>* _data,
                      const float fr,
                      const Scale<Flt>& _scale,
                      ColourMapType _cmt,
                      const float _hue = 0.0f) {
            // Set up...
            this->shaderprog = sp;
            this->mv_offset = _offset;
            this->viewmatrix.translate (this->mv_offset);
            this->colourScale = _scale;
            this->dataCoords = _coords;
            this->scalarData = _data;
            this->radiusFixed = fr;

            this->cm.setHue (_hue);
            this->cm.setType (_cmt);

            this->initializeVertices();
            this->postVertexInit();
        }
#endif

        //! Quick hack to add an additional point
        void add (morph::Vector<float> coord, Flt value)
        {
            std::array<float, 3> clr = this->cm.convert (value);
            this->computeSphere (this->curr_idx, coord, clr, this->radiusFixed, 16, 20);
            this->reinit_buffers();
        }

        //! Compute spheres for a scatter plot
        void initializeVertices (void)
        {
            unsigned int ncoords = this->dataCoords->size();
            unsigned int ndata = this->scalarData == (const std::vector<Flt>*)0 ? 0 : this->scalarData->size();
            // If we have vector data, then manipulate colour accordingly.
            unsigned int nvdata = this->vectorData == (const std::vector<Vector<Flt>>*)0 ? 0 : this->vectorData->size();

            if (ndata > 0 && ncoords != ndata) {
                std::cout << "ScatterVisual Error: ncoords ("<<ncoords<<") != ndata ("<<ndata<<"), return (no model)." << std::endl;
                return;
            }
            if (nvdata > 0 && ncoords != nvdata) {
                std::cout << "ScatterVisual Error: ncoords ("<<ncoords<<") != nvdata ("<<nvdata<<"), return (no model)." << std::endl;
                return;
            }

            // Find the minimum distance between points to get a radius? Or just allow
            // client code to set it?

            std::vector<Flt> dcopy;
            std::vector<Flt> vdcopy1;
            std::vector<Flt> vdcopy2;
            std::vector<Flt> vdcopy3;
            if (ndata && !nvdata) {
                dcopy = *(this->scalarData);
                this->colourScale.do_autoscale = true;
                this->colourScale.transform (*this->scalarData, dcopy);
            } else if (nvdata) {
                vdcopy1.resize(this->vectorData->size());
                vdcopy2.resize(this->vectorData->size());
                vdcopy3.resize(this->vectorData->size());

                std::vector<Flt> dcopy2, dcopy3;
                dcopy.resize(this->vectorData->size());
                dcopy2.resize(this->vectorData->size());
                dcopy3.resize(this->vectorData->size());

                for (unsigned int i = 0; i < this->vectorData->size(); ++i) {
                    dcopy[i] = (*this->vectorData)[i][0];
                    dcopy2[i] = (*this->vectorData)[i][1];
                    dcopy3[i] = (*this->vectorData)[i][2];
                }

                this->colourScale.do_autoscale = true;
                this->colourScale2.do_autoscale = true;
                this->colourScale3.do_autoscale = true;

                this->colourScale.transform (dcopy, vdcopy1);
                this->colourScale2.transform (dcopy2, vdcopy2);
                this->colourScale3.transform (dcopy3, vdcopy3);

#if 0
                for (unsigned int i = 0; i < this->vectorData->size(); ++i) {
                    std::cout << "i=" << i
                              << " R: " << vdcopy1[i]
                              << ", G: " << vdcopy2[i]
                              << ", B: " << vdcopy3[i] << std::endl;
                }
#endif
            } // else no scaling required - spheres will be one colour

            // The indices index
            VBOint idx = 0;

            for (unsigned int i = 0; i < ncoords; ++i) {
                // Scale colour (or use single colour)
                std::array<float, 3> clr = this->cm.getHueRGB();
                if (ndata && !nvdata) {
                    clr = this->cm.convert (dcopy[i]);
                } else if (nvdata) {
                    // Combine colour from two values. vdcopy1, vdcopy2? OR just do RGB for now?
                    // ColourMap in 'dual hue' (or triple hue) mode.
                    //std::cout << "Convert colour from vdcopy1[i]: " << vdcopy1[i] << ", vdcopy2[i]: " << vdcopy2[i] << std::endl;
                    clr = this->cm.convert (vdcopy1[i], vdcopy2[i]);
                }
                this->computeSphere (idx, (*this->dataCoords)[i], clr, this->radiusFixed, 16, 20);
            }

            this->curr_idx = idx;
        }

        //! Set this->radiusFixed, then re-compute vertices.
        void setRadius (float fr)
        {
            this->radiusFixed = fr;
            this->reinit();
        }

        //! Change this to get larger or smaller spheres.
        Flt radiusFixed = 0.05;

        // Hues for colour control with vectorData
        float hue1 = 0.1f;
        float hue2 = 0.5f;
        float hue3 = -1.0f;

        // Save the index so that additional points can be added
        VBOint curr_idx = 0;
    };

} // namespace morph
