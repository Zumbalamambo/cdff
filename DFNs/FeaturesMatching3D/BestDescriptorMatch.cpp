/**
 * @author Alessandro Bianco
 */

/**
 * @addtogroup DFNs
 * @{
 */

#include "BestDescriptorMatch.hpp"

#include <EigenTransformToTransform3DConverter.hpp>
#include <Macros/YamlcppMacros.hpp>
#include <Errors/Assert.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/search/search.h>
#include <pcl/registration/icp.h>
#include <pcl/common/geometry.h>
#include <yaml-cpp/yaml.h>

//#include <Visualizers/PclVisualizer.hpp>

using namespace Converters;
using namespace PoseWrapper;
using namespace VisualPointFeatureVector3DWrapper;
using namespace SupportTypes;

namespace CDFF
{
namespace DFN
{
namespace FeaturesMatching3D
{

BestDescriptorMatch::BestDescriptorMatch()
{
	parametersHelper.AddParameter<double>(
		"GeneralParameters", "MaxCorrespondenceDistance",
		parameters.maxCorrespondenceDistance, DEFAULT_PARAMETERS.maxCorrespondenceDistance);
	parametersHelper.AddParameter<int>(
		"GeneralParameters", "MaximumIterations",
		parameters.maximumIterations, DEFAULT_PARAMETERS.maximumIterations);
	parametersHelper.AddParameter<double>(
		"GeneralParameters", "TransformationEpsilon",
		parameters.transformationEpsilon, DEFAULT_PARAMETERS.transformationEpsilon);
	parametersHelper.AddParameter<double>(
		"GeneralParameters", "EuclideanFitnessEpsilon",
		parameters.euclideanFitnessEpsilon, DEFAULT_PARAMETERS.euclideanFitnessEpsilon);

	configurationFilePath = "";
}

BestDescriptorMatch::~BestDescriptorMatch()
{
}

void BestDescriptorMatch::configure()
{
	parametersHelper.ReadFile(configurationFilePath);
	ValidateParameters();
}

void BestDescriptorMatch::process()
{
	// Handle empty keypoint vectors
	if (GetNumberOfPoints(inSourceFeatures) == 0 || GetNumberOfPoints(inSinkFeatures) == 0)
	{
		outSuccess = false;
		Reset(outTransform);
		return;
	}

	// Read data from input ports
	PointCloudWithFeatures inputSourceCloud =
		visualPointFeatureVector3DToPclPointCloud.Convert(&inSourceFeatures);
	PointCloudWithFeatures inputSinkCloud =
		visualPointFeatureVector3DToPclPointCloud.Convert(&inSinkFeatures);
	ValidateInputs(inputSourceCloud, inputSinkCloud);

	// Process data
	pcl::PointCloud<pcl::PointXYZ>::Ptr bestMatchSourceCloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >();
	pcl::PointCloud<pcl::PointXYZ>::Ptr bestMatchSinkCloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >();
	ComputeBestMatches(inputSourceCloud, inputSinkCloud, bestMatchSourceCloud, bestMatchSinkCloud);

	if (bestMatchSourceCloud->points.size() == 0)
		{
		outSuccess = false;
		return;
		}

	Pose3DConstPtr tmp = ComputeTransform(bestMatchSourceCloud, bestMatchSinkCloud);

	// Write data to output port
	outSuccess = true;
	Copy(*tmp, outTransform);
	delete tmp;
}

const BestDescriptorMatch::BestDescriptorMatchOptionsSet BestDescriptorMatch::DEFAULT_PARAMETERS =
{
	/*.maxCorrespondenceDistance =*/ 0.05,
	/*.maximumIterations =*/ 50,
	/*.transformationEpsilon =*/ 1e-8,
	/*.euclideanFitnessEpsilon =*/ 1.0
};


void BestDescriptorMatch::ComputeBestMatches( PointCloudWithFeatures sourceCloud, PointCloudWithFeatures sinkCloud, pcl::PointCloud<pcl::PointXYZ>::Ptr bestMatchSourceCloud, 
	pcl::PointCloud<pcl::PointXYZ>::Ptr bestMatchSinkCloud)
	{
	int numberOfSourcePoints = sourceCloud.pointCloud->points.size();
	int numberOfSinkPoints = sinkCloud.pointCloud->points.size();
	std::vector<float> minimumDistances(numberOfSourcePoints);
	std::vector<int> minimumSinkIndex(numberOfSourcePoints);

	for(int sourceIndex = 0; sourceIndex < numberOfSourcePoints; sourceIndex++)
		{
		const FeatureType& sourceDescriptor = sourceCloud.featureCloud->points.at(sourceIndex);
		float& minimumDistance = minimumDistances.at(sourceIndex);
		minimumDistance = std::numeric_limits<float>::infinity();

		for(int sinkIndex = 0; sinkIndex < numberOfSinkPoints; sinkIndex++)
			{
			const FeatureType& sinkDescriptor = sinkCloud.featureCloud->points.at(sinkIndex);

			float descriptorDistance = ComputeDistance(sourceDescriptor, sinkDescriptor, sourceCloud.descriptorSize);
			if (minimumDistance > descriptorDistance)
				{
				minimumDistance = descriptorDistance;
				minimumSinkIndex.at(sourceIndex) = sinkIndex;
				}
			}
		}

	for(int sourceIndex = 0; sourceIndex < numberOfSourcePoints; sourceIndex++)
		{
		if ( minimumDistances.at(sourceIndex) < parameters.maxCorrespondenceDistance )
			{
			pcl::PointXYZ sourcePoint = sourceCloud.pointCloud->points.at(sourceIndex);
			pcl::PointXYZ sinkPoint = sinkCloud.pointCloud->points.at( minimumSinkIndex.at(sourceIndex) );
			bestMatchSourceCloud->points.push_back(sourcePoint);
			bestMatchSinkCloud->points.push_back(sinkPoint);
			}
		}
	}

Pose3DConstPtr BestDescriptorMatch::ComputeTransform(pcl::PointCloud<pcl::PointXYZ>::Ptr sourceCloud, pcl::PointCloud<pcl::PointXYZ>::Ptr sinkCloud)
{
	pcl::registration::TransformationEstimationSVD<pcl::PointXYZ, pcl::PointXYZ, float> registration;

	Eigen::Matrix4f eigenTransform;
	registration.estimateRigidTransformation(*sourceCloud, *sinkCloud, eigenTransform);

	return EigenTransformToTransform3DConverter().Convert(eigenTransform);
}

float BestDescriptorMatch::ComputeDistance(const SupportTypes::FeatureType& feature1, const SupportTypes::FeatureType& feature2, int numberOfFeatureComponents)
	{
	float squaredDistance = 0;
	for(int componentIndex = 0; componentIndex < numberOfFeatureComponents; componentIndex++)
		{
		float componentDistance = feature1.histogram[componentIndex] - feature2.histogram[componentIndex];
		squaredDistance += componentDistance*componentDistance;
		}
	return std::sqrt(squaredDistance);
	}

Pose3DConstPtr BestDescriptorMatch::Convert(Eigen::Matrix4f eigenTransform)
{
	Pose3DPtr transform = new Pose3D;

	Eigen::Matrix3f eigenRotationMatrix = eigenTransform.block(0,0,3,3);
	Eigen::Quaternionf eigenRotation(eigenRotationMatrix);

	SetPosition(*transform, eigenTransform(0, 3), eigenTransform(1, 3), eigenTransform(2, 3) );
	SetOrientation(*transform, eigenRotation.x(), eigenRotation.y(), eigenRotation.z(), eigenRotation.w());

	return transform;
}

void BestDescriptorMatch::ValidateParameters()
{
	ASSERT(parameters.maxCorrespondenceDistance >= 0, "BestDescriptorMatch Configuration error, Max Correspondence Distance is negative");
	ASSERT(parameters.maximumIterations >= 0, "BestDescriptorMatch Configuration error, Maximum Iterations is negative");
	ASSERT(parameters.transformationEpsilon > 0, "BestDescriptorMatch Configuration error, Transformation Epsilon is not strictly positive");
	ASSERT(parameters.euclideanFitnessEpsilon > 0, "BestDescriptorMatch Configuration error, Euclidean Fitness Epsilon is not strictly positive");
}

void BestDescriptorMatch::ValidateInputs(PointCloudWithFeatures sourceCloud, PointCloudWithFeatures sinkCloud)
{
	ASSERT(sourceCloud.descriptorSize == sinkCloud.descriptorSize, "BestDescriptorMatch Error, source cloud and sink cloud have a different descriptor size");
	ValidateCloud(sourceCloud);
	ValidateCloud(sinkCloud);
}

void BestDescriptorMatch::ValidateCloud(PointCloudWithFeatures cloud)
{
	ASSERT(cloud.pointCloud->points.size() == cloud.featureCloud->points.size(), "BestDescriptorMatch Error, point cloud and feature cloud don't have the same size");

	for (unsigned pointIndex = 0; pointIndex < cloud.pointCloud->points.size(); pointIndex++)
	{
		pcl::PointXYZ point = cloud.pointCloud->points.at(pointIndex);
		FeatureType feature = cloud.featureCloud->points.at(pointIndex);
		ASSERT(point.x == point.x, "BestDescriptorMatch Error, cloud contains a NaN point");
		ASSERT(point.y == point.y, "BestDescriptorMatch Error, cloud contains a NaN point");
		ASSERT(point.z == point.z, "BestDescriptorMatch Error, cloud contains a NaN point");
		for (unsigned componentIndex = 0; componentIndex < cloud.descriptorSize; componentIndex++)
		{
			ASSERT(feature.histogram[componentIndex] == feature.histogram[componentIndex], "BestDescriptorMatch Error, feature cloud contains a NaN feature");
		}
		for (unsigned componentIndex = cloud.descriptorSize; componentIndex < MAX_FEATURES_NUMBER; componentIndex++)
		{
			ASSERT(feature.histogram[componentIndex] == 0, "BestDescriptorMatch Error, feature cloud contains an invalid feature (probably coming from a conversion error)");
		}
	}
}

}
}
}

/** @} */