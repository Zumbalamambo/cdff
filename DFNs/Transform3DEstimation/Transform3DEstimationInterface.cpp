/**
 * @addtogroup DFNs
 * @{
 */

#include "Transform3DEstimationInterface.hpp"

namespace CDFF
{
namespace DFN
{

Transform3DEstimationInterface::Transform3DEstimationInterface() :
inMatches(),
outTransforms(),
outSuccess(),
outError()
{
}

Transform3DEstimationInterface::~Transform3DEstimationInterface()
{
}

void Transform3DEstimationInterface::matchesInput(const asn1SccCorrespondenceMaps3DSequence& data)
{
    inMatches = data;
}

const asn1SccPosesSequence& Transform3DEstimationInterface::transformsOutput() const
{
    return outTransforms;
}

bool Transform3DEstimationInterface::successOutput() const
{
    return outSuccess;
}

float Transform3DEstimationInterface::errorOutput() const
{
    return outError;
}

}
}

/** @} */
