#include <stdexcept>

#include "JpgEncoder.hpp"
#include "util/debug.h"
ScalingFactor::ScalingFactor(const tjscalingfactor *pFactor):
_pFactor(pFactor){

}
ScalingFactor::~ScalingFactor(){

}
//缩放比例
float ScalingFactor::scalingPercentage(){
  return (float)_pFactor->num/(float)_pFactor->denom;
}

//分子 
int ScalingFactor::num(){
  return _pFactor->num;
}

//分母
int ScalingFactor::denom(){
  return _pFactor->denom;
}


int ScalingFactor::loadScalingFactors(){
  return 0;
}


Resizer::Resizer(int sampleType):
  mSubsampling(sampleType)
{

}



bool Resizer::resize(Minicap::Frame *pFrame, unsigned char **ppBuffer, unsigned long *pBufferSize){

  mCompressedDataSize = tjBufSize(
    pFrame->width,
    pFrame->height,
    mSubsampling
  );

  MCINFO("Allocating %ld bytes for JPG resizer", mCompressedDataSize);

  tjhandle _tjCompressHandler = tjInitCompress();
  mCompressedData = tjAlloc(mCompressedDataSize);
  int ret =  tjCompress2(
    _tjCompressHandler,
    (unsigned char*) pFrame->data,
    pFrame->width,
    pFrame->stride * pFrame->bpp,
    pFrame->height,
    JpgEncoder::convertFormat(pFrame->format),
    &mCompressedData,
    &mCompressedDataSize,
    mSubsampling,
    100,
    TJFLAG_FASTDCT | TJFLAG_NOREALLOC
  );

  printf("%d from tjCompress2\r\n", ret);

    int width = 540;
    int height = 960;
    int pixelType = TJPF_GRAY;
    int pixelSize = tjPixelSize[pixelType];
    mDecompressDataSize = width * pixelSize * height;
    mDecompressData = tjAlloc(mDecompressDataSize);
    tjhandle tjDecompressHandler = tjInitDecompress();

  ret = tjDecompress2(
      tjDecompressHandler, 
      mCompressedData, mCompressedDataSize, 
      mDecompressData, 
      width, pixelSize, height, 
      pixelType, 
      TJFLAG_FASTDCT);

  printf("%d from tjDecompress2\r\n", ret);
  ret = tjCompress2(
    _tjCompressHandler,
    mDecompressData,
    width,
    pixelSize * width,
    height,
    TJPF_RGBA,
    ppBuffer,
    pBufferSize,
    mSubsampling,
    10,
    TJFLAG_FASTDCT | TJFLAG_NOREALLOC
  );

  printf("%d from tjCompress2: %s\r\n", ret, tjGetErrorStr());
  return ret == 0;
}

JpgEncoder::JpgEncoder(unsigned int prePadding, unsigned int postPadding, unsigned int sampling, float scaling)
  : mTjCompressHandle(tjInitCompress()),
    mTjDecompressHandle(tjInitDecompress()),
    mScaling(scaling),
    mSubsampling(sampling),
    mEncodedData(NULL),
    mPrePadding(prePadding),
    mPostPadding(postPadding),
    mMaxWidth(0),
    mMaxHeight(0),
    mCompressBuffer(NULL),
    mDecompressBuffer(NULL)

{
}

JpgEncoder::~JpgEncoder() {

  //printf("%x\r\n", mCompressBuffer);
  tjFree(mCompressBuffer);
  //printf("%x\r\n", mDecompressBuffer);
  tjFree(mDecompressBuffer);
  //printf("%x\r\n", mEncodedData);
  tjFree(mEncodedData);
}

bool
JpgEncoder::encode(Minicap::Frame* frame, unsigned int quality) {
/*
  unsigned char *mCompressBuffer;
  unsigned long mCompressBufferSize;

  unsigned char *mDecompressBuffer;
  unsigned long mDecompressBufferSize;
*/

  unsigned long compressDataSize = 0;
  int ret =  tjCompress2(
    mTjCompressHandle,
    (unsigned char*) frame->data,
    frame->width,
    frame->stride * frame->bpp,
    frame->height,
    convertFormat(frame->format),
    &mCompressBuffer,
    &compressDataSize,
    mSubsampling,
    100,
    TJFLAG_FASTDCT | TJFLAG_NOREALLOC
  );

  MCINFO("Encoding raw data with info Width: %d, Heigth: %d, BytePerPixel: %d, RawSize: %d, EncodedSize: %d\r\n", 
      frame->width, frame->height,frame->bpp, frame->size/1024, compressDataSize/1024);
    
    int width = frame->width * mScaling;
    int height = frame->height * mScaling;
    int pixelType = convertFormat(frame->format);
    int pixelSize = tjPixelSize[pixelType];

    ret = tjDecompress2(mTjDecompressHandle, mCompressBuffer, compressDataSize, mDecompressBuffer, width, width*pixelSize, height, convertFormat(frame->format), TJFLAG_FASTDCT | TJFLAG_NOREALLOC);

  unsigned char* offset = getEncodedData();

   ret =  tjCompress2(
    mTjCompressHandle,
    mDecompressBuffer,
    width,
    width * pixelSize,
    height,
    convertFormat(frame->format),
    &offset,
    &mEncodedSize,
    mSubsampling,
    quality,
    TJFLAG_FASTDCT | TJFLAG_NOREALLOC
  );

  //return false;
  return ret == 0;
}

int
JpgEncoder::getEncodedSize() {
  return mEncodedSize;
}

unsigned char*
JpgEncoder::getEncodedData() {
  return mEncodedData + mPrePadding;
}

//TODO修正缓冲区大小为图片缩放后重新计算得到的缓冲区大小
bool
JpgEncoder::reserveData(uint32_t width, uint32_t height) {
  //printf("reserver data buffer for %dx%d", width, height);
  if (width == mMaxWidth && height == mMaxHeight) {
    return 0;
  }

   tjFree(mCompressBuffer);
   
  mCompressBufferSize =  tjBufSize(
    width,
    height,
    mSubsampling
  );
  MCINFO("Allocating %ld bytes for JPG resizer compression", mCompressBufferSize);
  mCompressBuffer = tjAlloc(mCompressBufferSize);
   //printf("%x\r\n", mCompressBuffer);
  

tjFree(mDecompressBuffer);
    mDecompressBufferSize = tjBufSize(
      width, 
      height,
      mSubsampling
    );
    mDecompressBufferSize =( width*mScaling ) * 4 * (height*mScaling);
    MCINFO("Allocating %ld bytes for JPG resizer decompression", mDecompressBufferSize);
    mDecompressBuffer = tjAlloc(mDecompressBufferSize);
//printf("%x\r\n", mDecompressBuffer);
  

  tjFree(mEncodedData);

  unsigned long maxSize = mPrePadding + mPostPadding + tjBufSize(
    width,
    height,
    mSubsampling
  );

  MCINFO("Allocating %ld bytes for JPG encoder", maxSize);

  mEncodedData = tjAlloc(maxSize);
//printf("%x\r\n", mEncodedData);
  if (mEncodedData == NULL) {
    return false;
  }

  mMaxWidth = width;
  mMaxHeight = height;

  return true;
}

int
JpgEncoder::convertFormat(Minicap::Format format) {
  switch (format) {
  case Minicap::FORMAT_RGBA_8888:
    return TJPF_RGBA;
  case Minicap::FORMAT_RGBX_8888:
    return TJPF_RGBX;
  case Minicap::FORMAT_RGB_888:
    return TJPF_RGB;
  case Minicap::FORMAT_BGRA_8888:
    return TJPF_BGRA;
  default:
    throw std::runtime_error("Unsupported pixel format");
  }
}

const char *
JpgEncoder::convertSampling(int n){
  switch(n){
  /**
   * 4:4:4 chrominance subsampling (no chrominance subsampling).  The JPEG or
   * YUV image will contain one chrominance component for every pixel in the
   * source image.
   */
  case TJSAMP_444:
  return "TJSAMP_444";
  /**
   * 4:2:2 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 2x1 block of pixels in the source image.
   */
  case TJSAMP_422:
  return "TJSAMP_422";
  /**
   * 4:2:0 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 2x2 block of pixels in the source image.
   */
  case TJSAMP_420:
  return "TJSAMP_422";
  /**
   * Grayscale.  The JPEG or YUV image will contain no chrominance components.
   */
  case TJSAMP_GRAY:
  return "TJSAMP_422";
  /**
   * 4:4:0 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 1x2 block of pixels in the source image.
   *
   * @note 4:4:0 subsampling is not fully accelerated in libjpeg-turbo.
   */
  case TJSAMP_440:
  return "TJSAMP_422";
  /**
   * 4:1:1 chrominance subsampling.  The JPEG or YUV image will contain one
   * chrominance component for every 4x1 block of pixels in the source image.
   * JPEG images compressed with 4:1:1 subsampling will be almost exactly the
   * same size as those compressed with 4:2:0 subsampling, and in the
   * aggregate, both subsampling methods produce approximately the same
   * perceptual quality.  However, 4:1:1 is better able to reproduce sharp
   * horizontal features.
   *
   * @note 4:1:1 subsampling is not fully accelerated in libjpeg-turbo.
   */
  case TJSAMP_411:
  return "TJSAMP_422";

  default:
  return "UNKNOWN";
  }
}