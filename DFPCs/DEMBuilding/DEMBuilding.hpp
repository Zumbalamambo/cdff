/**
 * @addtogroup DFPCs
 * @{
 */

#ifndef DEMBUILDING_HPP
#define DEMBUILDING_HPP

#include "DEMBuildingInterface.hpp"

namespace CDFF
{
namespace DFPC
{
    /**
     * TODO Class documentation
     */
    class DEMBuilding : public DEMBuildingInterface
    {
        public:

            DEMBuilding();
            virtual ~DEMBuilding();

            virtual void setup() override;
            virtual void run() override;

    };
}
}

#endif // DEMBUILDING_HPP

/** @} */
