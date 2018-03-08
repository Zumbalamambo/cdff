/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* --------------------------------------------------------------------------
*/

/*!
 * @file Map.hpp
 * @date 26/02/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNs
 * 
 *  This a general interface for accessing a map database, a future implementation should link to the DPM.
 *  
 *
 * @{
 */

#ifndef MAP_HPP
#define MAP_HPP

/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <PointCloud.hpp>
#include <Pose.hpp>
#include <BaseTypes.hpp>
#include <Frame.hpp>

namespace dfpc_ci {

/* --------------------------------------------------------------------------
 *
 * Class definition
 *
 * --------------------------------------------------------------------------
 */
    class Map
    {
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
        public:
		Map() { };
		~Map() { };
		virtual void AddFrames(FrameWrapper::FrameConstPtr leftFrame, FrameWrapper::FrameConstPtr rightFrame) = 0;
		virtual FrameWrapper::FrameConstPtr GetNextReferenceLeftFrame() = 0;
		virtual FrameWrapper::FrameConstPtr GetNextReferenceRightFrame() = 0;

		virtual void AddFramePoseInReference(PoseWrapper::Pose3DConstPtr poseInReference) = 0;
		virtual PoseWrapper::Pose3DConstPtr GetCurrentFramePoseInOrigin() = 0;
		virtual void AddPointCloudInLastReference(PointCloudWrapper::PointCloudConstPtr pointCloudInReference) = 0;
		virtual PointCloudWrapper::PointCloudConstPtr GetPartialScene(BaseTypesWrapper::Point3D origin, float radius) = 0;
		virtual PointCloudWrapper::PointCloudConstPtr GetPartialScene(float radius) = 0;
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

    };
}
#endif
/* Map.hpp */
/** @} */
