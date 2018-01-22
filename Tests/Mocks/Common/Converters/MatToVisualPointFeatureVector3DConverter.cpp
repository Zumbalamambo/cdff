/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file MatToVisualPointFeatureVector3DConverter.cpp
 * @date 01/12/2017
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup Mocks
 * 
 * Implementation of the MatToVisualPointFeatureVector3DConverter class.
 * 
 * 
 * @{
 */


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include "MatToVisualPointFeatureVector3DConverter.hpp"
#include <Errors/Assert.hpp>
#include "Mocks/MockMacro.hpp"

namespace Mocks {

using namespace VisualPointFeatureVector3DWrapper;

/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
MatToVisualPointFeatureVector3DConverter::~MatToVisualPointFeatureVector3DConverter()
	{

	}

VisualPointFeatureVector3DConstPtr MatToVisualPointFeatureVector3DConverter::Convert(const cv::Mat& featuresVector)
	MOCK_METHOD(Converters::MatToVisualPointFeatureVector3DConverter, Convert, VisualPointFeatureVector3DConstPtr, (featuresVector) )	

}

/** @} */