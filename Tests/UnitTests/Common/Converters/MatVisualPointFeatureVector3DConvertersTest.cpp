/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file MatVisualPointFeatureVector3DTest.cpp
 * @date 01/12/2017
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup CommonTests
 * 
 * Testing conversion from Mat to VisualPointFeaturesVector3D and viceversa.
 * 
 * 
 * @{
 */

/* --------------------------------------------------------------------------
 *
 * Definitions
 * Catch definition must be before the includes, otherwise catch will not compile.
 *
 * --------------------------------------------------------------------------
 */
#define CATCH_CONFIG_MAIN


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <VisualPointFeatureVector3DToMatConverter.hpp>
#include <MatToVisualPointFeatureVector3DConverter.hpp>
#include <VisualPointFeatureVector3D.hpp>
#include <Catch/catch.h>
#include <Errors/Assert.hpp>

using namespace Converters;
using namespace VisualPointFeatureVector3DWrapper;

TEST_CASE( "Mat to VisualPointFeatureVector3D", "[MatToVisualPointFeatureVector3D]" )
	{
	MatToVisualPointFeatureVector3DConverter firstConverter;
	VisualPointFeatureVector3DToMatConverter secondConverter;

	cv::Mat inputMatrix(100, 5, CV_32FC1, cv::Scalar(0));
	for(int rowIndex = 0; rowIndex < inputMatrix.rows; rowIndex++)
		{
		for(int columnIndex = 0; columnIndex < inputMatrix.cols; columnIndex++)
			{
			inputMatrix.at<float>(rowIndex, columnIndex) = (float) (rowIndex + columnIndex);
			}
		}

	VisualPointFeatureVector3DSharedConstPtr asnVector = firstConverter.ConvertShared(inputMatrix);
	REQUIRE(GetNumberOfPoints(*asnVector) == inputMatrix.rows);
	int descriptorSize = GetNumberOfDescriptorComponents(*asnVector, 0);
	REQUIRE(descriptorSize == inputMatrix.cols - 3);
	for(int rowIndex = 0; rowIndex < inputMatrix.rows; rowIndex++)
		{
		REQUIRE(GetXCoordinate(*asnVector, rowIndex) == inputMatrix.at<float>(rowIndex, 0) );
		REQUIRE(GetYCoordinate(*asnVector, rowIndex) == inputMatrix.at<float>(rowIndex, 1) );
		REQUIRE(GetZCoordinate(*asnVector, rowIndex) == inputMatrix.at<float>(rowIndex, 2) );
		REQUIRE(descriptorSize == GetNumberOfDescriptorComponents(*asnVector, rowIndex) );
		for(int columnIndex = 3; columnIndex < inputMatrix.cols; columnIndex++)
			{
			REQUIRE(GetDescriptorComponent(*asnVector, rowIndex, columnIndex-3) == inputMatrix.at<float>(rowIndex, columnIndex) );
			}		
		}

	cv::Mat outputMatrix = secondConverter.ConvertShared(asnVector);

	REQUIRE(outputMatrix.rows == inputMatrix.rows);
	REQUIRE(outputMatrix.cols == inputMatrix.cols);
	REQUIRE(outputMatrix.type() == inputMatrix.type());
	for(int rowIndex = 0; rowIndex < inputMatrix.rows; rowIndex++)
		{
		for(int columnIndex = 0; columnIndex < inputMatrix.cols; columnIndex++)
			{
			REQUIRE(outputMatrix.at<float>(rowIndex, columnIndex) == inputMatrix.at<float>(rowIndex, columnIndex));		 		 
			}
		}	

	asnVector.reset();			
	} 


TEST_CASE( "VisualPointFeatureVector3D to Mat", "[VisualPointFeatureVector3DToMat]" )
	{
	MatToVisualPointFeatureVector3DConverter firstConverter;
	VisualPointFeatureVector3DToMatConverter secondConverter;

	cv::Mat inputMatrix(100, 5, CV_32FC1, cv::Scalar(0));
	for(int rowIndex = 0; rowIndex < inputMatrix.rows; rowIndex++)
		{
		for(int columnIndex = 0; columnIndex < inputMatrix.cols; columnIndex++)
			{
			inputMatrix.at<float>(rowIndex, columnIndex) = (float) (rowIndex + columnIndex);
			}
		}

	VisualPointFeatureVector3DSharedConstPtr asnVector = firstConverter.ConvertShared(inputMatrix);
	cv::Mat intermediateMatrix = secondConverter.ConvertShared(asnVector);
	VisualPointFeatureVector3DSharedConstPtr outputVector = firstConverter.ConvertShared(intermediateMatrix);	

	REQUIRE(GetNumberOfPoints(*asnVector) == GetNumberOfPoints(*outputVector));
	REQUIRE(GetNumberOfPoints(*asnVector) > 0);
	int descriptorSize = GetNumberOfDescriptorComponents(*asnVector, 0);
	for(int pointIndex = 0; pointIndex < GetNumberOfPoints(*asnVector); pointIndex++)
		{
		REQUIRE(GetXCoordinate(*asnVector, pointIndex) == GetXCoordinate(*outputVector, pointIndex) );
		REQUIRE(GetYCoordinate(*asnVector, pointIndex) == GetYCoordinate(*outputVector, pointIndex) );
		REQUIRE(GetZCoordinate(*asnVector, pointIndex) == GetZCoordinate(*outputVector, pointIndex) );
		REQUIRE(GetNumberOfDescriptorComponents(*asnVector, pointIndex) == descriptorSize );
		REQUIRE(GetNumberOfDescriptorComponents(*outputVector, pointIndex) == descriptorSize );

		for(int componentIndex = 0; componentIndex < descriptorSize; componentIndex++)
			{
			REQUIRE(GetDescriptorComponent(*asnVector, pointIndex, componentIndex) == GetDescriptorComponent(*outputVector, pointIndex, componentIndex) );
			}
		}

	asnVector.reset();	
	outputVector.reset();			
	}


TEST_CASE( "Attempt conversion of a Bad VisualPointFeatureVector3D", "[BadVisualPointFeatureVector3D]")
	{
	VisualPointFeatureVector3DSharedPtr asnVector = std::make_shared<VisualPointFeatureVector3D>();
	
	AddPoint(*asnVector, 0, 0, 0);
	AddDescriptorComponent(*asnVector, 0, 0);

	AddPoint(*asnVector, 1, 1, 1);
	AddDescriptorComponent(*asnVector, 1, 1);
	AddDescriptorComponent(*asnVector, 1, 2);

	VisualPointFeatureVector3DToMatConverter converter;
	REQUIRE_THROWS_AS( converter.ConvertShared(asnVector), AssertException);	

	asnVector.reset();		
	}

TEST_CASE( "Points with empty descriptors", "[EmptyDescriptors3D]")
	{
	MatToVisualPointFeatureVector3DConverter firstConverter;
	VisualPointFeatureVector3DToMatConverter secondConverter;

	cv::Mat inputMatrix(100, 3, CV_32FC1, cv::Scalar(0));
	for(int rowIndex = 0; rowIndex < inputMatrix.rows; rowIndex++)
		{
		for(int columnIndex = 0; columnIndex < inputMatrix.cols; columnIndex++)
			{
			inputMatrix.at<float>(rowIndex, columnIndex) = (float) (rowIndex + columnIndex);
			}
		}

	VisualPointFeatureVector3DSharedConstPtr asnVector = firstConverter.ConvertShared(inputMatrix);
	cv::Mat outputMatrix = secondConverter.ConvertShared(asnVector);

	REQUIRE(outputMatrix.rows == inputMatrix.rows);
	REQUIRE(outputMatrix.cols == inputMatrix.cols);
	REQUIRE(outputMatrix.type() == inputMatrix.type());
	for(int rowIndex = 0; rowIndex < inputMatrix.rows; rowIndex++)
		{
		for(int columnIndex = 0; columnIndex < inputMatrix.cols; columnIndex++)
			{
			REQUIRE(outputMatrix.at<float>(rowIndex, columnIndex) == inputMatrix.at<float>(rowIndex, columnIndex));		 		 
			}
		}

	asnVector.reset();		
	}

TEST_CASE("Empty Features Vector 3D", "[EmptyFeaturesVector3D]")
	{
	MatToVisualPointFeatureVector3DConverter firstConverter;
	VisualPointFeatureVector3DToMatConverter secondConverter;

	cv::Mat inputMatrix;
	VisualPointFeatureVector3DSharedConstPtr asnVector = firstConverter.ConvertShared(inputMatrix);
	cv::Mat outputMatrix = secondConverter.ConvertShared(asnVector);

	REQUIRE(outputMatrix.cols == 0);
	REQUIRE(outputMatrix.rows == 0);

	asnVector.reset();				
	}