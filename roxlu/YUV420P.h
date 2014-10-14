/*
  # YUV420P
 
  This class is a basic container for YUV420P based video data.
  We assume the given VideoBuffer contains AVDecoderFrames/AVFrames
  of the type AV_PIX_FMT_YUV420P. 

  This classes uses 3 fast VideoTextures to stream video data to the 
  GPU (using round robbin', orphaned pbos)

 */
#ifndef ROXLU_YUV420P_H
#define ROXLU_YUV420P_H

#include <roxlu/core/Log.h>
#include <roxlu/opengl/GL.h>
#include <av/AVDecoder.h>
#include <video/VideoTexture.h>

class YUV420P {
 public:
  YUV420P();
  ~YUV420P();
  bool setup(AVFrame* f);
  void print();                                                 /* print debug info */
  bool updateTextures(AVFrame* f);
  GLuint getYTexture();
  GLuint getUTexture();
  GLuint getVTexture();
 public:
  int linesize[3];                                              /* strides of the 3 planes */
  int width[3];                                                 /* widths of the 3 planes */
  int height[3];                                                /* heigts of the 3 planes */
  VideoTexture vt_y;
  VideoTexture vt_u;
  VideoTexture vt_v;
};

inline GLuint YUV420P::getYTexture() {
  return vt_y.getTexture();
}
inline GLuint YUV420P::getUTexture() {
  return vt_u.getTexture();
}
inline GLuint YUV420P::getVTexture() {
  return vt_v.getTexture();
}

#endif
