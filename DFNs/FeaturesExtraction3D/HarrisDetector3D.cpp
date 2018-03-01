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
	parametersHelper.AddParameter<OutputFormat, OutputFormatHelper>("GeneralParameters", "OutputFormat", parameters.outputFormat, DEFAULT_PARAMETERS.outputFormat);

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
	pcl::PointIndicesConstPtr harrisPoints = ComputeHarrisPoints(inputPointCloud);
	outFeaturesSet = Convert(inputPointCloud, harrisPoints);
	}

HarrisDetector3D::OutputFormatHelper::OutputFormatHelper(const std::string& parameterName, OutputFormat& boundVariable, const OutputFormat& defaultValue) :
	ParameterHelper(parameterName, boundVariable, defaultValue)
	{

	}

HarrisDetector3D::OutputFormat HarrisDetector3D::OutputFormatHelper::Convert(const std::string& outputFormat)
	{
	if (outputFormat == "Positions" || outputFormat == "0")
		{
		return POSITIONS_OUTPUT;
		}
	else if (outputFormat == "References" || outputFormat == "1")
		{
		return REFERENCES_OUTPUT;
		}
	ASSERT(false, "ShotDescriptor3d Error: unhandled output format");
	return POSITIONS_OUTPUT;
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
	.method = pcl::HarrisKeypoint3D<pcl::PointXYZ, pcl::PointXYZI>::HARRIS,
	.outputFormat = POSITIONS_OUTPUT,
	};


pcl::PointIndicesConstPtr HarrisDetector3D::ComputeHarrisPoints(pcl::PointCloud<pcl::PointXYZ>::ConstPtr pointCloud)
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
	pcl::PointIndicesConstPtr harrisPoints = detector.getKeypointsIndices();

	return harrisPoints;
	}

VisualPointFeatureVector3DConstPtr HarrisDetector3D::Convert(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr inputCloud, const pcl::PointIndicesConstPtr indicesList)
	{
	VisualPointFeatureVector3DPtr featuresVector = new VisualPointFeatureVector3D();
	ClearPoints(*featuresVector);
	for(unsigned pointIndex = 0; pointIndex < indicesList->indices.size(); pointIndex++)
		{
		if (parameters.outputFormat == POSITIONS_OUTPUT)
			{
			pcl::PointXYZ point = inputCloud->points.at(indicesList->indices.at(pointIndex));
			AddPoint(*featuresVector, point.x, point.y, point.z);
			}
		else if (parameters.outputFormat == REFERENCES_OUTPUT)
			{
			AddPoint(*featuresVector, indicesList->indices.at(pointIndex) );
			}
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
	for(unsigned pointIndex; pointIndex < pointCloud->points.size(); pointIndex++)
		{
		pcl::PointXYZ point = pointCloud->points.at(pointIndex);
		if (point.x != point.x || point.y != point.y || point.z != point.z)
			{
			ASSERT(false, "HarrisDetector3D Error: Invalid point in input point cloud");
			}
		}
	}


}


/** @} */
