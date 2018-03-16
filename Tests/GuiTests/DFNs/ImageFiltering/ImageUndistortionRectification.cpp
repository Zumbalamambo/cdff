/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file ImageUndistortionRectification.cpp
 * @date 09/03/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNsTest
 * 
 * Testing application for the DFN ImageUndistortionRectification.
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
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <ImageFiltering/ImageUndistortionRectification.hpp>
#include <Stubs/Common/ConversionCache/CacheHandler.hpp>
#include <ConversionCache/ConversionCache.hpp>
#include <FrameToMatConverter.hpp>
#include <MatToFrameConverter.hpp>
#include <Mocks/Common/Converters/FrameToMatConverter.hpp>
#include <Mocks/Common/Converters/MatToFrameConverter.hpp>
#include <Errors/Assert.hpp>
#include <GuiTests/ParametersInterface.hpp>
#include <GuiTests/MainInterface.hpp>
#include <GuiTests/DFNs/DFNTestInterface.hpp>


using namespace dfn_ci;
using namespace Common;
using namespace Converters;
using namespace FrameWrapper;


class ImageUndistortionRectificationTestInterface : public DFNTestInterface
	{
	public:
		ImageUndistortionRectificationTestInterface(std::string dfnName, int buttonWidth, int buttonHeight);
		~ImageUndistortionRectificationTestInterface();
	protected:

	private:
		Stubs::CacheHandler<FrameConstPtr, cv::Mat>* stubInputCache;
		Mocks::FrameToMatConverter* mockInputConverter;
		Stubs::CacheHandler<cv::Mat, FrameConstPtr>* stubOutputCache;
		Mocks::MatToFrameConverter* mockOutputConverter;
		ImageUndistortionRectification* undistort;

		cv::Mat cvImage;
		FrameConstPtr inputImage;
		std::string outputWindowName;

		void SetupMocksAndStubs();
		void SetupParameters();
		void DisplayResult();
	};

ImageUndistortionRectificationTestInterface::ImageUndistortionRectificationTestInterface(std::string dfnName, int buttonWidth, int buttonHeight)
	: DFNTestInterface(dfnName, buttonWidth, buttonHeight), inputImage()
	{
	undistort = new ImageUndistortionRectification();
	SetDFN(undistort);

	MatToFrameConverter converter;
	cv::Mat doubleImage = cv::imread("../../tests/Data/Images/SmestechChair.png", cv::IMREAD_COLOR);
	cvImage = doubleImage(cv::Rect(0,0,doubleImage.cols/2, doubleImage.rows));
	inputImage = converter.Convert(cvImage);
	undistort->imageInput(inputImage);
	outputWindowName = "Image Undistortion Rectification Result";
	}

ImageUndistortionRectificationTestInterface::~ImageUndistortionRectificationTestInterface()
	{
	delete(stubInputCache);
	delete(mockInputConverter);
	delete(stubOutputCache);
	delete(mockOutputConverter);
	delete(undistort);
	delete(inputImage);
	}

void ImageUndistortionRectificationTestInterface::SetupMocksAndStubs()
	{
	stubInputCache = new Stubs::CacheHandler<FrameConstPtr, cv::Mat>();
	mockInputConverter = new Mocks::FrameToMatConverter();
	ConversionCache<FrameConstPtr, cv::Mat, FrameToMatConverter>::Instance(stubInputCache, mockInputConverter);

	stubOutputCache = new Stubs::CacheHandler<cv::Mat, FrameConstPtr>();
	mockOutputConverter = new Mocks::MatToFrameConverter();
	ConversionCache<cv::Mat, FrameConstPtr, MatToFrameConverter>::Instance(stubOutputCache, mockOutputConverter);
	}

void ImageUndistortionRectificationTestInterface::SetupParameters()
	{
	AddParameter("GeneralParameters", "ConstantBorderValue", 0, 100, 1e-2);
	AddParameter("GeneralParameters", "InterpolationMethod", 0, 3);
	AddParameter("GeneralParameters", "BorderMode", 0, 2);
	AddParameter("GeneralParameters", "CameraConfigurationMode", 0, 2);

	AddParameter("CameraMatrix", "FocalLengthX", 693.4181807813, 700, 1e-5);
	AddParameter("CameraMatrix", "FocalLengthY", 690.36629049483, 700, 1e-5);
	AddParameter("CameraMatrix", "PrinciplePointX", 671.7716154809, 700, 1e-5);
	AddParameter("CameraMatrix", "PrinciplePointY", 391.33378485796, 700, 1e-5);

	AddParameter("Distortion", "UseK3", 1, 1);
	AddParameter("Distortion", "UseK4ToK6", 1, 1);
	AddParameter("Distortion", "K1", 0.39160333319788, 100, 1e-5);
	AddSignedParameter("Distortion", "K2", -67.273810332838, 100, 1e-5);
	AddParameter("Distortion", "P1", 0.00056073969847596, 100, 1e-5);
	AddSignedParameter("Distortion", "P2", -0.0035917800798291, 100, 1e-5);
	AddParameter("Distortion", "K3", 237.61237318275, 300, 1e-5);
	AddParameter("Distortion", "K4", 0.35372515932617, 100, 1e-5);
	AddSignedParameter("Distortion", "K5", -66.934609418439, 100, 1e-5);
	AddParameter("Distortion", "K6", 236.75743075463, 300, 1e-5);

	AddParameter("ImageSize", "Width", 1280, 1280);
	AddParameter("ImageSize", "Height", 720, 720);

	AddParameter("RectificationMatrix", "Element_0_0", 0.99998692232279, 1, 1e-8);
	AddParameter("RectificationMatrix", "Element_0_1", 0.00045433768478094, 1, 1e-8);
	AddSignedParameter("RectificationMatrix", "Element_0_2", -0.0050939926049538, 1, 1e-8);
	AddSignedParameter("RectificationMatrix", "Element_1_0", -0.00046438147916384, 1, 1e-8);
	AddParameter("RectificationMatrix", "Element_1_1", 0.999997950374, 1, 1e-8);
	AddSignedParameter("RectificationMatrix", "Element_1_2", -0.0019706845597646, 1, 1e-8);
	AddParameter("RectificationMatrix", "Element_2_0", 0.0050930868079138, 1, 1e-8);
	AddParameter("RectificationMatrix", "Element_2_1", 0.0019730243436088, 1, 1e-8);
	AddParameter("RectificationMatrix", "Element_2_2", 0.99998508370961, 1, 1e-8);
	}

void ImageUndistortionRectificationTestInterface::DisplayResult()
	{
	FrameConstPtr undistortedImage= undistort->filteredImageOutput();
	FrameToMatConverter converter;
	cv::Mat undistortedCvImage = converter.Convert(undistortedImage);

	cv::namedWindow(outputWindowName, CV_WINDOW_NORMAL);
	cv::imshow(outputWindowName, undistortedCvImage);
	cv::namedWindow("original", CV_WINDOW_NORMAL);
	cv::imshow("original", cvImage);
	PRINT_TO_LOG("The processing took (seconds): ", GetLastProcessingTimeSeconds() );
	PRINT_TO_LOG("Virtual Memory used (Kb): ", GetTotalVirtualMemoryUsedKB() );

	delete(undistortedImage);
	}


int main(int argc, char** argv)
	{
	ImageUndistortionRectificationTestInterface interface("ImageUndistortionRectification", 100, 40);
	interface.Run();
	};

/** @} */
