/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file SelectionTester.hpp
 * @date 07/05/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 * 
 * This class will test requirement 4.1.1.3 of deliverable 5.5.
 * "90% of manually defined matches on manually defined features should be detected correctly.  Manually defined matches are considered to be matches between features made by a 
 * human on close inspection of a set of features on a pair of images.  A human will inspect each set of matches and identify those that are incorrect."
 *
 * Requirement 4.1.1.3 has also another statement, but this one is the one that should be verified by the Requirement 4.1.1.2 since it referes to the detection function only.
 *
 * @{
 */

#ifndef SELECTION_TESTER_HPP
#define SELECTION_TESTER_HPP


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <FeaturesDescription2D/FeaturesDescription2DInterface.hpp>
#include <FeaturesMatching2D/FeaturesMatching2DInterface.hpp>
#include <Errors/Assert.hpp>

#include <VisualPointFeatureVector2D.hpp>
#include <Frame.hpp>
#include <Pose.hpp>
#include <BaseTypes.hpp>
#include <CorrespondenceMap2D.hpp>
#include <MatToFrameConverter.hpp>

#include <ConversionCache/ConversionCache.hpp>
#include <Stubs/Common/ConversionCache/CacheHandler.hpp>
#include <Mocks/Common/Converters/MatToFrameConverter.hpp>
#include <Mocks/Common/Converters/FrameToMatConverter.hpp>
#include <Mocks/Common/Converters/MatToVisualPointFeatureVector2DConverter.hpp>
#include <Mocks/Common/Converters/VisualPointFeatureVector2DToMatConverter.hpp>

#include <stdlib.h>
#include <fstream>
#include <string>
#include <vector>

/* --------------------------------------------------------------------------
 *
 * Class definition
 *
 * --------------------------------------------------------------------------
 */
class SelectionTester
	{
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
	public:
		SelectionTester();
		~SelectionTester();

		void SetDfns(dfn_ci::FeaturesDescription2DInterface* descriptor, dfn_ci::FeaturesMatching2DInterface* matcher);
		void SetConfigurationFilePaths(std::string featuresDescriptorConfigurationFilePath, std::string featuresMatcherConfigurationFilePath);
		void SetInputFilesPaths(std::string sourceImageFilePath, std::string sinkImageFilePath, std::string correspondencesImageFilePath);
		void ExecuteDfns();
		bool AreCorrespondencesValid(float percentageThreshold);

	/* --------------------------------------------------------------------
	 * Protected
	 * --------------------------------------------------------------------
	 */
	protected:


	/* --------------------------------------------------------------------
	 * Private
	 * --------------------------------------------------------------------
	 */
	private:
		Stubs::CacheHandler<cv::Mat, FrameWrapper::FrameConstPtr>* stubFrameCache;
		Mocks::MatToFrameConverter* mockFrameConverter;

		Stubs::CacheHandler<FrameWrapper::FrameConstPtr, cv::Mat>* stubInverseFrameCache;
		Mocks::FrameToMatConverter* mockInverseFrameConverter;

		Stubs::CacheHandler<cv::Mat, VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr>* stubVector2dCache;
		Mocks::MatToVisualPointFeatureVector2DConverter* mockVector2dConverter;

		Stubs::CacheHandler<VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr, cv::Mat>* stubInverseVector2dCache;
		Mocks::VisualPointFeatureVector2DToMatConverter* mockInverseVector2dConverter;

		std::string featuresDescriptorConfigurationFilePath;
		std::string featuresMatcherConfigurationFilePath;
		std::string sourceImageFilePath;
		std::string sinkImageFilePath;
		std::string correspondencesImageFilePath;

		FrameWrapper::FrameConstPtr inputSourceFrame, inputSinkFrame;
		VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr inputSourceKeypointsVector, inputSinkKeypointsVector;
		VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr sourceFeaturesVector, sinkFeaturesVector;
		CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr outputCorrespondenceMap;
		CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr referenceCorrespondenceMap;

		Converters::MatToFrameConverter frameConverter;
		dfn_ci::FeaturesDescription2DInterface* descriptor;
		dfn_ci::FeaturesMatching2DInterface* matcher;

		bool dfnsWereLoaded;
		bool inputImagesWereLoaded;
		bool inputKeypointsWereLoaded;
		bool precisionReferenceWasLoaded;

		void SetUpMocksAndStubs();
		void LoadInputImage(const std::string& imageFilePath, FrameWrapper::FrameConstPtr& frame);
		void LoadReferenceCorrespondenceMap();
		void ConfigureDfns();

		bool ValidateCorrespondences(float percentageThreshold);
		bool CorrespondencesAreTheSame(int referenceIndex, int outputIndex);
	};

#endif

/* SelectionTester.hpp */
/** @} */
