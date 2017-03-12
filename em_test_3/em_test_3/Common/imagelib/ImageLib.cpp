#define XMD_H

//#include <windows.h>
#include "ImageLib.h"
#include "png/png.h"
#include <math.h>
//#include <tchar.h>
//#define JAS_WIN_MSVC_BUILD
//#include "jpeg2000/jasper.h"
int ImageLib::gAlphaComposeColor = 0xFFFFFF;
bool ImageLib::gAutoLoadAlpha = true;
#include <iostream>

#include "../../Macros.h"
#include "Trace.h"
#include "File/CFile.h"
#include "CDirector.h"

#if OS == OS_ANDROID
#include "libwebp/prebuilt_android/include/decode.h"
#else
#include "libwebp/webp/decode.h"
#endif
extern "C"
{
#include "jpeg/jpeglib.h"
#include "jpeg/jerror.h"
}

//#include "jpeg2000/jasper.h"
typedef enum {
	kTexture2DPixelFormat_Automatic = 0,
	kTexture2DPixelFormat_RGBA8888,
	kTexture2DPixelFormat_RGB565,
	kTexture2DPixelFormat_A8,
} Texture2DPixelFormat;

using namespace ImageLib;

Image::Image():
mWidth(0),
mHeight(0),
mBits(NULL),
mBufferChar(NULL),
mIsAlpha(true)
{
	
}

Image::~Image()
{
	if(mBits != NULL)
	{
		delete[] mBits;
		mBits = NULL;
	}
    if(mBufferChar != NULL)
	{
		delete[] mBufferChar;
		mBufferChar = NULL;
	}
}

int	Image::GetWidth() const
{
	return mWidth;
}

int	Image::GetHeight() const
{
	return mHeight;
}

uint32_t* Image::GetBits()
{
	return mBits;
}

#if 1
//////////////////////////////////////////////////////////////////////////
// PNG Pak Support

#include "pngstruct.h"

static void png_pak_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
	png_size_t check;

	/* fread() returns 0 on error, so it is OK to store this in a png_size_t
	* instead of an int, which is what fread() actually returns.
	*/
    check = (png_size_t)((CFile*)png_ptr->io_ptr)->Read(data, (png_size_t)1 * length);

    if (check != length)
	{
		png_error(png_ptr, "Read Error");
    }
}

static void png_error_fn(png_structp png_ptr, png_const_charp error_message)
{
    longjmp(png_ptr->jmp_buf_local, 1);
}

static void png_waring_fn(png_structp png_ptr, png_const_charp error_message)
{
    std::string message = "Warning during loading PNG file|";
    message += *(std::string*)png_ptr->user_data_p + "|";
    message += error_message;
    CDirector::GetDirector().SetEngineError(1400, message, true);
}

//------------------------
//wepb support
Image* GetWebpImageSize(const std::string& path)
{
    CFile imageFile(path);
    imageFile.OpenRead();
    
    if (!imageFile.IsOpen())
    {
        ALOG("ERROR: image not loaded: %s",  path.data());
        return NULL;
    }
    
    // get file size:
    size_t lSize = imageFile.GetFileSize();
    
    //create buffer and load image file here
    uint8_t *buffer = (uint8_t*) malloc (sizeof(uint8_t)*lSize);
    if (buffer == NULL)
    {
        return NULL;
    }
    
    // copy the file into the buffer
    imageFile.Read(buffer, lSize);
    
    int width, height;
    
    if ( WebPGetInfo(buffer, lSize, &width, &height) != 0 )
    {
        free(buffer);
        Image* anImage = new Image();
        anImage->mWidth = width;
        anImage->mHeight = height;
        anImage->mIsAlpha = true;
        
        return anImage;
    }
    
    free(buffer);
    
    return NULL;
}

Image* DecodeWebpData(void* data, size_t dataLenght)
{
    bool bRet = false;
    do
    {
        WebPDecoderConfig config;
        if (WebPInitDecoderConfig(&config) == 0) break;
        if (WebPGetFeatures((uint8_t*)data, dataLenght, &config.input) != VP8_STATUS_OK) break;
        if (config.input.width == 0 || config.input.height == 0) break;
        
        config.output.colorspace = MODE_RGBA;
        
        int bufferSize = config.input.width * config.input.height * 4;
        unsigned char* buffer = new unsigned char[bufferSize];
        
        config.output.u.RGBA.rgba = (uint8_t*)buffer;
        config.output.u.RGBA.stride = config.input.width * 4;
        config.output.u.RGBA.size = bufferSize;
        config.output.is_external_memory = 1;
        
        if (WebPDecode((uint8_t*)data, dataLenght, &config) == VP8_STATUS_OK)
        {
            Image* anImage = new Image();
            anImage->mWidth = config.input.width;
            anImage->mHeight = config.input.height;
            anImage->mIsAlpha = true;
            anImage->mBits = (uint32_t*)buffer;
            
            return anImage;
        }
        
        bRet = true;
    } while (0);
    
    return NULL;
}

Image* GetWebpImage(const std::string& path)
{
    CFile imageFile(path);
    imageFile.OpenRead();
    
    if (!imageFile.IsOpen())
    {
        ALOG("ERROR: image not loaded: %s",  path.data());
        return NULL;
    }

    size_t lSize = imageFile.GetFileSize();
    
    uint8_t *buffer = (uint8_t*) malloc (sizeof(uint8_t)*lSize);
    if (buffer == NULL)
    {
        return NULL;
    }
    
    imageFile.Read(buffer, lSize);
    Image* result = DecodeWebpData(buffer, lSize);
    free(buffer);
    return result;
}
//------------------------

Image* GetPNGImageSize(const std::string& theFileName)
{
	png_structp png_ptr;
	png_infop info_ptr;
	//unsigned int sig_read = 0;
	png_uint_32 width, height;
	CFile fp(theFileName);
    fp.OpenRead();
	
	if (!fp.IsOpen())
	{
		printf("Image LIb p_fopen return NULL \n");
		return NULL;
	}
    
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
									 NULL, png_error_fn, png_waring_fn);
    png_ptr->user_data_p = (void*)&theFileName;
	png_set_read_fn(png_ptr, (png_voidp)&fp, png_pak_read_data);
	
	if (png_ptr == NULL)
	{
		printf("Image LIb png_ptr == NULL \n");
		//p_fclose(fp);
		return NULL;
	}
	
	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		//p_fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		printf("Image LIb Error Allocate memmory FILE\n");
		return NULL;
	}
	
	/* Set error handling if you are using the setjmp/longjmp method (this is
	 * the normal method of doing things with libpng).  REQUIRED unless you
	 * set up your own error handlers in the png_create_read_struct() earlier.
	 */
	if (setjmp(png_ptr->jmp_buf_local))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		//p_fclose(fp);
		printf("Image LIb Error reading FILE\n");
		/* If we get here, we had a problem reading the file */
		return NULL;
	}
	
	//png_init_io(png_ptr, fp);
	
	//png_ptr->io_ptr = (png_voidp)fp;
	
	png_read_info(png_ptr, info_ptr);
	width = png_ptr->width;
    height = png_ptr->height;
    
    png_destroy_info_struct(png_ptr, &info_ptr);
    
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
	
	/* close the file */
	//p_fclose(fp);
	
	Image* anImage = new Image();
	anImage->mWidth = width;
	anImage->mHeight = height;
	//anImage->mBits = aBits;
	
	return anImage;
}

Image* GetPNGImage(const std::string& theFileName)
{
	png_structp png_ptr;
	png_infop info_ptr;
//	unsigned int sig_read = 0;
	png_uint_32 width, height;
	int bit_depth, color_type, interlace_type;
	CFile fp(theFileName);
    fp.OpenRead();

	if (!fp.IsOpen())
		return NULL;
    
	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
	  NULL, png_error_fn, png_waring_fn);
    png_ptr->user_data_p = (void*)&theFileName;
	png_set_read_fn(png_ptr, (png_voidp)&fp, png_pak_read_data);

	if (png_ptr == NULL)
	{
		//p_fclose(fp);
		return NULL;
	}

	/* Allocate/initialize the memory for image information.  REQUIRED. */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		//p_fclose(fp);
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		return NULL;
	}

   /* Set error handling if you are using the setjmp/longjmp method (this is
    * the normal method of doing things with libpng).  REQUIRED unless you
    * set up your own error handlers in the png_create_read_struct() earlier.
    */
	if (setjmp(png_ptr->jmp_buf_local))
	{
		/* Free all of the memory associated with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		//p_fclose(fp);
		/* If we get here, we had a problem reading the file */
		return NULL;
	}

	//png_init_io(png_ptr, fp);

	//png_ptr->io_ptr = (png_voidp)fp;

	png_read_info(png_ptr, info_ptr);
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, NULL, NULL);
    
    png_int_16 number_passes = 1;
    if (interlace_type == 1)
    {
        number_passes = png_set_interlace_handling(png_ptr);
        ALOG("ImageLib GetPNGImage WARNING: Loaded interlaced image : %s !!!", theFileName.c_str());
    }

	/* Add filler (or alpha) byte (before/after each RGB triplet) */
	png_set_expand(png_ptr);
	png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	//png_set_gray_1_2_4_to_8(png_ptr);
	png_set_palette_to_rgb(png_ptr);
	png_set_gray_to_rgb(png_ptr);
	//png_set_bgr(png_ptr);

//	int aNumBytes = png_get_rowbytes(png_ptr, info_ptr) * height / 4;
	uint32_t* aBits = NULL;
	try
	{
		aBits = new uint32_t[width*height];
	}
	catch (std::bad_alloc& ba)
	{
		ALOG("GetPNGImage: failed get %d bytes for image %s: bad_alloc", width*height, theFileName.c_str());
		aBits = NULL;
	}
	if (aBits == NULL)
		return NULL;
	uint32_t* anAddr = aBits;
    if (number_passes > 1)
    {
        for (png_int_16 pass = 0; pass < number_passes; ++pass)
        {
            anAddr = aBits;
            for (int i = 0; i < height; i++)
            {
                png_read_rows(png_ptr, (png_bytepp) &anAddr, NULL, 1);
                anAddr += width;
            }
        }
    }
    else
    {
        for (int i = 0; i < height; i++)
        {
            png_read_rows(png_ptr, (png_bytepp) &anAddr, NULL, 1);
            anAddr += width;
        }
    }
	

	/* read rest of file, and get additional chunks in info_ptr - REQUIRED */
	png_read_end(png_ptr, info_ptr);

    png_destroy_info_struct(png_ptr, &info_ptr);
    
	/* clean up after the read, and free any memory allocated - REQUIRED */
	png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);

	/* close the file */
	//p_fclose(fp);

	Image* anImage = new Image();
	anImage->mWidth = width;
	anImage->mHeight = height;
	anImage->mBits = aBits;

	return anImage;
}

Image* GetTGAImage(const std::string& theFileName)
{
    
	CFile aTGAFile(theFileName);
    aTGAFile.OpenRead();
	if (!aTGAFile.IsOpen())
		return NULL;

	BYTE aHeaderIDLen;
    aTGAFile.Read(&aHeaderIDLen, sizeof(BYTE));

	BYTE aColorMapType;
    aTGAFile.Read(&aColorMapType, sizeof(BYTE));
	
	BYTE anImageType;
    aTGAFile.Read(&anImageType, sizeof(BYTE));

	unsigned short aFirstEntryIdx;
    aTGAFile.Read(&aFirstEntryIdx, sizeof(short));

	unsigned short aColorMapLen;
    aTGAFile.Read(&aColorMapLen, sizeof(short));

	BYTE aColorMapEntrySize;
    aTGAFile.Read(&aColorMapEntrySize, sizeof(BYTE));

	unsigned short anXOrigin;
    aTGAFile.Read(&anXOrigin, sizeof(short));

	unsigned short aYOrigin;
    aTGAFile.Read(&aYOrigin, sizeof(short));

	unsigned short anImageWidth;
    aTGAFile.Read(&anImageWidth, sizeof(short));

	unsigned short anImageHeight;
    aTGAFile.Read(&anImageHeight, sizeof(short));

	BYTE aBitCount = 32;
    aTGAFile.Read(&aBitCount, sizeof(BYTE));

	BYTE anImageDescriptor = 8 | (1<<5);
    aTGAFile.Read(&anImageDescriptor, sizeof(BYTE));

	if ((aBitCount != 32) ||
		(anImageDescriptor != (8 | (1<<5))))
	{
		return NULL;
	}

	Image* anImage = new Image();

	anImage->mWidth = anImageWidth;
	anImage->mHeight = anImageHeight;
	anImage->mBits = new uint32_t[anImageWidth*anImageHeight];

    aTGAFile.Read(anImage->mBits, 4 * anImage->mWidth*anImage->mHeight);

	return anImage;
}

int ReadBlobBlock(CFile* fp, char* data)
{
	unsigned char aCount = 0;
	fp->Read(&aCount, sizeof(char));
	fp->Read(data, sizeof(char) * aCount);
	return aCount;
}

Image* GetGIFImage(const std::string& theFileName)
{
	#define BitSet(byte,bit)  (((byte) & (bit)) == (bit))
	#define LSBFirstOrder(x,y)  (((y) << 8) | (x))

	int
		opacity,
		status;

	register int i;

	register unsigned char *p;

	unsigned char
		background,
		c,
		flag,
		*global_colormap,
		header[1664],
		magick[12];

	unsigned int
		delay,
		dispose,
		global_colors,
		image_count,
		iterations;

	/*
	Open image file.
	*/

	CFile fp(theFileName);
    fp.OpenRead();

	if (!fp.IsOpen())
		return NULL;
	/*
	Determine if this is a GIF file.
	*/
	status=fp.Read(magick, sizeof(char)*6);

	if (((strncmp((char *) magick,"GIF87",5) != 0) &&
		(strncmp((char *) magick,"GIF89",5) != 0)))
		return NULL;

	global_colors=0;
	global_colormap=(unsigned char *) NULL;

	short pw;
	short ph;

	fp.Read(&pw, sizeof(short));
	fp.Read(&ph, sizeof(short));
	fp.Read(&flag, sizeof(char));
	fp.Read(&background, sizeof(char));
	fp.Read(&c, sizeof(char));

	if (BitSet(flag,0x80))
	{
		/*
		opacity global colormap.
		*/
		global_colors=1 << ((flag & 0x07)+1);
		global_colormap=new unsigned char[3*global_colors];
		if (global_colormap == (unsigned char *) NULL)
		return NULL;

		fp.Read(global_colormap, 3*global_colors);
	}

	delay=0;
	dispose=0;
	iterations=1;
	opacity=(-1);
	image_count=0;

	for ( ; ; )
	{
		if (fp.Read(&c, sizeof(char)) == 0)
		break;

		if (c == ';')
		break;  /* terminator */
		if (c == '!')
		{
			/*
			GIF Extension block.
			*/
			fp.Read(&c, sizeof(char));

			switch (c)
			{
			case 0xf9:
				{
					/*
					Read Graphics Control extension.
					*/
					while (ReadBlobBlock(&fp,(char *) header) > 0);

					dispose=header[0] >> 2;
					delay=(header[2] << 8) | header[1];
					if ((header[0] & 0x01) == 1)
					opacity=header[3];
					break;
				}
			case 0xfe:
				{
					char *comments;
					int length;

					/*
					Read Comment extension.
					*/
					comments=(char *) NULL;
					for ( ; ; )
					{
						length=ReadBlobBlock(&fp,(char *) header);
						if (length <= 0)
						break;
						if (comments == NULL)
						{
							comments= new char[length+1];
							if (comments != (char *) NULL)
							*comments='\0';
						}

						header[length]='\0';
						strcat(comments,(char *) header);
					}
					if (comments == (char *) NULL)
						break;

					delete [] comments;
					break;
				}
			case 0xff:
				{
					int
					loop;

					/*
					Read Netscape Loop extension.
					*/
					loop=false;
					if (ReadBlobBlock(&fp,(char *) header) > 0)
					loop=!strncmp((char *) header,"NETSCAPE2.0",11);
					while (ReadBlobBlock(&fp,(char *) header) > 0)
					if (loop)
					iterations=(header[2] << 8) | header[1];
					break;
				}
			default:
				{
					while (ReadBlobBlock(&fp,(char *) header) > 0);
					break;
				}
			}
		}

		if (c != ',')
			continue;

		if (image_count != 0)
		{
			/*
			Allocate next image structure.
			*/

			/*AllocateNextImage(image_info,image);
			if (image->next == (Image *) NULL)
			{
			DestroyImages(image);
			return((Image *) NULL);
			}
			image=image->next;
			MagickMonitor(LoadImagesText,TellBlob(image),image->filesize);*/
		}
		image_count++;

		short pagex;
		short pagey;
		short width;
		short height;
		int colors;
		bool interlaced;

		fp.Read(&pagex, sizeof(short));
		fp.Read(&pagey, sizeof(short));
		fp.Read(&width, sizeof(short));
		fp.Read(&height, sizeof(short));
		fp.Read(&flag, sizeof(char));

		colors=!BitSet(flag,0x80) ? global_colors : 1 << ((flag & 0x07)+1);
		uint32_t* colortable = new uint32_t[colors];

		interlaced = BitSet(flag,0x40);

		delay=0;
		dispose=0;
		iterations=1;
		/*if (image_info->ping)
		{
		f (opacity >= 0)
		/image->matte=true;

		CloseBlob(image);
		return(image);
		}*/
		if ((width == 0) || (height == 0))
			return NULL;
		/*
		Inititialize colormap.
		*/
		/*if (!AllocateImageColormap(image,image->colors))
		ThrowReaderException(ResourceLimitWarning,"Memory allocation failed",
		image);*/
		if (!BitSet(flag,0x80))
		{
			/*
			Use global colormap.
			*/
			p=global_colormap;
			for (i=0; i < (int) colors; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				colortable[i] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}

			//image->background_color=
			//image->colormap[Min(background,image->colors-1)];
		}
		else
		{
			unsigned char
			*colormap;

			/*
			Read local colormap.
			*/
			colormap=new unsigned char[3*colors];

			//int pos = p_ftell(fp);
			fp.Read(colormap, sizeof(char)*3*colors);

			p=colormap;
			for (i=0; i < (int) colors; i++)
			{
				int r = *p++;
				int g = *p++;
				int b = *p++;

				colortable[i] = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
			delete [] colormap;
		}

		/*if (opacity >= (int) colors)
		{
		for (i=colors; i < (opacity+1); i++)
		{
		image->colormap[i].red=0;
		image->colormap[i].green=0;
		image->colormap[i].blue=0;
		}
		image->colors=opacity+1;
		}*/
		/*
		Decode image.
		*/
		//status=DecodeImage(image,opacity,exception);

		//if (global_colormap != (unsigned char *) NULL)
		// LiberateMemory((void **) &global_colormap);
		if (global_colormap != NULL)
		{
			delete [] global_colormap;
			global_colormap = NULL;
		}

		//while (image->previous != (Image *) NULL)
		//    image=image->previous;

		#define MaxStackSize  4096
		#define NullCode  (-1)

		int
			available,
			bits,
			code,
			clear,
			code_mask,
			code_size,
			count,
			end_of_information,
			in_code,
			offset,
			old_code,
			pass,
			y;

		register int
			x;

		register unsigned int
			datum;

		short
			*prefix;

		unsigned char
			data_size,
			first,
			*packet,
			*pixel_stack,
			*suffix,
			*top_stack;

		/*
		Allocate decoder tables.
		*/

		packet=new unsigned char[256];
		prefix=new short[MaxStackSize];
		suffix=new unsigned char [MaxStackSize];
		pixel_stack= new unsigned char[MaxStackSize+1];

		/*
		Initialize GIF data stream decoder.
		*/
		fp.Read(&data_size, sizeof(char));
		clear=1 << data_size;
		end_of_information=clear+1;
		available=clear+2;
		old_code=NullCode;
		code_size=data_size+1;
		code_mask=(1 << code_size)-1;
		for (code=0; code < clear; code++)
		{
			prefix[code]=0;
			suffix[code]=(unsigned char) code;
		}
		/*
		Decode GIF pixel stream.
		*/
		datum=0;
		bits=0;
		c=0;
		count=0;
		first=0;
		offset=0;
		pass=0;
		top_stack=pixel_stack;

		uint32_t* aBits = new uint32_t[width*height];

		register unsigned char *c = NULL;

		for (y=0; y < (int) height; y++)
		{
			//q=SetImagePixels(image,0,offset,width,1);
			//if (q == (PixelPacket *) NULL)
			//break;
			//indexes=GetIndexes(image);

			uint32_t* q = aBits + offset*width;



			for (x=0; x < (int) width; )
			{
				if (top_stack == pixel_stack)
				{
					if (bits < code_size)
					{
						/*
						Load bytes until there is enough bits for a code.
						*/
						if (count == 0)
						{
							/*
							Read a new data block.
							*/
							//int pos = p_ftell(fp);

							count=ReadBlobBlock(&fp,(char *) packet);
							if (count <= 0)
							break;
							c=packet;
						}
						datum+=(*c) << bits;
						bits+=8;
						c++;
						count--;
						continue;
					}
					/*
					Get the next code.
					*/
					code=datum & code_mask;
					datum>>=code_size;
					bits-=code_size;
					/*
					Interpret the code
					*/
					if ((code > available) || (code == end_of_information))
					break;
					if (code == clear)
					{
						/*
						Reset decoder.
						*/
						code_size=data_size+1;
						code_mask=(1 << code_size)-1;
						available=clear+2;
						old_code=NullCode;
						continue;
					}
					if (old_code == NullCode)
					{
						*top_stack++=suffix[code];
						old_code=code;
						first=(unsigned char) code;
						continue;
					}
					in_code=code;
					if (code >= available)
					{
						*top_stack++=first;
						code=old_code;
					}
					while (code >= clear)
					{
						*top_stack++=suffix[code];
						code=prefix[code];
					}
					first=suffix[code];
					/*
					Add a new string to the string table,
					*/
					if (available >= MaxStackSize)
						break;
					*top_stack++=first;
					prefix[available]=old_code;
					suffix[available]=first;
					available++;
					if (((available & code_mask) == 0) && (available < MaxStackSize))
					{
						code_size++;
						code_mask+=available;
					}
					old_code=in_code;
				}
				/*
				Pop a pixel off the pixel stack.
				*/
				top_stack--;

				int index=(*top_stack);

				*q=colortable[index];

				if (index == opacity)
					*q = 0;

				x++;
				q++;
			}

			if (!interlaced)
				offset++;
			else
			{
				switch (pass)
				{
				case 0:
				default:
					{
						offset+=8;
						if (offset >= height)
						{
							pass++;
							offset=4;
						}
						break;
					}
				case 1:
					{
						offset+=8;
						if (offset >= height)
						{
							pass++;
							offset=2;
						}
						break;
					}
				case 2:
					{
						offset+=4;
						if (offset >= height)
						{
							pass++;
							offset=1;
						}
						break;
					}
				case 3:
					{
						offset+=2;
						break;
					}
				}
			}

			if (x < width)
				break;

			/*if (image->previous == (Image *) NULL)
			if (QuantumTick(y,image->rows))
			MagickMonitor(LoadImageText,y,image->rows);*/
		}
		delete [] pixel_stack;
		delete[] suffix;
		delete[] prefix;
		delete[] packet;

		delete[] colortable;

		//if (y < image->rows)
		//failed = true;

		Image* anImage = new Image();

		anImage->mWidth = width;
		anImage->mHeight = height;
		anImage->mBits = aBits;

		//TODO: Change for animation crap
		return anImage;
	}

	return NULL;
}

typedef struct my_error_mgr * my_error_ptr;

struct my_error_mgr
{
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);

}

bool ImageLib::WriteJPEGImage(const std::string& theFileName, Image* theImage)
{
	FILE *fp;
#if(!DEF_QT)

	if ((fp = fopen(theFileName.c_str(), "wb")) == NULL)
		return false;

	struct jpeg_compress_struct cinfo;
	struct my_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_compress(&cinfo);
		fclose(fp);
		return false;
	}

	jpeg_create_compress(&cinfo);

	cinfo.image_width = theImage->mWidth;
	cinfo.image_height = theImage->mHeight;
	cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    cinfo.optimize_coding = 1;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 80, TRUE);

	jpeg_stdio_dest(&cinfo, fp);

	jpeg_start_compress(&cinfo, true);

	int row_stride = theImage->GetWidth() * 3;

	unsigned char* aTempBuffer = new unsigned char[row_stride];

	uint32_t* aSrcPtr = theImage->mBits;

	for (int aRow = 0; aRow < theImage->mHeight; aRow++)
	{
		unsigned char* aDest = aTempBuffer;

		for (int aCol = 0; aCol < theImage->mWidth; aCol++)
		{
			unsigned long src = *(aSrcPtr++);

			*aDest++ = (src >> 16) & 0xFF;
			*aDest++ = (src >>  8) & 0xFF;
			*aDest++ = (src      ) & 0xFF;
		}

		jpeg_write_scanlines(&cinfo, &aTempBuffer, 1);
	}

	delete [] aTempBuffer;

	jpeg_finish_compress(&cinfo);
	jpeg_destroy_compress(&cinfo);

	fclose(fp);
#endif
	return true;
}

bool ImageLib::WritePNGImage(const std::string& theFileName, Image* theImage)
{
	png_structp png_ptr;
	png_infop info_ptr;

	FILE *fp;

	if ((fp = fopen(theFileName.c_str(), "wb")) == NULL)
		return false;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
	  NULL, NULL, NULL);

	if (png_ptr == NULL)
	{
		fclose(fp);
		return false;
	}

	// Allocate/initialize the memory for image information.  REQUIRED.
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL)
	{
		fclose(fp);
		png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
		return false;
	}

   // Set error handling if you are using the setjmp/longjmp method (this is
   // the normal method of doing things with libpng).  REQUIRED unless you
   // set up your own error handlers in the png_create_write_struct() earlier.

	if (setjmp(png_ptr->jmp_buf_local))
	{
		// Free all of the memory associated with the png_ptr and info_ptr
		png_destroy_write_struct(&png_ptr, &info_ptr);
		fclose(fp);
		// If we get here, we had a problem writeing the file
		return NULL;
	}

	png_init_io(png_ptr, fp);

	png_color_8 sig_bit;
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	/* if the image has an alpha channel then */
	sig_bit.alpha = 8;
	png_set_sBIT(png_ptr, info_ptr, &sig_bit);
	png_set_bgr(png_ptr);

	png_set_IHDR(png_ptr, info_ptr, theImage->mWidth, theImage->mHeight, 8, PNG_COLOR_TYPE_RGB_ALPHA,
       PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	// Add filler (or alpha) byte (before/after each RGB triplet)
	//png_set_expand(png_ptr);
	//png_set_filler(png_ptr, 0xff, PNG_FILLER_AFTER);
	//png_set_gray_1_2_4_to_8(png_ptr);
	//png_set_palette_to_rgb(png_ptr);
	//png_set_gray_to_rgb(png_ptr);

	png_write_info(png_ptr, info_ptr);

	for (int i = 0; i < theImage->mHeight; i++)
	{
		png_bytep aRowPtr = (png_bytep) (theImage->mBits + i*theImage->mWidth);
		png_write_rows(png_ptr, &aRowPtr, 1);
	}

	// write rest of file, and get additional chunks in info_ptr - REQUIRED
	png_write_end(png_ptr, info_ptr);

	// clean up after the write, and free any memory allocated - REQUIRED
	png_destroy_write_struct(&png_ptr, &info_ptr);

	// close the file
	fclose(fp);

	return true;
}

bool ImageLib::WriteTGAImage(const std::string& theFileName, Image* theImage)
{
	FILE* aTGAFile = fopen(theFileName.c_str(), "wb");
	if (aTGAFile == NULL)
		return false;

	BYTE aHeaderIDLen = 0;
	fwrite(&aHeaderIDLen, sizeof(BYTE), 1, aTGAFile);

	BYTE aColorMapType = 0;
	fwrite(&aColorMapType, sizeof(BYTE), 1, aTGAFile);
	
	BYTE anImageType = 2;
	fwrite(&anImageType, sizeof(BYTE), 1, aTGAFile);

	unsigned short aFirstEntryIdx = 0;
	fwrite(&aFirstEntryIdx, sizeof(short), 1, aTGAFile);

	unsigned short aColorMapLen = 0;
	fwrite(&aColorMapLen, sizeof(short), 1, aTGAFile);

	BYTE aColorMapEntrySize = 0;
	fwrite(&aColorMapEntrySize, sizeof(BYTE), 1, aTGAFile);	

	unsigned short anXOrigin = 0;
	fwrite(&anXOrigin, sizeof(short), 1, aTGAFile);

	unsigned short aYOrigin = 0;
	fwrite(&aYOrigin, sizeof(short), 1, aTGAFile);

	unsigned short anImageWidth = theImage->mWidth;
	fwrite(&anImageWidth, sizeof(short), 1, aTGAFile);	

	unsigned short anImageHeight = theImage->mHeight;
	fwrite(&anImageHeight, sizeof(short), 1, aTGAFile);	

	BYTE aBitCount = 32;
	fwrite(&aBitCount, sizeof(BYTE), 1, aTGAFile);	

	BYTE anImageDescriptor = 8 | (1<<5);
	fwrite(&anImageDescriptor, sizeof(BYTE), 1, aTGAFile);

	fwrite(theImage->mBits, 4, theImage->mWidth*theImage->mHeight, aTGAFile);

	fclose(aTGAFile);

	return true;
}

bool ImageLib::WriteBMPImage(const std::string& theFileName, Image* theImage)
{
	/*FILE* aFile = fopen(theFileName.c_str(), "wb");
	if (aFile == NULL)
		return false;

	BITMAPFILEHEADER aFileHeader;
	BITMAPINFOHEADER aHeader;

	memset(&aFileHeader,0,sizeof(aFileHeader));
	memset(&aHeader,0,sizeof(aHeader));

	int aNumBytes = theImage->mWidth*theImage->mHeight*4;

	aFileHeader.bfType = ('M'<<8) | 'B';
	aFileHeader.bfSize = sizeof(aFileHeader) + sizeof(aHeader) + aNumBytes;
	aFileHeader.bfOffBits = sizeof(aHeader); 

	aHeader.biSize = sizeof(aHeader);
	aHeader.biWidth = theImage->mWidth;
	aHeader.biHeight = theImage->mHeight;
	aHeader.biPlanes = 1;
	aHeader.biBitCount = 32;
	aHeader.biCompression = BI_RGB;

	fwrite(&aFileHeader,sizeof(aFileHeader),1,aFile);
	fwrite(&aHeader,sizeof(aHeader),1,aFile);
	DWORD *aRow = theImage->mBits + (theImage->mHeight-1)*theImage->mWidth;
	int aRowSize = theImage->mWidth*4;
	for (int i=0; i<theImage->mHeight; i++, aRow-=theImage->mWidth)
		fwrite(aRow,4,theImage->mWidth,aFile);

	fclose(aFile);*/
	return true;
}

////////////////////////////////////////////////////////////////////////// 
// JPEG Pak Reader

typedef struct {
	struct jpeg_source_mgr pub;	/* public fields */

	CFile * infile;		/* source stream */
	JOCTET * buffer;		/* start of buffer */
	boolean start_of_file;	/* have we gotten any data yet? */
} pak_source_mgr;

typedef pak_source_mgr * pak_src_ptr;

#define INPUT_BUF_SIZE 4096

METHODDEF(void) init_source (j_decompress_ptr cinfo)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;
	src->start_of_file = TRUE;
}

METHODDEF(boolean) fill_input_buffer (j_decompress_ptr cinfo)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;
	size_t nbytes;

	nbytes = src->infile->Read(src->buffer, INPUT_BUF_SIZE);
	//((size_t) fread((void *) (buf), (size_t) 1, (size_t) (sizeofbuf), (file)))

	if (nbytes <= 0) {
		if (src->start_of_file)	/* Treat empty input file as fatal error */
			ERREXIT(cinfo, JERR_INPUT_EMPTY);
		WARNMS(cinfo, JWRN_JPEG_EOF);
		/* Insert a fake EOI marker */
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		nbytes = 2;
	}

	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = nbytes;
	src->start_of_file = FALSE;

	return TRUE;
}

METHODDEF(void) skip_input_data (j_decompress_ptr cinfo, long num_bytes)
{
	pak_src_ptr src = (pak_src_ptr) cinfo->src;

	if (num_bytes > 0) {
		while (num_bytes > (long) src->pub.bytes_in_buffer) {
			num_bytes -= (long) src->pub.bytes_in_buffer;
			(void) fill_input_buffer(cinfo);
		}
		src->pub.next_input_byte += (size_t) num_bytes;
		src->pub.bytes_in_buffer -= (size_t) num_bytes;
	}
}

METHODDEF(void) term_source (j_decompress_ptr cinfo)
{
	/* no work necessary here */
}

void jpeg_pak_src (j_decompress_ptr cinfo, CFile* infile)
{
	pak_src_ptr src;

	/* The source object and input buffer are made permanent so that a series
	* of JPEG images can be read from the same file by calling jpeg_stdio_src
	* only before the first one.  (If we discarded the buffer at the end of
	* one image, we'd likely lose the start of the next one.)
	* This makes it unsafe to use this manager and a different source
	* manager serially with the same JPEG object.  Caveat programmer.
	*/
	if (cinfo->src == NULL) {	/* first time for this JPEG object? */
		cinfo->src = (struct jpeg_source_mgr *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			sizeof(pak_source_mgr));
		src = (pak_src_ptr) cinfo->src;
		src->buffer = (JOCTET *)
			(*cinfo->mem->alloc_small) ((j_common_ptr) cinfo, JPOOL_PERMANENT,
			INPUT_BUF_SIZE * sizeof(JOCTET));
	}

	src = (pak_src_ptr) cinfo->src;
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buffer;
	src->pub.skip_input_data = skip_input_data;
#if(!DEF_QT)
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */
#endif
	src->pub.term_source = term_source;
	src->infile = infile;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */
}

Image* GetJPEGImageSize(const std::string& theFileName)
{
	CFile fp(theFileName);
    fp.OpenRead();
	
	if (!fp.IsOpen())
		return NULL;
	
	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
#if(!DEF_QT)
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	
	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		return 0;
	}
	
	jpeg_create_decompress(&cinfo);
	jpeg_pak_src(&cinfo, &fp);
	jpeg_read_header(&cinfo, TRUE);
    
	Image* anImage = new Image();
	anImage->mWidth = cinfo.image_width;
	anImage->mHeight = cinfo.image_height;
    anImage->mIsAlpha = false;
	//anImage->mBits = aBits;
	
	//jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);
	
	
	return anImage;
#endif
    return NULL;
}

Image* GetJPEGImage(const std::string& theFileName)
{
	CFile fp(theFileName);
    fp.OpenRead();
	
	if (!fp.IsOpen())
		return NULL;

	struct jpeg_decompress_struct cinfo;
	struct my_error_mgr jerr;
#if(!DEF_QT)
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;

	if (setjmp(jerr.setjmp_buffer))
	{
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		return 0;
	}

	jpeg_create_decompress(&cinfo);
	jpeg_pak_src(&cinfo, &fp);
	jpeg_read_header(&cinfo, TRUE);
	jpeg_start_decompress(&cinfo);
	int row_stride = cinfo.output_width * cinfo.output_components;

	unsigned char** buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	uint32_t* aBits = NULL;

	try
	{
		 aBits = new uint32_t[cinfo.output_width*cinfo.output_height];
	}
	catch (std::bad_alloc& ba)
	{
		ALOG("GetJPEGImage: failed get %d bytes for image %s: bad_alloc", cinfo.output_width*cinfo.output_height, theFileName.c_str());
		aBits = NULL;
	}

	if (aBits == NULL)
		return NULL;

	uint32_t* q = aBits;

	if (cinfo.output_components==1)
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			unsigned char* p = *buffer;
			for (int i = 0; i < cinfo.output_width; i++)
			{
				int r = *p++;
				//*q++ = 0xFF000000 | (r << 16) | (r << 8) | (r);
                *q++ = 0xFF000000 | (r) | (r << 8) | (r << 16);

			}
		}
	}
	else
	{
		while (cinfo.output_scanline < cinfo.output_height)
		{
			jpeg_read_scanlines(&cinfo, buffer, 1);

			unsigned char* p = *buffer;
			for (int i = 0; i < cinfo.output_width; i++)
			{
                int b = *p++;
				int g = *p++;
                int r = *p++;

				*q++ = 0xFF000000 | (r << 16) | (g << 8) | (b);
			}
		}
	}

	Image* anImage = new Image();
	anImage->mWidth = cinfo.output_width;
	anImage->mHeight = cinfo.output_height;
	anImage->mBits = aBits;
    anImage->mIsAlpha = false;

	jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	return anImage;
#endif
    return NULL;
}
// ============================================================
#if 1
//Image* GetJPEG2000Image(const std::string& theFileName)
//{
//	DWORD aTick = GetTickCount();
//
//	static bool inited = false;
//	if (!inited)
//	{
//		inited = true;
//		jas_init();
//	}
//
//	jas_stream_t* aStream = jas_stream_fopen(theFileName.c_str(),"rb");
//	if (!aStream)
//		return NULL;
//
//	// Read Image Header
//	int aFormat = jas_image_getfmt(aStream);
//	if (aFormat < 0)
//	{
//		jas_stream_close(aStream);
//		return NULL;
//	}
//
//	jas_image_t* aJasImage = jas_image_decode(aStream,aFormat,NULL);
//	if (!aJasImage)
//	{
//		jas_stream_close(aStream);
//		return NULL;
//	}
//
//	DWORD aDecodeTime = GetTickCount() - aTick;
//
//	int aNumComponents = jas_image_numcmpts(aJasImage);
//	if (aNumComponents<1 || aNumComponents>4)
//	{
//		jas_stream_close(aStream);
//		jas_image_destroy(aJasImage);
//		return NULL;
//	}
//
//	int i;
//	int aMaxWidth = 0;
//	int aMaxHeight = 0;
//	for (i=0; i<aNumComponents; i++)
//	{
//		int hstep = jas_image_cmpthstep(aJasImage,i);
//		int numHSteps = jas_image_cmptwidth(aJasImage,i);
//
//		int vstep = jas_image_cmptvstep(aJasImage,i);
//		int numVSteps = jas_image_cmptheight(aJasImage,i);
//
//		int aWidth = jas_image_cmpttlx(aJasImage,i) + hstep*numHSteps;
//		int aHeight = jas_image_cmpttly(aJasImage,i) + vstep*numVSteps;
//
//		if (aWidth > aMaxWidth)
//			aMaxWidth = aWidth;
//
//		if (aHeight > aMaxHeight)
//			aMaxHeight = aHeight;
//	}
//
//	// Read Image Data
//	Image *anImage = new Image;
//	anImage->mWidth = aMaxWidth;
//	anImage->mHeight = aMaxHeight;
//	anImage->mBits = new unsigned long[aMaxWidth * aMaxHeight];
//	memset(anImage->mBits,0,aMaxWidth * aMaxHeight*4);
//
//	int aColorModel = jas_image_clrspc(aJasImage);
//
//	for (i=0; i<aNumComponents; i++)
//	{
//		int hstep = jas_image_cmpthstep(aJasImage,i);
//		int vstep = jas_image_cmptvstep(aJasImage,i);
//		int numHSteps = jas_image_cmptwidth(aJasImage,i);
//		int numVSteps = jas_image_cmptheight(aJasImage,i);
//		int xorig = jas_image_cmpttlx(aJasImage,i);
//		int yorig = jas_image_cmpttly(aJasImage,i);
//		bool sign = jas_image_cmptsgnd(aJasImage,i)?true:false;
//
//		jas_matrix_t* aMatrix = jas_matrix_create(1,numHSteps);
//		if (!aMatrix)
//		{
//			delete anImage;
//			jas_image_destroy(aJasImage);
//			jas_stream_close(aStream);
//
//			return NULL;
//		}
//
//		int aShift = 8 - jas_image_cmptprec(aJasImage,i);
//		if (aShift<0)
//		{
//			delete anImage;
//			jas_matrix_destroy(aMatrix);
//			jas_image_destroy(aJasImage);
//			jas_stream_close(aStream);
//
//			return NULL;
//		}
//
//		unsigned long* destRow = anImage->mBits + yorig*numVSteps*anImage->GetWidth() + xorig*numHSteps;
//
//		// color model
//		int aComponentType = jas_image_cmpttype(aJasImage,i);
//		int aColorType = JAS_IMAGE_CT_COLOR(aComponentType);
//
//		switch (aColorType)
//		{
//			case JAS_IMAGE_CT_RGB_R: aShift += 16; break;
//			case JAS_IMAGE_CT_RGB_G: aShift += 8; break;
//			case JAS_IMAGE_CT_RGB_B: break;
//			default: aShift += 24; break; 
//		}
//
//		for (int y=0; y<numVSteps; y++)
//		{
//			if (jas_image_readcmpt(aJasImage,i,0,y,numHSteps,1,aMatrix) )
//			{
//				delete anImage;
//				jas_matrix_destroy(aMatrix);
//				jas_image_destroy(aJasImage);
//				jas_stream_close(aStream);
//
//				return NULL;			
//			}
//
//			unsigned long* dest = destRow;
//			for (int x=0; x<numHSteps; x++)
//			{
//				int aVal = jas_matrix_getv(aMatrix,x);
//				if (sign)
//					aVal = (unsigned char)(aVal + 128);
//
//				aVal <<= aShift;
//
//				unsigned long *destRowWriter = dest;
//				for (int j=0; j<vstep; j++)
//				{
//					unsigned long *destWriter = destRowWriter;
//					for (int k=0; k<hstep; k++)
//						*destWriter++ |= aVal;
//
//					destRowWriter += anImage->GetWidth();
//				}
//
//				dest += hstep;
//			}
//			destRow += vstep*anImage->GetWidth();
//		}
//
//		// release decoding matrix
//		jas_matrix_destroy(aMatrix);
//	}
//
//	DWORD aReadTime = GetTickCount() - aTick;
//	char aBuf[512];
//	sprintf(aBuf,"%d %d\n",aDecodeTime,aReadTime);
//	OutputDebugString(aBuf);
//
//
//	if (aNumComponents < 4) // add 255 alpha
//	{
//		int aSize = anImage->GetWidth()*anImage->GetHeight();
//		unsigned long *dest = anImage->mBits;
//		for (i=0; i<aSize; i++)
//			*dest++ |= 0xff000000;
//	}
//
//	jas_image_destroy(aJasImage);
//	jas_stream_close(aStream);
//
//	return anImage;
//}
// ============================================================
#else

/// dmbreaker commented it:
#include "j2k-codec\j2k-codec.h"

HMODULE gJ2KCodec = NULL;
std::string gJ2KCodecKey = "huLThJSVctbcXk0JynB!bgi!BPd?9cNvS0p9p05ADhMjYN60ScNLz4h8S6u0hZeZFT";



void ImageLib::InitJPEG2000()
{
	gJ2KCodec = ::LoadLibrary(_T("j2k-codec.dll"));
}

void ImageLib::CloseJPEG2000()
{
	if (gJ2KCodec != NULL)
	{
		::FreeLibrary(gJ2KCodec);
		gJ2KCodec = NULL;
	}
}

void ImageLib::SetJ2KCodecKey(const std::string& theKey)
{
	gJ2KCodecKey = theKey;
}

Image* GetJPEG2000Image(const std::string& theFileName)
{
	if (gJ2KCodec != NULL)
	{
		PFILE* aFP = p_fopen(theFileName.c_str(), "rb");
		if (aFP == NULL)
			return NULL;

		static int (__stdcall *fJ2K_getVersion)() = NULL;
		static void (__stdcall *fJ2K_Unlock)(const char*) = NULL;
		static void* (__stdcall *fJ2K_OpenCustom)(void*, J2K_Callbacks*) = NULL;
		static void* (__stdcall *fJ2K_OpenFile)(const char*) = NULL;
		static void (__stdcall *fJ2K_Close)(void*) = NULL;
		static int (__stdcall *fJ2K_GetInfo)(void*, int*, int*, int*) = NULL;
		static int (__stdcall *fJ2K_GetResolutionDimensions)(void*, int, int*, int*) = NULL;
		static int (__stdcall *fJ2K_Decode)(void*, unsigned char**, int*, char*, int*) = NULL;
		static int (__stdcall *fJ2K_getLastError)() = NULL;
		static const char* (__stdcall *fJ2K_getErrorStr)(int) = NULL;
		static bool loadFuncs = true;

		if (loadFuncs)
		{
			loadFuncs = false;
			*((void**)&fJ2K_getVersion) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getVersion@0");
			*((void**)&fJ2K_Unlock) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Unlock@4");
			*((void**)&fJ2K_OpenCustom) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_OpenCustom@8");
			*((void**)&fJ2K_OpenFile) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_OpenFile@4");
			*((void**)&fJ2K_Close) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Close@4");
			*((void**)&fJ2K_GetInfo) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_GetInfo@16");
			*((void**)&fJ2K_GetResolutionDimensions) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_GetResolutionDimensions@16");
			*((void**)&fJ2K_Decode) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_Decode@20");
			*((void**)&fJ2K_getLastError) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getLastError@0");
			*((void**)&fJ2K_getErrorStr) = (void*)::GetProcAddress(gJ2KCodec, "_J2K_getErrorStr@4");

			// j2k guys didn't use declare the export names. yay! now we have to update these mangled names any time the DLL changes.

			if (!(fJ2K_getVersion != NULL && 
				  fJ2K_Unlock != NULL && 
				  fJ2K_OpenCustom != NULL && 
				  fJ2K_OpenFile != NULL && 
				  fJ2K_Close != NULL && 
				  fJ2K_GetInfo != NULL && 
				  fJ2K_GetResolutionDimensions != NULL && 
				  fJ2K_Decode != NULL &&
				  fJ2K_getLastError != NULL &&
				  fJ2K_getErrorStr != NULL))
			{
				CloseJPEG2000();
				return NULL;
			}

			int aJ2kVer = (*fJ2K_getVersion)();
			if (aJ2kVer < 0x120000) 
			{
				CloseJPEG2000();
				return NULL;
			}

			(*fJ2K_Unlock)(gJ2KCodecKey.c_str()); 
		}

		J2K_Callbacks aCallbacks;
		aCallbacks.read = Pak_read;
		aCallbacks.seek = Pak_seek;
		aCallbacks.close = Pak_close;

		//theFileName.c_str()
		void* aJ2KImage = (*fJ2K_OpenCustom)(aFP, &aCallbacks);
		if (aJ2KImage == NULL)
		{
			int anErrNum = (*fJ2K_getLastError)();
			std::string anErrorMessage = (*fJ2K_getErrorStr)(anErrNum);
			return NULL;
		}

		int aWidth, aHeight, aComponents;
		if ((*fJ2K_GetInfo)(aJ2KImage, &aWidth, &aHeight, &aComponents) != J2KERR_SUCCESS) 
		{
			(*fJ2K_Close)(aJ2KImage);
			return NULL;
		}

		(*fJ2K_GetResolutionDimensions)(aJ2KImage, 0, &aWidth, &aHeight);
		
		unsigned long* aBuffer = new unsigned long[aWidth*aHeight];
		if (aBuffer == NULL)
		{
			(*fJ2K_Close)(aJ2KImage);
			return NULL;
		}

		char anOptsBuffer[32];
		strcpy(anOptsBuffer, "bpp=4,rl=0");

		int aSize = aWidth*aHeight*4;
		int aPitch = aWidth*4;
		if ((*fJ2K_Decode)(aJ2KImage, (unsigned char**)&aBuffer, &aSize, anOptsBuffer, &aPitch) != J2KERR_SUCCESS)
		{
			(*fJ2K_Close)(aJ2KImage);
			delete[] aBuffer;
			return NULL;
		}
		(*fJ2K_Close)(aJ2KImage);

		ImageLib::Image* anImage = new ImageLib::Image;
		anImage->mBits = aBuffer;
		anImage->mWidth = aWidth;
		anImage->mHeight = aHeight;
		if (gIgnoreJPEG2000Alpha)
		{
			DWORD *aPtr = anImage->mBits;
			DWORD *anEnd = aPtr+anImage->mWidth*anImage->mHeight;
			for (; aPtr!=anEnd; ++aPtr)
				*aPtr |= 0xFF000000;
		}

		p_fclose(aFP);

		return anImage;
	}
	return NULL;
}

#endif

/*int __stdcall Pak_seek(void *data_source, int offset)
{
	return p_fseek((PFILE*) data_source, offset, SEEK_SET);
}

int __stdcall Pak_read(void *ptr, int size, void *data_source)
{
	return p_fread(ptr, 1, size, (PFILE*) data_source);
}

void __stdcall Pak_close(void *data_source)
{	
}
*/


//bool ImageLib::gIgnoreJPEG2000Alpha = true;

//Image* ImageLib::GetImage(const std::string& theFilename, bool lookForAlphaImage)
//{
//	if (!gAutoLoadAlpha)
//		lookForAlphaImage = false;
//
//	int aLastDotPos = theFilename.rfind('.');
//	int aLastSlashPos = max((int)theFilename.rfind('\\'), (int)theFilename.rfind('/'));
//
//	std::string anExt;
//	std::string aFilename;
//
//	if (aLastDotPos > aLastSlashPos)
//	{
//		anExt = theFilename.substr(aLastDotPos, theFilename.length() - aLastDotPos);
//		aFilename = theFilename.substr(0, aLastDotPos);
//	}
//	else
//		aFilename = theFilename;
//
//	Image* anImage = NULL;
//
//	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".tga") == 0) || (anExt.length() == 0)))
//		 anImage = GetTGAImage(aFilename + ".tga");
//
//	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".jpg") == 0) || (anExt.length() == 0)))
//		 anImage = GetJPEGImage(aFilename + ".jpg");
//
//	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".png") == 0) || (anExt.length() == 0)))
//		 anImage = GetPNGImage(aFilename + ".png");
//
//	if ((anImage == NULL) && ((stricmp(anExt.c_str(), ".gif") == 0) || (anExt.length() == 0)))
//		 anImage = GetGIFImage(aFilename + ".gif");
//
//	if (anImage == NULL && (stricmp(anExt.c_str(), ".j2k") == 0 || anExt.length() == 0))
//		anImage = GetJPEG2000Image(aFilename + ".j2k");
//	if (anImage == NULL && (stricmp(anExt.c_str(), ".jp2") == 0 || anExt.length() == 0))
//		anImage = GetJPEG2000Image(aFilename + ".jp2");
//
//
//	// Check for alpha images
//	Image* anAlphaImage = NULL;
//	if(lookForAlphaImage)
//	{
//		// Check _ImageName
//		anAlphaImage = GetImage(theFilename.substr(0, aLastSlashPos+1) + "_" +
//			theFilename.substr(aLastSlashPos+1, theFilename.length() - aLastSlashPos - 1), false);
//
//		// Check ImageName_
//		if(anAlphaImage==NULL)
//			anAlphaImage = GetImage(theFilename + "_", false);
//	}
//
//
//
//	// Compose alpha channel with image
//	if (anAlphaImage != NULL) 
//	{
//		if (anImage != NULL)
//		{
//			if ((anImage->mWidth == anAlphaImage->mWidth) &&
//				(anImage->mHeight == anAlphaImage->mHeight))
//			{
//				unsigned long* aBits1 = anImage->mBits;
//				unsigned long* aBits2 = anAlphaImage->mBits;
//				int aSize = anImage->mWidth*anImage->mHeight;
//
//				for (int i = 0; i < aSize; i++)
//				{
//					*aBits1 = (*aBits1 & 0x00FFFFFF) | ((*aBits2 & 0xFF) << 24);
//					++aBits1;
//					++aBits2;
//				}
//			}
//
//			delete anAlphaImage;
//		}
//		else if (gAlphaComposeColor==0xFFFFFF)
//		{
//			anImage = anAlphaImage;
//
//			unsigned long* aBits1 = anImage->mBits;
//
//			int aSize = anImage->mWidth*anImage->mHeight;
//			for (int i = 0; i < aSize; i++)
//			{
//				*aBits1 = (0x00FFFFFF) | ((*aBits1 & 0xFF) << 24);
//				++aBits1;
//			}
//		}
//		else
//		{
//			const int aColor = gAlphaComposeColor;
//			anImage = anAlphaImage;
//
//			unsigned long* aBits1 = anImage->mBits;
//
//			int aSize = anImage->mWidth*anImage->mHeight;
//			for (int i = 0; i < aSize; i++)
//			{
//				*aBits1 = aColor | ((*aBits1 & 0xFF) << 24);
//				++aBits1;
//			}
//		}
//	}
//
//	return anImage;
//}
#endif

#ifndef __WIN32__
#if OS != OS_WP8
int _stricmp(const char* str1, const char *str2)
{
	size_t l1 = strlen(str1), l2 = strlen(str2);
	if(l1 != l2)
		return 1;
	for(int i = 0; i < l1; i++)
		if(tolower(str1[i]) != tolower(str2[i]))
			return 1;
		
	return 0;
}
#endif
#endif

/*Image* GetPNGImage(std::string filename)
{
	RR_ERROR(GetPNGImage not implemented);
	Image* im = new Image();
	im->mWidth = 1;
	im->mHeight =1;
	if(!im->mBits)
		im->mBits = new unsigned long[1*1];
	return im;
}

Image* GetJPEGImage(std::string filename)
{
	RR_ERROR(GetJPEGImage not implemented);
	Image* im = new Image();
	im->mWidth = 1;
	im->mHeight =1;
	if(!im->mBits)
		im->mBits = new unsigned long[1*1];
	return im;
}

Image* GetTGAImage(std::string filename)
{
	RR_ERROR(GetTGAImage not implemented);
	Image* im = new Image();
	im->mWidth = 1;
	im->mHeight =1;
	if(!im->mBits)
		im->mBits = new unsigned long[1*1];
	return im;
}

Image* GetGIFImage(std::string filename)
{
	RR_ERROR(GetGIFImage not implemented);
	Image* im = new Image();
	im->mWidth = 1;
	im->mHeight =1;
	if(!im->mBits)
		im->mBits = new unsigned long[1*1];
	return im;
}*/
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
Image* ImageLib::GetImage(const std::string& theFilename, bool lookForAlphaImage,const std::string& theAlphaFilename)
{
    std::string anExt = CFile::GetExtension(theFilename);
    bool isSupportExtention = CImage::IsSuportExtension(anExt);
    std::string aFilename = (!anExt.empty() && isSupportExtention)
                                ? theFilename.substr(0, theFilename.size() - (anExt.size() + 1))
                                : theFilename;

    Image* anImage = NULL;
    
    if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "webp") == 0) || (anExt.length() == 0)))
        anImage = GetWebpImage( aFilename + ".webp");
    
	if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "png") == 0) || (anExt.length() == 0)))
		 anImage = GetPNGImage( aFilename + ".png");

	if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "jpg") == 0) || (anExt.length() == 0)))
		 anImage = GetJPEGImage(aFilename + ".jpg");

	if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "tga") == 0) || (anExt.length() == 0)))
		 anImage = GetTGAImage(aFilename + ".tga");

	if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "gif") == 0) || (anExt.length() == 0)))
		 anImage = GetGIFImage(aFilename + ".gif");
    
    if (anImage == NULL)
        anImage = GetPNGImage(aFilename);
    
	if (anImage == NULL)
        anImage = GetJPEGImage(aFilename);
	
	if(anImage == NULL)
	{
        return NULL;
	}

	// Check for alpha images
	Image* anAlphaImage = NULL;
	if(theAlphaFilename[0]!= 0)
	{
        anExt = CFile::GetExtension(theAlphaFilename);
        bool isSupportExtension = CImage::IsSuportExtension(anExt);
        aFilename = (!anExt.empty() && isSupportExtension)
                        ? theAlphaFilename.substr(0, theAlphaFilename.size() - (anExt.size() + 1))
                        : theAlphaFilename;

		anAlphaImage = GetImage(aFilename,false);
	}
	else if(lookForAlphaImage)
	{
		// Check _ImageName
        std::string fName = CFile::GetName(theFilename);
        std::string fPath = (!fName.empty())
                                ? theFilename.substr(0, theFilename.size() - fName.size())
                                : fName;

		anAlphaImage = GetImage(fPath + "_" + fName, false);

		// Check ImageName_
		if(anAlphaImage==NULL)
			anAlphaImage = GetImage(theFilename + "_", false);
	}



	// Compose alpha channel with image
	if (anAlphaImage != NULL) 
	{
		if (anImage != NULL)
		{
			if ((anImage->mWidth == anAlphaImage->mWidth) &&
				(anImage->mHeight == anAlphaImage->mHeight))
			{
				uint32_t* aBits1 = anImage->mBits;
				uint32_t* aBits2 = anAlphaImage->mBits;
				int aSize = anImage->mWidth*anImage->mHeight;

				for (int i = 0; i < aSize; i++)
				{
					*aBits1 = (*aBits1 & 0x00FFFFFF) | ((*aBits2 & 0xFF) << 24);
					++aBits1;
					++aBits2;
				}
			}

			delete anAlphaImage;
            anAlphaImage = NULL;
		}
		else if (gAlphaComposeColor==0xFFFFFF)
		{
			anImage = anAlphaImage;

			uint32_t* aBits1 = anImage->mBits;

			int aSize = anImage->mWidth*anImage->mHeight;
			for (int i = 0; i < aSize; i++)
			{
				*aBits1 = (0x00FFFFFF) | ((*aBits1 & 0xFF) << 24);
				++aBits1;
			}
		}
		else
		{
			const int aColor = gAlphaComposeColor;
			anImage = anAlphaImage;

			uint32_t* aBits1 = anImage->mBits;

			int aSize = anImage->mWidth*anImage->mHeight;
			for (int i = 0; i < aSize; i++)
			{
				*aBits1 = aColor | ((*aBits1 & 0xFF) << 24);
				++aBits1;
			}
		}
        
        if (anImage) anImage->mIsAlpha = true;
        if (anAlphaImage) anAlphaImage->mIsAlpha = true;
	}

	return anImage;
}

sSize ImageLib::GetImageSize(const std::string& theFilename)
{
    const std::string& anExt = CFile::GetExtension(theFilename);
    bool isSupportExtension = CImage::IsSuportExtension(anExt);
    std::string aFilename = (!anExt.empty() && isSupportExtension)
                            ? theFilename.substr(0, theFilename.size() - (anExt.size() + 1))
                            : theFilename;

	Image* anImage = NULL;
    
    if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "webp") == 0) || (anExt.length() == 0)))
        anImage = GetWebpImageSize( aFilename + ".webp");
    
	if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "png") == 0) || (anExt.length() == 0)))
		anImage = GetPNGImageSize( aFilename + ".png");
	
	if ((anImage == NULL) && ((_stricmp(anExt.c_str(), "jpg") == 0) || (anExt.length() == 0)))
		anImage = GetJPEGImageSize(aFilename + ".jpg");

	sSize res;
	if(anImage)
	{
		res.width = anImage->mWidth;
		res.height = anImage->mHeight;
        res.mIsAlpha = anImage->mIsAlpha;
        delete anImage;
	}
	else 
	{
		res.width = 0;
		res.height = 0;
        res.mIsAlpha = false;
	}
	return res;

}
