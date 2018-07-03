/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file CannyDetector.cpp
 * @date 11/04/2018
 * @author Nassir W. Oumer
 */

/*!
 * @addtogroup DFNsTest
 *
 * Unit Test for the DFN CannyDetector.
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
#include <catch.hpp>
#include <EdgeDetection/CannyDetector.hpp>
#include <Stubs/Common/ConversionCache/CacheHandler.hpp>
#include <ConversionCache/ConversionCache.hpp>
#include <FrameToMatConverter.hpp>
#include <MatToFrameConverter.hpp>
#include <Mocks/Common/Converters/FrameToMatConverter.hpp>
#include <Mocks/Common/Converters/MatToFrameConverter.hpp>
#include <Errors/Assert.hpp>

using namespace dfn_ci;
using namespace Common;
using namespace Converters;
using namespace FrameWrapper;

/* --------------------------------------------------------------------------
 *
 * Test Cases
 *
 * --------------------------------------------------------------------------
 */
TEST_CASE( "Call to process (canny detector)", "[process]" )
	{
	Stubs::CacheHandler<FrameConstPtr, cv::Mat>* stubInputCache = new Stubs::CacheHandler<FrameConstPtr, cv::Mat>();
	Mocks::FrameToMatConverter* mockInputConverter = new Mocks::FrameToMatConverter();
	ConversionCache<FrameConstPtr, cv::Mat, FrameToMatConverter>::Instance(stubInputCache, mockInputConverter);

	Stubs::CacheHandler<cv::Mat, FrameConstPtr>* stubOutputCache = new Stubs::CacheHandler<cv::Mat, FrameConstPtr>();
	Mocks::MatToFrameConverter* mockOutputConverter = new Mocks::MatToFrameConverter();
	ConversionCache<cv::Mat, FrameConstPtr, MatToFrameConverter>::Instance(stubOutputCache, mockOutputConverter);

	cv::Mat inputImage;
	cv::Mat testImage = cv::imread("../tests/Data/Images/AlgeriaDesert.jpg", cv::IMREAD_COLOR);
 	cv::cvtColor(testImage, inputImage, cv::COLOR_BGR2GRAY);

	mockInputConverter->AddBehaviour("Convert", "1", (void*) (&inputImage) );

	FrameConstPtr outputImage = new Frame();
	mockOutputConverter->AddBehaviour("Convert", "1", (void*) (&outputImage) );

	CannyDetector detector;
	FrameConstPtr input = new Frame();
	detector.imageInput(input);
	detector.process();

	FrameConstPtr output = detector.edgeMapOutput();

	delete(input);
	delete(output);
	}

TEST_CASE( "Call to configure (canny detector)", "[configure]" )
	{
	CannyDetector detector;
	detector.setConfigurationFile("../tests/ConfigurationFiles/DFNs/EdgeDetection/CannyDetector_Conf.yaml");
	detector.configure();
	}

/** @} */
