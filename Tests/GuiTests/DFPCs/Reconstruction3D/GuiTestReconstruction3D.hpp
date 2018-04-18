/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file GuiTestReconstruction3D.hpp
 * @date 28/03/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 * 
 * This is the main interface class for implementations of the DFPC Reconstruction 3D.
 * It performs some common operations
 *
 * @{
 */

#ifndef GUI_TEST_RECONSTRUCTION_3D_HPP
#define GUI_TEST_RECONSTRUCTION_3D_HPP


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <Reconstruction3D/ReconstructionFromStereo.hpp>
#include <Errors/Assert.hpp>

#include <MatToFrameConverter.hpp>
#include <Frame.hpp>
#include <VisualPointFeatureVector2D.hpp>
#include <VisualPointFeatureVector3D.hpp>
#include <Pose.hpp>
#include <PointCloud.hpp>
#include <SupportTypes.hpp>

#include <Converters/SupportTypes.hpp>
#include <ConversionCache/ConversionCache.hpp>
#include <Stubs/Common/ConversionCache/CacheHandler.hpp>
#include <Mocks/Common/Converters/FrameToMatConverter.hpp>
#include <Mocks/Common/Converters/MatToFrameConverter.hpp>
#include <Mocks/Common/Converters/MatToVisualPointFeatureVector2DConverter.hpp>
#include <Mocks/Common/Converters/VisualPointFeatureVector2DToMatConverter.hpp>
#include <Mocks/Common/Converters/PointCloudToPclPointCloudConverter.hpp>
#include <Mocks/Common/Converters/MatToTransform3DConverter.hpp>
#include <Mocks/Common/Converters/Transform3DToMatConverter.hpp>
#include <Mocks/Common/Converters/PclPointCloudToPointCloudConverter.hpp>
#include <Mocks/Common/Converters/PointCloudToPclPointCloudConverter.hpp>
#include <Mocks/Common/Converters/MatToVisualPointFeatureVector3DConverter.hpp>
#include <Mocks/Common/Converters/PointCloudToPclNormalsCloudConverter.hpp>
#include <Mocks/Common/Converters/EigenTransformToTransform3DConverter.hpp>
#include <Mocks/Common/Converters/VisualPointFeatureVector3DToPclPointCloudConverter.hpp>

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>

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
class GuiTestReconstruction3D
	{
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
	public:
		enum ImageFilesType
			{
			MONO_CAMERA,
			STEREO_CAMERA_ONE_FILE,
			STEREO_CAMERA_TWO_FILES
			};

		GuiTestReconstruction3D(std::string configurationFilePath, std::string imageFilesFolder, std::string imagesListFileName, ImageFilesType imageFilesType);
		~GuiTestReconstruction3D();

		void Run(dfpc_ci::Reconstruction3DInterface& reconstructor3d);

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

		Stubs::CacheHandler<FrameWrapper::FrameConstPtr, cv::Mat>* stubFrameCache;
		Mocks::FrameToMatConverter* mockFrameConverter;

		Stubs::CacheHandler<cv::Mat, FrameWrapper::FrameConstPtr>* stubInverseFrameCache;
		Mocks::MatToFrameConverter* mockInverseFrameConverter;

		Stubs::CacheHandler<cv::Mat, VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr>* stubMatToVectorCache;
		Mocks::MatToVisualPointFeatureVector2DConverter* mockMatToVectorConverter;

		Stubs::CacheHandler<VisualPointFeatureVector2DWrapper::VisualPointFeatureVector2DConstPtr, cv::Mat>* stubVectorToMatCache;
		Mocks::VisualPointFeatureVector2DToMatConverter* mockVectorToMatConverter;

		Stubs::CacheHandler<cv::Mat, PoseWrapper::Pose3DConstPtr>* stubEssentialPoseCache;
		Mocks::MatToPose3DConverter* mockEssentialPoseConverter;

		Stubs::CacheHandler<PoseWrapper::Pose3DConstPtr, cv::Mat>* stubTriangulationPoseCache;
		Mocks::Pose3DToMatConverter* mockTriangulationPoseConverter;

		Stubs::CacheHandler<pcl::PointCloud<pcl::PointXYZ>::ConstPtr, PointCloudWrapper::PointCloudConstPtr>* stubCloudCache;
		Mocks::PclPointCloudToPointCloudConverter* mockCloudConverter;

		Stubs::CacheHandler<PointCloudWrapper::PointCloudConstPtr, pcl::PointCloud<pcl::PointXYZ>::ConstPtr>* stubInverseCloudCache;
		Mocks::PointCloudToPclPointCloudConverter* mockInverseCloudConverter;

		Stubs::CacheHandler<Eigen::Matrix4f, PoseWrapper::Transform3DConstPtr>* stubOutputCache;
		Mocks::EigenTransformToTransform3DConverter* mockOutputConverter;

		Stubs::CacheHandler<VisualPointFeatureVector3DWrapper::VisualPointFeatureVector3DConstPtr, Converters::SupportTypes::PointCloudWithFeatures >* stubVector3dCache;
		Mocks::VisualPointFeatureVector3DToPclPointCloudConverter* mockVector3dConverter;

		Stubs::CacheHandler<Eigen::Matrix4f, PoseWrapper::Transform3DConstPtr>* stubTransform3dCache;
		Mocks::EigenTransformToTransform3DConverter* mockTransform3dConverter;

		std::string imageFilesFolder;
		std::string configurationFilePath;
		std::vector<std::string> imageFileNamesList;
		std::vector<std::string> secondImageFileNamesList;
		ImageFilesType imageFilesType;

		Converters::MatToFrameConverter frameConverter;

		void SetUpMocksAndStubs();
		void LoadImagesList(std::string imagesListFileName);
		bool LoadNextImages(FrameWrapper::FrameConstPtr& leftImage, FrameWrapper::FrameConstPtr& rightImage);

	};

#endif

/* GuiTestReconstruction3D.hpp */
/** @} */
