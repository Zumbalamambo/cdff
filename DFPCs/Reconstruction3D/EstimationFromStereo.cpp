/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file EstimationFromStereo.cpp
 * @date 25/07/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNs
 * 
 * Implementation of the EstimationFromStereo class.
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
#include "EstimationFromStereo.hpp"
#include "Errors/Assert.hpp"
#include <Visualizers/OpencvVisualizer.hpp>
#include <Visualizers/PclVisualizer.hpp>
#include <VisualPointFeatureVector3D.hpp>

#define DELETE_PREVIOUS(object) \
	{ \
	if (object != NULL) \
		{ \
		delete(object); \
		object = NULL; \
		} \
	} \

namespace dfpc_ci {

using namespace dfn_ci;
using namespace FrameWrapper;
using namespace PoseWrapper;
using namespace PointCloudWrapper;
using namespace VisualPointFeatureVector2DWrapper;
using namespace VisualPointFeatureVector3DWrapper;
using namespace CorrespondenceMap2DWrapper;
using namespace MatrixWrapper;
using namespace CorrespondenceMap3DWrapper;
using namespace BaseTypesWrapper;

/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
EstimationFromStereo::EstimationFromStereo()
	{
	parametersHelper.AddParameter<float>("GeneralParameters", "PointCloudMapResolution", parameters.pointCloudMapResolution, DEFAULT_PARAMETERS.pointCloudMapResolution);
	parametersHelper.AddParameter<float>("GeneralParameters", "SearchRadius", parameters.searchRadius, DEFAULT_PARAMETERS.searchRadius);
	parametersHelper.AddParameter<int>("GeneralParameters", "NumberOfAdjustedStereoPairs", parameters.numberOfAdjustedStereoPairs, DEFAULT_PARAMETERS.numberOfAdjustedStereoPairs);
	parametersHelper.AddParameter<float>("GeneralParameters", "Baseline", parameters.baseline, DEFAULT_PARAMETERS.baseline);

	leftImage = NewFrame();
	rightImage = NewFrame();
	filteredLeftImage = NULL;
	filteredRightImage = NULL;
	imageCloud = NULL;
	
	leftKeypointVector = NULL;
	rightKeypointVector = NULL;
	leftFeatureVector = NULL;
	rightFeatureVector = NULL;
	leftRightCorrespondenceMap = NULL;
	leftTimeCorrespondenceMap = NULL;
	rightTimeCorrespondenceMap = NULL;
	triangulatedKeypointCloud = NULL;

	correspondenceMapSequence = NewCorrespondenceMaps3DSequence();
	cameraPoseList = NewPoses3DSequence();
	previousCameraPose = NewPose3D();
	emptyFeaturesVector = NewVisualPointFeatureVector3D();

	optionalLeftFilter = NULL;
	optionalRightFilter = NULL;
	reconstructor3d = NULL;
	featuresExtractor2d = NULL;
	optionalFeaturesDescriptor2d = NULL;
	featuresMatcher2d = NULL;
	reconstructor3dfrom2dmatches = NULL;
	transformEstimator = NULL;

	configurationFilePath = "";
	currentInputNumber = 0;
	oldestCameraIndex = 0;
	firstTimeBundle = true;
	}

EstimationFromStereo::~EstimationFromStereo()
	{
	if (optionalLeftFilter != NULL)
		{
		DELETE_PREVIOUS(filteredLeftImage);
		}
	if (optionalRightFilter != NULL)
		{
		DELETE_PREVIOUS(filteredRightImage);
		}
	if (optionalFeaturesDescriptor2d != NULL)
		{
		DELETE_PREVIOUS(leftFeatureVector);
		DELETE_PREVIOUS(rightFeatureVector);
		}
	delete(leftImage);
	delete(rightImage);
	DELETE_PREVIOUS(imageCloud);
	DELETE_PREVIOUS(leftKeypointVector);
	DELETE_PREVIOUS(rightKeypointVector);
	DELETE_PREVIOUS(leftRightCorrespondenceMap);
	DELETE_PREVIOUS(leftTimeCorrespondenceMap);
	DELETE_PREVIOUS(rightTimeCorrespondenceMap);
	DELETE_PREVIOUS(triangulatedKeypointCloud);

	delete(correspondenceMapSequence);
	delete(cameraPoseList);
	DELETE_PREVIOUS(emptyFeaturesVector);
	DELETE_PREVIOUS(previousCameraPose);

	for(int stereoIndex = 0; stereoIndex < imageCloudList.size(); stereoIndex++)
		{
		#ifdef TESTING
		DELETE_PREVIOUS(leftImageList.at(stereoIndex));
		DELETE_PREVIOUS(rightImageList.at(stereoIndex));
		#endif
		DELETE_PREVIOUS(imageCloudList.at(stereoIndex));
		DELETE_PREVIOUS(leftFeatureVectorList.at(stereoIndex));
		DELETE_PREVIOUS(rightFeatureVectorList.at(stereoIndex));
		DELETE_PREVIOUS(leftRightCorrespondenceMapList.at(stereoIndex));
		DELETE_PREVIOUS(triangulatedKeypointCloudList.at(stereoIndex));
		}
	}


void EstimationFromStereo::run() 
	{
	DEBUG_PRINT_TO_LOG("Estimation from stereo start", "");
 
	Copy(inLeftImage, *leftImage);
	Copy(inRightImage, *rightImage);

	FilterImages();
	ComputeStereoPointCloud();
	ComputeVisualPointFeatures();

	if (currentInputNumber+1 < parameters.numberOfAdjustedStereoPairs)
		{
		currentInputNumber++;
		outSuccess = false;
		}
	else
		{
		currentInputNumber = parameters.numberOfAdjustedStereoPairs;
		outSuccess = ComputeCameraPoses();
		}

	if (outSuccess)
		{
		Pose3DConstPtr outputPose;
		if(firstTimeBundle)
			{
			outputPose = AddAllPointCloudsToMap();
			}
		else
			{
			outputPose = AddLastPointCloudToMap();
			}
		Copy(*outputPose, outPose);

		PointCloudWrapper::PointCloudConstPtr outputPointCloud = pointCloudMap.GetScenePointCloudInOrigin(outputPose, parameters.searchRadius);
		Copy(*outputPointCloud, outPointCloud); 
		DEBUG_SHOW_POINT_CLOUD(outputPointCloud);
		DELETE_PREVIOUS(outputPointCloud);
		}
	}

void EstimationFromStereo::setup()
	{
	configurator.configure(configurationFilePath);
	ConfigureExtraParameters(); //Configuration shall happen before alias assignment here.
	AssignDfnsAlias();
	}

/* --------------------------------------------------------------------------
 *
 * Private Member Variables
 *
 * --------------------------------------------------------------------------
 */

const EstimationFromStereo::EstimationFromStereoOptionsSet EstimationFromStereo::DEFAULT_PARAMETERS = 
	{
	.searchRadius = 20,
	.pointCloudMapResolution = 1e-2,
	.numberOfAdjustedStereoPairs = 4,
	.baseline = 1
	};

/* --------------------------------------------------------------------------
 *
 * Private Member Functions
 *
 * --------------------------------------------------------------------------
 */
void EstimationFromStereo::ConfigureExtraParameters()
	{
	parametersHelper.ReadFile( configurator.GetExtraParametersConfigurationFilePath() );

	ASSERT(parameters.pointCloudMapResolution > 0, "EstimationFromStereo Error, Point Cloud Map resolution is not positive");
	pointCloudMap.SetResolution(parameters.pointCloudMapResolution);
	
	for(int stereoIndex = 0; stereoIndex < parameters.numberOfAdjustedStereoPairs; stereoIndex++)
		{
		#ifdef TESTING
		leftImageList.push_back(NewFrame());
		rightImageList.push_back(NewFrame());
		#endif
		leftFeatureVectorList.push_back(NULL);
		rightFeatureVectorList.push_back(NULL);
		leftRightCorrespondenceMapList.push_back(NULL);
		triangulatedKeypointCloudList.push_back(NULL);
		imageCloudList.push_back(NULL);
		}
	}

void EstimationFromStereo::AssignDfnsAlias()
	{
	optionalLeftFilter = static_cast<ImageFilteringInterface*>( configurator.GetDfn("leftFilter", true) );
	optionalRightFilter = static_cast<ImageFilteringInterface*>( configurator.GetDfn("rightFilter", true) );
	reconstructor3d = static_cast<StereoReconstructionInterface*>( configurator.GetDfn("reconstructor3d") );
	featuresExtractor2d = static_cast<FeaturesExtraction2DInterface*>( configurator.GetDfn("featuresExtractor2d") );
	optionalFeaturesDescriptor2d = static_cast<FeaturesDescription2DInterface*>( configurator.GetDfn("featuresDescriptor2d", true) );
	featuresMatcher2d = static_cast<FeaturesMatching2DInterface*>( configurator.GetDfn("featuresMatcher2d") );
	reconstructor3dfrom2dmatches = static_cast<PointCloudReconstruction2DTo3DInterface*>( configurator.GetDfn("reconstructor3dfrom2dmatches", true) );
	transformEstimator = static_cast<Transform3DEstimationInterface*>( configurator.GetDfn("transformEstimator") );

	ASSERT(reconstructor3d != NULL, "DFPC Estimation from stereo error: reconstructor3D DFN configured incorrectly");
	ASSERT(featuresExtractor2d != NULL, "DFPC Estimation from stereo error: featuresExtractor2d DFN configured incorrectly");
	ASSERT(featuresMatcher2d != NULL, "DFPC Estimation from stereo error: featuresMatcher3d DFN configured incorrectly");
	ASSERT(reconstructor3dfrom2dmatches != NULL, "DFPC Estimation from stereo error: reconstructor3dfrom2dmatches DFN configured incorrectly");		
	ASSERT(transformEstimator != NULL, "DFPC Estimation from stereo error: bundleAdjuster DFN configured incorrectly");

	if (optionalLeftFilter != NULL)
		{
		filteredLeftImage = NewFrame();
		}
	if (optionalRightFilter != NULL)
		{
		filteredRightImage = NewFrame();
		}
	}

/**
* The method filters the left and right images, and uses them for the computation of a point cloud.
*
**/
void EstimationFromStereo::FilterImages()
	{
	FilterImage(leftImage, optionalLeftFilter, filteredLeftImage);
	FilterImage(rightImage, optionalRightFilter, filteredRightImage);
	}

void EstimationFromStereo::FilterImage(FramePtr image, ImageFilteringInterface* filter, FramePtr& filteredImage)
	{
	if (filter != NULL)
		{
		filter->imageInput(*image);
		filter->process();
		Copy(optionalLeftFilter->imageOutput(), *filteredImage);
		DEBUG_PRINT_TO_LOG("Filtered Frame", "");
		DEBUG_SHOW_IMAGE(filteredImage);
		}
	else
		{
		filteredImage = image;
		}
	}

void EstimationFromStereo::ComputeStereoPointCloud()
	{
	reconstructor3d->leftInput(*filteredLeftImage);
	reconstructor3d->rightInput(*filteredRightImage);
	reconstructor3d->process();

	PointCloudPtr newPointCloud = NewPointCloud();
	Copy(reconstructor3d->pointcloudOutput(), *newPointCloud);

	//Adding the extracted point cloud to storage
	imageCloud = newPointCloud;
	if (currentInputNumber < parameters.numberOfAdjustedStereoPairs)
		{
		imageCloudList.at(currentInputNumber) = imageCloud;
		}
	else
		{
		DELETE_PREVIOUS( imageCloudList.at(oldestCameraIndex) );
		imageCloudList.at(oldestCameraIndex) = imageCloud;
		}

	DEBUG_PRINT_TO_LOG("Point Cloud", GetNumberOfPoints(*imageCloud));
	DEBUG_SHOW_POINT_CLOUD(imageCloud);
	}

#define MINIMUM(a, b) ( a < b ? a : b )

void EstimationFromStereo::ComputeVisualPointFeatures()
	{
	ExtractFeatures(filteredLeftImage, leftKeypointVector);
	ExtractFeatures(filteredRightImage, rightKeypointVector);
	DescribeFeatures(filteredLeftImage, leftKeypointVector, leftFeatureVector);
	DescribeFeatures(filteredRightImage, rightKeypointVector, rightFeatureVector);
	leftRightCorrespondenceMap = MatchFeatures(leftFeatureVector, rightFeatureVector);
	DEBUG_SHOW_2D_CORRESPONDENCES(filteredLeftImage, filteredRightImage, leftRightCorrespondenceMap);
	ComputeKeypointCloud(leftRightCorrespondenceMap);

	int pastLength;
	if (currentInputNumber < parameters.numberOfAdjustedStereoPairs)
		{
		#ifdef TESTING
		Copy(*filteredLeftImage, *(leftImageList.at(currentInputNumber)));
		Copy(*filteredRightImage, *(rightImageList.at(currentInputNumber)));
		#endif
		leftFeatureVectorList.at(currentInputNumber) = leftFeatureVector;
		rightFeatureVectorList.at(currentInputNumber) = rightFeatureVector;
		leftRightCorrespondenceMapList.at(currentInputNumber) = leftRightCorrespondenceMap;
		triangulatedKeypointCloudList.at(currentInputNumber) = triangulatedKeypointCloud;
		pastLength = currentInputNumber + 1;
		}
	else
		{
		#ifdef TESTING
		Copy(*filteredLeftImage, *(leftImageList.at(oldestCameraIndex)));
		Copy(*filteredRightImage, *(rightImageList.at(oldestCameraIndex)));	
		#endif	
		DELETE_PREVIOUS( leftFeatureVectorList.at(oldestCameraIndex) );
		leftFeatureVectorList.at(oldestCameraIndex) = leftFeatureVector;
		DELETE_PREVIOUS( rightFeatureVectorList.at(oldestCameraIndex) );
		rightFeatureVectorList.at(oldestCameraIndex) = rightFeatureVector;
		DELETE_PREVIOUS( leftRightCorrespondenceMapList.at(oldestCameraIndex) );
		leftRightCorrespondenceMapList.at(oldestCameraIndex) = leftRightCorrespondenceMap;
		DELETE_PREVIOUS( triangulatedKeypointCloudList.at(oldestCameraIndex) );
		triangulatedKeypointCloudList.at(oldestCameraIndex) = triangulatedKeypointCloud;
		pastLength = parameters.numberOfAdjustedStereoPairs;
		oldestCameraIndex = (oldestCameraIndex+1) % parameters.numberOfAdjustedStereoPairs;
		}
	DEBUG_PRINT_TO_LOG("pastLength", pastLength);
	DEBUG_PRINT_TO_LOG("oldestCameraIndex", oldestCameraIndex);

	//Computing the correspondences between current and past features and adding them to storage
	CorrespondenceMaps3DSequencePtr newSequence = NewCorrespondenceMaps3DSequence();
	int startIndex = (oldestCameraIndex - 2) % parameters.numberOfAdjustedStereoPairs;
	if (startIndex < 0)
		{
		startIndex = startIndex + parameters.numberOfAdjustedStereoPairs;
		}	
	DEBUG_PRINT_TO_LOG("startIndex", startIndex);
	for(int time = 0; time < pastLength - 1; time++)
		{
		DEBUG_PRINT_TO_LOG("time", time);
		int cameraIndex = (startIndex - time) % parameters.numberOfAdjustedStereoPairs;
		if (cameraIndex < 0)
			{
			cameraIndex = cameraIndex + parameters.numberOfAdjustedStereoPairs;
			}
		DEBUG_PRINT_TO_LOG("camera index", cameraIndex);
		leftTimeCorrespondenceMap = MatchFeatures( leftFeatureVector, leftFeatureVectorList.at(cameraIndex) );
		rightTimeCorrespondenceMap = MatchFeatures( rightFeatureVector, rightFeatureVectorList.at(cameraIndex) );		
		CorrespondenceMap2DConstPtr pastCorrespondenceMap = leftRightCorrespondenceMapList.at(cameraIndex);
		PointCloudConstPtr pastKeypointCloud = triangulatedKeypointCloudList.at(cameraIndex);

		std::vector<CorrespondenceMap2DConstPtr> correspondenceMapList = {leftRightCorrespondenceMap, leftTimeCorrespondenceMap, rightTimeCorrespondenceMap, pastCorrespondenceMap};
		std::vector<PointCloudConstPtr> pointCloudList = {triangulatedKeypointCloud, pastKeypointCloud};
		#ifdef TESTING
		std::vector<FrameConstPtr> imageList = { filteredLeftImage, filteredRightImage, leftImageList.at(cameraIndex), rightImageList.at(cameraIndex) };
		DEBUG_SHOW_QUADRUPLE_2D_CORRESPONDENCES( imageList, correspondenceMapList );
		#endif

		CorrespondenceMap3DPtr newCorrespondenceMap = Extract3DCorrespondencesFromTwoImagePairs(correspondenceMapList, pointCloudList);
		AddCorrespondenceMap(*newSequence, *newCorrespondenceMap);
		} 
	DEBUG_PRINT_TO_LOG("correspondenceMapSequence size", GetNumberOfCorrespondenceMaps(*correspondenceMapSequence));
	DEBUG_PRINT_TO_LOG("currentInputNumber", currentInputNumber);

	//Adding the old correspondences to the current ones. All correspondences are taken except those with the oldest left and right images
	if (0 < currentInputNumber && currentInputNumber < parameters.numberOfAdjustedStereoPairs)
		{
		for(int correspondenceIndex = 0; correspondenceIndex < GetNumberOfCorrespondenceMaps(*correspondenceMapSequence); correspondenceIndex++)
			{
			AddCorrespondenceMap(*newSequence, GetCorrespondenceMap(*correspondenceMapSequence, correspondenceIndex));
			}
		}
	else if (currentInputNumber > 0)
		{
		int correspondenceIndex = 0;
		for(int sourceIndex = 0; sourceIndex < (parameters.numberOfAdjustedStereoPairs - 1); sourceIndex++)
			{
			for(int sinkIndex = sourceIndex + 1; sinkIndex < (parameters.numberOfAdjustedStereoPairs - 1); sinkIndex++)
				{
				AddCorrespondenceMap(*newSequence, GetCorrespondenceMap(*correspondenceMapSequence, correspondenceIndex));	
				correspondenceIndex++;
				}
			correspondenceIndex += 2;
			}
		}

	//Updating the latestCorrespondenceMapsSequence
	DELETE_PREVIOUS(correspondenceMapSequence);
	correspondenceMapSequence = newSequence;
	}

void EstimationFromStereo::ExtractFeatures(FrameWrapper::FrameConstPtr filteredImage, VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr& keypointVector)
	{
	featuresExtractor2d->frameInput(*filteredImage);
	featuresExtractor2d->process();

	VisualPointFeatureVector2DPtr newKeypointVector = NewVisualPointFeatureVector2D();
	Copy(featuresExtractor2d->featuresOutput(), *newKeypointVector);
	keypointVector = newKeypointVector;

	DEBUG_PRINT_TO_LOG("Extracted Current Features", GetNumberOfPoints(*keypointVector) );
	}

void EstimationFromStereo::DescribeFeatures(FrameConstPtr image, VisualPointFeatureVector2DConstPtr keypointVector, VisualPointFeatureVector2DConstPtr& featureVector)
	{
	if (optionalFeaturesDescriptor2d != NULL)
		{
		optionalFeaturesDescriptor2d->frameInput(*image);
		optionalFeaturesDescriptor2d->featuresInput(*keypointVector);
		optionalFeaturesDescriptor2d->process();

		VisualPointFeatureVector2DPtr newFeatureVector = NewVisualPointFeatureVector2D();
		Copy(optionalFeaturesDescriptor2d->featuresOutput(), *newFeatureVector);
		featureVector = newFeatureVector;

		DEBUG_PRINT_TO_LOG("Described Current Features", GetNumberOfPoints(*featureVector) );
		DELETE_PREVIOUS(keypointVector); //Keypoints are no longer needed
		}
	else
		{
		featureVector = keypointVector;
		}
	}

CorrespondenceMap2DConstPtr EstimationFromStereo::MatchFeatures(VisualPointFeatureVector2DConstPtr sourceFeatureVector, VisualPointFeatureVector2DConstPtr sinkFeatureVector)
	{
	featuresMatcher2d->sourceFeaturesInput(*sourceFeatureVector);
	featuresMatcher2d->sinkFeaturesInput(*sinkFeatureVector);
	featuresMatcher2d->process();
	CorrespondenceMap2DPtr newCorrespondence = NewCorrespondenceMap2D();
	Copy(featuresMatcher2d->matchesOutput(), *newCorrespondence);	
	DEBUG_PRINT_TO_LOG("Correspondences", GetNumberOfCorrespondences(*newCorrespondence) );
	return newCorrespondence;
	}

bool EstimationFromStereo::ComputeCameraPoses()
	{
	transformEstimator->matchesInput(*correspondenceMapSequence);
	transformEstimator->process();
	bool successOutput = transformEstimator->successOutput();
	
	if (successOutput)
		{
		Copy(transformEstimator->transformsOutput(), *cameraPoseList);
		for(int poseIndex = 0; poseIndex < GetNumberOfPoses(*cameraPoseList); poseIndex++)
			{
			DEBUG_PRINT_TO_LOG("Transform Estimation success", ToString( GetPose(*cameraPoseList, poseIndex) ) );
			}
		}
	else
		{
		DEBUG_PRINT_TO_LOG("Transform Estimation failure", "");
		}
	
	if (!successOutput)
		{
		return successOutput;
		}

	Pose3D firstPose = GetPose(*cameraPoseList, 0);
	bool validPose = GetXOrientation(firstPose) > 0 || GetYOrientation(firstPose) > 0 || GetZOrientation(firstPose) > 0 || GetWOrientation(firstPose) > 0;
	if (validPose)
		{
		DEBUG_PRINT_TO_LOG("The first pose is valid", "");
		}
	else
		{
		DEBUG_PRINT_TO_LOG("The first pose is NOT valid", "");
		}
	return validPose;
	}

PoseWrapper::Pose3DConstPtr EstimationFromStereo::AddAllPointCloudsToMap()
	{
	for(int stereoIndex = 0; stereoIndex < parameters.numberOfAdjustedStereoPairs; stereoIndex++)
		{
		const Pose3D& pose = GetPose(*cameraPoseList, parameters.numberOfAdjustedStereoPairs - 1 - stereoIndex);
		pointCloudMap.AddPointCloud(imageCloudList.at(stereoIndex), emptyFeaturesVector, &pose);
		}
	Copy(GetPose(*cameraPoseList, 0), *previousCameraPose);
	return previousCameraPose;
	}

PoseWrapper::Pose3DConstPtr EstimationFromStereo::AddLastPointCloudToMap()
	{
	const Pose3D& inversePose = GetPose(*cameraPoseList, 1);
	
	float newPoseX = GetXPosition(*previousCameraPose) - GetXPosition(inversePose);
	float newPoseY = GetYPosition(*previousCameraPose) - GetYPosition(inversePose);
	float newPoseZ = GetZPosition(*previousCameraPose) - GetZPosition(inversePose);
	float newOrientationX = GetXOrientation(*previousCameraPose) - GetXOrientation(inversePose);
	float newOrientationY = GetYOrientation(*previousCameraPose) - GetYOrientation(inversePose);
	float newOrientationZ = GetZOrientation(*previousCameraPose) - GetZOrientation(inversePose);
	float newOrientationW = GetWOrientation(*previousCameraPose) - GetWOrientation(inversePose);
	float norm = std::sqrt(newOrientationX*newOrientationX + newOrientationY * newOrientationY + newOrientationZ * newOrientationZ + newOrientationW * newOrientationW);

	SetPosition(*previousCameraPose, newPoseX, newPoseY, newPoseZ);
	SetOrientation(*previousCameraPose, newOrientationX/norm, newOrientationY/norm, newOrientationZ/norm, newOrientationW/norm);

	return previousCameraPose;
	}

void EstimationFromStereo::ComputeKeypointCloud(CorrespondenceMap2DConstPtr inputCorrespondenceMap)
	{
	Pose3D rightCameraPose;
	SetPosition(rightCameraPose, -parameters.baseline, 0, 0);
	SetOrientation(rightCameraPose, 0, 0, 0, 1);

	reconstructor3dfrom2dmatches->poseInput(rightCameraPose);
	reconstructor3dfrom2dmatches->matchesInput(*inputCorrespondenceMap);
	reconstructor3dfrom2dmatches->process();

	PointCloudPtr newPointCloud = NewPointCloud();
	Copy( reconstructor3dfrom2dmatches->pointcloudOutput(), *newPointCloud);
	DEBUG_PRINT_TO_LOG("Left Right Point Cloud", GetNumberOfPoints(*newPointCloud));
	DEBUG_SHOW_POINT_CLOUD(newPointCloud);	
	triangulatedKeypointCloud = newPointCloud;

	bool thereIsOne3dPointForEachCorrespondence = GetNumberOfPoints(*newPointCloud) == GetNumberOfCorrespondences( *inputCorrespondenceMap );
	VERIFY( thereIsOne3dPointForEachCorrespondence, 
		"EstimationFromStereo error, the reconstructor3dTo2d you used does not output a 3d point for each correspondence, please adjust the option setting or use another DFN");
	}


#define CONTINUE_ON_INVALID_2D_POINT(point) \
	if (point.x != point.x || point.y != point.y) \
		{ \
		continue; \
		}
#define CONTINUE_ON_INVALID_3D_POINT(point) \
	if (point.x != point.x || point.y != point.y || point.z != point.z) \
		{ \
		continue; \
		}
#define CONTINUE_ON_DISTINCT_POINTS(point1, point2) \
	if (point1.x != point2.x || point1.y != point2.y) \
		{ \
		continue; \
		}
#ifndef TESTING
	#define PRINT_CORRESPONDENCE(message, source, sink)
#else
	#define PRINT_CORRESPONDENCE(message, source, sink) \
		{ \
		std::stringstream stream; \
		stream << "(" << source.x << ", " << source.y <<") (" << sink.x << ", " << sink.y << ")"; \
		std::string string = stream.str(); \
		DEBUG_PRINT_TO_LOG(message, string); \
		}
#endif 
CorrespondenceMap3DPtr EstimationFromStereo::Extract3DCorrespondencesFromTwoImagePairs(std::vector<CorrespondenceMap2DConstPtr> mapList, std::vector<PointCloudConstPtr> pointCloudList)
	{
	ASSERT( mapList.size() == 4 && pointCloudList.size() == 2, "EstimationFromStereo::Extract3DCorrespondencesFromTwoImagePairs, input do not have expected size");
	CorrespondenceMap3DPtr newCorrespondenceMap = NewCorrespondenceMap3D();
	CorrespondenceMap2DConstPtr leftRightCorrespondenceMap = mapList.at(0);
	CorrespondenceMap2DConstPtr leftTimeCorrespondenceMap = mapList.at(1);
	CorrespondenceMap2DConstPtr rightTimeCorrespondenceMap = mapList.at(2);
	CorrespondenceMap2DConstPtr pastCorrespondenceMap = mapList.at(3);
	PointCloudConstPtr triangulatedKeypointCloud = pointCloudList.at(0);
	PointCloudConstPtr pastKeypointCloud = pointCloudList.at(1);
	for(int correspondenceIndex1 = 0; correspondenceIndex1 < GetNumberOfCorrespondences(*leftRightCorrespondenceMap); correspondenceIndex1++)
		{
		Point2D leftRightSource = GetSource(*leftRightCorrespondenceMap, correspondenceIndex1);
		Point2D leftRightSink = GetSink(*leftRightCorrespondenceMap, correspondenceIndex1);
		CONTINUE_ON_INVALID_2D_POINT(leftRightSource);
		CONTINUE_ON_INVALID_2D_POINT(leftRightSink);
		for(int correspondenceIndex2 = 0; correspondenceIndex2 < GetNumberOfCorrespondences(*leftTimeCorrespondenceMap); correspondenceIndex2++)
			{
			Point2D leftTimeSource = GetSource(*leftTimeCorrespondenceMap, correspondenceIndex2);
			Point2D leftTimeSink = GetSink(*leftTimeCorrespondenceMap, correspondenceIndex2);
			CONTINUE_ON_INVALID_2D_POINT(leftTimeSource);
			CONTINUE_ON_INVALID_2D_POINT(leftTimeSink);
			CONTINUE_ON_DISTINCT_POINTS(leftRightSource, leftTimeSource);
			for(int correspondenceIndex3 = 0; correspondenceIndex3 < GetNumberOfCorrespondences(*rightTimeCorrespondenceMap); correspondenceIndex3++)
				{
				Point2D rightTimeSource = GetSource(*rightTimeCorrespondenceMap, correspondenceIndex3);
				Point2D rightTimeSink = GetSink(*rightTimeCorrespondenceMap, correspondenceIndex3);
				CONTINUE_ON_INVALID_2D_POINT(rightTimeSource);
				CONTINUE_ON_INVALID_2D_POINT(rightTimeSink);
				CONTINUE_ON_DISTINCT_POINTS(leftRightSink, rightTimeSource);
				for(int correspondenceIndex4 = 0; correspondenceIndex4 < GetNumberOfCorrespondences(*pastCorrespondenceMap); correspondenceIndex4++)
					{
					Point2D pastSource = GetSource(*pastCorrespondenceMap, correspondenceIndex4);
					Point2D pastSink = GetSink(*pastCorrespondenceMap, correspondenceIndex4);
					CONTINUE_ON_INVALID_2D_POINT(pastSource);
					CONTINUE_ON_INVALID_2D_POINT(pastSink);
					CONTINUE_ON_DISTINCT_POINTS(pastSource, leftTimeSink);
					CONTINUE_ON_DISTINCT_POINTS(pastSink, rightTimeSink);

					Point3D sourcePoint, sinkPoint;
					sourcePoint.x = GetXCoordinate(*triangulatedKeypointCloud, correspondenceIndex1);
					sourcePoint.y = GetYCoordinate(*triangulatedKeypointCloud, correspondenceIndex1);
					sourcePoint.z = GetZCoordinate(*triangulatedKeypointCloud, correspondenceIndex1);
					sinkPoint.x = GetXCoordinate(*pastKeypointCloud, correspondenceIndex4);
					sinkPoint.y = GetYCoordinate(*pastKeypointCloud, correspondenceIndex4);
					sinkPoint.z = GetZCoordinate(*pastKeypointCloud, correspondenceIndex4);

					CONTINUE_ON_INVALID_3D_POINT(sourcePoint);
					CONTINUE_ON_INVALID_3D_POINT(sinkPoint);
					AddCorrespondence(*newCorrespondenceMap, sourcePoint, sinkPoint, 1);
					}
				}
			}
		}
	return newCorrespondenceMap;
	}

}



/** @} */
