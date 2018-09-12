/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file HuInvariants.cpp
 * @date 07/11/2018
 * @author Irene Sanz
 */

/*!
 * @addtogroup DFNsTest
 * 
 * Testing application for the DFN HuInvariants.
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
#include <PrimitiveMatching/HuInvariants.hpp>
#include <MatToFrameConverter.hpp>
#include <Errors/Assert.hpp>
#include <GuiTests/ParametersInterface.hpp>
#include <GuiTests/MainInterface.hpp>
#include <GuiTests/DFNs/DFNTestInterface.hpp>


using namespace CDFF::DFN::PrimitiveMatching;
using namespace Converters;
using namespace FrameWrapper;


class HuInvariantsTestInterface : public DFNTestInterface
{
	public:
		HuInvariantsTestInterface(std::string dfnName, int buttonWidth, int buttonHeight, std::string imageFilePath = DEFAULT_IMAGE_FILE_PATH);
		~HuInvariantsTestInterface();
	protected:

	private:
		static const std::string DEFAULT_IMAGE_FILE_PATH;
		static const int ORB_DESCRIPTOR_SIZE;

		HuInvariants* huInvariants;

		cv::Mat cvImage;
		FrameConstPtr inputImage;
		std::string outputWindowName;

		void SetupParameters();
		void DisplayResult();
};

const std::string HuInvariantsTestInterface::DEFAULT_IMAGE_FILE_PATH = "../../tests/Data/Images/primitive_matching/test_images/robot.jpg";

HuInvariantsTestInterface::HuInvariantsTestInterface(std::string dfnName, int buttonWidth, int buttonHeight, std::string imageFilePath)
	: DFNTestInterface(dfnName, buttonWidth, buttonHeight), inputImage()
{
	huInvariants = new HuInvariants();
	SetDFN(huInvariants);

    MatToFrameConverter converter;
	if (imageFilePath == DEFAULT_IMAGE_FILE_PATH)
	{
		cvImage = cv::imread(imageFilePath, cv::IMREAD_COLOR);
	}
	else
	{
		cvImage = cv::imread(imageFilePath, cv::IMREAD_COLOR);
	}

	int erosion_size = 5;
	cv::Mat element = getStructuringElement(cv::MORPH_CROSS, cv::Size(2 * erosion_size + 1, 2 * erosion_size + 1), cv::Point(erosion_size, erosion_size) );
    cv::dilate(cvImage, cvImage, element);
    cv::erode(cvImage, cvImage, element);

	inputImage = converter.Convert(cvImage);
	huInvariants->frameInput(*inputImage);
	outputWindowName = "Hu Invariants Result";
}

HuInvariantsTestInterface::~HuInvariantsTestInterface()
{
	delete(huInvariants);
	delete(inputImage);
}

void HuInvariantsTestInterface::SetupParameters()
{
	AddParameter("GeneralParameters", "MinimumArea", 0.0, 1000.0);
}

void HuInvariantsTestInterface::DisplayResult()
{
	asn1SccT_String matched_primitive = huInvariants->primitiveMatchedOutput();
    std::string primitive(reinterpret_cast<char const*>(matched_primitive.arr), matched_primitive.nCount);
    cv::Mat matched_image = cv::imread("../../tests/Data/Images/primitive_matching/templates/"+primitive+".jpg", cv::IMREAD_COLOR);

    PRINT_TO_LOG("Processing time (seconds): ", GetLastProcessingTimeSeconds());
    PRINT_TO_LOG("Virtual memory used (kb): ", GetTotalVirtualMemoryUsedKB());
    PRINT_TO_LOG("Primitive matched: ", primitive);

    const Frame& frame_with_contour = huInvariants->imageWithMatchedContourOutput();
    cv::Mat image_with_contour = FrameToMatConverter().Convert(&frame_with_contour);

    cv::namedWindow("ORIGINAL IMAGE", CV_WINDOW_NORMAL);
    cv::imshow("ORIGINAL IMAGE", image_with_contour);

    cv::namedWindow("MATCHED IMAGE", CV_WINDOW_NORMAL);
    cv::imshow("MATCHED IMAGE", matched_image);

}

int main(int argc, char** argv)
{
	HuInvariantsTestInterface* interface = new HuInvariantsTestInterface("HuInvariants", 100, 40);
	interface->Run();
	delete(interface);
};

/** @} */