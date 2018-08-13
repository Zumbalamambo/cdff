#ifndef BUNDLEHISTORY_HPP
#define BUNDLEHISTORY_HPP

#include <Frame.hpp>
#include <VisualPointFeatureVector2D.hpp>
#include <PointCloud.hpp>
#include <CorrespondenceMap2D.hpp>

#include <vector>
#include <map>

namespace dfpc_ci {

class BundleHistory
	{
	public:
		typedef std::vector<FrameWrapper::FrameConstPtr> ImageList;
		typedef std::vector<VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr> FeatureVectorList;
		typedef std::vector<CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr> CorrespondenceMapList;
		typedef std::vector<PointCloudWrapper::PointCloudConstPtr> PointCloudList;

		BundleHistory(int size);
		~BundleHistory();

		void AddImages(const FrameWrapper::Frame& leftImage, const FrameWrapper::Frame& rightImage);
		void AddFeatures(const VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2D& featureVector, std::string featureCategory = "DEFAULT");
		void AddMatches(const CorrespondenceMap2DWrapper::CorrespondenceMap2D& leftRightCorrespondenceMap, std::string correspondenceCategory = "DEFAULT");
		void AddPointCloud(const PointCloudWrapper::PointCloud&, std::string cloudCategory = "DEFAULT");
		
		FrameWrapper::FrameConstPtr GetLeftImage(int backwardSteps);
		FrameWrapper::FrameConstPtr GetRightImage(int backwardSteps);
		VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr GetFeatures(int backwardSteps, std::string featureCategory = "DEFAULT");
		CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr GetMatches(int backwardSteps, std::string correspondenceCategory = "DEFAULT");
		PointCloudWrapper::PointCloudConstPtr GetPointCloud(int backwardSteps, std::string cloudCategory = "DEFAULT");

		void RemoveEntry(int backwardSteps);
		void RemoveOldestEntry();

	protected:

	private:
		static const int NO_RECENT_ENTRY = -1;
		int size;
		int mostRecentEntryIndex;
		int oldestEntryIndex;		

		ImageList leftImageList;
		ImageList rightImageList;
		std::map<std::string, FeatureVectorList> featureVectorList;
		std::map<std::string, CorrespondenceMapList> leftRightCorrespondenceMapList;
		std::map<std::string, PointCloudList> pointCloudList;

		void AddImages(FrameWrapper::FrameConstPtr leftImage, FrameWrapper::FrameConstPtr rightImage);
		void AddFeatures(VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr featureVector, std::string featureCategory = "DEFAULT");
		void AddMatches(CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr leftRightCorrespondenceMap, std::string correspondenceCategory = "DEFAULT");
		void AddPointCloud(PointCloudWrapper::PointCloudConstPtr, std::string cloudCategory = "DEFAULT");

		int BackwardStepsToIndex(int backwardSteps);
	};

}
#endif
/* BundleHistory.hpp */
/** @} */
