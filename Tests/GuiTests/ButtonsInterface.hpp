/* --------------------------------------------------------------------------
*
* (C) Copyright …
*
* ---------------------------------------------------------------------------
*/

/*!
 * @file ButtonsInterface.hpp
 * @date 24/11/2017
 * @author Alessandro Bianco
 */

/*!
 * @addtogroup GuiTests
 * 
 * This class creates a buttons interface by using basic GUI functions from OpenCV library. 
 * The class constructor needs a windowName, and the dimensions of the buttons.
 * A button is defined by a text and a callback function of the type "void function()".
 *
 * @{
 */

#ifndef BUTTONS_INTERFACE_HPP
#define BUTTONS_INTERFACE_HPP


/* --------------------------------------------------------------------------
 *
 * Includes
 *
 * --------------------------------------------------------------------------
 */
#include <stdlib.h>
#include <string>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


/* --------------------------------------------------------------------------
 *
 * Class definition
 *
 * --------------------------------------------------------------------------
 */
class ButtonsInterface
	{
	/* --------------------------------------------------------------------
	 * Public
	 * --------------------------------------------------------------------
	 */
	public:

		using on_button_clicked_cb_t = std::function<void()>;

		ButtonsInterface(std::string windowName, int buttonWidth, int buttonHeight);
		~ButtonsInterface() = default;

		void Display();

		/**
		* Add a button to this `ButtonsInterface` with default styling
		*
		* @param label The text of the button
		* @param callback The callback function to call when the button is clicked
		*/
		void AddButton(
			std::string const &label,
			on_button_clicked_cb_t callback
		);

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
		typedef void (*Callback)(void*);

		struct Button
			{
			cv::Point topLeftCorner;
			cv::Point bottomRightCorner;
			std::string label;
			on_button_clicked_cb_t callback;
			};

		const std::string windowName;
		std::vector<Button> buttonList;
		cv::Mat buttonsImage;
		int buttonWidth;
		int buttonHeight;
		static const int buttonPadding;
		static const int xTextPadding;
		static const int yTextPadding;
		static const cv::Scalar backgroundColor;
		static const cv::Scalar buttonColor;
		static const cv::Scalar textColor;

		static void OnMouseCallback(int event, int x, int y, int z, void* data);
		void OnMouseCallback(int event, int x, int y, int z);


	};

#endif

/* ButtonsInterface.hpp */
/** @} */
