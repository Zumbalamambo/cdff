/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file PerformanceTestInterface.cpp
 * @date 22/03/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 * 
 * Implementation of the performance test interface class.
 * 
 * 
 * @{
 */


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include "PerformanceTestInterface.hpp"
#include <Errors/Assert.hpp>


/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
PerformanceTestInterface::PerformanceTestInterface(std::string folderPath, std::vector<std::string> baseConfigurationFileNamesList, std::string performanceMeasuresFileName)
	: PerformanceTestBase(folderPath, baseConfigurationFileNamesList, performanceMeasuresFileName)
	{

	}

PerformanceTestInterface::~PerformanceTestInterface()
	{

	}

void PerformanceTestInterface::AddDfn(dfn_ci::DFNCommonInterface* dfn)
	{
	dfnsList.push_back(dfn);
	}

/* --------------------------------------------------------------------------
 *
 * Private Member Functions
 *
 * --------------------------------------------------------------------------
 */
void PerformanceTestInterface::Configure()
	{
	ASSERT(dfnsList.size() == temporaryConfigurationFilePathsList.size(), "Configuration error, the number of dfns is not the same as the number of configuration files");
	for(unsigned dfnIndex = 0; dfnIndex < dfnsList.size(); dfnIndex++)
		{
		dfn_ci::DFNCommonInterface* dfn = dfnsList.at(dfnIndex);
		dfn->setConfigurationFile( temporaryConfigurationFilePathsList.at(dfnIndex) );
		dfn->configure();
		}
	}

void PerformanceTestInterface::Process()
	{
	ExecuteDfns();
	}

/** @} */
