/**
 * @addtogroup DFNs
 * @{
 */

#include "PointCloudAssemblyInterface.hpp"

namespace CDFF
{
namespace DFN
{

PointCloudAssemblyInterface::PointCloudAssemblyInterface()
{
}

PointCloudAssemblyInterface::~PointCloudAssemblyInterface()
{
}

void PointCloudAssemblyInterface::firstPointCloudInput(const asn1SccPointcloud& data)
{
    inFirstPointCloud = data;
}

void PointCloudAssemblyInterface::secondPointCloudInput(const asn1SccPointcloud& data)
{
    inSecondPointCloud = data;
}

const asn1SccPointcloud& PointCloudAssemblyInterface::assembledCloudOutput() const
{
    return outAssembledPointCloud;
}

}
}

/** @} */
