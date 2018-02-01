/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file HarrisDetector3D.cpp
 * @date 01/12/2017
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNs
 * 
 * Implementation of the Harris Detector 3D class.
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
#include "HarrisDetector3D.hpp"
#include <Errors/Assert.hpp>
#include <PointCloudToPclPointCloudConverter.hpp>
#include <MatToVisualPointFeatureVector3DConverter.hpp>
#include <ConversionCache/ConversionCache.hpp>
#include <Macros/YamlcppMacros.hpp>

#include <stdlib.h>
#include <fstream>


using namespace Common;
using namespace Converters;
using namespace VisualPointFeatureVector3DWrapper;
using namespace PointCloudWrapper;

namespace dfn_ci {


/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
HarrisDetector3D::HarrisDetector3D()
	{
	parametersHelper.AddParameter<bool>("GeneralParameters", "NonMaxSuppression", parameters.nonMaxSuppression, DEFAULT_PARAMETERS.nonMaxSuppression);
	parametersHelper.AddParameter<float>("GeneralParameters", "Radius", parameters.radius, DEFAULT_PARAMETERS.radius);
	parametersHelper.AddParameter<float>("GeneralParameters", "SearchRadius", parameters.searchRadius, DEFAULT_PARAMETERS.searchRadius);
	parametersHelper.AddParameter<float>("GeneralParameters", "DetectionThreshold", parameters.detectionThreshold, DEFAULT_PARAMETERS.detectionThreshold);
	parametersHelper.AddParameter<bool>("GeneralParameters", "EnableRefinement", parameters.enableRefinement, DEFAULT_PARAMETERS.enableRefinement);
	parametersHelper.AddParameter<int>("GeneralParameters", "NumberOfThreads", parameters.numberOfThreads, DEFAULT_PARAMETERS.numberOfThreads);
	parametersHelper.AddParameter<HarrisMethod, HarrisMethodHelper>("GeneralParameters", "HarrisMethod", parameters.method, DEFAULT_PARAMETERS.method);

	configurationFilePath = "";
	}

HarrisDetector3D::~HarrisDetector3D()
	{

	}

void HarrisDetector3D::configure()
	{
	parametersHelper.ReadFile(configurationFilePath);
	ValidateParameters();
	}


void HarrisDetector3D::process() 
	{
	pcl::PointCloud<pcl::PointXYZ>::ConstPtr inputPointCloud = 
		ConversionCache<PointCloudConstPtr, pcl::PointCloud<pcl::PointXYZ>::ConstPtr, PointCloudToPclPointCloudConverter>::Convert(inPointCloud);
	ValidateInputs(inputPointCloud);
	cv::Mat harrisPoints = ComputeHarrisPoints(inputPointCloud);
	outFeaturesSet = ConversionCache<cv::Mat, VisualPointFeatureVector3DConstPtr, MatToVisualPointFeatureVector3DConverter>::Convert(harrisPoints);
	}

HarrisDetector3D::HarrisMethodHelper::HarrisMethodHelper(const std::string& parameterName, HarrisMethod& boundVariable, const HarrisMethod& defaultValue) :
	ParameterHelper(parameterName, boundVariable, defaultValue)
	{

	}

HarrisDetector3D::HarrisMethod HarrisDetector3D::HarrisMethodHelper::Convert(const std::string& method)
	{
	if (method == "Harris" || method == "0")
		{
		return pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::HARRIS;
		}
	else if (method == "Noble" || method == "1")
		{
		return pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::NOBLE;
		}
	else if (method == "Lowe" || method == "2")
		{
		return pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::LOWE;
		}
	else if (method == "Tomasi" || method == "3")
		{
		return pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::TOMASI;
		}
	else if (method == "Curvature" || method == "4")
		{
		return pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::CURVATURE;
		}

	ASSERT(false, "HarrisDetector3D Configuration error, Harris Method is not one of {Harris, Noble, Lowe, Tomasi, Curvature}");
	}

const HarrisDetector3D::HarryOptionsSet HarrisDetector3D::DEFAULT_PARAMETERS
	{
	.nonMaxSuppression = true,
	.radius = 0.01,
	.searchRadius = 0.01,
	.detectionThreshold = 0,
	.enableRefinement = false,
	.numberOfThreads = 0,
	.method = pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::HARRIS
	};


cv::Mat HarrisDetector3D::ComputeHarrisPoints(pcl::PointCloud<pcl::PointXYZ>::ConstPtr pointCloud)
	{
	pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI> detector; 
    	detector.setNonMaxSupression (parameters.nonMaxSuppression);
    	detector.setRadius (parameters.radius);
    	detector.setRadiusSearch (parameters.searchRadius);
	detector.setMethod (parameters.method);
	detector.setThreshold(parameters.detectionThreshold);
	detector.setRefine(parameters.enableRefinement);
	detector.setNumberOfThreads(parameters.numberOfThreads);
	//ISSUE: should we use this one more parameter? 
	//detector.setSearchSurface(pointCloud);
    	detector.setInputCloud(pointCloud);

    	pcl::PointCloud<pcl::PointXYZI>::Ptr keypoints = boost::make_shared<pcl::PointCloud<pcl::PointXYZI> >();
    	detector.compute(*keypoints); 
	
	std::vector<unsigned> validIndicesVector;
	for(unsigned pointIndex = 0; pointIndex < keypoints->points.size(); pointIndex++)
		{
		bool validPoint = keypoints->points.at(pointIndex).x == keypoints->points.at(pointIndex).x &&
				  keypoints->points.at(pointIndex).y == keypoints->points.at(pointIndex).y &&
				  keypoints->points.at(pointIndex).z == keypoints->points.at(pointIndex).z;
		if (validPoint)
			validIndicesVector.push_back(pointIndex);
		}	
	
	cv::Mat featuresVector(validIndicesVector.size(), 3, CV_32FC1, cv::Scalar(0));
	for(unsigned indexPosition = 0; indexPosition < validIndicesVector.size(); indexPosition++)
		{
		unsigned validIndex = validIndicesVector.at(indexPosition);
		featuresVector.at<float>(indexPosition, 0) = keypoints->points.at(validIndex).x;
		featuresVector.at<float>(indexPosition, 1) = keypoints->points.at(validIndex).y;
		featuresVector.at<float>(indexPosition, 2) = keypoints->points.at(validIndex).z;
		}

	return featuresVector;
	}


void HarrisDetector3D::ValidateParameters()
	{
	ASSERT( parameters.radius > 0, "HarrisDetector3D Configuration error, radius is not positive");
	ASSERT( parameters.numberOfThreads >= 0, "HarrisDetector3D Configuration error, number of threads is negative");
	VERIFY( parameters.nonMaxSuppression || parameters.detectionThreshold < std::numeric_limits<float>::epsilon(), "Warning HarrisDetector3D: ineffective threshold when non max suppression is false");
	}

void HarrisDetector3D::ValidateInputs(pcl::PointCloud<pcl::PointXYZ>::ConstPtr pointCloud)
	{

	}


}


/** @} */
