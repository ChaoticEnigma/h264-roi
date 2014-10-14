/*

--------------------------------------------------------------------------------

                                               oooo
                                               `888
                oooo d8b  .ooooo.  oooo    ooo  888  oooo  oooo
                `888""8P d88' `88b  `88b..8P'   888  `888  `888
                 888     888   888    Y888'     888   888   888
                 888     888   888  .o8"'88b    888   888   888
                d888b    `Y8bod8P' o88'   888o o888o  `V88V"V8P'

                                                  www.roxlu.com
                                             www.apollomedia.nl

--------------------------------------------------------------------------------


  Tiny library with a couple of handy functions for opengl based applications.
  Sent changes to: https://gist.github.com/roxlu/7788294

  Usage:
  ------
  #define ROXLU_USE_ALL              - to include all code
  #define ROXLU_USE_OPENGL           - to use the opengl code
  #define ROXLU_USE_PNG              - to use the png loader and saver
  #define ROXLU_USE_MATH             - to use the vec2, vec3, vec4, mat4 classes


  SHADER - define `ROXLU_USE_OPENGL` before including
  ===================================================================================
  vert = rx_create_shader(GL_VERTEX_SHADER, source_char_p);    - create a shader, pass type
  prog = rx_create_program(vert, frag);                        - create a problem - DOES NOT LINK



  IMAGES - define `ROXLU_USE_PNG` before including
  ===================================================================================
  rx_save_png("filename.png", pixels, 640, 480, 3);            - writes a png using lib png
  rx_load_png("filepath.png", &pix, width, height, nchannels)  - load the pixels, width, height and nchannels for the given filepath. make sure to delete pix (which is unsigned char*)



  UTILS
  ===================================================================================
  std::string path = rx_get_exe_path();                  - returns the path to the exe
  std::string contents = rx_read_file("filepath.txt");   - returns the contents of the filepath.




  MATH - define `ROXLU_USE_MATH` before including.
  ===================================================================================

       vec2, vec3, vec4
       ------------------------------------------------------------------------------
       float length(v)         - get the length of the vector
       float dot(a, b)         - get the dot product aka squared root
       vec2 max(a)             - get the biggest component value
       vec2 min(a)             - get the lowest component value
       vec2 max(a,b)           - get the biggest vector
       vec2 nmin(a,b)          - get the smallest vector
       vec2 floor(a)           - floor the components
       vec2 ceil(a)            - ceil the compoments
       vec2 fract(a)           - get the decimal part
       vec2 normalized(v)      - get the normalized vector
       void print()            - print the x and y
       vec3 cross(a,b)         - cross product (vec3)

       mat4
       ------------------------------------------------------------------------------
       mat4& mat4.transpose()
       mat4& mat4.rotateX(degrees)
       mat4& mat4.rotateY(degrees)
       mat4& mat4.rotateZ(degrees)
       mat4& mat4.rotate(degrees, x, y, z)
       mat4& mat4.rotate(degrees, vec3)
       mat4& mat4.scale(x, y, z)
       mat4& mat4.scale(vec3 v)
       mat4& mat4.translate(x, y, z)
       mat4& mat4.translate(vec3 v)
       mat4& mat4.ortho(l, r, b, t, n , f)
       mat4& mat4.frustum(l, r, b, t, n, f)
       mat4& mat4.perspective(fov, aspect, near, far)
       mat4& mat4.invert()
       void  mat4.print()

  Resources:
  ----------
  Math code mostly from: https://github.com/evanw/gl4/blob/master/gl4.cpp

 */

#ifndef ROXLU_TINYLIB_H
#define ROXLU_TINYLIB_H

#if defined(ROXLU_USE_ALL)
#  define ROXLU_USE_OPENGL
#  define ROXLU_USE_PNG
#  define ROXLU_USE_MATH
#endif

#include <cmath>
#include <iostream>
#include <assert.h>
#include <iterator>
#include <algorithm>
#include <string>
#include <fstream>
#include <math.h>

#if defined(__APPLE__)
#  if !defined(__gl_h_) && defined(ROXLU_USE_OPENGL)
#    include <OpenGL/gl3.h>
#    include <OpenGL/glext.h>
#  endif
#  include <libgen.h> /* dirname */
#  include <CoreFoundation/CFRunLoop.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <mach-o/dyld.h> /* _NSGetExecutablePath */
#  include <sys/resource.h>
#  include <sys/sysctl.h>
#  include <sys/stat.h> /* stat() */
#  include <unistd.h>  /* sysconf */
#else
#  include <GLXW/glxw.h>
#endif

#if defined(ROXLU_USE_PNG)
#include <png.h>
#endif

#ifndef PI
#define PI 3.14159265358979323846
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef TWO_PI
#define TWO_PI 6.28318530717958647693
#endif

#ifndef M_TWO_PI
#define M_TWO_PI 6.28318530717958647693
#endif

#ifndef FOUR_PI
#define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
#define HALF_PI 1.57079632679489661923
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD (PI/180.0)
#endif

#ifndef RAD_TO_DEG
#define RAD_TO_DEG (180.0/PI)
#endif

#ifndef MIN
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef MAX
#define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef CLAMP
#define CLAMP(val,min,max) (MAX(MIN(val,max),min))
#endif

#ifndef ABS
#define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

#ifndef DX
#define DX(i,j,w)((j)*(w))+(i)
#endif

#if defined(ROXLU_USE_OPENGL)
  static void rx_print_shader_link_info(GLuint shader) {
    GLint status = 0;
    GLint count = 0;
    GLchar* error = NULL;
    GLsizei nchars = 0;
    glGetProgramiv(shader, GL_LINK_STATUS, &status);
    if(!status) {
      glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &count);
      if(count > 0) {
        error = new GLchar[count];
        glGetProgramInfoLog(shader, 2048, &nchars, error);
        printf("------\n");
        printf("%s\n", error);
        printf("------\n");
        delete[] error;
        error = NULL;
        assert(0);
      }
    }
  }

  static void rx_print_shader_compile_info(GLuint shader) {
    GLint status = 0;
    GLint count = 0;
    GLchar* error = NULL;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if(!status) {
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &count);
      if(count > 0) {
        error = new GLchar[count];
        glGetShaderInfoLog(shader, count, NULL, error);
        printf("------\n");
        printf("%s\n", error);
        printf("------\n");
        delete[] error;
        error = NULL;
        assert(0);
      }
    }
  }

  static GLuint rx_create_program(GLuint vert, GLuint frag) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);
    return prog;
  }

  static GLuint rx_create_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src,  NULL);
    glCompileShader(s);
    rx_print_shader_compile_info(s);
    return s;
  }
#endif // ROXLU_USE_OPENGL

// UTILS
// ---------------------------------------------------------------------------
#if defined(_WIN32) // rx_get_exe_path()
//static std::string rx_get_exe_path() {
//  char buffer[MAX_PATH];

//  // Try to get the executable path with a buffer of MAX_PATH characters.
//  DWORD result = ::GetModuleFileNameA(nullptr, buffer, static_cast<DWORD>(MAX_PATH));
//  if(result == 0) {
//    return "";
//  }

//  std::string::size_type pos = std::string(buffer).find_last_of( "\\/" );

//  return std::string(buffer).substr(0, pos) +"\\";
//}
#elif defined(__APPLE__) // rx_get_exe_path()
static std::string rx_get_exe_path() {
  char buffer[1024];
  uint32_t usize = sizeof(buffer);;

  int result;
  char* path;
  char* fullpath;

  result = _NSGetExecutablePath(buffer, &usize);
  if (result) {
    return "";
  }

  path = (char*)malloc(2 * PATH_MAX);
  fullpath = realpath(buffer, path);

  if (fullpath == NULL) {
    free(path);
    return "";
  }
  strncpy(buffer, fullpath, usize);

  const char* dn = dirname(buffer);
  usize = strlen(dn);
  std::string ret(dn, usize) ;
  ret.push_back('/');

  free(path);
  return ret;
}
#elif defined(__linux) // rx_get_exe_path()
static std::string rx_get_exe_path() {
  char buffer[MAX_PATH];
  size_t size = MAX_PATH;
  ssize_t n = readlink("/proc/self/exe", buffer, size - 1);
  if (n <= 0) {
    return "";
  }
  buffer[n] = '\0';


  const char* dn = dirname(buffer);
  size = strlen(dn);
  std::string ret(dn, size) ;
  ret.push_back('/');
  return ret;
}
#endif // rx_get_exe_path()

#if defined(ROXLU_USE_PNG)
  // write an w*h array of pixels to a png file
  static bool rx_save_png(std::string filepath, unsigned char* pixels, int w, int h, int channels = 3) {

    if(!w || !h) {
      printf("error: cannot save png because the given width and height are invalid: %d x %d\n", w, h);
      return false;
    }

    if(!channels || channels > 4) {
      printf("error: cannot save png because the number of color channels is invalid: %d\n", channels);
      return false;
    }

    if(!pixels) {
      printf("error: cannot save png because we got an invalid pixels array: %p\n", pixels);
      return false;
    }

    if(!filepath.size()) {
      printf("error: cannot save png because the given filepath is invalid.\n");
      return false;
    }

    png_structp png_ptr;
    png_infop info_ptr;

    FILE* fp = fopen(filepath.c_str(), "wb");
    if(!fp) {
      printf("error: canont save png because we cannot open the filepath: %s\n", filepath.c_str());
      fp = NULL;
      return false;
    }

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(png_ptr == NULL) {
      printf("error: cannot save png because we cannot create a png write struct.\n");
      fclose(fp);
      fp = NULL;
      return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(info_ptr == NULL) {
      printf("error: cannot save png brecause we cannot create a png info struct.\n");
      fclose(fp);
      fp = NULL;
      return false;
    }

    if(setjmp(png_jmpbuf(png_ptr))) {
      printf("error: cannot save png because we cannot set the jump pointer.\n");
      png_destroy_write_struct(&png_ptr, &info_ptr);
      fclose(fp);
      fp = NULL;
      return false;
    }

    png_uint_32 color_type;
    switch(channels) {
      case 1: {
        color_type = PNG_COLOR_TYPE_GRAY;
        break;
      }
      case 3: {
        color_type = PNG_COLOR_TYPE_RGB;
        break;
      }
      case 4: {
        color_type = PNG_COLOR_TYPE_RGB_ALPHA;
        break;
      }
      default: {
        printf("error: cannot save png because we cannot detect the color type based on the number of channels.\n");
        fclose(fp);
        fp = NULL;
        return false;
      }
    };

    png_set_IHDR(png_ptr,
                 info_ptr,
                 w,
                 h,
                 8,
                 color_type,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);

    png_bytep* row_ptrs = new png_bytep[h];
    for(size_t j = 0; j < h; ++j) {
      row_ptrs[j] = pixels + (j * (w * channels));
    }

    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_ptrs);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_destroy_write_struct(&png_ptr, &info_ptr);

    delete[] row_ptrs;

    fclose(fp);

    return true;

  }

  static bool rx_load_png(std::string filepath,
                          unsigned char** pixels,
                          uint32_t& width,
                          uint32_t& height,
                          uint32_t& nchannels
  )
  {
    png_structp png_ptr;
    png_infop info_ptr;

    FILE* fp = fopen(filepath.c_str(), "rb");
    if(!fp) {
      printf("Error: cannot load the png file: %s\n", filepath.c_str());
      fp = NULL;
      return false;
    }

    unsigned char sig[8];
    size_t r = 0;
    r = fread(sig, 1, 8, fp);
    if(r != 8) {
      printf("Error: invalid png signature (not enough bytes read) in: %s.\n", filepath.c_str());
      fclose(fp);
      fp = NULL;
      return  false;
    }

    if(!png_check_sig(sig, 8)) {
      printf("Error: invalid png signature (wrong siganture) in: %s.\n", filepath.c_str());
      fclose(fp);
      fp = NULL;
      return false;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if(!png_ptr) {
      printf("Error: cannot create png read struct: %s\n", filepath.c_str());
      fclose(fp);
      fp = NULL;
      return false;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if(!info_ptr) {
      png_destroy_read_struct(&png_ptr, NULL, NULL);
      printf("Error: cannot create png info struct for: %s\n", filepath.c_str());
      fclose(fp);
      fp = NULL;
      return false;
    }

    #if !defined(_WIN32)
    if(setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(fp);
      fp = NULL;
      return false;
    }
    #endif

    // @TODO - add option to rescale to 8bit color info or 16bit
    // @TODO - add option to strip the alpha (not recommended in the example)
    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    uint32_t stride = 0;
    uint32_t num_bytes = 0;
    uint32_t bit_depth = png_get_bit_depth(png_ptr, info_ptr);
    uint32_t color_type = png_get_color_type(png_ptr, info_ptr);
    width = png_get_image_width(png_ptr, info_ptr);
    height = png_get_image_height(png_ptr, info_ptr);
    nchannels = png_get_channels(png_ptr, info_ptr);

    if(width == 0 || height == 0) {
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(fp);
      fp = NULL;
      return false;
    }

    // @TODO - add option to allow input colors/gray values to be not converted
    switch(color_type) {
      case PNG_COLOR_TYPE_PALETTE: {
        png_set_palette_to_rgb(png_ptr);
        nchannels = 3;
        break;
      }
      case PNG_COLOR_TYPE_GRAY: {
        if(bit_depth < 8) {
          png_set_expand_gray_1_2_4_to_8(png_ptr);
          bit_depth = 8;
        }
        break;
      }
      default:break;
    };

    // When transparency is set convert it to a full alpha channel
    if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
      png_set_tRNS_to_alpha(png_ptr);
      nchannels += 1;
    }

    stride = width * bit_depth * nchannels / 8;
    num_bytes = width * height * bit_depth * nchannels / 8;

    *pixels = new unsigned char[num_bytes];
    if(!pixels) {
      printf("Error: image is to big: %s\n", filepath.c_str());
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(fp);
      fp = NULL;
      pixels = NULL;
      return false;
    }

    png_bytep* row_ptrs = new png_bytep[height];
    if(!row_ptrs) {
      printf("Error: image is to big: %s\n", filepath.c_str());
      png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
      fclose(fp);
      fp = NULL;
      delete[] *pixels;
      pixels = 0;
      return false;
    }

    for(size_t i = 0; i < height; ++i) {
      row_ptrs[i] = (png_bytep)(*pixels) +(i * stride);
    }

    png_read_image(png_ptr, row_ptrs);

    delete[] row_ptrs;
    row_ptrs = NULL;
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    fclose(fp);
    return true;
  }

  static std::string rx_read_file(std::string filepath) {
    std::ifstream ifs(filepath.c_str(), std::ios::in);
    if(!ifs.is_open()) {
      return "";
    }
    std::string str((std::istreambuf_iterator<char>(ifs)) , std::istreambuf_iterator<char>());
    return str;
  }
#endif // ROXLU_USE_PNG

// MATH
// -------------------------------------------------------------------------
#if defined(ROXLU_USE_MATH)
  struct vec2 {
    union {
      struct { float x, y; };
      struct { float s, t; };
      struct { float r, g; };
      float xy[2];
      float st[2];
      float rg[2];
    };

    vec2() : x(), y() {}
    vec2(float x, float y) : x(x), y(y) {}
    vec2(const vec2 &xy) : x(xy.x), y(xy.y) {}
    explicit vec2(float f) : x(f), y(f) {}

    vec2 operator + () const { return vec2(+x, +y); }
    vec2 operator - () const { return vec2(-x, -y); }
    float& operator[](const unsigned int dx) { return xy[dx]; }

    vec2 operator + (const vec2 &vec) const { return vec2(x + vec.x, y + vec.y); }
    vec2 operator - (const vec2 &vec) const { return vec2(x - vec.x, y - vec.y); }
    vec2 operator * (const vec2 &vec) const { return vec2(x * vec.x, y * vec.y); }
    vec2 operator / (const vec2 &vec) const { return vec2(x / vec.x, y / vec.y); }
    vec2 operator + (float s) const { return vec2(x + s, y + s); }
    vec2 operator - (float s) const { return vec2(x - s, y - s); }
    vec2 operator * (float s) const { return vec2(x * s, y * s); }
    vec2 operator / (float s) const { return vec2(x / s, y / s); }

    friend vec2 operator + (float s, const vec2 &vec) { return vec2(s + vec.x, s + vec.y); }
    friend vec2 operator - (float s, const vec2 &vec) { return vec2(s - vec.x, s - vec.y); }
    friend vec2 operator * (float s, const vec2 &vec) { return vec2(s * vec.x, s * vec.y); }
    friend vec2 operator / (float s, const vec2 &vec) { return vec2(s / vec.x, s / vec.y); }

    vec2 &operator += (const vec2 &vec) { return *this = *this + vec; }
    vec2 &operator -= (const vec2 &vec) { return *this = *this - vec; }
    vec2 &operator *= (const vec2 &vec) { return *this = *this * vec; }
    vec2 &operator /= (const vec2 &vec) { return *this = *this / vec; }
    vec2 &operator += (float s) { return *this = *this + s; }
    vec2 &operator -= (float s) { return *this = *this - s; }
    vec2 &operator *= (float s) { return *this = *this * s; }
    vec2 &operator /= (float s) { return *this = *this / s; }

    bool operator == (const vec2 &vec) const { return x == vec.x && y == vec.y; }
    bool operator != (const vec2 &vec) const { return x != vec.x || y != vec.y; }

    friend float length(const vec2 &v) { return sqrtf(v.x * v.x + v.y * v.y); }
    friend float dot(const vec2 &a, const vec2 &b) { return a.x * b.x + a.y * b.y; }
    friend float max(const vec2 &v) { return fmaxf(v.x, v.y); }
    friend float min(const vec2 &v) { return fminf(v.x, v.y); }
    friend vec2 max(const vec2 &a, const vec2 &b) { return vec2(fmaxf(a.x, b.x), fmaxf(a.y, b.y)); }
    friend vec2 min(const vec2 &a, const vec2 &b) { return vec2(fminf(a.x, b.x), fminf(a.y, b.y)); }
    friend vec2 floor(const vec2 &v) { return vec2(floorf(v.x), floorf(v.y)); }
    friend vec2 ceil(const vec2 &v) { return vec2(ceilf(v.x), ceilf(v.y)); }
    friend vec2 abs(const vec2 &v) { return vec2(fabsf(v.x), fabsf(v.y)); }
    friend vec2 fract(const vec2 &v) { return v - floor(v); }
    friend vec2 normalized(const vec2 &v) { return v / length(v); }

    friend std::ostream &operator << (std::ostream &out, const vec2 &v) {
      return out << "vec2(" << v.x << ", " << v.y << ")";
    }

    void print() { printf("vec2(%f, %f)\n", x, y); }
  };

  struct vec3 {
    union {
      struct { float x, y, z; };
      struct { float s, t, p; };
      struct { float r, g, b; };
      float xyz[3];
      float stp[3];
      float rgb[3];
    };

    vec3() : x(), y(), z() {}
    vec3(float x, float y, float z) : x(x), y(y), z(z) {}
    vec3(const vec2 &xy, float z) : x(xy.x), y(xy.y), z(z) {}
    vec3(float x, const vec2 &yz) : x(x), y(yz.x), z(yz.y) {}
    vec3(const vec3 &xyz) : x(xyz.x), y(xyz.y), z(xyz.z) {}
    explicit vec3(float f) : x(f), y(f), z(f) {}

    vec3 operator + () const { return vec3(+x, +y, +z); }
    vec3 operator - () const { return vec3(-x, -y, -z); }
    float& operator [](const unsigned int dx) { return xyz[dx]; }

    vec3 operator + (const vec3 &vec) const { return vec3(x + vec.x, y + vec.y, z + vec.z); }
    vec3 operator - (const vec3 &vec) const { return vec3(x - vec.x, y - vec.y, z - vec.z); }
    vec3 operator * (const vec3 &vec) const { return vec3(x * vec.x, y * vec.y, z * vec.z); }
    vec3 operator / (const vec3 &vec) const { return vec3(x / vec.x, y / vec.y, z / vec.z); }
    vec3 operator + (float s) const { return vec3(x + s, y + s, z + s); }
    vec3 operator - (float s) const { return vec3(x - s, y - s, z - s); }
    vec3 operator * (float s) const { return vec3(x * s, y * s, z * s); }
    vec3 operator / (float s) const { return vec3(x / s, y / s, z / s); }

    friend vec3 operator + (float s, const vec3 &vec) { return vec3(s + vec.x, s + vec.y, s + vec.z); }
    friend vec3 operator - (float s, const vec3 &vec) { return vec3(s - vec.x, s - vec.y, s - vec.z); }
    friend vec3 operator * (float s, const vec3 &vec) { return vec3(s * vec.x, s * vec.y, s * vec.z); }
    friend vec3 operator / (float s, const vec3 &vec) { return vec3(s / vec.x, s / vec.y, s / vec.z); }

    vec3 &operator += (const vec3 &vec) { return *this = *this + vec; }
    vec3 &operator -= (const vec3 &vec) { return *this = *this - vec; }
    vec3 &operator *= (const vec3 &vec) { return *this = *this * vec; }
    vec3 &operator /= (const vec3 &vec) { return *this = *this / vec; }
    vec3 &operator += (float s) { return *this = *this + s; }
    vec3 &operator -= (float s) { return *this = *this - s; }
    vec3 &operator *= (float s) { return *this = *this * s; }
    vec3 &operator /= (float s) { return *this = *this / s; }

    bool operator == (const vec3 &vec) const { return x == vec.x && y == vec.y && z == vec.z; }
    bool operator != (const vec3 &vec) const { return x != vec.x || y != vec.y || z != vec.z; }

    friend float length(const vec3 &v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z); }
    friend float dot(const vec3 &a, const vec3 &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
    friend float max(const vec3 &v) { return fmaxf(fmaxf(v.x, v.y), v.z); }
    friend float min(const vec3 &v) { return fminf(fminf(v.x, v.y), v.z); }
    friend vec3 max(const vec3 &a, const vec3 &b) { return vec3(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)); }
    friend vec3 min(const vec3 &a, const vec3 &b) { return vec3(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)); }
    friend vec3 floor(const vec3 &v) { return vec3(floorf(v.x), floorf(v.y), floorf(v.z)); }
    friend vec3 ceil(const vec3 &v) { return vec3(ceilf(v.x), ceilf(v.y), ceilf(v.z)); }
    friend vec3 abs(const vec3 &v) { return vec3(fabsf(v.x), fabsf(v.y), fabsf(v.z)); }
    friend vec3 fract(const vec3 &v) { return v - floor(v); }
    friend vec3 normalized(const vec3 &v) { return v / length(v); }
    friend vec3 cross(const vec3 &a, const vec3 &b) { return vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }

    friend std::ostream &operator << (std::ostream &out, const vec3 &v) {
      return out << "vec3(" << v.x << ", " << v.y << ", " << v.z << ")";
    }

    void print() { printf("vec3(%f, %f, %f)\n", x, y, z); }

  };

  struct vec4 {
    union {
      struct { float x, y, z, w; };
      struct { float s, t, p, q; };
      struct { float r, g, b, a; };
      float xyzw[4];
      float stpq[4];
      float rgba[4];
    };

    vec4() : x(), y(), z(), w() {}
    vec4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
    vec4(const vec2 &xy, float z, float w) : x(xy.x), y(xy.y), z(z), w(w) {}
    vec4(float x, const vec2 &yz, float w) : x(x), y(yz.x), z(yz.y), w(w) {}
    vec4(float x, float y, const vec2 &zw) : x(x), y(y), z(zw.x), w(zw.y) {}
    vec4(const vec2 &xy, const vec2 &zw) : x(xy.x), y(xy.y), z(zw.x), w(zw.y) {}
    vec4(const vec3 &xyz, float w) : x(xyz.x), y(xyz.y), z(xyz.z), w(w) {}
    vec4(float x, const vec3 &yzw) : x(x), y(yzw.x), z(yzw.y), w(yzw.z) {}
    vec4(const vec4 &xyzw) : x(xyzw.x), y(xyzw.y), z(xyzw.z), w(xyzw.w) {}
    explicit vec4(float f) : x(f), y(f), z(f), w(f) {}

    vec4 operator + () const { return vec4(+x, +y, +z, +w); }
    vec4 operator - () const { return vec4(-x, -y, -z, -w); }

    vec4 operator + (const vec4 &vec) const { return vec4(x + vec.x, y + vec.y, z + vec.z, w + vec.w); }
    vec4 operator - (const vec4 &vec) const { return vec4(x - vec.x, y - vec.y, z - vec.z, w - vec.w); }
    vec4 operator * (const vec4 &vec) const { return vec4(x * vec.x, y * vec.y, z * vec.z, w * vec.w); }
    vec4 operator / (const vec4 &vec) const { return vec4(x / vec.x, y / vec.y, z / vec.z, w / vec.w); }
    vec4 operator + (float s) const { return vec4(x + s, y + s, z + s, w + s); }
    vec4 operator - (float s) const { return vec4(x - s, y - s, z - s, w - s); }
    vec4 operator * (float s) const { return vec4(x * s, y * s, z * s, w * s); }
    vec4 operator / (float s) const { return vec4(x / s, y / s, z / s, w / s); }

    friend vec4 operator + (float s, const vec4 &vec) { return vec4(s + vec.x, s + vec.y, s + vec.z, s + vec.w); }
    friend vec4 operator - (float s, const vec4 &vec) { return vec4(s - vec.x, s - vec.y, s - vec.z, s - vec.w); }
    friend vec4 operator * (float s, const vec4 &vec) { return vec4(s * vec.x, s * vec.y, s * vec.z, s * vec.w); }
    friend vec4 operator / (float s, const vec4 &vec) { return vec4(s / vec.x, s / vec.y, s / vec.z, s / vec.w); }

    vec4 &operator += (const vec4 &vec) { return *this = *this + vec; }
    vec4 &operator -= (const vec4 &vec) { return *this = *this - vec; }
    vec4 &operator *= (const vec4 &vec) { return *this = *this * vec; }
    vec4 &operator /= (const vec4 &vec) { return *this = *this / vec; }
    vec4 &operator += (float s) { return *this = *this + s; }
    vec4 &operator -= (float s) { return *this = *this - s; }
    vec4 &operator *= (float s) { return *this = *this * s; }
    vec4 &operator /= (float s) { return *this = *this / s; }

    bool operator == (const vec4 &vec) const { return x == vec.x && y == vec.y && z == vec.z && w == vec.w; }
    bool operator != (const vec4 &vec) const { return x != vec.x || y != vec.y || z != vec.z || w != vec.w; }

    friend float length(const vec4 &v) { return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w); }
    friend float dot(const vec4 &a, const vec4 &b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * a.w; }
    friend float max(const vec4 &v) { return fmaxf(fmaxf(v.x, v.y), fmaxf(v.z, v.w)); }
    friend float min(const vec4 &v) { return fminf(fminf(v.x, v.y), fminf(v.z, v.w)); }
    friend vec4 max(const vec4 &a, const vec4 &b) { return vec4(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z), fmaxf(a.w, b.w)); }
    friend vec4 min(const vec4 &a, const vec4 &b) { return vec4(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z), fminf(a.w, b.w)); }
    friend vec4 floor(const vec4 &v) { return vec4(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w)); }
    friend vec4 ceil(const vec4 &v) { return vec4(ceilf(v.x), ceilf(v.y), ceilf(v.z), ceilf(v.w)); }
    friend vec4 abs(const vec4 &v) { return vec4(fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w)); }
    friend vec4 fract(const vec4 &v) { return v - floor(v); }
    friend vec4 normalized(const vec4 &v) { return v / length(v); }

    friend std::ostream &operator << (std::ostream &out, const vec4 &v) {
      return out << "vec4(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    }
  };

  struct mat4 {
    union {
      struct {
        float m00, m01, m02, m03;
        float m10, m11, m12, m13;
        float m20, m21, m22, m23;
        float m30, m31, m32, m33;
      };
      float m[16];
    };

  mat4() :
      m00(1), m01(), m02(), m03(),
      m10(), m11(1), m12(), m13(),
      m20(), m21(), m22(1), m23(),
      m30(), m31(), m32(), m33(1) {}
  mat4(const vec4 &r0, const vec4 &r1, const vec4 &r2, const vec4 &r3) :
      m00(r0.x), m01(r0.y), m02(r0.z), m03(r0.w),
      m10(r1.x), m11(r1.y), m12(r1.z), m13(r1.w),
      m20(r2.x), m21(r2.y), m22(r2.z), m23(r2.w),
      m30(r3.x), m31(r3.y), m32(r3.z), m33(r3.w) {}
  mat4(
    float m00, float m01, float m02, float m03,
    float m10, float m11, float m12, float m13,
    float m20, float m21, float m22, float m23,
    float m30, float m31, float m32, float m33) :
    m00(m00), m01(m01), m02(m02), m03(m03),
    m10(m10), m11(m11), m12(m12), m13(m13),
    m20(m20), m21(m21), m22(m22), m23(m23),
    m30(m30), m31(m31), m32(m32), m33(m33) {}

    mat4 &transpose();

    mat4 &rotateX(float degrees);
    mat4 &rotateY(float degrees);
    mat4 &rotateZ(float degrees);
    mat4 &rotate(float degrees, float x, float y, float z);
    mat4 &rotate(float degrees, const vec3 &v) { return rotate(degrees, v.x, v.y, v.z); }
    mat4 &scale(float x, float y, float z);
    mat4 &scale(const vec3 &v) { return scale(v.x, v.y, v.z); }
    mat4 &translate(float x, float y, float z);
    mat4 &translate(const vec3 &v) { return translate(v.x, v.y, v.z); }
    mat4 &ortho(float l, float r, float b, float t, float n, float f);
    mat4 &frustum(float l, float r, float b, float t, float n, float f);
    mat4 &perspective(float fov, float aspect, float near, float far);
    mat4 &invert();

    float& operator[](const unsigned int dx);
    mat4 &operator *= (const mat4 &t);
    vec4 operator * (const vec4 &v);
    friend vec4 operator * (const vec4 &v, const mat4 &t);
    friend std::ostream &operator << (std::ostream &out, const mat4 &t);
    void print();
  };

  inline float& mat4::operator[](const unsigned int dx) {
    return m[dx];
  }

  inline mat4 &mat4::transpose() {
    std::swap(m01, m10); std::swap(m02, m20); std::swap(m03, m30);
    std::swap(m12, m21); std::swap(m13, m31); std::swap(m23, m32);
    return *this;
  }

  inline mat4 &mat4::rotateX(float degrees) {
    float radians = degrees * (M_PI / 180);
    float s = sinf(radians), c = cosf(radians);
    float t01 = m01, t02 = m02;
    float t11 = m11, t12 = m12;
    float t21 = m21, t22 = m22;
    float t31 = m31, t32 = m32;
    m01 = c * t01 - s * t02;
    m02 = c * t02 + s * t01;
    m11 = c * t11 - s * t12;
    m12 = c * t12 + s * t11;
    m21 = c * t21 - s * t22;
    m22 = c * t22 + s * t21;
    m31 = c * t31 - s * t32;
    m32 = c * t32 + s * t31;
    return *this;
  }

  inline mat4 &mat4::rotateY(float degrees) {
    float radians = degrees * (M_PI / 180);
    float s = sinf(radians), c = cosf(radians);
    float t02 = m02, t00 = m00;
    float t12 = m12, t10 = m10;
    float t22 = m22, t20 = m20;
    float t32 = m32, t30 = m30;
    m02 = c * t02 - s * t00;
    m00 = c * t00 + s * t02;
    m12 = c * t12 - s * t10;
    m10 = c * t10 + s * t12;
    m22 = c * t22 - s * t20;
    m20 = c * t20 + s * t22;
    m32 = c * t32 - s * t30;
    m30 = c * t30 + s * t32;
    return *this;
  }

  inline mat4 &mat4::rotateZ(float degrees) {
    float radians = degrees * (M_PI / 180);
    float s = sinf(radians), c = cosf(radians);
    float t00 = m00, t01 = m01;
    float t10 = m10, t11 = m11;
    float t20 = m20, t21 = m21;
    float t30 = m30, t31 = m31;
    m00 = c * t00 - s * t01;
    m01 = c * t01 + s * t00;
    m10 = c * t10 - s * t11;
    m11 = c * t11 + s * t10;
    m20 = c * t20 - s * t21;
    m21 = c * t21 + s * t20;
    m30 = c * t30 - s * t31;
    m31 = c * t31 + s * t30;
    return *this;
  }

  inline mat4 &mat4::rotate(float degrees, float x, float y, float z) {
    float radians = degrees * (M_PI / 180);
    float d = sqrtf(x*x + y*y + z*z);
    float s = sinf(radians), c = cosf(radians), t = 1 - c;
    x /= d; y /= d; z /= d;
    return *this *= mat4(
      x*x*t + c, x*y*t - z*s, x*z*t + y*s, 0,
      y*x*t + z*s, y*y*t + c, y*z*t - x*s, 0,
      z*x*t - y*s, z*y*t + x*s, z*z*t + c, 0,
      0, 0, 0, 1);
  }

  inline mat4 &mat4::scale(float x, float y, float z) {
    m00 *= x; m01 *= y; m02 *= z;
    m10 *= x; m11 *= y; m12 *= z;
    m20 *= x; m21 *= y; m22 *= z;
    m30 *= x; m31 *= y; m32 *= z;
    return *this;
  }

  inline mat4 &mat4::translate(float x, float y, float z) {
    m03 += m00 * x + m01 * y + m02 * z;
    m13 += m10 * x + m11 * y + m12 * z;
    m23 += m20 * x + m21 * y + m22 * z;
    m33 += m30 * x + m31 * y + m32 * z;
    return *this;
  }

  inline mat4 &mat4::ortho(float l, float r, float b, float t, float n, float f) {
    return *this *= mat4(
      2 / (r - l), 0, 0, (r + l) / (l - r),
      0, 2 / (t - b), 0, (t + b) / (b - t),
      0, 0, 2 / (n - f), (f + n) / (n - f),
      0, 0, 0, 1);
  }

  inline mat4 &mat4::frustum(float l, float r, float b, float t, float n, float f) {
    return *this *= mat4(
      2 * n / (r - l), 0, (r + l) / (r - l), 0,
      0, 2 * n / (t - b), (t + b) / (t - b), 0,
      0, 0, (f + n) / (n - f), 2 * f * n / (n - f),
      0, 0, -1, 0);
  }

  inline mat4 &mat4::perspective(float fov, float aspect, float near, float far) {
    float y = tanf(fov * M_PI / 360) * near, x = y * aspect;
    return frustum(-x, x, -y, y, near, far);
  }

  inline mat4 &mat4::invert() {
    float t00 = m00, t01 = m01, t02 = m02, t03 = m03;
    *this = mat4(
      m11*m22*m33 - m11*m32*m23 - m12*m21*m33 + m12*m31*m23 + m13*m21*m32 - m13*m31*m22,
      -m01*m22*m33 + m01*m32*m23 + m02*m21*m33 - m02*m31*m23 - m03*m21*m32 + m03*m31*m22,
      m01*m12*m33 - m01*m32*m13 - m02*m11*m33 + m02*m31*m13 + m03*m11*m32 - m03*m31*m12,
      -m01*m12*m23 + m01*m22*m13 + m02*m11*m23 - m02*m21*m13 - m03*m11*m22 + m03*m21*m12,

      -m10*m22*m33 + m10*m32*m23 + m12*m20*m33 - m12*m30*m23 - m13*m20*m32 + m13*m30*m22,
      m00*m22*m33 - m00*m32*m23 - m02*m20*m33 + m02*m30*m23 + m03*m20*m32 - m03*m30*m22,
      -m00*m12*m33 + m00*m32*m13 + m02*m10*m33 - m02*m30*m13 - m03*m10*m32 + m03*m30*m12,
      m00*m12*m23 - m00*m22*m13 - m02*m10*m23 + m02*m20*m13 + m03*m10*m22 - m03*m20*m12,

      m10*m21*m33 - m10*m31*m23 - m11*m20*m33 + m11*m30*m23 + m13*m20*m31 - m13*m30*m21,
      -m00*m21*m33 + m00*m31*m23 + m01*m20*m33 - m01*m30*m23 - m03*m20*m31 + m03*m30*m21,
      m00*m11*m33 - m00*m31*m13 - m01*m10*m33 + m01*m30*m13 + m03*m10*m31 - m03*m30*m11,
      -m00*m11*m23 + m00*m21*m13 + m01*m10*m23 - m01*m20*m13 - m03*m10*m21 + m03*m20*m11,

      -m10*m21*m32 + m10*m31*m22 + m11*m20*m32 - m11*m30*m22 - m12*m20*m31 + m12*m30*m21,
      m00*m21*m32 - m00*m31*m22 - m01*m20*m32 + m01*m30*m22 + m02*m20*m31 - m02*m30*m21,
      -m00*m11*m32 + m00*m31*m12 + m01*m10*m32 - m01*m30*m12 - m02*m10*m31 + m02*m30*m11,
        m00*m11*m22 - m00*m21*m12 - m01*m10*m22 + m01*m20*m12 + m02*m10*m21 - m02*m20*m11
    );
    float det = m00 * t00 + m10 * t01 + m20 * t02 + m30 * t03;
    for (int i = 0; i < 16; i++) m[i] /= det;
    return *this;
  }

  inline mat4 &mat4::operator *= (const mat4 &t) {
    *this = mat4(
      m00*t.m00 + m01*t.m10 + m02*t.m20 + m03*t.m30,
      m00*t.m01 + m01*t.m11 + m02*t.m21 + m03*t.m31,
      m00*t.m02 + m01*t.m12 + m02*t.m22 + m03*t.m32,
      m00*t.m03 + m01*t.m13 + m02*t.m23 + m03*t.m33,

      m10*t.m00 + m11*t.m10 + m12*t.m20 + m13*t.m30,
      m10*t.m01 + m11*t.m11 + m12*t.m21 + m13*t.m31,
      m10*t.m02 + m11*t.m12 + m12*t.m22 + m13*t.m32,
      m10*t.m03 + m11*t.m13 + m12*t.m23 + m13*t.m33,

      m20*t.m00 + m21*t.m10 + m22*t.m20 + m23*t.m30,
      m20*t.m01 + m21*t.m11 + m22*t.m21 + m23*t.m31,
      m20*t.m02 + m21*t.m12 + m22*t.m22 + m23*t.m32,
      m20*t.m03 + m21*t.m13 + m22*t.m23 + m23*t.m33,

      m30*t.m00 + m31*t.m10 + m32*t.m20 + m33*t.m30,
      m30*t.m01 + m31*t.m11 + m32*t.m21 + m33*t.m31,
      m30*t.m02 + m31*t.m12 + m32*t.m22 + m33*t.m32,
      m30*t.m03 + m31*t.m13 + m32*t.m23 + m33*t.m33
    );
    return *this;
  }

  inline vec4 mat4::operator * (const vec4 &v) {
    return vec4(
      m00*v.x + m01*v.y + m02*v.z + m03*v.w,
      m10*v.x + m11*v.y + m12*v.z + m13*v.w,
      m20*v.x + m21*v.y + m22*v.z + m23*v.w,
      m30*v.x + m31*v.y + m32*v.z + m33*v.w
    );
  }

  inline vec4 operator * (const vec4 &v, const mat4 &t) {
    return vec4(
      t.m00*v.x + t.m10*v.y + t.m20*v.z + t.m30*v.w,
      t.m01*v.x + t.m11*v.y + t.m21*v.z + t.m31*v.w,
      t.m02*v.x + t.m12*v.y + t.m22*v.z + t.m32*v.w,
      t.m03*v.x + t.m13*v.y + t.m23*v.z + t.m33*v.w
    );
  }

  inline void mat4::print() {
    printf("mat4(%f, %f, %f, %f\n     %f, %f, %f, %f\n     %f, %f, %f, %f\n     %f, %f, %f, %f)\n",
           m[0], m[4], m[8], m[12],
           m[1], m[5], m[9], m[13],
           m[2], m[6], m[10], m[14],
           m[3], m[7], m[11], m[15]);
  }

  inline std::ostream &operator << (std::ostream &out, const mat4 &t) {
    return out << "\nmat4("
        << t.m00 << ", " << t.m01 << ", " << t.m02 << ", " << t.m03 << ",\n     "
        << t.m10 << ", " << t.m11 << ", " << t.m12 << ", " << t.m13 << ",\n     "
        << t.m20 << ", " << t.m21 << ", " << t.m22 << ", " << t.m23 << ",\n     "
        << t.m30 << ", " << t.m31 << ", " << t.m32 << ", " << t.m33 << ")";
  }
#endif // ROXLU_USE_MATH

  // ----------------------------------------------------------------------------
#endif
