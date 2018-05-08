/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* --------------------------------------------------------------------------
*/

/*!
 * @file ImagePainter.hpp
 * @date 04/05/2018
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup DataGenerators
 * 
 *  This class is used to paint keypoints on an image.
 *  
 *
 * @{
 */

#ifndef IMAGE_PAINTER_HPP
#define IMAGE_PAINTER_HPP

/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include "ImageZooming.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>


namespace DataGenerators {

/* --------------------------------------------------------------------------
 *
 * Class definition
 *
 * --------------------------------------------------------------------------
 */
    class ImagePainter
    {
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
        public:
        	ImagePainter(std::string inputImageFilePath, std::string outputImageFilePath);
        	~ImagePainter();

		void Run();
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
		static const int BASE_WINDOW_WIDTH;
		static const int BASE_WINDOW_HEIGHT;

		ImageZooming* imageZooming;

		cv::Mat originalImage;
		cv::Mat overlayImage;

		std::string inputImageFilePath;
		std::string outputImageFilePath;

		void LoadImage();
		static void MouseCallback(int event, int x, int y, int flags, void* userdata);
		void MouseCallback(int event, int x, int y);
		void DrawImage();
		void ExecuteCommand(char command);
		void SaveImage();
    };

}
#endif
/* ImagePainter.hpp */
/** @} */
