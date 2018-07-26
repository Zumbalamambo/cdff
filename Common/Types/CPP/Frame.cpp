/**
 * @author Alessandro Bianco
 */

/**
 * @addtogroup FrameWrapper
 * @{
 */

#include "Frame.hpp"

namespace FrameWrapper
{

using namespace BaseTypesWrapper;

	void CopyImageData(Frame& destination, const Frame& source)
	{
		// copy buffers
		ASSERT_ON_TEST(source.image.nCount < MAX_DATA_BYTE_SIZE, "Image data exceeds limits");
		std::memcpy(&destination.image.arr, &source.image.arr, source.image.nCount);
		destination.image.nCount = source.image.nCount;
	}


void Copy(const Frame& source, Frame& destination)
{
	SetFrameTime(destination, GetFrameTime(source));
	SetReceivedTime(destination, GetReceivedTime(source));
	SetDataDepth(destination, GetDataDepth(source));
	SetPixelSize(destination, GetPixelSize(source));
	SetRowSize(destination, GetRowSize(source));
	SetFrameMode(destination, GetFrameMode(source));
	SetFrameStatus(destination, GetFrameStatus(source));
	SetFrameSize(destination, GetFrameSize(source));
	ClearAttributes(destination);
	for (unsigned attributeIndex = 0; attributeIndex < GetNumberOfAttributes(source); attributeIndex++)
	{
		AddAttribute(destination, GetAttribute(source, attributeIndex));
	}
	ClearData(destination);
	CopyImageData(destination,source);
}

FramePtr NewFrame()
{
	FramePtr frame = new Frame();
	Initialize(*frame);
	return frame;
}

FrameSharedPtr NewSharedFrame()
{
	FrameSharedPtr sharedFrame = std::make_shared<Frame>();
	Initialize(*sharedFrame);
	return sharedFrame;
}

FrameConstPtr Clone(const Frame& source)
{
	FramePtr frame = new Frame();
	Copy(source, *frame);
	return frame;
}

FrameSharedPtr SharedClone(const Frame& source)
{
	FrameSharedPtr sharedFrame = std::make_shared<Frame>();
	Copy(source, *sharedFrame);
	return sharedFrame;
}

void Initialize(Frame& frame)
{
	SetFrameTime(frame, 0);
	SetReceivedTime(frame, 0);
	SetDataDepth(frame, 0);
	SetPixelSize(frame, 0);
	SetRowSize(frame, 0);
	SetFrameSize(frame, 0, 0);
	SetFrameMode(frame, MODE_UNDEFINED);
	SetFrameStatus(frame, STATUS_EMPTY);
	ClearAttributes(frame);
	ClearData(frame);
}

void SetFrameTime(Frame& frame, T_Int64 time)
{
	frame.frame_time.microseconds = time;
	frame.frame_time.usecPerSec = 1;
}

T_Int64 GetFrameTime(const Frame& frame)
{
	return frame.frame_time.microseconds;
}

void SetReceivedTime(Frame& frame, T_Int64 time)
{
	frame.received_time.microseconds = time;
	frame.received_time.usecPerSec = 1;
}

T_Int64 GetReceivedTime(const Frame& frame)
{
	return frame.received_time.microseconds;
}

void SetDataDepth(Frame& frame, T_UInt32 dataDepth)
{
	frame.data_depth = dataDepth;
}

T_UInt32 GetDataDepth(const Frame& frame)
{
	return frame.data_depth;
}

void SetPixelSize(Frame& frame, T_UInt32 pixelSize)
{
	frame.pixel_size = pixelSize;
}

T_UInt32 GetPixelSize(const Frame& frame)
{
	return frame.pixel_size;
}

void SetRowSize(Frame& frame, T_UInt32 rowSize)
{
	frame.row_size = rowSize;
}

T_UInt32 GetRowSize(const Frame& frame)
{
	return frame.row_size;
}

void SetFrameMode(Frame& frame, FrameMode frameMode)
{
	frame.frame_mode = frameMode;
}

FrameMode GetFrameMode(const Frame& frame)
{
	return frame.frame_mode;
}

void SetFrameStatus(Frame& frame, FrameStatus frameStatus)
{
	frame.frame_status = frameStatus;
}

FrameStatus GetFrameStatus(const Frame& frame)
{
	return frame.frame_status;
}

void SetFrameSize(Frame& frame, T_UInt16 width, T_UInt16 height)
{
	frame.datasize.width = width;
	frame.datasize.height = height;
}

void SetFrameSize(Frame& frame, FrameSize frameSize)
{
	frame.datasize.width = frameSize.width;
	frame.datasize.height = frameSize.height;
}

T_UInt16 GetFrameWidth(const Frame& frame)
{
	return frame.datasize.width;
}

T_UInt16 GetFrameHeight(const Frame& frame)
{
	return frame.datasize.height;
}

FrameSize GetFrameSize(const Frame& frame)
{
	return frame.datasize;
}

void AddAttribute(Frame& frame, T_String data, T_String name)
{
	ASSERT_ON_TEST(frame.attributes.nCount < MAX_FRAME_ATTRIBUTES, "Adding more Frame attributes than allowed");
	int currentIndex = frame.attributes.nCount;
	frame.attributes.arr[currentIndex].data = data;
	frame.attributes.arr[currentIndex].att_name = name;
	frame.attributes.nCount++;
}

void AddAttribute(Frame& frame, FrameAttribute attribute)
{
	ASSERT_ON_TEST(frame.attributes.nCount < MAX_FRAME_ATTRIBUTES, "Adding more Frame attributes than allowed");
	int currentIndex = frame.attributes.nCount;
	frame.attributes.arr[currentIndex].data = attribute.data;
	frame.attributes.arr[currentIndex].att_name = attribute.att_name;
	frame.attributes.nCount++;
}

void ClearAttributes(Frame& frame)
{
	frame.attributes.nCount = 0;
}

void RemoveAttribute(Frame& frame, int index)
{
	ASSERT_ON_TEST(index < frame.attributes.nCount, "Requesting a missing attribute from a Frame");
	for (int iteratorIndex = index; iteratorIndex <  frame.attributes.nCount - 1; iteratorIndex++)
	{
		frame.attributes.arr[iteratorIndex] = frame.attributes.arr[iteratorIndex+1];
	}
	frame.attributes.nCount--;
}

FrameAttribute GetAttribute(const Frame& frame, int index)
{
	ASSERT_ON_TEST(index < frame.attributes.nCount, "Requesting a missing attribute from a Frame");
	return frame.attributes.arr[index];
}

unsigned GetNumberOfAttributes(const Frame& frame)
{
	return frame.attributes.nCount;
}

void ClearData(Frame& frame)
{
	frame.image.nCount = 0;
}

byte GetDataByte(const Frame& frame, int index)
{
	ASSERT_ON_TEST(index < frame.image.nCount, "Requesting missing image data");
	return frame.image.arr[index];
}

int GetNumberOfDataBytes(const Frame& frame)
{
	return frame.image.nCount;
}

BitStream ConvertToBitStream(const Frame& frame)
	CONVERT_TO_BIT_STREAM(frame, asn1SccFrame_REQUIRED_BYTES_FOR_ENCODING, asn1SccFrame_Encode)

void ConvertFromBitStream(BitStream bitStream, Frame& frame)
	CONVERT_FROM_BIT_STREAM(bitStream, asn1SccFrame_REQUIRED_BYTES_FOR_ENCODING, frame, asn1SccFrame_Decode)

}

/** @} */
