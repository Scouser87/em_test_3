#ifndef __IMAGELIB_H__
#define __IMAGELIB_H__

#include <string>
#include <stdint.h>
#ifndef BYTE
    #if (__QT__ && __WIN32__)
        typedef unsigned char BYTE;
        #include <stdint.h>
	#elif DEF_WP8 == 0
        #define BYTE   unsigned char
    #endif
#endif /* __QT__ && __WIN32__ */
namespace ImageLib
{
struct sSize
{
	int width, height;
    bool mIsAlpha;
};
	
class Image
{
public:
	int						mWidth;
	int						mHeight;
	uint32_t*               mBits;
    unsigned char*			mBufferChar;
    bool                    mIsAlpha;
public:
	Image();
	virtual ~Image();

	int						GetWidth() const;
	int						GetHeight() const;
	uint32_t*               GetBits();
};

bool WriteJPEGImage(const std::string& theFileName, Image* theImage);
bool WritePNGImage(const std::string& theFileName, Image* theImage);
bool WriteTGAImage(const std::string& theFileName, Image* theImage);
bool WriteBMPImage(const std::string& theFileName, Image* theImage);
extern int gAlphaComposeColor;
extern bool gAutoLoadAlpha;
//extern bool gIgnoreJPEG2000Alpha;  // I've noticed alpha in jpeg2000's that shouldn't have alpha so this defaults to true


//Image* GetImage(const std::string& theFileName, bool lookForAlphaImage = true);
Image* GetImage(const std::string& theFileName, bool lookForAlphaImage = false,const std::string& theAlphaFilename = "");
sSize GetImageSize(const std::string& theFileName);

//void InitJPEG2000();
//void CloseJPEG2000();
//void SetJ2KCodecKey(const std::string& theKey);

}

#endif //__IMAGELIB_H__
