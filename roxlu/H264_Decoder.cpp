#include "H264_Decoder.h"

H264_Decoder::H264_Decoder(h264_decoder_callback frameCallback, void* user)
  :codec(NULL)
  ,codec_context(NULL)
  ,parser(NULL)
  ,fp(NULL)
  ,frame(0)
  ,cb_frame(frameCallback)
  ,cb_user(user)
{
  avcodec_register_all();
}

H264_Decoder::~H264_Decoder() {

  if(parser) {
    av_parser_close(parser);
    parser = NULL;
  }

  if(codec_context) {
    avcodec_close(codec_context);
    av_free(codec_context);
    codec_context = NULL;
  }

  if(picture) {
    av_free(picture);
    picture = NULL;
  }

  if(fp) {
    fclose(fp);
    fp = NULL;
  }

  cb_frame = NULL;
  cb_user = NULL;
  frame = 0;
}

bool H264_Decoder::load(std::string filepath, float fps) {

  codec = avcodec_find_decoder(AV_CODEC_ID_H264);
  if(!codec) {
    printf("Error: cannot find the h264 codec: %s\n", filepath.c_str());
    return false;
  }

  codec_context = avcodec_alloc_context3(codec);

  if(codec->capabilities & CODEC_CAP_TRUNCATED) {
    codec_context->flags |= CODEC_FLAG_TRUNCATED;
  }

  if(avcodec_open2(codec_context, codec, NULL) < 0) {
    printf("Error: could not open codec.\n");
    return false;
  }

  fp = fopen(filepath.c_str(), "rb");

  if(!fp) {
    printf("Error: cannot open: %s\n", filepath.c_str());
    return false;
  }

  picture = av_frame_alloc();
  parser = av_parser_init(AV_CODEC_ID_H264);

  if(!parser) {
    printf("Erorr: cannot create H264 parser.\n");
    return false;
  }

  // kickoff reading...
  readBuffer();

  return true;
}

bool H264_Decoder::readFrame(){
    bool needs_more = false;
    while(!update(needs_more)){
        if(needs_more) {
            readBuffer();
        }
    }
    return true;
}

void H264_Decoder::decodeFrame(uint8_t* data, int size) {

  AVPacket pkt;
  int got_picture = 0;
  int len = 0;

  av_init_packet(&pkt);

  pkt.data = data;
  pkt.size = size;

  len = avcodec_decode_video2(codec_context, picture, &got_picture, &pkt);
  if(len < 0) {
    printf("Error while decoding a frame.\n");
  }

  if(got_picture == 0) {
    return;
  }

  ++frame;

  if(cb_frame) {
    cb_frame(picture, &pkt, this, cb_user);
  }
}

int H264_Decoder::readBuffer() {

  int bytes_read = (int)fread(inbuf, 1, H264_INBUF_SIZE, fp);

  if(bytes_read) {
    std::copy(inbuf, inbuf + bytes_read, std::back_inserter(buffer));
  }

  return bytes_read;
}

bool H264_Decoder::update(bool& needsMoreBytes) {

  needsMoreBytes = false;

  if(!fp) {
    printf("Cannot update .. file not opened...\n");
    return false;
  }

  if(buffer.size() == 0) {
    needsMoreBytes = true;
    return false;
  }

  uint8_t* data = NULL;
  int size = 0;
  int len = av_parser_parse2(parser, codec_context, &data, &size, &buffer[0], buffer.size(), 0, 0, AV_NOPTS_VALUE);

  if(size == 0 && len >= 0) {
    needsMoreBytes = true;
    return false;
  }

  if(len) {
    decodeFrame(&buffer[0], size);
    buffer.erase(buffer.begin(), buffer.begin() + len);
    return true;
  }

  return false;
}
