/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file MatToImageTypeConverter.cpp
 * @date 27/11/2017
 * @authors Alessandro Bianco, Xavier Martinez
 */

/*!
 * @addtogroup Converters
 * 
 * Implementation of MatToFrameConverter class.
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

#include "MatToFrameConverter.hpp"
#include <Errors/Assert.hpp>

namespace Converters {

using namespace FrameWrapper;

/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
FrameConstPtr MatToFrameConverter::Convert(const cv::Mat& image)
	{
	FramePtr frame = new Frame();

	SetFrameSize(*frame, image.cols, image.rows);
	SetFrameMode(*frame, MODE_RGB);

	if (image.rows == 0 && image.cols == 0)
		return frame;		
	ASSERT(image.type() ==  CV_8UC3, "MatToFrameConverter: Only CV_8UC3 type is supported for this conversion at the moment");

	for(int rowIndex = 0; rowIndex < image.rows; rowIndex++)
		{
		for(int columnIndex = 0; columnIndex < image.cols; columnIndex++)
			{
			cv::Vec3b pixel = image.at<cv::Vec3b>(rowIndex, columnIndex);
			AddDataByte(*frame, (uint8_t) pixel[0] );
			AddDataByte(*frame, (uint8_t) pixel[1] );
			AddDataByte(*frame, (uint8_t) pixel[2] );
			}
		}

	return frame;
	}

FrameSharedConstPtr MatToFrameConverter::ConvertShared(const cv::Mat& image)
	{
	FrameConstPtr frame = Convert(image);
	FrameSharedConstPtr sharedFrame(frame);
	return sharedFrame;
	}

}

/** @} */