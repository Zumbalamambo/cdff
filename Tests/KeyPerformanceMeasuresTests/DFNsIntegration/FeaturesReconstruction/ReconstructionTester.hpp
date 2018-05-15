/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file ReconstructionTester.hpp
 * @date 15/05/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 * 
 * This class will test requirement 4.1.1.5 of deliverable 5.5.
 * "Triangulated points are guaranteed by the algorithm to be placed within the field of view of the camera."
 *
 * @{
 */

#ifndef RECONSTRUCTION_TESTER_HPP
#define RECONSTRUCTION_TESTER_HPP


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <FundamentalMatrixComputation/FundamentalMatrixComputationInterface.hpp>
#include <CamerasTransformEstimation/CamerasTransformEstimationInterface.hpp>
#include <PointCloudReconstruction2DTo3D/PointCloudReconstruction2DTo3DInterface.hpp>
#include <Errors/Assert.hpp>

#include <Pose.hpp>
#include <Matrix.hpp>
#include <CorrespondenceMap2D.hpp>
#include <PointCloud.hpp>

#include <ConversionCache/ConversionCache.hpp>
#include <Stubs/Common/ConversionCache/CacheHandler.hpp>
#include <Mocks/Common/Converters/MatToTransform3DConverter.hpp>
#include <Mocks/Common/Converters/Transform3DToMatConverter.hpp>

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
class ReconstructionTester
	{
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
	public:
		ReconstructionTester();
		~ReconstructionTester();

		void SetDfns(dfn_ci::FundamentalMatrixComputationInterface* fundamentalMatrixEstimator, 
				dfn_ci::CamerasTransformEstimationInterface* poseEstimator, dfn_ci::PointCloudReconstruction2DTo3DInterface* reconstructor);
		void SetConfigurationFilePaths(std::string fundamentalMatrixEstimatorFilePath, std::string poseEstimatorConfigurationFilePath, std::string reconstructorConfigurationFilePath);
		void SetInputFilePath(std::string inputCorrespodencesFilePath);
		void ExecuteDfns();
		bool AreTriangulatedPointsValid(float fieldOfViewX, float fieldOfViewY);

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
		Stubs::CacheHandler<cv::Mat, PoseWrapper::Transform3DConstPtr>* stubTransformCache;
		Mocks::MatToTransform3DConverter* mockTransformConverter;

		Stubs::CacheHandler<PoseWrapper::Transform3DConstPtr, cv::Mat>* stubInverseTransformCache;
		Mocks::Transform3DToMatConverter* mockInverseTransformConverter;

		std::string poseEstimatorConfigurationFilePath;
		std::string reconstructorConfigurationFilePath;
		std::string fundamentalMatrixEstimatorFilePath;
		std::string inputCorrespodencesFilePath;

		CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr inputCorrespondenceMap;
		MatrixWrapper::Matrix3dConstPtr fundamentalMatrix;
		bool fundamentalMatrixSuccess;
		PoseWrapper::Transform3DConstPtr cameraTransform;
		bool poseEstimatorWasSuccessful;
		PointCloudWrapper::PointCloudConstPtr outputPointCloud;

		dfn_ci::FundamentalMatrixComputationInterface* fundamentalMatrixEstimator;
		dfn_ci::CamerasTransformEstimationInterface* poseEstimator;
		dfn_ci::PointCloudReconstruction2DTo3DInterface* reconstructor;

		bool dfnsWereLoaded;
		bool inputCorrespondencesWereLoaded;
		bool dfnsWereExecuted;

		void SetUpMocksAndStubs();
		void LoadInputCorrespondences();
		void ConfigureDfns();
		bool AllPointsAreInTheFieldOfView(float fieldOfViewX, float fieldOfViewY);
	};

#endif

/* ReconstructionTester.hpp */
/** @} */
