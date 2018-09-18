/**
 * @author Alessandro Bianco
 */

/**
 * @addtogroup DFNs
 * @{
 */

#ifndef POINTCLOUDASSEMBLY_NEIGHBOURPOINTAVERAGE_HPP
#define POINTCLOUDASSEMBLY_NEIGHBOURPOINTAVERAGE_HPP

#include "PointCloudAssemblyInterface.hpp"

#include <PointCloud.hpp>
#include <Pose.hpp>
#include <PointCloudToPclPointCloudConverter.hpp>
#include <Helpers/ParametersListHelper.hpp>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/keypoints/harris_3d.h>
#include <yaml-cpp/yaml.h>

#include <stdlib.h>
#include <string>

namespace CDFF
{
namespace DFN
{
namespace PointCloudAssembly
{
	/**
	 * Fusion of two point clouds using the neighbour point average algorithm.
	 *
	 * @param maxNeighbourDistance, maximum distance for a point to be considered neighbour of another point
	 * @param incrementalMode, when this is false the output is given by the fusion of the two input clouds, when this is true only the first cloud will be used and will be fused with
	 * the an incrementally constructed stored cloud.
	 */
	class NeighbourPointAverage : public PointCloudAssemblyInterface
	{
		public:

			NeighbourPointAverage();
			virtual ~NeighbourPointAverage();

			virtual void configure();
			virtual void process();

		private:
			struct NeighbourPointAverageOptionsSet
			{
				float maxNeighbourDistance;
				bool useIncrementalMode;
				bool useDistanceFilter;
			};

			Helpers::ParametersListHelper parametersHelper;
			NeighbourPointAverageOptionsSet parameters;
			static const NeighbourPointAverageOptionsSet DEFAULT_PARAMETERS;

			Converters::PointCloudToPclPointCloudConverter pointCloudToPclPointCloud;
			pcl::PointCloud<pcl::PointXYZ>::ConstPtr firstCloud;
			pcl::PointCloud<pcl::PointXYZ>::ConstPtr secondCloud;
			pcl::PointCloud<pcl::PointXYZ>::Ptr storedCloud;
			pcl::PointCloud<pcl::PointXYZ>::Ptr assembledCloud;

			//correspondenceMap: For each point of index x in firstCloud, if correspondenceMap.at(x) is the list of points y in secondCloud, such that x is the 
			//closest neighbour of y in firstCloud.
			std::vector< std::vector<int> > correspondenceMap;

			// For each point P of index x of a point cloud, if x appears in correspondenceMap, then replacementMap[x] is the point that should replace P in the fused cloud.
			std::map<int, pcl::PointXYZ > firstReplacementMap, secondReplacementMap;

			//This method computes the correspondenceMap, only after the call to this method correspondenceMap will be valid.
			void ComputeCorrespondenceMap();

			//This method computes the replacement points, only after this method firstReplacementMap and secondReplacementMap are valid.
			void ComputeReplacementPoints();

			//This method assemble the final point cloud.
			void AssemblePointCloud();
			void AssembleLeftoverPoints(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, const std::map<int, pcl::PointXYZ >& replacementMap);

			pcl::PointXYZ ComputeAveragePoint(pcl::PointCloud<pcl::PointXYZ>::ConstPtr cloud, const std::vector<int>& indexList);
			pcl::PointXYZ ComputeAveragePoint(const pcl::PointXYZ& firstPoint, const pcl::PointXYZ& secondPoint);
			pcl::PointXYZ DisplacePointBySameDistance(const pcl::PointXYZ& pointToDisplace, const pcl::PointXYZ& startDistanceReference, const pcl::PointXYZ& endDistanceReference);

			void PrepareOutAssembledPointCloud();

			void ValidateParameters();
	};
}
}
}

#endif // POINTCLOUDASSEMBLY_NEIGHBOURPOINTAVERAGE_HPP

/** @} */
