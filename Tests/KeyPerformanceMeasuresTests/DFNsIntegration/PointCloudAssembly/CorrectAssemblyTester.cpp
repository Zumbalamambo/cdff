/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file CorrectAssemblyTester.cpp
 * @date 11/10/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 *
 * Implementation of the CorrectAssemblyTester class.
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
#include "CorrectAssemblyTester.hpp"
#include <pcl/io/ply_io.h>
#include <ctime>

using namespace CDFF::DFN;
using namespace Converters;
using namespace PointCloudWrapper;
using namespace PoseWrapper;

/* --------------------------------------------------------------------------
 *
 * Public Member Functions
 *
 * --------------------------------------------------------------------------
 */
CorrectAssemblyTester::CorrectAssemblyTester(std::string configurationFile, PointCloudAssemblyInterface* assemblyDfn, std::string transformerConfigurationFile, PointCloudTransformInterface* transformDfn) :
	assembler( assemblyDfn ),
	transformer( transformDfn )
	{
	this->transformerConfigurationFile = transformerConfigurationFile;
	this->configurationFile = configurationFile;
	this->assemblyDfn = assemblyDfn;
	this->transformDfn = transformDfn;
	ConfigureDfns();

	inputCloud = NULL;
	outputCloud = NULL;

	inputsWereLoaded = false;
	}

CorrectAssemblyTester::~CorrectAssemblyTester()
	{

	}

void CorrectAssemblyTester::SetFiles(std::string dataFolderPath, std::string inputPointCloudListFile, std::string outputPointCloudFile)
	{
	this->dataFolderPath = dataFolderPath;
	this->inputPointCloudListFile = inputPointCloudListFile;
	this->outputPointCloudFile = outputPointCloudFile;

	LoadInputPointClouds();
	inputsWereLoaded = true;
	}

void CorrectAssemblyTester::ExecuteDfns()
	{
	Pose3D zeroPose;
	SetPosition(zeroPose, 0, 0, 0);
	SetOrientation(zeroPose, 0, 0, 0, 1);
	float radius = 1e10;

	ASSERT(inputsWereLoaded, "Error: there was a call to ExecuteDfns before actually loading inputs");
	float processingTime = 0;

	int numberOfEntries = pointCloudList.size();
	for(int entryIndex = 0; entryIndex < numberOfEntries; entryIndex++)
		{
		PointCloudFileEntry& entry = pointCloudList.at(entryIndex);
		inputCloud = LoadPointCloud(entry.filePath);

		PRINT_TO_LOG("Processing File: ", entryIndex);		
		float localProcessingTime = 0;
		clock_t localBeginTime, localEndTime;
		localBeginTime = clock();
		
		PointCloudConstPtr transformedCloud = NULL;
		transformer.Execute(*inputCloud, entry.pose, transformedCloud);

		outputCloud = NULL;
		assembler.Execute(*transformedCloud, zeroPose, radius, outputCloud);

		DeleteIfNotNull(inputCloud);
		localEndTime = clock();
		localProcessingTime = float(localEndTime - localBeginTime) / CLOCKS_PER_SEC;
		PRINT_TO_LOG("Processing file took (seconds):", localProcessingTime);
		processingTime += localProcessingTime;		
		}
	PRINT_TO_LOG("Total processing took (seconds):", processingTime);
	}

void CorrectAssemblyTester::SaveOutput()
	{
	pcl::PointCloud<pcl::PointXYZ>::ConstPtr pclOutputCloud = pclPointCloudConverter.Convert(outputCloud);
	std::string fileString = dataFolderPath + "/" + outputPointCloudFile;

	pcl::PLYWriter writer;
	writer.write(fileString, *pclOutputCloud, true);	
	}

/* --------------------------------------------------------------------------
 *
 * Private Member Functions
 *
 * --------------------------------------------------------------------------
 */
void CorrectAssemblyTester::LoadInputPointClouds()
	{
	std::string fileString = dataFolderPath + "/" + inputPointCloudListFile;
	std::ifstream file( fileString.c_str() );
	while(file.good())
		{
		std::string pointCloudFilePath;
		float x, y, z, qx, qy, qz, qw;
		file >> pointCloudFilePath;
		file >> x;
		file >> y;
		file >> z;
		file >> qx;
		file >> qy;
		file >> qz;
		file >> qw;

		if (file.good())
			{
			PointCloudFileEntry entry;
			entry.filePath = dataFolderPath + "/" + pointCloudFilePath;
			SetPosition(entry.pose, x, y, z);
			SetOrientation(entry.pose, qx, qy, qz, qw);
			pointCloudList.push_back(entry);
			}
		}
	file.close();
	}

PointCloudConstPtr CorrectAssemblyTester::LoadPointCloud(std::string pointCloudFilePath)
	{
	pcl::PointCloud<pcl::PointXYZ>::Ptr pclPointCloud = boost::make_shared<pcl::PointCloud<pcl::PointXYZ> >();
	pcl::io::loadPLYFile(pointCloudFilePath, *pclPointCloud);

	return pointCloudConverter.Convert(pclPointCloud);
	}

void CorrectAssemblyTester::ConfigureDfns()
	{
	if (configurationFile != "")
		{
		assemblyDfn->setConfigurationFile(configurationFile);
		assemblyDfn->configure();
		}

	if (transformerConfigurationFile != "")
		{
		transformDfn->setConfigurationFile(transformerConfigurationFile);
		transformDfn->configure();
		}
	}

/** @} */