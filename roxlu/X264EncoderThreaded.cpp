#include "X264EncoderThreaded.h"

// ----------------------------------------------------------------

void x264_encoder_thread(void* user) {
  X264EncoderThreaded& enc_threaded = *(static_cast<X264EncoderThreaded*>(user));
  X264Encoder& enc = enc_threaded.enc;
  char* frame = NULL;

  AVPicture pic;
  int num_bytes = avpicture_fill(&pic, NULL, enc.in_pixel_format, enc.in_width, enc.in_height);
  frame = new char[num_bytes * 2]; // lib swscale needs some more space

  std::vector<X264Data> work_data;
  while(!enc_threaded.must_stop) {

    // get data we need to encode
    enc_threaded.lock();
    {
      std::copy(enc_threaded.input_data.begin(), enc_threaded.input_data.end(), std::back_inserter(work_data));
      enc_threaded.input_data.clear();
    }
    enc_threaded.unlock();

    // And encode
    for(std::vector<X264Data>::iterator it = work_data.begin(); it != work_data.end(); ++it) {
      X264Data& data = *it;
      enc_threaded.buffer.read(frame, data.nbytes);
      enc.encode(frame);
    }
    work_data.clear();
  }
  enc.close();

  delete[] frame;
  frame = NULL;
}

// ----------------------------------------------------------------

X264EncoderThreaded::X264EncoderThreaded()
  :must_stop(false)
  ,buffer(1024 * 1024 * 512)  /* 300 mb,  must be enough for quite some frames */
  ,num_bytes(0)
{
  uv_mutex_init(&mutex);
}

X264EncoderThreaded::~X264EncoderThreaded() {
  stop();
  uv_thread_join(&thread_id);
  uv_mutex_destroy(&mutex);
}

bool X264EncoderThreaded::start(std::string filename, bool datapath) {
  if(!enc.open(filename, datapath)) {
    return false;
  }

  // get the size for one frame.
  AVPicture pic;
  num_bytes = avpicture_fill(&pic, NULL, enc.in_pixel_format, enc.in_width, enc.in_height);

  uv_thread_create(&thread_id, x264_encoder_thread, this);
  return true;
}

void X264EncoderThreaded::encode(char* pixels) {
  X264Data data;
  data.offset = buffer.getWriteIndex();
  data.nbytes = num_bytes;


  size_t written = buffer.write((const char*)pixels, num_bytes);
  lock();
  input_data.push_back(data);
  unlock();

  //RX_VERBOSE("WRITTEN %ld, write index: %ld", written, buffer.getWriteIndex());
}
