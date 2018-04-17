/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file ScanlineOptimization.cpp
 * @date 15/03/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNs
 * 
 * Implementation of the ScanlineOptimization class.
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
#include "ScanlineOptimization.hpp"
#include <Errors/Assert.hpp>
#include <ConversionCache/ConversionCache.hpp>
#include <SupportTypes.hpp>
#include <Macros/YamlcppMacros.hpp>
#include <Eigen/Geometry>
#include <pcl/stereo/stereo_matching.h>
#include <PclPointCloudToPointCloudConverter.hpp>
#include <Visualizers/PclVisualizer.hpp>

#include <stdlib.h>
#include <fstream>


using namespace Common;
using namespace Converters;
using namespace PoseWrapper;
using namespace PointCloudWrapper;
using namespace FrameWrapper;
using namespace SupportTypes;
using namespace BaseTypesWrapper;

namespace dfn_ci {


/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
ScanlineOptimization::ScanlineOptimization()
	{
	parametersHelper.AddParameter<int>("GeneralParameters", "CostAggregationRadius", parameters.costAggregationRadius, DEFAULT_PARAMETERS.costAggregationRadius);
	parametersHelper.AddParameter<int>("GeneralParameters", "SpatialBandwidth", parameters.spatialBandwidth, DEFAULT_PARAMETERS.spatialBandwidth);
	parametersHelper.AddParameter<int>("GeneralParameters", "ColorBandwidth", parameters.colorBandwidth, DEFAULT_PARAMETERS.colorBandwidth);
	parametersHelper.AddParameter<int>("GeneralParameters", "StrongSmoothnessPenalty", parameters.strongSmoothnessPenalty, DEFAULT_PARAMETERS.strongSmoothnessPenalty);
	parametersHelper.AddParameter<int>("GeneralParameters", "WeakSmoothnessPenalty", parameters.weakSmoothnessPenalty, DEFAULT_PARAMETERS.weakSmoothnessPenalty);
	parametersHelper.AddParameter<float>("GeneralParameters", "PointCloudSamplingDensity", parameters.pointCloudSamplingDensity, DEFAULT_PARAMETERS.pointCloudSamplingDensity);

	parametersHelper.AddParameter<int>("Matching", "NumberOfDisparities", parameters.matchingOptionsSet.numberOfDisparities, DEFAULT_PARAMETERS.matchingOptionsSet.numberOfDisparities);
	parametersHelper.AddParameter<int>("Matching", "HorizontalOffset", parameters.matchingOptionsSet.horizontalOffset, DEFAULT_PARAMETERS.matchingOptionsSet.horizontalOffset);
	parametersHelper.AddParameter<int>("Matching", "RatioFilter", parameters.matchingOptionsSet.ratioFilter, DEFAULT_PARAMETERS.matchingOptionsSet.ratioFilter);
	parametersHelper.AddParameter<int>("Matching", "PeakFilter", parameters.matchingOptionsSet.peakFilter, DEFAULT_PARAMETERS.matchingOptionsSet.peakFilter);
	parametersHelper.AddParameter<bool>("Matching", "UsePreprocessing", parameters.matchingOptionsSet.usePreprocessing, DEFAULT_PARAMETERS.matchingOptionsSet.usePreprocessing);
	parametersHelper.AddParameter<bool>("Matching", "useLeftRightConsistencyCheck", 
			parameters.matchingOptionsSet.useLeftRightConsistencyCheck, DEFAULT_PARAMETERS.matchingOptionsSet.useLeftRightConsistencyCheck);
	parametersHelper.AddParameter<int>("Matching", "NumberOfDisparities", 
			parameters.matchingOptionsSet.leftRightConsistencyThreshold, DEFAULT_PARAMETERS.matchingOptionsSet.leftRightConsistencyThreshold);

	parametersHelper.AddParameter<float>("StereoCamera", "LeftPrinciplePointX", parameters.cameraParameters.leftPrinciplePointX, DEFAULT_PARAMETERS.cameraParameters.leftPrinciplePointX);
	parametersHelper.AddParameter<float>("StereoCamera", "LeftPrinciplePointY", parameters.cameraParameters.leftPrinciplePointY, DEFAULT_PARAMETERS.cameraParameters.leftPrinciplePointY);
	parametersHelper.AddParameter<float>("StereoCamera", "LeftFocalLength", parameters.cameraParameters.leftFocalLength, DEFAULT_PARAMETERS.cameraParameters.leftFocalLength);
	parametersHelper.AddParameter<float>("StereoCamera", "Baseline", parameters.cameraParameters.baseline, DEFAULT_PARAMETERS.cameraParameters.baseline);

	parametersHelper.AddParameter<float>("ReconstructionSpace", "LimitX", parameters.reconstructionSpace.limitX, DEFAULT_PARAMETERS.reconstructionSpace.limitX);
	parametersHelper.AddParameter<float>("ReconstructionSpace", "LimitY", parameters.reconstructionSpace.limitY, DEFAULT_PARAMETERS.reconstructionSpace.limitY);
	parametersHelper.AddParameter<float>("ReconstructionSpace", "LimitZ", parameters.reconstructionSpace.limitZ, DEFAULT_PARAMETERS.reconstructionSpace.limitZ);

	configurationFilePath = "";
	}

ScanlineOptimization::~ScanlineOptimization()
	{

	}

void ScanlineOptimization::configure()
	{
	parametersHelper.ReadFile(configurationFilePath);
	ValidateParameters();
	}

void ScanlineOptimization::process() 
	{
	PclImagePtr inputLeftImage = Convert(inLeftImage);
	PclImagePtr inputRightImage = Convert(inRightImage);

	PclPointCloudConstPtr pointCloud = ComputePointCloud(inputLeftImage, inputRightImage);
	outPointCloud = SampleCloud(pointCloud);
	}

const ScanlineOptimization::ScanlineOptimizationOptionsSet ScanlineOptimization::DEFAULT_PARAMETERS =
	{
	.costAggregationRadius = 5,
	.spatialBandwidth = 25,
	.colorBandwidth = 15,
	.strongSmoothnessPenalty = 1,
	.weakSmoothnessPenalty = 1,
	.pointCloudSamplingDensity = 1,
	.matchingOptionsSet =
		{
		.numberOfDisparities = 60,
		.horizontalOffset = 0,
		.ratioFilter = 5,
		.peakFilter = 0,
		.usePreprocessing = false,
		.useLeftRightConsistencyCheck = false,
		.leftRightConsistencyThreshold = 1
		},
	.cameraParameters =
		{
		.leftPrinciplePointX = 0,
		.leftPrinciplePointY = 0,
		.leftFocalLength = 1,
		.baseline = 1
		},
	.reconstructionSpace =
		{
		.limitX = 20,
		.limitY = 20,
		.limitZ = 40
		}
	};

ScanlineOptimization::PclPointCloudPtr ScanlineOptimization::ComputePointCloud(PclImagePtr leftImage, PclImagePtr rightImage)
	{
	pcl::AdaptiveCostSOStereoMatching stereo;
	stereo.setRadius(parameters.costAggregationRadius);
	stereo.setGammaS(parameters.spatialBandwidth);
	stereo.setGammaC(parameters.colorBandwidth);
	stereo.setSmoothWeak(parameters.strongSmoothnessPenalty);
	stereo.setSmoothStrong(parameters.weakSmoothnessPenalty);

	stereo.setMaxDisparity(parameters.matchingOptionsSet.numberOfDisparities);
	stereo.setXOffset(parameters.matchingOptionsSet.horizontalOffset);
	stereo.setRatioFilter(parameters.matchingOptionsSet.ratioFilter);
	stereo.setPeakFilter(parameters.matchingOptionsSet.peakFilter);
	stereo.setPreProcessing(parameters.matchingOptionsSet.usePreprocessing);
	stereo.setLeftRightCheck(parameters.matchingOptionsSet.useLeftRightConsistencyCheck);
	stereo.setLeftRightCheckThreshold(parameters.matchingOptionsSet.leftRightConsistencyThreshold);

	stereo.compute(*leftImage, *rightImage);
	
	PclImagePtr visualMap(new PclImage);
	stereo.getVisualMap(visualMap);
	DEBUG_SHOW_PCL_IMAGE(visualMap);

	PclPointCloudPtr pointCloud(new PclPointCloud);
	stereo.getPointCloud
		(
		parameters.cameraParameters.leftPrinciplePointX, 
		parameters.cameraParameters.leftPrinciplePointY, 
		parameters.cameraParameters.leftFocalLength,
		parameters.cameraParameters.baseline,
		pointCloud
		);
	
	return pointCloud;
	}

ScanlineOptimization::PclImagePtr ScanlineOptimization::Convert(FrameConstPtr frame)
	{
	PclImagePtr image(new PclImage);
	image->width = GetFrameWidth(*frame);
	image->height = GetFrameHeight(*frame);
	image->resize( image->width * image->height);

	for(unsigned row = 0; row < image->height; row++)
		{
		for(unsigned column = 0; column < image->width; column++)
			{
			int dataIndex = row * image->width + column;
			image->points.at(dataIndex).r = GetDataByte(*frame, 3*dataIndex + 0);
			image->points.at(dataIndex).g = GetDataByte(*frame, 3*dataIndex + 0);
			image->points.at(dataIndex).b = GetDataByte(*frame, 3*dataIndex + 0);
			}
		}

	return image;
	}

PointCloudConstPtr ScanlineOptimization::SampleCloud(PclPointCloudConstPtr pointCloud)
	{
	PointCloudPtr sampledPointCloud = new PointCloud();

	unsigned validPointCount = 0;
	unsigned pickUpPeriod = static_cast<unsigned>(1 / parameters.pointCloudSamplingDensity) ;
	for(unsigned pointIndex = 0; pointIndex < pointCloud->points.size(); pointIndex++)
		{
		pcl::PointXYZ point = pointCloud->points.at(pointIndex);

		bool validPoint = (point.x == point.x && point.y == point.y && point.z == point.z);
		validPoint = validPoint && ( std::abs(point.x) <= parameters.reconstructionSpace.limitX ) && ( std::abs(point.y) <= parameters.reconstructionSpace.limitY );
		validPoint = validPoint && ( point.z >= 0 ) && ( point.z <= parameters.reconstructionSpace.limitZ );
		if (validPoint)
			{
			validPointCount++;
			if (validPointCount % pickUpPeriod == 0)
				{
				AddPoint(*sampledPointCloud, point.x, point.y, point.z); 
				}
			}
		}

	return sampledPointCloud;
	}

void ScanlineOptimization::ValidateParameters()
	{
	ASSERT(parameters.matchingOptionsSet.numberOfDisparities > 0, "ScanlineOptimization Configuration Error: number of disparities needs to be positive");
	ASSERT(parameters.pointCloudSamplingDensity > 0 && parameters.pointCloudSamplingDensity <= 1, "ScanlineOptimization Configuration Error: pointCloudSamplingDensity has to be in the set (0, 1]");
	ASSERT( parameters.reconstructionSpace.limitX > 0, "ScanlineOptimization Configuration Error: Limits for reconstruction space have to be positive");
	ASSERT( parameters.reconstructionSpace.limitY > 0, "ScanlineOptimization Configuration Error: Limits for reconstruction space have to be positive");
	ASSERT( parameters.reconstructionSpace.limitZ > 0, "ScanlineOptimization Configuration Error: Limits for reconstruction space have to be positive");
	}


}


/** @} */