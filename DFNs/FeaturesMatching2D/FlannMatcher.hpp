/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* --------------------------------------------------------------------------
*/

/*!
 * @file FlannMatcher.hpp
 * @date 29/01/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DFNs
 * 
 *  This DFN implements the Flann Matcher.
 *  
 *
 * @{
 */

#ifndef FLANN_MATCHER_HPP
#define FLANN_MATCHER_HPP

/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <FeaturesMatching2D/FeaturesMatching2DInterface.hpp>
#include <CorrespondenceMap2D.hpp>
#include <VisualPointFeatureVector2D.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <yaml-cpp/yaml.h>
#include "opencv2/features2d/features2d.hpp"
#include <Helpers/ParametersListHelper.hpp>


namespace dfn_ci {

/* --------------------------------------------------------------------------
 *
 * Class definition
 *
 * --------------------------------------------------------------------------
 */
    class FlannMatcher : public FeaturesMatching2DInterface
    {
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
        public:
            FlannMatcher();
            ~FlannMatcher();
            void process();
            void configure();

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

		typedef cvflann::flann_centers_init_t CenterInitializationMethod;
		class CenterInitializationMethodHelper : public Helpers::ParameterHelper<CenterInitializationMethod, std::string>
			{
			public:
				CenterInitializationMethodHelper(const std::string& parameterName, CenterInitializationMethod& boundVariable, const CenterInitializationMethod& defaultValue);
			private:
				CenterInitializationMethod Convert(const std::string& value);
			};		

		enum MatcherMethod
			{
			KD_TREE_SEARCH,
			K_MEANS_CLUSTERING,
			AUTOTUNED_SEARCH,
			HIERARCHICAL_CLUSTERING,
			LOCALITY_SENSITIVE_HASHING,	
			COMPOSITE_SEARCH,
			LINEAR_SEARCH
			};
		class MatcherMethodHelper : public Helpers::ParameterHelper<MatcherMethod, std::string>
			{
			public:
				MatcherMethodHelper(const std::string& parameterName, MatcherMethod& boundVariable, const MatcherMethod& defaultValue);
			private:
				MatcherMethod Convert(const std::string& value);
			};

		struct GeneralOptionsSet
			{
			float distanceThreshold;
			int numberOfChecks;
			float epsilon;
			bool sortedSearch;
			MatcherMethod matcherMethod;			
			};
	
		struct KdTreeSearchOptionsSet
			{
			int numberOfTrees;
			};

		struct KMeansClusteringOptionsSet
			{
			int branching;
			int iterations;
			CenterInitializationMethod centersInitialization;
			float convertibleBoundIndex;
			};

		struct AutotunedOptionsSet
			{
			float targetPrecision;
			float buildWeight;
			float memoryWeight;
			float sampleFraction;
			};

		struct HierarchicalClusteringOptionsSet
			{
			int branching;
			CenterInitializationMethod centersInitialization;
			int numberOfTrees;
			int leafSize;
			};

		struct LocalitySensitiveHashingOptionsSet
			{
			int tableNumber;
			int keySize;
			int multiProbeLevel;
			};
		
		struct CompositeSearchOptionsSet
			{
			int branching;
			int iterations;
			CenterInitializationMethod centersInitialization;
			float convertibleBoundIndex;
			int numberOfTrees;
			};

		struct FlannMatcherOptionsSet
			{
			GeneralOptionsSet generalOptionsSet;
			KdTreeSearchOptionsSet kdTreeSearchOptionsSet;
			KMeansClusteringOptionsSet kMeansClusteringOptionsSet;
			AutotunedOptionsSet autotunedOptionsSet;
			HierarchicalClusteringOptionsSet hierarchicalClusteringOptionsSet;
			LocalitySensitiveHashingOptionsSet localitySensitiveHashingOptionsSet;
			CompositeSearchOptionsSet compositeSearchOptionsSet;
			};

		Helpers::ParametersListHelper parametersHelper;
		FlannMatcherOptionsSet parameters;
		static const FlannMatcherOptionsSet DEFAULT_PARAMETERS;

		cv::Ptr<cv::flann::IndexParams> ConvertParameters();
		std::vector< cv::DMatch > ComputeMatches(cv::Mat sourceDescriptorsMatrix, cv::Mat sinkDescriptorsMatrix);
		cv::Mat ConvertToValidType(cv::Mat floatDescriptorsMatrix);
		CorrespondenceMap2DWrapper::CorrespondenceMap2DConstPtr Convert(std::vector< cv::DMatch > matchesVector, cv::Mat sourceFeaturesMatrix, cv::Mat sinkFeaturesMatrix);

		void ValidateParameters();
		void ValidateInputs(cv::Mat sourceFeaturesMatrix, cv::Mat sinkFeaturesMatrix);
    };
}
#endif
/* FlannMatcher.hpp */
/** @} */
