/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file ImageUndistortionRectification.cpp
 * @date 23/04/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNsTest
 *
 * Performance Test for the DFN Image Undistortion Rectification.
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
#include "ImageFiltering.hpp"
#include <ImageFiltering/ImageUndistortionRectification.hpp>

using namespace CDFF::DFN;
using namespace CDFF::DFN::ImageFiltering;

const std::string USAGE =
	"The program takes up to five optional parameters: \n \
	(i) The name of the configuration file (the file has to be put into Tests/ConfigurationFiles/DFNs/ImageFiltering/ folder) \n \
	(ii) the base folder containing the images List file \n \
	(iii) the name of the images List file containing the relative path to one image in each line \n \
	(iv) the relative path from the base folder to the output image, the name of the image should not contain the image extension \n \
	(v) the extension of theoutput images \n \n \
	Example Usage: ./image_undistortion_rectification_performance_test ImageUndistortionRectification_conf.yaml /path/Set/Images ImagesList.txt ../RectifiedImages/image .jpg \n";

int main(int argc, char** argv)
	{
	std::string configurationFileName = "ImageUndistortionRectification_Conf1.yaml";
	if (argc >= 2)
		{
		configurationFileName = argv[1];
		if (configurationFileName == "-help")
			{
			PRINT_TO_LOG("", USAGE);
			return 0;
			}
		}

	ImageFilteringInterface* filter = new ImageUndistortionRectification;
	ImageFilteringTestInterface interface("../tests/ConfigurationFiles/DFNs/ImageFiltering", configurationFileName, "ImageUndistortionOutput.txt", filter);

	if (argc >= 4)
		{
		interface.SetImageFilePath(argv[2], argv[3]);
		}

	if (argc >= 6)
		{
		interface.SetOutputFile(argv[4], argv[5]);
		}

	interface.Run();
	};

/** @} */
