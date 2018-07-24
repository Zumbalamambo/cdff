/**
 * @addtogroup DFPCs
 * @{
 */

#include "PoseFusionInterface.hpp"

namespace dfpc_ci
{

PoseFusionInterface::PoseFusionInterface()
{
}

PoseFusionInterface::~PoseFusionInterface()
{
}

void PoseFusionInterface::wheelOdometryInput(const asn1SccTransformWithCovariance& data)
{
    inWheelOdometry = data;
}

void PoseFusionInterface::visualOdometryInput(const asn1SccTransformWithCovariance& data)
{
    inVisualOdometry = data;
}

const asn1SccTransformWithCovariance& PoseFusionInterface::estimatedCurrentPoseOutput() const
{
    return outEstimatedCurrentPose;
}

const asn1SccTransformWithCovariance& PoseFusionInterface::estimatedPastPosesOutput() const
{
    return outEstimatedPastPoses;
}

}

/** @} */
