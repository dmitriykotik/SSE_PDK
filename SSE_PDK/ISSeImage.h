#pragma once

enum ESSeImageFile
{
	eSSeImageFileAutoDetect,
	eSSeImageFilePng,
	eSSeImageFileJpeg
};

// All channels are 8-bit unless specified
enum ESSeImagePixelFormat
{
	eSSeImageARGB,
	eSSeImageABGR,
	eSSeImageRGBA,
	eSSeImageBGRA,
	eSSeImageRGB,
	eSSeImageBGR,
	eSSeImageR10G10B10A2
};

struct TSSeRect
{
	int32 left;
	int32 top;
	int32 right;
	int32 bottom;
};

struct TSSePoint
{
	int32 left;
	int32 top;
};

class ISSeImageSurface
{
public:
	virtual void Release() = 0;

	virtual uint32 GetWidth() = 0;
	virtual uint32 GetHeight() = 0;
	virtual int GetPixelSize() = 0;
	virtual void* GetPixelData() = 0;
	virtual ESSeImagePixelFormat GetPixelFormat() = 0;
	virtual bool SetPixelFormat(ESSeImagePixelFormat eNewPixelFormat, bool bConvertToNewPixelFormat) = 0;
};

class ISSeImage
{
public:
	virtual ISSeImageSurface* CreateSurface(int width, int height, ESSeImagePixelFormat ePixFormat = eSSeImageRGBA, const void* pubInitData = 0, uint32 cubInitData = 0) = 0;
	virtual ISSeImageSurface* CreateSurfaceFromFile(const char* pszFile, ESSeImageFile eFileType = eSSeImageFileAutoDetect, ESSeImagePixelFormat ePixFormat = eSSeImageRGBA) = 0;
	virtual ISSeImageSurface* CreateSurfaceFromFileInMemory(const void* pubData, uint32 cubData, ESSeImageFile eFileType = eSSeImageFileAutoDetect, ESSeImagePixelFormat ePixFormat = eSSeImageRGBA) = 0;
	virtual bool SaveSurfaceToFile(ISSeImageSurface* pSurface, const char* pszFile, ESSeImageFile eFileType = eSSeImageFileAutoDetect, int quality = 80) = 0;

	// Convert pixel format to another.
	//  pPixelData is the source data, when pDestPixelData is null, pPixelData will be converted directly.
	//  pDestPixelData is new destination, optional. However, when converting from 3 bytes pixel to 4 bytes pixel you may need to
	//  allocate a bigger surface data and pass it to pDestPixelData.
	//  The srcPitch and destPitch both can be zero, the width * pixel_width will be used as a pitch instead.
	virtual void ConvertFormat(void* pPixelData, void* pDestPixelData, uint32 width, uint32 height, int srcPitch, int destPitch, ESSeImagePixelFormat eFromFormat, ESSeImagePixelFormat eToFormat) = 0;

	// Copy surface from source to destination.
	//  The pSrcRect and pDestPoint can be null to use entire surface.
	virtual bool CopySurface(ISSeImageSurface* pSrcSurface, TSSeRect* pSrcRect, ISSeImageSurface* pDestSurface, TSSePoint* pDestPoint) = 0;
};
