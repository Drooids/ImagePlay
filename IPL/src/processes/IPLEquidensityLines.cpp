//#############################################################################
//
//  This file is part of ImagePlay.
//
//  ImagePlay is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  ImagePlay is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with ImagePlay.  If not, see <http://www.gnu.org/licenses/>.
//
//#############################################################################

#include "IPLEquidensityLines.h"

void IPLEquidensityLines::init()
{
    // init
    _result     = NULL;

    // basic settings
    setClassName("IPLEquidensityLines");
    setTitle("Agglomerate");
    setCategory(IPLProcess::CATEGORY_LOCALOPERATIONS);

    // inputs and outputs
    addInput("Image", IPLData::IMAGE_COLOR);
    addOutput("Gray Image", IPLImage::IMAGE_BW);

    // properties
    addProcessPropertyDouble("threshold", "Threshold", "0.0 < threshold < 1.0", 0.5, IPL_WIDGET_SLIDER, 0.0, 1.0);
}

void IPLEquidensityLines::destroy()
{
    delete _result;
}

bool IPLEquidensityLines::processInputData(IPLImage* image , int, bool useOpenCV)
{
    // delete previous result
    delete _result;
    _result = NULL;

    int width = image->width();
    int height = image->height();
    if( image->type() == IPLData::IMAGE_GRAYSCALE )
        _result = new IPLImage( IPLImage::IMAGE_BW, width, height );
    else
        _result = new IPLImage( image->type(), width, height );

    // get properties
    float threshold = getProcessPropertyDouble("threshold");

    int progress = 0;
    int maxProgress = image->height() * image->getNumberOfPlanes();
    int nrOfPlanes = image->getNumberOfPlanes();

    #pragma omp parallel for
    for( int planeNr=0; planeNr < nrOfPlanes; planeNr++ )
    {
        IPLImagePlane* plane = image->plane( planeNr );
        IPLImagePlane* newplane = _result->plane( planeNr );
        {
            for(int y=0; y<height; y++)
            {
                // progress
                notifyProgressEventHandler(100*progress++/maxProgress);
                for(int x=0; x<width; x++)
                {
                    newplane->p(x,y) = (plane->p(x,y) < threshold) ? 0.0f : 1.0f;
                }
            }
        }
    }
    return true;
}

IPLData* IPLEquidensityLines::getResultData( int )
{
    return _result;
}
