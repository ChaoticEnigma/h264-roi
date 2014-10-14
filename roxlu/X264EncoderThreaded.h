#ifndef ROXLU_X264_ENCODER_THREADED_H
#define ROXLU_X264_ENCODER_THREADED_H

extern "C" {
#include <uv.h>
}

#include <string>
#include <vector>
#include <roxlu/Roxlu.h>                     /* we need the RingBuffer */
#include <video/X264Encoder.h>

void x264_encoder_thread(void* user);

struct X264Data {
  size_t offset;
  size_t nbytes;
};

class X264EncoderThreaded {
 public:
  X264EncoderThreaded();
  ~X264EncoderThreaded();
  bool start(std::string filename, bool datapath);
  void encode(char* pixels);
  void stop();
  void lock();
  void unlock();
 public:
  RingBuffer buffer;
  X264Encoder enc;                           /* the encoder we use to encode to h264 */
  uv_thread_t thread_id;
  uv_mutex_t mutex;
  size_t num_bytes;                          /* num bytes per frame */
  volatile bool must_stop;
  std::vector<X264Data> input_data;
};

inline void X264EncoderThreaded::lock() {
  uv_mutex_lock(&mutex);
}

inline void X264EncoderThreaded::unlock() {
  uv_mutex_unlock(&mutex);
}

inline void X264EncoderThreaded::stop() {
  must_stop = true;
}
#endif
