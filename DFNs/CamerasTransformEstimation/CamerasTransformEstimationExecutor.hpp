/**
 * @addtogroup DFNs
 * @{
 */

#ifndef CAMERASTRANSFORESTIMATION_EXECUTOR_HPP
#define CAMERASTRANSFORESTIMATION_EXECUTOR_HPP

#include "DFNCommonInterface.hpp"
#include "CamerasTransformEstimationInterface.hpp"
#include <VisualPointFeatureVector2D.h>
#include <Matrix.hpp>
#include <CorrespondenceMap2D.hpp>
#include <Pose.hpp>

namespace CDFF
{
namespace DFN
{

/**
* All the methods in this class execute the DFN for thecomputation of the camera transform. A DFN instance has to be passed in the constructor of these class. Each method takes the following parameters:
* @param inputMatrix: input fundamental matrix
* @param inputMatches: input 2d matches between 2d features.
* @param outputTransform: output estimated pose of the second camera in the reference frame of the first camera.
* @param success: output boolean telling whether the estimation was successfull.
*
* The main difference between the four methods are input and output types:
* Methods (i) and (ii) have the constant pointer as input, Methods (iii)  and (iv) have a constant reference as input;
* Methods (i) and (iii) are non-creation methods, they give constant pointers as output, the output is just the output reference in the DFN;
* When using creation methods, the output has to be initialized to NULL.
* Methods (ii) and (iv) are creation methods, they copy the output of the DFN in the referenced output variable. Method (ii) takes a pointer, method (iv) takes a reference.
*/
    class CamerasTransformEstimationExecutor
    {
        public:

            CamerasTransformEstimationExecutor(CamerasTransformEstimationInterface* dfn);
            ~CamerasTransformEstimationExecutor();

	    void Execute(MatrixWrapper::Matrix3dConstPtr inputMatrix, CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr inputMatches, 
			PoseWrapper::Pose3DConstPtr& outputTransform, bool& success);

	    void Execute(MatrixWrapper::Matrix3dConstPtr inputMatrix, CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr inputMatches, 
			PoseWrapper::Pose3DPtr outputTransform, bool& success);

	    void Execute(const MatrixWrapper::Matrix3d& inputMatrix, const CorrespondenceMap2DWrapper::CorrespondenceMap2D& inputMatches, 
			PoseWrapper::Pose3DConstPtr& outputTransform, bool& success);

	    void Execute(const MatrixWrapper::Matrix3d& inputMatrix, const CorrespondenceMap2DWrapper::CorrespondenceMap2D& inputMatches, 
			PoseWrapper::Pose3D& outputTransform, bool& success);

        private:

            CamerasTransformEstimationInterface* dfn;
    };
}
}

#endif // CAMERASTRANSFORESTIMATION_EXECUTOR_HPP

/** @} */
