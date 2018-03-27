/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file PerformanceTestInterface.hpp
 * @date 16/03/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 * 
 * @brief This class is used for extensive testing of the same DFNs, with the aim of computing performance measures.
 *
 * PerformanceTestInterface offers basic mechanisms for executing a DFN multiple time over many combinations of parameters and different inputs. Measure on the performance of the DFN are stored 
 * on a file for future processing.
 *
 * A complete performance test for a DFN requires the following:
 * 1) A class derived from PerformanceTestInterface, the class has to implement the SetNextInputs() and ExtractMeasures() methods;
 * 2) A performance test configuration file, which is the same as the DFN configuration file with the exception that for every parameter it is possible to specify more than one value;
 * 3) SetNextInputs() allows you to load the inputs for a DFN;
 * 4) ExtractMeasures() allows you to extract measures from the output of the DFN; time and memory are already recorded by PerformanceTestInterface.
 * 
 * @{
 */

#ifndef PERFORMANCE_TEST_INTERFACE_HPP
#define PERFORMANCE_TEST_INTERFACE_HPP


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string>
#include <vector>
#include <DFNCommonInterface.hpp>
#include <map>
#include <yaml-cpp/yaml.h>


/* --------------------------------------------------------------------------
 *
 * Class definition
 *
 * --------------------------------------------------------------------------
 */
class PerformanceTestInterface
	{
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
	public:
		/*
		* @brief The constructor requires the folder were the configuration files and outputs will be stored, it also requires the configuration file name and the output file name
		*
		* @param folderPath, the path containing the configuration file, the output file and a temporary DFN configuration file will be written here as well;
		* @param baseConfigurationFileName, the name of the configuration file that contains the range of input parameters for the tests;
		* @param performanceMeasuresFileName, the name of the output file;
		*
		*/
		PerformanceTestInterface(std::string folderPath, std::string baseConfigurationFileName, std::string performanceMeasuresFileName);

		/*
		* @brief The destructor does nothing.
		*
		*/
		~PerformanceTestInterface();

		/*
		* @brief The function will execute the DFN for each input provided by SetNextInputs(), and for each combination of parameters in the Configuration File defined in the constructor.
		*
		*/
		void Run();

		/*
		* @brief The function defines the main DFN the performance test is executed upon.
		*
		*/
		void SetDfn(dfn_ci::DFNCommonInterface* dfn);

	/* --------------------------------------------------------------------
	 * Protected
	 * --------------------------------------------------------------------
	 */
	protected:	
		typedef std::map<std::string, float> MeasuresMap;

		dfn_ci::DFNCommonInterface* dfn;

	/* --------------------------------------------------------------------
	 * Private
	 * --------------------------------------------------------------------
	 */
	private:	
		struct Parameter
			{
			unsigned groupIndex;
			std::string name;
			unsigned optionsNumber;
			unsigned currentOption;
			std::vector<std::string> optionsList;
			};

		static const std::string temporaryConfigurationFileName;
		std::string baseConfigurationFilePath;
		std::string performanceMeasuresFilePath;
		std::string temporaryConfigurationFilePath;

		YAML::Node configuration;
		std::vector<Parameter> changingParametersList;
		bool firstRun;
		bool firstRunOnInput;
		bool firstMeasureTimeForCurrentInput;

		int GetTotalVirtualMemoryUsedKB();
		void SaveNewInputsLine();
		void SaveMeasures(MeasuresMap measuresMap);
		bool PrepareConfigurationFile();
		void SaveToYaml();
		void SaveRunTime(float time, unsigned numberOfTests);
		void ReadConfiguration();
		std::vector<std::string> SplitString(std::string inputString);

		/*
		* @brief This method has to set the inputs of the DFN, it returns true if and only if an input is actually set.
		*
		* The idea is that inputs will be provided one at a time, when all inputs have already been provided the method should return false;
		*
		*/
		virtual bool SetNextInputs() = 0;
		
		/*
		* @brief This method computes a measure of the performance of the DFN as a map from Measure Name (String) to value of the measure (float).
		*
		* The idea is that the output of the DFN is accessed and evaluated. Then ad-hoc performance measures are extracted, for example by comparing the output to some expected or optimal result.
		*
		*/		
		virtual MeasuresMap ExtractMeasures() = 0;

	};

#endif

/* PerformanceTestInterface.hpp */
/** @} */
