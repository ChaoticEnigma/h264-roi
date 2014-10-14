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
  Send changes to: https://github.com/roxlu/tinylib

  You can find more in depth documentation on [Read The Docs](http://tiny-lib.readthedocs.org/en/latest/index.html)

  TODO:
  -----
  - we're using GLXW to include GL headers now, because default windows headers are GL 1

  USAGE
  ------
  The tinylib.h file contains the function definitions and declarations. In your
  header files you only use ROXLU_USE_{ALL, OPENGL, PNG, etc..} but in your main.cpp
  file you use all the defines that you use in your app (e.g. ROXLU_USE_ALL) +
  ROXLU_IMPLEMENTATION. When you add ROXLU_IMPLEMENTATION, which is only allowed in a
  .cpp file, all the source code will be added to so your application can link with it.

  #define ROXLU_USE_ALL              - to include all code
  #define ROXLU_USE_OPENGL           - to use the opengl code
  #define ROXLU_USE_PNG              - to use the png loader and saver (needs libpng)
  #define ROXLU_USE_JPG              - add support for jpg loading
  #define ROXLU_USE_MATH             - to use the vec2, vec3, vec4, mat4 classes
  #define ROXLU_USE_FONT             - to make the PixelFont font available
  #define ROXLU_USE_AUDIO            - to use AudioPlayer for simple 44100,2-channel audio playback (need libcubeb)
  #define ROXLU_USE_CURL             - enable some curl helpers
  #define ROXLU_USE_LOG              - use the logging features


  OPENGL - define `ROXLU_USE_OPENGL` before including
  ===================================================================================
  rx_create_shader(GL_VERTEX_SHADER, source_char_p);                        - create a shader, pass type
  rx_create_shader_from_file(GL_VERTEX_SHADER, path);                       - create a shader from a file, give a full path, returns -1 on error
  rx_create_program(vert, frag, link);                                      - create a program, linkes when link == true
  rx_create_program_with_attribs(vert, frag, 2, attr);                      - create a program with attribs + LINKS THE PROGRAM
  rx_print_shader_link_info(prog)                                           - print the program link info
  rx_print_shader_compile_info(vert)                                        - print the shader compile info
  rx_create_texture(filepath)                                               - loads a png and creates a texture (only when png is enabled)
  rx_get_uniform_location(prog, name)                                       - returns the location of the uniform or returns -1 and logs something in debug mode
  rx_uniform_1i(prog, name, value)                                          - set a 1i value - make sure to activate the shader before calling, e.g. glUseProgram(prog)
  rx_uniform_1f(prog, name, value)                                          - set a 1f value - make sure to activate the shader before calling, e.g. glUseProgram(prog)
  rx_uniform_mat4fv(prog, name, count, trans, ptr)                          - set a mat4fv - make sure to activate the shader before calling, e.g. glUseProgram(prog)

  rx_create_png_screenshot(filepath)                                        - create a png file from the current framebuffer (must have PNG enabled)
  rx_create_jpg_screenshot(filepath, quality)                               - create a jpg file from the current framebuffer (must have JPG enabled)

  Shader                                                                    - represents a GL shader; only works with files!
  Shader.load(type, filepath)                                               - load the source for the shader from file
  Shader.compile()                                                          - compiles the shader

  Program                                                                   - represents a GL program
  Program.add(Shader)                                                       - add a already compiled shader to the program
  Program.create(type, filepath, extra)                                     - create a Shader for the given filepath. extra is extra source code that gets prepended.
  Program.link(nattribs, char** attribs, int nfrags, const char** frags)    - link + bind the attrib locations and/or fragment locations
  Program.recompile()                                                       - recompiles the shader and links the program again

  VertexP                                                                   - vertex type for position data
  VertexPC                                                                  - vertex type for position and color (4) data
  VertexPT                                                                  - vertex type for position and texture coord data
  VertexPT3                                                                 - vertex type for position and texture coord with 3 elements (a q for projective mapping)
  VertexPTN                                                                 - vertex type for position, texture and normal
  VertexPN                                                                  - vertex type for position and normals

  OBJ                                                                       - class to load OBJ files
  OBJ.load(filepath)                                                        - load the .obj file, returns boolean
  OBJ.hasNormals()                                                          - returns true if the loaded obj has normals
  OBJ.hasTexCoords()                                                        - returns true if the loaded obj had texcoords
  OBJ.copy(std::vector<VertexPT>&)                                          - copy the loaded vertices

  Painter                                                                   - simple helper to draw lines, circles, rectangles, textures with GL 3.x
  Painter.clear()                                                           - clear all added elements, resets the canvas
  Painter.draw()                                                            - draw the added shapes
  Painter.rect(x, y, w, h)                                                  - draw a rectangle
  Painter.circle(x, y, radius)                                              - draw a circle at x/y with radius
  Painter.resolution(6)                                                     - sets the circle resolution
  Painter.line(x0, y0, x1, y1)                                              - draw one line
  Painter.texture(texid, x, y, w, h)                                        - draw a texture
  Painter.color(r,g,b,a)                                                    - set the draw color
  Painter.fill()                                                            - draw filled shapes
  Painter.nofill()                                                          - draw only outlines
  -
  Painter.resize(w,h)                                                       - call this when the viewport size changes
  Painter.width()                                                           - return the last set or queried viewport width
  Painter.height()                                                          - return the last set of queried viewport height
  -
  Painter.begin(GL_LINE_STRIP)                                              - begin drawing a type
  Painter.vertex(x, y)                                                      - add a vertex to the "begin/end" section
  Painter.end()                                                             - flush added vertices and make sure their drawn
  -

  FONT = define `ROXLU_USE_FONT` before include
  ===================================================================================
  PixelFont font;                                                           - creates an instance of the FreePixel font.
  font.clear()                                                              - clear all previously added text (make sure to clear() when you want to draw text that changes every time)
  font.write(10, 10, "Lorem ipsum_dolor sit")                               - write some text at 10,10
  font.draw()                                                               - draws all the batched writes
  font.color(r, g, b, a)                                                    - set the font color
  font.resize(w, h)                                                         - when the viewport size changes you must call this.

  IMAGES - define `ROXLU_USE_PNG` before including -                        - see https://gist.github.com/roxlu/9b9d555cf784385d67ba for some loading examples
  ===================================================================================

  Both the jpg and png loaders can reallocate the given pixel buffer so that
  it will be big enough to load the image. For this to work you need to pass
  the current capacity of the buffer and make sure that *pixels is not NULL.

  RX_FLAG_LOAD_AS_RGBA
     You can load a png and convert it directly to 4 channels, which helps you
     when you want to upload pixels to the GPU using the optimal transfer path.
     pass the flag, RX_FLAG_LOAD_AS_RGBA

  bool rx_save_png("filename.png", pixels, 640, 480, 3, flip);                         - writes a png using lib png, set flip = true if you want to flip horizontally
  int rx_load_png("filepath.png", &pix, width, height, nchannels)                      - load the pixels, width, height and nchannels for the given filepath. make sure to delete pix (which is unsigned char*)
  int rx_load_png("filepath.png", &pix, width, height, nchannels, &allocated, flags)   - load the pixels, the allocated param should point to an integer that holds the number of bytes in the *pix buffer. It will try to reuse or reallocate this buffer. Returns number of bytes in image buffer. You can pass RX_FLAG_LOAD_AS_RGBA if you want to force the output as RGBA
  int rx_load_jpg("filepath.jpg", &pix, width, height, nchannels)                      - loads an jpg file, allocates the buffer that you need to free, will return the number of bytes (int)
  int rx_load_jpg("filepath.jpg", &pix, width, height, nchannels, &allocated)          - loads an jpg file. the allocated should point to an integer that holds the number of bytes in the *pix buffer. It will try to reuse this or reallocate the buffer if needed. this will return the number of allocated bytes
  bool rx_save_jpg("filepath.jpg", pixels, 640, 480, 3);                               - save a jpg file

  UTILS
  ===================================================================================
  rx_to_float("0.15");                                                     - convert a string to float
  rx_to_int("10");                                                         - convert a string to integer
  rx_int_to_string(15);                                                    - convert an integer to string
  rx_float_to_string(5.5);                                                 - convert a float to string
  rx_string_id(std::string& v);                                            - generate an mostly unique uint32_t id for the given string.

  rx_get_exe_path();                                                       - returns the path to the exe
  rx_read_file("filepath.txt");                                            - returns the contents of the filepath.
  rx_to_data_path("filename.txt")                                          - convert the given filename to the data dir
  rx_is_dir("path")                                                        - returns true when the path is a dir
  rx_strip_filename("/path/filename")                                      - removes the filename from the given path
  rx_strip_file_ext("/path/filename")                                      - removes the extension from the given filename, including the dot. e.g. image.jpg becomes "image"
  rx_strip_dir("/path/filename")                                           - removes the path from the given path leaving only the filename
  rx_create_dir("/path/to")                                                - creates the given directory
  rx_create_path("/a/b/c")                                                 - creates the path. all subdirectories too)
  rx_norm_path("/some/path/")                                              - creates a normalized, cross platform path. always pass in forward slashes; on windows you'll get backslashes
  rx_get_files("/path/", "jpg")                                            - returns a std::vector<std::string> with the files found in the given dir which have the given extension.
  rx_get_files("/path/", "*")                                              - returns all the files in the path.
  rx_get_file_ext("/file/path.jpg")                                        - returns the extension of a file ("jpg", "gif", etc..)

  rx_split("string", '/')                                                  - splits a string on the given character returning std::vector<std::string>
  rx_string_replace("string", "replace", "with")                           - replaces characters or string from one to another

  rx_hrtime()                                                              - high resolution timer (time in nano sec)
  rx_millis()                                                              - returns the elapsed millis since the first call as float, 1000 millis returns 1.0
  rx_strftime("%Y/%m%d/")                                                  - strftime wrapper
  rx_get_time_string()                                                     - get a unique string with the current date time: 2014.01.16_13.12_883  yyyy.mm.dd.hh.ss.iii (ii =  millis)
  rx_get_date_string()                                                     - get a unique string with the current date 2014.01.16
  rx_get_year()                                                            - get the current year as int, e.g. 2014
  rx_get_month()                                                           - get the current month as int [00-11]
  rx_get_day()                                                             - get the day of the month [00-31]
  rx_get_hour()                                                            - get the hour of day [00-23]
  rx_get_minute()                                                          - get the minuts of the hours, [00-59]

  rx_rgb_to_hsv(r,g,b,h,s,v)                                               - convert rgb in range 0-1 to hsv in the same range. h,s,v are references
  rx_rgb_to_hsv(rgb, hsv)                                                  - convert given vector, hsv will be set (reference)
  rx_rgb_to_hsv(rgb, float*)                                               - "", different typed parameters
  rx_rgb_to_hsv(float*, float*)                                            - "", ""

  rx_hsv_to_rgb(h,s,v,r,g,b)                                               - convert rgb to hsv all in 0-1 range. r,g,b will be set, are references
  rx_hsv_to_rgb(hsv, rgb)                                                  - convert rgb to hsv all in 0-1 rnage. rgb will be set, is a reference
  rx_hsv_to_rgb(hsv, float*)                                               - "", ""
  rx_hsv_to_rgb(float,* float*)                                            - "", ""


  LOG
  ===================================================================================
  rx_log_init(std::string path = "")                                       - Initialize logging, creates a log file like: log-[date-string].log in the same directory of the executable when path is not given.
  rx_log_disable_stdout()                                                  - Disable output to stdout
  rx_log_enable_stdout()                                                   - Enable output to stdout
  rx_log_set_level(int level)                                              - Set the log level, RX_LOG_LEVEL_{ALL, ERROR, WARNING, VERBOSE}
  RX_VERBOSE("%s %s", "Hello", "world");                                   - Log a verbose message
  RX_WARNING("Warning, wrong input");                                      - Log a warning
  RX_ERROR("Log an error: %s", strerror(errno));                           - Log an erorr.


  MATH - define `ROXLU_USE_MATH` before including.
  ===================================================================================

  utils
  -----------------------------------------------------------------------------------
  float rx_random(max)                                                     - generate a random value but limit to max
  float rx_random(min, max)                                                - generate a random value between min and max

  vec2, vec3, vec4
  -----------------------------------------------------------------------------------
  float length(v)                                                          - get the length of the vector
  float dot(a, b)                                                          - get the dot product aka squared root
  vec2 max(a)                                                              - get the biggest component value
  vec2 min(a)                                                              - get the lowest component value
  vec2 max(a,b)                                                            - get the biggest vector
  vec2 nmin(a,b)                                                           - get the smallest vector
  vec2 floor(a)                                                            - floor the components
  vec2 ceil(a)                                                             - ceil the compoments
  vec2 fract(a)                                                            - get the decimal part
  vec2 normalized(v)                                                       - get the normalized vector
  void print()                                                             - print the x and y
  vec3 cross(a,b)                                                          - cross product (vec3)

  vec3
  -----------------------------------------------------------------------------------
  vec3 perpendicular(a)                                                    - get a perpendicular vector from the given vec, this vector doesn't have to be normalized!, based on http://lolengine.net/blog/2013/09/21/picking-orthogonal-vector-combing-coconuts
  bool intersect(a,b,c,d, &result)                                         - checks if two lines intersect line (b-a) and (d-c). resturns true when they intersect and it will set result to the intersection point

  mat4
  -----------------------------------------------------------------------------------
  mat4& mat4.rotateX(rad)
  mat4& mat4.rotateY(rad)
  mat4& mat4.rotateZ(rad)
  mat4& mat4.rotate(rad, x, y, z)
  mat4& mat4.scale(x, y, z)
  mat4& mat4.scale(s)
  mat4& mat4.translate(x, y, z)
  mat4& mat4.translate(vec3 v)
  mat4& mat4.position(vec3 v)
  mat4& mat4.position(x, y, z)
  mat4& mat4.ortho(l, r, b, t, n , f)                                      - pm.ortho(0, w, h, 0, 0.0f, 100.0f);
  mat4& mat4.frustum(l, r, b, t, n, f)
  mat4& mat4.perspective(fov, aspect, near, far)                           - create a perspective projection matrix
  mat4& mat4.lookat(eye, pos, up)
  void  mat4.print()
  float* mat4.ptr()                                                        - get a pointer to the data


  <example>
  // create an ortho matrix with 0,0 at the top left
  mat4 m;
  m.ortho(0.0f, w, h, 0, 0.0f, 100.0f);
  </example>


  Spline<T>  - catmull rom interpolation (MAKE SURE TO USE AT LEAST 4 POINTS!)
  -----------------------------------------------------------------------------------

  Spline<T>.size()                                                       - returns the number of elements added
  Spline<T>.clear()                                                      - removes all added elements
  Spline<T>.push_back(T)                                                 - add an element
  Spline<T>.assign(begin, end)                                           - assign multiple values
  Spline<T>.at(float t)                                                  - get the interpolated value at this point

  <example>
     Spline<float> spline;
     spline.push_back(1.0);
     spline.push_back(3.0);
     spline.push_back(6.0);
     spline.push_back(5.0);

     int num = 10;
     for(int i = 0; i <= num; ++i) {
       float p = float(i)/num;
       printf("%d: %f (perc: %f)\n",i, spline.at(p), p);
     }
  </example>

  Perlin
  -----------------------------------------------------------------------------------
  Perlin noise, thanks to Ken P.
  The class is based on: http://www.flipcode.com/archives/Perlin_Noise_Class.shtml
  with some super tiny changes. Thanks guys!

  octaves: use a value between 1 - 16, 1 = smooth, 16 = noisy, values between 4 - 8 give conventional noise results
  freq:    use a value between 1 - 8, which will give reasanoble results (you can use any value you want)
  ampl:    a value of 1, will result in values between -1 and 1
  seed:    random seed, eg. 94

  Perlin(octaves, freq, amplitude, seed)                             - constructor, see values in the description above
  Perlin.get(x)                                                      - get the value for this `x` range
  Perlin.get(x, y)                                                   - 2d perlin


  CURL - define `ROXLU_USE_CURL`
  ===================================================================================
  rx_fetch_url(std::string url, std::string& result)                 - downloads an url into the given result.
  rx_download_file(std::string url, std::string filepath)            - downloads a file to the given filepath.

  AUDIO - define `ROXLU_USE_AUDIO`  (will need libcubeb: https://github.com/kinetiknz/cubeb and libsndfile)
  ===================================================================================
  AudioPlayer player;                                                - creates a audio player
  player.add(0, "bleep.wav");                                        - add the bleep.wav file (only 44100, 2 channel support atm)
  player.add(1, "boing.wav");                                        - add the boing.wav with name '1'.
  player.play(0, 1.0);                                               - playback audio file 0 with volume 1.0

*/

#if defined(ROXLU_USE_ALL)
#  define ROXLU_USE_OPENGL
#  define ROXLU_USE_PNG
#  define ROXLU_USE_MATH
#  define ROXLU_USE_JPG
#  define ROXLU_USE_FONT
#  define ROXLU_USE_CURL
#  define ROXLU_USE_AUDIO
#endif

// ------------------------------------------------------------------------------------
//                              T  I  N  Y  L  I  B
// ------------------------------------------------------------------------------------

#include <assert.h>
#include <iostream>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#if defined(_WIN32)
#  include <direct.h>                              /* _mkdir */
#  include <Shlwapi.h>
#  include <stdint.h>
#  include <time.h>
#  include <sys/stat.h>                           /* _stat() */
#  if defined(ROXLU_USE_OPENGL)
//#    include <GLXW/glxw.h>
#  endif
#elif defined(__APPLE__)
#  if defined(ROXLU_USE_OPENGL)
//#    include <OpenGL/gl3.h>
#  endif
/* For JPEG 9a, we need to fix this */
#  if defined(ROXLU_USE_JPG)
#    define TRUE 1
#    define FALSE 0
#    include <jpeglib.h>
#  endif
#  include <libgen.h>                               /* dirname */
#  include <CoreFoundation/CFRunLoop.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <mach-o/dyld.h>                          /* _NSGetExecutablePath */
#  include <sys/resource.h>
#  include <sys/sysctl.h>
#  include <sys/stat.h>                             /* stat() */
#  include <unistd.h>                               /* sysconf */
#  include <dirent.h>                               /* DIR */
#  include <errno.h>                                /* errno */
#  include <stdint.h>
#elif defined(__linux)
#  include <string.h>                               /* strlen() */
#  include <dirent.h>                               /* stat() */
#  include <errno.h>                                /* errno */
#  include <unistd.h>                               /* readlink(), getcwd() */
#  include <sys/time.h>                             /* timeofday() */
#  include <libgen.h>                               /* dirname() */
#  include <stdint.h>                               /* uint*_t types */
#  include <sys/stat.h>
#  include <stdarg.h>
#  if defined(ROXLU_USE_OPENGL)
//#     include <GLXW/glxw.h>
#  endif
#  define MAX_PATH 4096
#endif

#if defined(ROXLU_USE_PNG)
#  include <png.h>
#endif

#ifndef ROXLU_TINYLIB_H
#define ROXLU_TINYLIB_H
#ifndef PI
#  define PI 3.14159265358979323846
#endif

#ifndef TWO_PI
#  define TWO_PI 6.28318530717958647693
#endif

#ifndef M_TWO_PI
#  define M_TWO_PI 6.28318530717958647693
#endif

#ifndef FOUR_PI
#define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
#  define HALF_PI 1.57079632679489661923
#endif

#ifndef DEG_TO_RAD
#  define DEG_TO_RAD (PI/180.0)
#endif

#ifndef RAD_TO_DEG
#  define RAD_TO_DEG (180.0/PI)
#endif

#ifndef LOWEST
#  define LOWEST(x,y) (((x) < (y)) ? (x) : (y))
#endif

#ifndef HEIGHEST
#  define HEIGHEST(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef CLAMP
#  define CLAMP(val,min,max) (HEIGHEST(LOWEST(val,max),min))
#endif

#ifndef ABS
#  define ABS(x) (((x) < 0) ? -(x) : (x))
#endif

#ifndef DX
#  define DX(i,j,w)((j)*(w))+(i)
#endif

#ifndef IS_INSIDE /* x, y is top left corner. */
#  define IS_INSIDE(mousex, mousey, x, y, w, h) ((mousex >= x) && (mousex <= (x+w)) && (mousey >= y) && (mousey <= (y+h)))
#endif


#define RX_FLAG_NONE 0x0000              /* default flag */
#define RX_FLAG_LOAD_AS_RGBA 0x0001      /* can be used by image loading functions to convert loaded data directory to RGBA. See the rx_load_png function. */

/* file utils */
extern std::string rx_get_exe_path();
extern std::string rx_to_data_path(const std::string filename);
extern uint64_t rx_get_file_mtime(std::string filepath);
extern bool rx_is_dir(std::string filepath);
extern bool rx_file_exists(std::string filepath);
extern std::string rx_strip_filename(std::string path);
extern std::string rx_strip_file_ext(std::string path);
extern std::string rx_strip_dir(std::string path);
extern bool rx_create_dir(std::string path);
extern bool rx_create_path(std::string path);
extern std::string rx_get_file_ext(std::string filename);
extern std::vector<std::string> rx_get_files(std::string path, std::string ext = "");
extern std::string rx_norm_path(std::string path);
extern std::string rx_read_file(std::string filepath);

/* string and conversion utils */
extern std::string rx_string_replace(std::string, char from, char to);
extern std::string rx_string_replace(std::string, std::string from, std::string to);
extern int rx_to_int(const std::string& v);
extern uint32_t rx_string_id(const std::string& v);
extern float rx_to_float(const std::string& v);
extern std::string rx_int_to_string(const int& v);
extern std::string rx_float_to_string(const float& v);
extern std::vector<std::string> rx_split(std::string str, char delim);

/* time utils */
extern uint64_t rx_hrtime();
extern float rx_millis();
extern std::string rx_strftime(const std::string fmt);
extern std::string rx_get_time_string();
extern std::string rx_get_date_string();
extern int rx_get_year();
extern int rx_get_month();
extern int rx_get_day();
extern int rx_get_hour();
extern int rx_get_minute();

#endif // ROXLU_TINYLIB_H

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ M A T H
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_MATH)
#  ifndef ROXLU_USE_MATH_H
#  define ROXLU_USE_MATH_H

template<class T>
class Vec2 {

 public:
  Vec2();
  Vec2(T x, T y);
  Vec2(const Vec2<T>& o);
  Vec2(T f);

  void set(T vx, T vy);
  T* ptr();
  T& operator [](const unsigned int dx);

  Vec2<T> operator + () const;
  Vec2<T> operator - () const;
  Vec2<T> operator + (const Vec2<T>& o) const;
  Vec2<T> operator - (const Vec2<T>& o) const;
  Vec2<T> operator * (const Vec2<T>& o) const;
  Vec2<T> operator / (const Vec2<T>& o) const;
  Vec2<T> operator + (float s) const;
  Vec2<T> operator - (float s) const;
  Vec2<T> operator * (float s) const;
  Vec2<T> operator / (float s) const;

  Vec2<T>& operator += (const Vec2<T>& o);
  Vec2<T>& operator -= (const Vec2<T>& o);
  Vec2<T>& operator *= (const Vec2<T>& o);
  Vec2<T>& operator /= (const Vec2<T>& o);
  Vec2<T>& operator += (float s);
  Vec2<T>& operator -= (float s);
  Vec2<T>& operator *= (float s);
  Vec2<T>& operator /= (float s);

  bool operator == (const Vec2<T>& o) const;
  bool operator != (const Vec2<T>& o) const;
  void print();

 public:
  T x;
  T y;
}; // Vec2<T>

template<class T> inline Vec2<T>::Vec2() : x(), y() {}
template<class T> inline Vec2<T>::Vec2(T x, T y) : x(x), y(y) {}
template<class T> inline Vec2<T>::Vec2(const Vec2<T>& o) : x(o.x), y(o.y) {}
template<class T> inline Vec2<T>::Vec2(T f) : x(f), y(f) {}
template<class T> inline void Vec2<T>::set(T vx, T vy) { x = vx; y = vy; }
template<class T> inline T* Vec2<T>::ptr() { return &x; }
template<class T> inline T& Vec2<T>::operator [](const unsigned int dx) { return *(&x + dx); }
template<class T> inline Vec2<T> Vec2<T>::operator + () const { return Vec2<T>(+x, +y); };
template<class T> inline Vec2<T> Vec2<T>::operator - () const { return Vec2<T>(-x, -y); };
template<class T> inline Vec2<T> Vec2<T>::operator + (const Vec2<T>& o) const { return Vec2<T>(x + o.x, y + o.y); }
template<class T> inline Vec2<T> Vec2<T>::operator - (const Vec2<T>& o) const { return Vec2<T>(x - o.x, y - o.y); }
template<class T> inline Vec2<T> Vec2<T>::operator * (const Vec2<T>& o) const { return Vec2<T>(x * o.x, y * o.y); }
template<class T> inline Vec2<T> Vec2<T>::operator / (const Vec2<T>& o) const { return Vec2<T>(x / o.x, y / o.y); }
template<class T> inline Vec2<T> Vec2<T>::operator + (float s) const { return Vec2<T>(x + s, y + s); }
template<class T> inline Vec2<T> Vec2<T>::operator - (float s) const { return Vec2<T>(x - s, y - s); }
template<class T> inline Vec2<T> Vec2<T>::operator * (float s) const { return Vec2<T>(x * s, y * s); }
template<class T> inline Vec2<T> Vec2<T>::operator / (float s) const { return Vec2<T>(x / s, y / s); }
template<class T> inline Vec2<T> operator + (float s, const Vec2<T>& o) { return Vec2<T>(s + o.x, s + o.y); }
template<class T> inline Vec2<T> operator - (float s, const Vec2<T>& o) { return Vec2<T>(s - o.x, s - o.y); }
template<class T> inline Vec2<T> operator * (float s, const Vec2<T>& o) { return Vec2<T>(s * o.x, s * o.y); }
template<class T> inline Vec2<T> operator / (float s, const Vec2<T>& o) { return Vec2<T>(s / o.x, s / o.y); }
template<class T> inline Vec2<T>& Vec2<T>::operator += (const Vec2<T>& o) { return *this = *this + o; }
template<class T> inline Vec2<T>& Vec2<T>::operator -= (const Vec2<T>& o) { return *this = *this - o; }
template<class T> inline Vec2<T>& Vec2<T>::operator *= (const Vec2<T>& o) { return *this = *this * o; }
template<class T> inline Vec2<T>& Vec2<T>::operator /= (const Vec2<T>& o) { return *this = *this / o; }
template<class T> inline Vec2<T>& Vec2<T>::operator += (float s) { return *this = *this + s; }
template<class T> inline Vec2<T>& Vec2<T>::operator -= (float s) { return *this = *this - s; }
template<class T> inline Vec2<T>& Vec2<T>::operator *= (float s) { return *this = *this * s; }
template<class T> inline Vec2<T>& Vec2<T>::operator /= (float s) { return *this = *this / s; }
template<class T> inline bool Vec2<T>::operator == (const Vec2<T>& o) const { return x == o.x && y == o.y; }
template<class T> inline bool Vec2<T>::operator != (const Vec2<T>& o) const { return x != o.x || y == o.y; }
template<class T> inline float length(const Vec2<T>& o) { return sqrtf(o.x * o.x + o.y * o.y); }
template<class T> inline float dot(const Vec2<T> &a, const Vec2<T> &b) { return a.x * b.x + a.y * b.y; }
template<class T> inline float heighest(const Vec2<T> &v) { return fmaxf(v.x, v.y); }
template<class T> inline float lowest(const Vec2<T> &v) { return fminf(v.x, v.y); }
template<class T> inline Vec2<T> lowest(const Vec2<T> &a, const Vec2<T> &b) { return Vec2<T>(fmaxf(a.x, b.x), fmaxf(a.y, b.y)); }
template<class T> inline Vec2<T> heighest(const Vec2<T> &a, const Vec2<T> &b) { return Vec2<T>(fminf(a.x, b.x), fminf(a.y, b.y)); }
template<class T> inline Vec2<T> floor(const Vec2<T> &v) { return Vec2<T>(floorf(v.x), floorf(v.y)); }
template<class T> inline Vec2<T> ceil(const Vec2<T> &v) { return Vec2<T>(ceilf(v.x), ceilf(v.y)); }
template<class T> inline Vec2<T> abs(const Vec2<T> &v) { return Vec2<T>(fabsf(v.x), fabsf(v.y)); }
template<class T> inline Vec2<T> fract(const Vec2<T> &v) { return v - floor(v); }
template<class T> inline Vec2<T> normalized(const Vec2<T> &v) { T l = length(v); if(!l) { return T(0); } else return v / l; }
template<class T> inline void Vec2<T>::print() { printf("x: %f, y: %f\n", x, y); }

template<class T>
class Vec3 {

 public:
  Vec3();;
  Vec3(T x, T y, T z);
  Vec3(const Vec3<T>& o);
  Vec3(T f);

  void set(const float xv, const float yv, const float zv);
  T* ptr();
  T& operator [](const unsigned int dx);

  Vec3<T> operator + () const;
  Vec3<T> operator - () const;
  Vec3<T> operator + (const Vec3<T>& o) const;
  Vec3<T> operator - (const Vec3<T>& o) const;
  Vec3<T> operator * (const Vec3<T>& o) const;
  Vec3<T> operator / (const Vec3<T>& o) const;
  Vec3<T> operator + (float s) const;
  Vec3<T> operator - (float s) const;
  Vec3<T> operator * (float s) const;
  Vec3<T> operator / (float s) const;

  Vec3<T>& operator += (const Vec3<T>& o);
  Vec3<T>& operator -= (const Vec3<T>& o);
  Vec3<T>& operator *= (const Vec3<T>& o);
  Vec3<T>& operator /= (const Vec3<T>& o);
  Vec3<T>& operator += (float s);
  Vec3<T>& operator -= (float s);
  Vec3<T>& operator *= (float s);
  Vec3<T>& operator /= (float s);

  bool operator == (const Vec3<T>& o) const;
  bool operator != (const Vec3<T>& o) const;

  void print();

 public:
  T x, y, z;
}; // Vec3<T>

template<class T> inline Vec3<T>::Vec3() : x(), y(), z() {}
template<class T> inline Vec3<T>::Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
template<class T> inline Vec3<T>::Vec3(const Vec3<T>& o) : x(o.x), y(o.y), z(o.z) {}
template<class T> inline Vec3<T>::Vec3(T f) : x(f), y(f), z(f) {}
template<class T> inline void Vec3<T>::set(const float xv, const float yv, const float zv) { x = xv; y = yv; z = zv; }
template<class T> inline T* Vec3<T>::ptr() { return &x; }
template<class T> inline T& Vec3<T>::operator [](const unsigned int dx) { return *(&x + dx); }
template<class T> inline Vec3<T> Vec3<T>::operator + () const { return Vec3<T>(+x, +y, +z); };
template<class T> inline Vec3<T> Vec3<T>::operator - () const { return Vec3<T>(-x, -y, -z); };
template<class T> inline Vec3<T> Vec3<T>::operator + (const Vec3<T>& o) const { return Vec3<T>(x + o.x, y + o.y, z + o.z); }
template<class T> inline Vec3<T> Vec3<T>::operator - (const Vec3<T>& o) const { return Vec3<T>(x - o.x, y - o.y, z - o.z); }
template<class T> inline Vec3<T> Vec3<T>::operator * (const Vec3<T>& o) const { return Vec3<T>(x * o.x, y * o.y, z * o.z); }
template<class T> inline Vec3<T> Vec3<T>::operator / (const Vec3<T>& o) const { return Vec3<T>(x / o.x, y / o.y, z / o.z); }
template<class T> inline Vec3<T> Vec3<T>::operator + (float s) const { return Vec3<T>(x + s, y + s, z + s); }
template<class T> inline Vec3<T> Vec3<T>::operator - (float s) const { return Vec3<T>(x - s, y - s, z - s); }
template<class T> inline Vec3<T> Vec3<T>::operator * (float s) const { return Vec3<T>(x * s, y * s, z * s); }
template<class T> inline Vec3<T> Vec3<T>::operator / (float s) const { return Vec3<T>(x / s, y / s, z / s); }
template<class T> inline Vec3<T> operator + (float s, const Vec3<T>& o) { return Vec3<T>(s + o.x, s + o.y, s + o.z); }
template<class T> inline Vec3<T> operator - (float s, const Vec3<T>& o) { return Vec3<T>(s - o.x, s - o.y, s - o.z); }
template<class T> inline Vec3<T> operator * (float s, const Vec3<T>& o) { return Vec3<T>(s * o.x, s * o.y, s * o.z); }
template<class T> inline Vec3<T> operator / (float s, const Vec3<T>& o) { return Vec3<T>(s / o.x, s / o.y, s / o.z); }
template<class T> inline Vec3<T>& Vec3<T>::operator += (const Vec3<T>& o) { return *this = *this + o; }
template<class T> inline Vec3<T>& Vec3<T>::operator -= (const Vec3<T>& o) { return *this = *this - o; }
template<class T> inline Vec3<T>& Vec3<T>::operator *= (const Vec3<T>& o) { return *this = *this * o; }
template<class T> inline Vec3<T>& Vec3<T>::operator /= (const Vec3<T>& o) { return *this = *this / o; }
template<class T> inline Vec3<T>& Vec3<T>::operator += (float s) { return *this = *this + s; }
template<class T> inline Vec3<T>& Vec3<T>::operator -= (float s) { return *this = *this - s; }
template<class T> inline Vec3<T>& Vec3<T>::operator *= (float s) { return *this = *this * s; }
template<class T> inline Vec3<T>& Vec3<T>::operator /= (float s) { return *this = *this / s; }
template<class T> inline bool Vec3<T>::operator == (const Vec3<T>& o) const { return x == o.x && y == o.y && z == o.z; }
template<class T> inline bool Vec3<T>::operator != (const Vec3<T>& o) const { return x != o.x || y == o.y || z == o.z; }
template<class T> inline float length(const Vec3<T>& o) { return sqrtf(o.x * o.x + o.y * o.y + o.z * o.z); }
template<class T> inline float dot(const Vec3<T> &a, const Vec3<T> &b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
template<class T> inline float heighest(const Vec3<T> &v) { return fmaxf(fmaxf(v.x, v.y), v.z); }
template<class T> inline float lowest(const Vec3<T> &v) { return fminf(fminf(v.x, v.y), v.z); }
template<class T> inline Vec3<T> heighest(const Vec3<T> &a, const Vec3<T> &b) { return Vec3<T>(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z)); }
template<class T> inline Vec3<T> lowest(const Vec3<T> &a, const Vec3<T> &b) { return Vec3<T>(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z)); }
template<class T> inline Vec3<T> floor(const Vec3<T> &v) { return Vec3<T>(floorf(v.x), floorf(v.y), floorf(v.z)); }
template<class T> inline Vec3<T> ceil(const Vec3<T> &v) { return Vec3<T>(ceilf(v.x), ceilf(v.y), ceilf(v.z)); }
template<class T> inline Vec3<T> abs(const Vec3<T> &v) { return Vec3<T>(fabsf(v.x), fabsf(v.y), fabsf(v.z)); }
template<class T> inline Vec3<T> fract(const Vec3<T> &v) { return v - floor(v); }
template<class T> inline Vec3<T> normalized(const Vec3<T> &v) { T l = length(v); if(!l) { return T(0); } else return v / l; }
template<class T> inline Vec3<T> cross(const Vec3<T> &a, const Vec3<T> &b) { return Vec3<T>(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
template<class T> inline Vec3<T> perpendicular(const Vec3<T>& v) {  return abs(v.x) > abs(v.z) ? Vec3<T>(-v.y, v.x, 0.0) : Vec3<T>(0.0, -v.z, v.y); }
template<class T> inline void Vec3<T>::print() { printf("x: %f, y: %f, z: %f\n", x, y, z); }

// lines must be in the xy-plane; only done in 2d
template<class T>
inline bool intersect(const Vec3<T>& p0, const Vec3<T>& p1, const Vec3<T>& p2, const Vec3<T>& p3, Vec3<T>& result) {
  Vec3<T> s1 = p1 - p0;
  Vec3<T> s2 = p3 - p2;

  float s, t;
  s = (-s1.y * (p0.x - p2.x) + s1.x * (p0.y - p2.y)) / (-s2.x * s1.y + s1.x * s2.y);
  t = ( s2.x * (p0.y - p2.y) - s2.y * (p0.x - p2.x)) / (-s2.x * s1.y + s1.x * s2.y);

  if(s >= 0.0f && s <= 1.0f && t >= 0.0f && t <= 1.0f) {
    result.x = p0.x + (t * s1.x);
    result.y = p0.y + (t * s1.y);
    return true;
  }

  return false;
}

template<class T>
class Vec4 {

 public:
  Vec4();
  Vec4(T x, T y, T z, T w);
  Vec4(const Vec4<T>& o);
  Vec4(T f);

  void set(const float xv, const float yv, const float zv, const float wv);
  T* ptr();
  T& operator [](const unsigned int dx);

  Vec4<T> operator + () const;
  Vec4<T> operator - () const;
  Vec4<T> operator + (const Vec4<T>& o) const;
  Vec4<T> operator - (const Vec4<T>& o) const;
  Vec4<T> operator * (const Vec4<T>& o) const;
  Vec4<T> operator / (const Vec4<T>& o) const;
  Vec4<T> operator + (float s) const;
  Vec4<T> operator - (float s) const;
  Vec4<T> operator * (float s) const;
  Vec4<T> operator / (float s) const;

  Vec4<T>& operator += (const Vec4<T>& o);
  Vec4<T>& operator -= (const Vec4<T>& o);
  Vec4<T>& operator *= (const Vec4<T>& o);
  Vec4<T>& operator /= (const Vec4<T>& o);
  Vec4<T>& operator += (float s);
  Vec4<T>& operator -= (float s);
  Vec4<T>& operator *= (float s);
  Vec4<T>& operator /= (float s);

  bool operator == (const Vec4<T>& o) const;
  bool operator != (const Vec4<T>& o) const;

  void print();

 public:
  T x, y, z, w;
}; // Vec4<T>

template<class T> inline Vec4<T>::Vec4() : x(), y(), z(), w() {}
template<class T> inline Vec4<T>::Vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
template<class T> inline Vec4<T>::Vec4(const Vec4<T>& o) : x(o.x), y(o.y), z(o.z), w(o.w) {}
template<class T> inline Vec4<T>::Vec4(T f) : x(f), y(f), z(f), w(f) {}
template<class T> inline void Vec4<T>::set(const float xv, const float yv, const float zv, const float wv) { x = xv; y = yv; z = zv; w = wv; }
template<class T> inline T* Vec4<T>::ptr() { return &x; }
template<class T> inline T& Vec4<T>::operator [](const unsigned int dx) { return *(&x + dx); }
template<class T> inline Vec4<T> Vec4<T>::operator + () const { return Vec4<T>(+x, +y, +z, +w); };
template<class T> inline Vec4<T> Vec4<T>::operator - () const { return Vec4<T>(-x, -y, -z, -w); };
template<class T> inline Vec4<T> Vec4<T>::operator + (const Vec4<T>& o) const { return Vec4<T>(x + o.x, y + o.y, z + o.z, w + o.w); }
template<class T> inline Vec4<T> Vec4<T>::operator - (const Vec4<T>& o) const { return Vec4<T>(x - o.x, y - o.y, z - o.z, w - o.w); }
template<class T> inline Vec4<T> Vec4<T>::operator * (const Vec4<T>& o) const { return Vec4<T>(x * o.x, y * o.y, z * o.z, w * o.w); }
template<class T> inline Vec4<T> Vec4<T>::operator / (const Vec4<T>& o) const { return Vec4<T>(x / o.x, y / o.y, z / o.z, w / o.w); }
template<class T> inline Vec4<T> Vec4<T>::operator + (float s) const { return Vec4<T>(x + s, y + s, z + s, w + s); }
template<class T> inline Vec4<T> Vec4<T>::operator - (float s) const { return Vec4<T>(x - s, y - s, z - s, w - s); }
template<class T> inline Vec4<T> Vec4<T>::operator * (float s) const { return Vec4<T>(x * s, y * s, z * s, w * s); }
template<class T> inline Vec4<T> Vec4<T>::operator / (float s) const { return Vec4<T>(x / s, y / s, z / s, w / s); }
template<class T> inline Vec4<T> operator + (float s, const Vec4<T>& o) { return Vec4<T>(s + o.x, s + o.y, s + o.z, s + o.w); }
template<class T> inline Vec4<T> operator - (float s, const Vec4<T>& o) { return Vec4<T>(s - o.x, s - o.y, s - o.z, s - o.w); }
template<class T> inline Vec4<T> operator * (float s, const Vec4<T>& o) { return Vec4<T>(s * o.x, s * o.y, s * o.z, s * o.w); }
template<class T> inline Vec4<T> operator / (float s, const Vec4<T>& o) { return Vec4<T>(s / o.x, s / o.y, s / o.z, s / o.w); }
template<class T> inline Vec4<T>& Vec4<T>::operator += (const Vec4<T>& o) { return *this = *this + o; }
template<class T> inline Vec4<T>& Vec4<T>::operator -= (const Vec4<T>& o) { return *this = *this - o; }
template<class T> inline Vec4<T>& Vec4<T>::operator *= (const Vec4<T>& o) { return *this = *this * o; }
template<class T> inline Vec4<T>& Vec4<T>::operator /= (const Vec4<T>& o) { return *this = *this / o; }
template<class T> inline Vec4<T>& Vec4<T>::operator += (float s) { return *this = *this + s; }
template<class T> inline Vec4<T>& Vec4<T>::operator -= (float s) { return *this = *this - s; }
template<class T> inline Vec4<T>& Vec4<T>::operator *= (float s) { return *this = *this * s; }
template<class T> inline Vec4<T>& Vec4<T>::operator /= (float s) { return *this = *this / s; }
template<class T> inline bool Vec4<T>::operator == (const Vec4<T>& o) const { return x == o.x && y == o.y && z == o.z && w == o.w; }
template<class T> inline bool Vec4<T>::operator != (const Vec4<T>& o) const { return x != o.x || y == o.y || z == o.z || w == o.w; }
template<class T> inline float length(const Vec4<T>& o) { return sqrtf(o.x * o.x + o.y * o.y + o.z * o.z + o.w * o.w); }
template<class T> inline float dot(const Vec4<T> &a, const Vec4<T> &b) { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * a.w; }
template<class T> inline float heighest(const Vec4<T> &v) { return fmaxf(fmaxf(v.x, v.y), fmaxf(v.z, v.w)); }
template<class T> inline float lowest(const Vec4<T> &v) { return fminf(fminf(v.x, v.y), fminf(v.z, v.w)); }
template<class T> inline Vec4<T> heighest(const Vec4<T> &a, const Vec4<T> &b) { return Vec4<T>(fmaxf(a.x, b.x), fmaxf(a.y, b.y), fmaxf(a.z, b.z), fmaxf(a.w, b.w)); }
template<class T> inline Vec4<T> lowest(const Vec4<T> &a, const Vec4<T> &b) { return Vec4<T>(fminf(a.x, b.x), fminf(a.y, b.y), fminf(a.z, b.z), fminf(a.w, b.w)); }
template<class T> inline Vec4<T> floor(const Vec4<T> &v) { return Vec4<T>(floorf(v.x), floorf(v.y), floorf(v.z), floorf(v.w)); }
template<class T> inline Vec4<T> ceil(const Vec4<T> &v) { return Vec4<T>(ceilf(v.x), ceilf(v.y), ceilf(v.z), ceilf(v.w)); }
template<class T> inline Vec4<T> abs(const Vec4<T> &v) { return Vec4<T>(fabsf(v.x), fabsf(v.y), fabsf(v.z), fabsf(v.w)); }
template<class T> inline Vec4<T> fract(const Vec4<T> &v) { return v - floor(v); }
template<class T> inline Vec4<T> normalized(const Vec4<T> &v) { return v / length(v); }
template<class T> inline void Vec4<T>::print() { printf("x: %f, y: %f, z: %f, w: %f\n", x, y, z, w); }

template<class T>
class Matrix4 {
 public:
  Matrix4();

  Matrix4<T>& rotateX(T rad);
  Matrix4<T>& rotateY(T rad);
  Matrix4<T>& rotateZ(T rad);
  Matrix4<T>& rotate(T rad, T x, T y, T z);
  Matrix4<T>& rotate(T rad, const Vec3<T>& v);
  Matrix4<T>& translate(T x, T y, T z);
  Matrix4<T>& translate(const Vec3<T>& v);
  Matrix4<T>& position(const Vec3<T>& v);
  Matrix4<T>& position(T x, T y, T z);
  Matrix4<T>& scale(T x, T y, T z);
  Matrix4<T>& scale(T s);
  Matrix4<T>& perspective(T fovDegrees, T aspect, T n, T f);
  Matrix4<T>& ortho(T l, T r, T b, T t, T n, T f);
  Matrix4<T>& frustum(T l, T r, T b, T t, T n, T f);
  Matrix4<T>& identity();
  Matrix4<T>& lookat(Vec3<T> pos, Vec3<T> target, Vec3<T> up);

  T* ptr() { return &m[0]; }

  Matrix4<T> rotation(T rad, T x, T y, T z);

  Matrix4<T>& operator *=(const Matrix4<T>& o);
  Matrix4<T> operator * (const Matrix4<T>& o) const;
  T& operator [] (const unsigned int dx) { return m[dx]; }

  void print();

 public:
  T m[16];
}; // Matrix4<T>

template<class T>
Matrix4<T>::Matrix4() {
  identity();
}

template<class T>
Matrix4<T>& Matrix4<T>::identity() {
  memset(m, 0x00, sizeof(T) * 16);
  m[0] = (T)1.0f;
  m[5] = (T)1.0f;
  m[10] =(T)1.0f;
  m[15] = (T)1.0f;
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::frustum(T l, T r, T b, T t, T n, T f) {
  m[1]  = T(0);
  m[2]  = T(0);
  m[3]  = T(0);
  m[4]  = T(0);
  m[6]  = T(0);
  m[7]  = T(0);
  m[12] = T(0);
  m[13] = T(0);

  m[0]  = T(2) * n / (r-l);
  m[5]  = T(2) * n / (t-b);
  m[8]  = (r+l) / (r-l);
  m[9]  = (t+b) / (t-b);
  m[10] = - (f+n) / (f-n);
  m[11] = - T(1);
  m[14] = - T(2) * f * n / (f-n);
  m[15] = T(0);
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::ortho(T l, T r, T b, T t, T n, T f) {
  m[1]  = T(0);
  m[2]  = T(0);
  m[3]  = T(0);
  m[4]  = T(0);
  m[6]  = T(0);
  m[7]  = T(0);
  m[8]  = T(0);
  m[9]  = T(0);
  m[11] = T(0);
  m[15] = T(1);

  float rml = r - l;
  float fmn = f - n;
  float tmb = t - b;
  m[0]  = T(2) / rml;
  m[5]  = T(2) / tmb;
  m[10] = -T(2) / fmn;
  m[12] = -(r+l)/rml;
  m[13] = -(t+b)/tmb;
  m[14] = -(f+n)/fmn;
  return *this;
}


template<class T>
Matrix4<T>& Matrix4<T>::perspective(T fovDegrees, T aspect, T n, T f) {
  T tan_hfov = tan( (fovDegrees * DEG_TO_RAD) * T(0.5) );
  m[1]  = T(0);
  m[2]  = T(0);
  m[3]  = T(0);
  m[4]  = T(0);
  m[6]  = T(0);
  m[7]  = T(0);
  m[12] = T(0);
  m[13] = T(0);
  m[15] = T(0);
  m[0] = T(1) / (aspect * tan_hfov);
  m[5] = T(1) / (tan_hfov);
  m[10] = - (f + n) / (f - n);
  m[11] = - T(1);
  m[14] = - (T(2) * f * n) / (f - n);
  return *this;
}

template<class T>
void Matrix4<T>::print() {
  printf("%f, %f, %f, %f\n", m[0], m[4], m[8], m[12]);
  printf("%f, %f, %f, %f\n", m[1], m[5], m[9], m[13]);
  printf("%f, %f, %f, %f\n", m[2], m[6], m[10], m[14]);
  printf("%f, %f, %f, %f\n", m[3], m[7], m[11], m[15]);
}

template<class T>
Matrix4<T>& Matrix4<T>::rotate(T rad, T x, T y, T z) {
  Matrix4<T> rot = rotation(rad, x, y, z);
  *this *= rot;
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::rotateX(T rad) {
  return rotate(rad, T(1), T(0), T(0));
}

template<class T>
Matrix4<T>& Matrix4<T>::rotateY(T rad) {
  return rotate(rad, T(0), T(1), T(0));
}

template<class T>
Matrix4<T>& Matrix4<T>::rotateZ(T rad) {
  return rotate(rad, T(0), T(0), T(1));
}

template<class T>
Matrix4<T>& Matrix4<T>::rotate(T rad, const Vec3<T>& v) {
  return rotate(rad, v.x, v.y, v.z);
}

template<class T>
Matrix4<T> Matrix4<T>::rotation(T rad, T x, T y, T z) {

  Matrix4<T> mat;

  float c = cos(rad);
  float s = sin(rad);
  float t = 1.0f - c;

  Vec3<T> ax(x,y,z);
  ax = normalized(ax);

  float tx = t * ax.x;
  float ty = t * ax.y;
  float tz = t * ax.z;

  float sx = s * ax.x;
  float sy = s * ax.y;
  float sz = s * ax.z;

  float txy = tx * ax.y;
  float tyz = tx * ax.z;
  float txz = tx * ax.z;

  mat.m[0]  = tx * ax.x + c;
  mat.m[4]  = txy - sz;
  mat.m[8]  = txz + sy;
  mat.m[12] = 0.0f;

  mat.m[1]  = txy + sz;
  mat.m[5]  = ty * ax.y + c;
  mat.m[9]  = tyz - sx;
  mat.m[13] = 0.0f;

  mat.m[2]  = txz - sy;
  mat.m[6]  = tyz + sx;
  mat.m[10] = tz * ax.z + c;
  mat.m[14] = 0.0f;

  mat.m[3]  = 0.0f;
  mat.m[7]  = 0.0f;
  mat.m[11] = 0.0f;
  mat.m[15] = 1.0f;

  return mat;
}

template<class T>
Matrix4<T>& Matrix4<T>::scale(T s) {
  return scale(s, s, s);
}

template<class T>
Matrix4<T>& Matrix4<T>::scale(T x, T y, T z) {
  m[0] *= x; m[4] *= y;  m[8]  *= z;
  m[1] *= x; m[5] *= y;  m[9]  *= z;
  m[2] *= x; m[6] *= y;  m[10] *= z;
  m[3] *= x; m[6] *= y;  m[11] *= z;
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::translate(const Vec3<T>& v) {
  return translate(v.x, v.y, v.z);
}

template<class T>
Matrix4<T>& Matrix4<T>::position(const Vec3<T>& v) {
  m[12] = v.x;
  m[13] = v.y;
  m[14] = v.z;
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::position(T x, T y, T z) {
  m[12] = x;
  m[13] = y;
  m[14] = z;
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::translate(T x, T y, T z) {
  m[12] += m[0] * x + m[4] * y + m[8] * z;
  m[13] += m[1] * x + m[5] * y + m[9] * z;
  m[14] += m[2] * x + m[6] * y + m[10] * z;
  m[15] += m[3] * x + m[7] * y + m[11] * z;
  return *this;
}

template<class T>
Matrix4<T>& Matrix4<T>::operator *= (const Matrix4<T>& o) {
  Matrix4<T> r;

  r.m[0]  =  m[0] * o.m[0]  +  m[4] * o.m[1]  +  m[8]  * o.m[2]  +  m[12] * o.m[3];
  r.m[1]  =  m[1] * o.m[0]  +  m[5] * o.m[1]  +  m[9]  * o.m[2]  +  m[13] * o.m[3];
  r.m[2]  =  m[2] * o.m[0]  +  m[6] * o.m[1]  +  m[10] * o.m[2]  +  m[14] * o.m[3];
  r.m[3]  =  m[3] * o.m[0]  +  m[7] * o.m[1]  +  m[11] * o.m[2]  +  m[15] * o.m[3];

  r.m[4]  =  m[0] * o.m[4]  +  m[4] * o.m[5]  +  m[8]  * o.m[6]  +  m[12] * o.m[7];
  r.m[5]  =  m[1] * o.m[4]  +  m[5] * o.m[5]  +  m[9]  * o.m[6]  +  m[13] * o.m[7];
  r.m[6]  =  m[2] * o.m[4]  +  m[6] * o.m[5]  +  m[10] * o.m[6]  +  m[14] * o.m[7];
  r.m[7]  =  m[3] * o.m[4]  +  m[7] * o.m[5]  +  m[11] * o.m[6]  +  m[15] * o.m[7];

  r.m[8]  =  m[0] * o.m[8]  +  m[4] * o.m[9]  +  m[8]  * o.m[10] +  m[12] * o.m[11];
  r.m[9]  =  m[1] * o.m[8]  +  m[5] * o.m[9]  +  m[9]  * o.m[10] +  m[13] * o.m[11];
  r.m[10] =  m[2] * o.m[8]  +  m[6] * o.m[9]  +  m[10] * o.m[10] +  m[14] * o.m[11];
  r.m[11] =  m[3] * o.m[8]  +  m[7] * o.m[9]  +  m[11] * o.m[10] +  m[15] * o.m[11];

  r.m[12] =  m[0] * o.m[12] +  m[4] * o.m[13] +  m[8]  * o.m[14] +  m[12] * o.m[15];
  r.m[13] =  m[1] * o.m[12] +  m[5] * o.m[13] +  m[9]  * o.m[14] +  m[13] * o.m[15];
  r.m[14] =  m[2] * o.m[12] +  m[6] * o.m[13] +  m[10] * o.m[14] +  m[14] * o.m[15];
  r.m[15] =  m[3] * o.m[12] +  m[7] * o.m[13] +  m[11] * o.m[14] +  m[15] * o.m[15];

  std::copy(&r.m[0], &r.m[15], &m[0]);
  return *this;
}

template<class T>
Matrix4<T> Matrix4<T>::operator * (const Matrix4<T>& o) const {
  Matrix4<T> r;

  r.m[0]  =  m[0] * o.m[0]  +  m[4] * o.m[1]  +  m[8]  * o.m[2]  +  m[12] * o.m[3];
  r.m[1]  =  m[1] * o.m[0]  +  m[5] * o.m[1]  +  m[9]  * o.m[2]  +  m[13] * o.m[3];
  r.m[2]  =  m[2] * o.m[0]  +  m[6] * o.m[1]  +  m[10] * o.m[2]  +  m[14] * o.m[3];
  r.m[3]  =  m[3] * o.m[0]  +  m[7] * o.m[1]  +  m[11] * o.m[2]  +  m[15] * o.m[3];

  r.m[4]  =  m[0] * o.m[4]  +  m[4] * o.m[5]  +  m[8]  * o.m[6]  +  m[12] * o.m[7];
  r.m[5]  =  m[1] * o.m[4]  +  m[5] * o.m[5]  +  m[9]  * o.m[6]  +  m[13] * o.m[7];
  r.m[6]  =  m[2] * o.m[4]  +  m[6] * o.m[5]  +  m[10] * o.m[6]  +  m[14] * o.m[7];
  r.m[7]  =  m[3] * o.m[4]  +  m[7] * o.m[5]  +  m[11] * o.m[6]  +  m[15] * o.m[7];

  r.m[8]  =  m[0] * o.m[8]  +  m[4] * o.m[9]  +  m[8]  * o.m[10] +  m[12] * o.m[11];
  r.m[9]  =  m[1] * o.m[8]  +  m[5] * o.m[9]  +  m[9]  * o.m[10] +  m[13] * o.m[11];
  r.m[10] =  m[2] * o.m[8]  +  m[6] * o.m[9]  +  m[10] * o.m[10] +  m[14] * o.m[11];
  r.m[11] =  m[3] * o.m[8]  +  m[7] * o.m[9]  +  m[11] * o.m[10] +  m[15] * o.m[11];

  r.m[12] =  m[0] * o.m[12] +  m[4] * o.m[13] +  m[8]  * o.m[14] +  m[12] * o.m[15];
  r.m[13] =  m[1] * o.m[12] +  m[5] * o.m[13] +  m[9]  * o.m[14] +  m[13] * o.m[15];
  r.m[14] =  m[2] * o.m[12] +  m[6] * o.m[13] +  m[10] * o.m[14] +  m[14] * o.m[15];
  r.m[15] =  m[3] * o.m[12] +  m[7] * o.m[13] +  m[11] * o.m[14] +  m[15] * o.m[15];
  return r;
}

template<class T>
Matrix4<T>& Matrix4<T>::lookat(Vec3<T> pos, Vec3<T> target, Vec3<T> up) {

  Vec3<T> f = normalized(target - pos);
  Vec3<T> u = normalized(up);
  Vec3<T> s = normalized(cross(f, u));
  u = cross(s, f);

  m[0] =  s.x;  m[4] =  s.y;  m[8] = s.z;
  m[1] =  u.x;  m[5] =  u.y;  m[9] = u.z;
  m[2] = -f.x;  m[6] = -f.y;  m[10] = -f.z;

  translate(-pos);

  return *this ;
}

template<class T>
struct Spline {
  size_t size();                                 /* the number of points */
  void clear();                                  /* remove all points */
  T at(float t);                                 /* interpolate using catmull rom */
  void push_back(const T point);                 /* add a point to the class */
  template<class I> void assign(I begin, I end); /* assign multiple values; just like std::vector<T>::assign() */
  T& operator[](const unsigned int);
  std::vector<T> points;
}; // Spline<T>

template<class T>
T& Spline<T>::operator[](const unsigned int dx) {
  return points[dx];
}

template<class T>
inline size_t Spline<T>::size() {
  return points.size();
}

template<class T>
inline void Spline<T>::clear() {
  return points.clear();
}

template<class T>
inline void Spline<T>::push_back(const T p) {
  points.push_back(p);
}

template<class T>
inline T Spline<T>::at(float t) {
  if(points.size() < 4) {
    return T();
  }
  if(t > 0.999f) {
    t = 0.99f;
  }
  else if(t < 0) {
    t = 0;
  }

  T result;

  // get local "t" (also mu)
  float curve_p = t * (points.size()-1);
  int curve_num = curve_p;
  t = curve_p - curve_num; // local t

  // get the 4 points
  int b = curve_num;
  int a = b - 1;
  int c = b + 1;
  int d = c + 1;
  if(a < 0) {
    a = 0;
  }
  if(d >= points.size()) {
    d = points.size()-1;
  }

  T& p0 = points[a]; // a
  T& p1 = points[b]; // b
  T& p2 = points[c]; // c
  T& p3 = points[d]; // d

  float t2 = t*t;
  float t3 = t*t*t;

  result = 0.5 * ((2 * p1) + (-p0 + p2) * t + (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 + (-p0 + 3 * p1 - 3 * p2 + p3) * t3);

  return result;
}

typedef Matrix4<float> mat4;
typedef Vec4<float> vec4;
typedef Vec3<float> vec3;
typedef Vec2<float> vec2;

extern float rx_random(float max);
extern float rx_random(float x, float y);
extern void rx_rgb_to_hsv(float r, float g, float b, float& h, float& s, float& v);
extern void rx_rgb_to_hsv(vec3 rgb, vec3& hsv);
extern void rx_rgb_to_hsv(float* rgb, float* hsv);
extern void rx_rgb_to_hsv(vec3 rgb, float* hsv);
extern void rx_hsv_to_rgb(float h, float s, float v, float& r, float& g, float& b);
extern void rx_hsv_to_rgb(vec3 hsv, float* rgb);
extern void rx_hsv_to_rgb(vec3 hsv, float* rgb);
extern void rx_hsv_to_rgb(float* hsv, float* rgb);

#define PERLIN_SIZE 1024

class Perlin {

 public:
  Perlin(int octaves, float freq, float amp, int seed);
  float get(float x);
  float get(float x, float y);

 private:
  void initPerlin(int n, float p);
  void init();
  float noise1(float arg);
  float noise2(float vec[2]);
  float noise3(float vec[3]);
  void normalize2(float v[2]);
  void normalize3(float v[3]);
  float noise2D(float vec[2]);

 private:
  int octaves;
  float freq;
  float amp;
  int seed;

  int p[PERLIN_SIZE + PERLIN_SIZE + 2];
  float g3[PERLIN_SIZE + PERLIN_SIZE + 2][3];
  float g2[PERLIN_SIZE + PERLIN_SIZE + 2][2];
  float g1[PERLIN_SIZE + PERLIN_SIZE + 2];
  bool start;
}; // Perlin

inline float Perlin::get(float x) {
  float vec[2] = {x, 0.0f};
  return noise2D(vec);
}

inline float Perlin::get(float x, float y) {
  float vec[2] = {x, y};
  return noise2D(vec);
}

#define PERLIN_B PERLIN_SIZE
#define PERLIN_BM (PERLIN_SIZE - 1)
#define PERLIN_N 0x1000
#define PERLIN_NP 12 /* 2^N */
#define PERLIN_NM 0xFFF
#define PERLIN_CURVE(t)  ( t * t * (3.0f - 2.0f * t) )
#define PERLIN_LERP(t, a, b) ( a + t * (b - a) )

#define PERLIN_SETUP(i, b0, b1, r0, r1)         \
  t = vec[i] + PERLIN_N;                        \
  b0 = ((int)t) & PERLIN_BM;                    \
  b1 = (b0+1) & PERLIN_BM;                      \
  r0 = t - (int)t;                              \
  r1 = r0 - 1.0f;

inline Perlin::Perlin(int octaves, float freq, float amp, int seed)
    :octaves(octaves)
    ,freq(freq)
    ,amp(amp)
    ,seed(seed)
    ,start(true)
{
}

inline float Perlin::noise1(float arg) {
  int bx0, bx1;
  float rx0, rx1, sx, t, u , v, vec[1];
  vec[0] = arg;

  if(start) {
    srand(seed);
    start = false;
    init();
  }

  PERLIN_SETUP(0, bx0, bx1, rx0, rx1);
  sx = PERLIN_CURVE(rx0);
  u = rx0 * g1[ p[bx0] ];
  v = rx1 * g1[ p[bx1] ];
  return PERLIN_LERP(sx, u, v);
}

inline float Perlin::noise2(float vec[2]) {
  int bx0, bx1, by0, by1, b00, b10, b01, b11;
  float rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
  int i, j;

  if(start) {
    srand(seed);
    start = false;
    init();
  }

  PERLIN_SETUP(0, bx0, bx1, rx0, rx1);
  PERLIN_SETUP(1, by0, by1, ry0, ry1);

  i = p[bx0];
  j = p[bx1];

  b00 = p[i + by0];
  b10 = p[j + by0];
  b01 = p[i + by1];
  b11 = p[j + by1];

  sx = PERLIN_CURVE(rx0);
  sy = PERLIN_CURVE(ry0);

#define at2(rx, ry) (rx * q[0] + ry * q[1])

  q = g2[b00];
  u = at2(rx0, ry0);
  q = g2[b10];
  v = at2(rx1, ry0);
  a = PERLIN_LERP(sx, u, v);

  q = g2[b01];
  u = at2(rx0, ry1);
  q = g2[b11];
  v = at2(rx1, ry1);
  b = PERLIN_LERP(sx, u, v);

  return PERLIN_LERP(sy, a, b);
}

inline float Perlin::noise3(float vec[3]) {
  int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
  float rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
  int i, j;

  if(start) {
    srand(seed);
    start = false;
    init();
  }

  PERLIN_SETUP(0, bx0, bx1, rx0, rx1);
  PERLIN_SETUP(1, by0, by1, ry0, ry1);
  PERLIN_SETUP(2, bz0, bz1, rz0, rz1);

  i = p[bx0];
  j = p[bx1];

  b00 = p[i + by0];
  b10 = p[j + by0];
  b01 = p[i + by1];
  b11 = p[j + by1];

  t  = PERLIN_CURVE(rx0);
  sy = PERLIN_CURVE(ry0);
  sz = PERLIN_CURVE(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

  q = g3[b00 + bz0];
  u = at3(rx0,ry0,rz0);
  q = g3[b10 + bz0];
  v = at3(rx1,ry0,rz0);
  a = PERLIN_LERP(t, u, v);

  q = g3[b01 + bz0];
  u = at3(rx0,ry1,rz0);
  q = g3[b11 + bz0];
  v = at3(rx1,ry1,rz0);
  b = PERLIN_LERP(t, u, v);

  c = PERLIN_LERP(sy, a, b);

  q = g3[b00 + bz1];
  u = at3(rx0,ry0,rz1);
  q = g3[b10 + bz1];
  v = at3(rx1,ry0,rz1);
  a = PERLIN_LERP(t, u, v);

  q = g3[ b01 + bz1 ];
  u = at3(rx0,ry1,rz1);
  q = g3[ b11 + bz1 ];
  v = at3(rx1,ry1,rz1);
  b = PERLIN_LERP(t, u, v);

  d = PERLIN_LERP(sy, a, b);

  return PERLIN_LERP(sz, c, d);
}

inline void Perlin::normalize2(float v[2]) {
  float s;

  s = (float)sqrt(v[0] * v[0] + v[1] * v[1]);
  s = 1.0f/s;
  v[0] = v[0] * s;
  v[1] = v[1] * s;
}

inline void Perlin::normalize3(float v[3]) {
  float s;

  s = (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  s = 1.0f/s;

  v[0] = v[0] * s;
  v[1] = v[1] * s;
  v[2] = v[2] * s;
}

inline void Perlin::init(void) {
  int i, j, k;

  for(i = 0 ; i < PERLIN_B ; i++) {
    p[i] = i;
    g1[i] = (float)((rand() % (PERLIN_B + PERLIN_B)) - PERLIN_B) / PERLIN_B;

    for(j = 0 ; j < 2 ; j++) {
      g2[i][j] = (float)((rand() % (PERLIN_B + PERLIN_B)) - PERLIN_B) / PERLIN_B;
    }

    normalize2(g2[i]);

    for(j = 0 ; j < 3 ; j++) {
      g3[i][j] = (float)((rand() % (PERLIN_B + PERLIN_B)) - PERLIN_B) / PERLIN_B;
    }

    normalize3(g3[i]);
  }

  while(--i) {
    k = p[i];
    p[i] = p[j = rand() % PERLIN_B];
    p[j] = k;
  }

  for(i = 0 ; i < PERLIN_B + 2 ; i++) {

    p[PERLIN_B + i] = p[i];
    g1[PERLIN_B + i] = g1[i];

    for (j = 0 ; j < 2 ; j++) {
      g2[PERLIN_B + i][j] = g2[i][j];
    }

    for(j = 0 ; j < 3 ; j++){
      g3[PERLIN_B + i][j] = g3[i][j];
    }

  }
}

inline float Perlin::noise2D(float vec[2]) {

  float result = 0.0f;
  float amplitude = amp;

  vec[0] *= freq;
  vec[1] *= freq;

  for( int i = 0; i < octaves; i++ ) {
    result += noise2(vec) * amplitude;
    vec[0] *= 2.0f;
    vec[1] *= 2.0f;
    amplitude *= 0.5f;
  }

  return result;
}

#  endif // ROXLU_USE_MATH_H
#endif // ROXLU_USE_MATH

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ C U R L
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_CURL)
#  ifndef ROXLU_USE_CURL_H
#  define ROXLU_USE_CURL_H
#  include <curl/curl.h>
#  define RX_CHECK_CURLCODE(c, str) { if(c != CURLE_OK) { printf("Error: %s\n", str); return false; } }

size_t rx_curl_write_string_data(void* ptr, size_t size, size_t nmemb, void* str);
size_t rx_curl_write_file_data(void* ptr, size_t size, size_t nmemb, void* fpptr);

bool rx_fetch_url(std::string url, std::string& result);
bool rx_download_file(std::string url, std::string filepath);

#  endif
#endif

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ P N G
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_PNG)
#  ifndef ROXLU_USE_PNG_H
#  define ROXLU_USE_PNG_H

extern int rx_load_png(std::string filepath, unsigned char** pixels, int& w, int& h, int& nchannels, int* allocated = NULL, int flags = 0);
extern bool rx_save_png(std::string filepath, unsigned char* pixels, int w, int h, int channels, bool flip);

#  endif // ROXLU_USE_PNG_H
#endif //  defined(ROXLU_USE_PNG)

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ J P G
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_JPG)
#  ifndef ROXLU_USE_JPG_H
#  define ROXLU_USE_JPG_H

#if defined(_WIN32)
#  define XMD_H
#  include <jpeglib.h>
#  undef XMD_H
#elif defined(__linux)
#  include <jpeglib.h>
#endif

extern int rx_load_jpg(std::string filepath, unsigned char** pix, int& width, int& height, int& nchannels, int* allocated = NULL);
extern bool rx_save_jpg(std::string filepath, unsigned char* pix, int width, int height, int nchannels, int quality = 80, bool flip = false, J_COLOR_SPACE colorSpace = JCS_RGB, J_DCT_METHOD dctMethod = JDCT_FASTEST);

#  endif // ROXLU_USE_JPG_H
#endif //  defined(ROXLU_USE_JPG)

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ O P E N G L
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_OPENGL)
#  ifndef ROXLU_USE_OPENGL_H
#  define ROXLU_USE_OPENGL_H

extern void rx_print_shader_link_info(GLuint shader);
extern void rx_print_shader_compile_into(GLuint shader);
extern GLuint rx_create_program(GLuint vert, GLuint frag, bool link = false);
extern GLuint rx_create_program_with_attribs(GLuint vert, GLuint frag, int nattribs, const char** attribs);
extern GLuint rx_create_shader(GLenum type, const char* src);
extern GLuint rx_create_shader_from_file(GLenum type, std::string filepath);
extern GLint rx_get_uniform_location(GLuint prog, std::string name);
extern void rx_uniform_1i(GLuint prog, std::string name, GLint v);
extern void rx_uniform_1f(GLuint prog, std::string name, GLfloat v);
extern void rx_uniform_mat4fv(GLuint prog, std::string name, GLsizei count, GLboolean transpose, const GLfloat* value);

/*
   As we so often use fullscreen vertex shaders we defined this one. Use this to create
   a fullscreen vertex shader that has a v_texcoord out varying member.
*/

static const char* ROXLU_OPENGL_FULLSCREEN_VS = ""
  "#version 330\n"
  "const vec2 verts[4] = vec2[] ("
  "  vec2(-1.0, 1.0), "
  "  vec2(-1.0, -1.0), "
  "  vec2(1.0, 1.0), "
  "  vec2(1.0, -1.0) "
  ");"
  "const vec2 texcoords[4] = vec2[] ("
  "  vec2(0.0, 1.0), "
  "  vec2(0.0, 0.0), "
  "  vec2(1.0, 1.0), "
  "  vec2(1.0, 0.0) "
  ");"
  "out vec2 v_texcoord;"
  "void main() {"
  "  gl_Position = vec4(verts[gl_VertexID], 0.0, 1.0);"
  "  v_texcoord = texcoords[gl_VertexID];"
  "}"
  "";

class Shader {                                                                                               /* represents a GL shader - works only with shaders loaded from file */

 public:
  Shader():type(-1),id(-1){}
  ~Shader() {}                                                                                               /* @todo - we should clean the resources here */
  Shader& load(GLenum type, std::string filepath, std::string extra = "");                                   /* load a shader file for the given type, you can pass extra data to the shader which is prepended */
  Shader& reload();                                                                                          /* reload the previously loaded file */
  Shader& compile();                                                                                         /* compile the shader */

 public:
  std::string filepath;                                                                                       /* filepath to the shader */
  std::string file_source;                                                                                    /* the source we loaded() */
  std::string extra_source;                                                                                   /* source of the shader that is loaded */
  int type;                                                                                                   /* the shader type, eg. GL_VERTEX_SHADER */
  int id;                                                                                                     /* the ID of the shader */
}; // Shader

class Program {

 public:
  Program():id(-1){};
  ~Program();                                                                                                 /* @todo - we should clean the resources here */
  Program& add(Shader* s);                                                                                    /* add a shader when you want to have ownership; you don't need to compile() it yourself */
  Program& create(GLenum type, std::string filepath, std::string extra = "");                                 /* create a shader and add it, we take ownership you can pass extra data to the shader, which is prepended */
  Program& link(int nattribs = 0, const char** attribs = NULL, int nfrags = 0, const char** frags = NULL);    /* compiles + links the shaders, if you want to bind attrib or frag locations pass them */
  Program& recompile();                                                                                       /* recompiles the shaders  + LINKS it for you! */

 public:
  std::vector<std::string> attribs;                                                                           /* any attributes added to link() */
  std::vector<std::string> frags;                                                                             /* output fragment locations */
  std::vector<Shader*> shaders;                                                                               /* the added shaders */
  std::vector<Shader*> created_shaders;                                                                       /* allocated shaders; we have ownership, see create() */
  int id;
}; // Program

#  endif // ROXLU_USE_OPENGL_H
#endif // defined(ROXLU_USE_OPENGL)

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ F O N T
// ------------------------------------------------------------------------------------

/*

  Font
  -----

  This Font class is for the most part a copy of the Remoxly Gui, BitmapFont
  class. It makes use of the BMFont application to generate a bitmap font. In
  tinylib we've embedded the bitmap font for FreePixel which is a nice clean
  sans-serif font.

  It's possible that we add new fonts in the future. To add a new font see the
  BitmapFontBaker subproject of remoxly (https://github.com/roxlu/remoxly).

  Note: The Font classes are in the `roxlu` namespace.


  Usage:
  -------
  Create a FreePixel instance, add text and draw:

  ````c++
  // in your setup
  PixelFont font;
  font.write(10, 10, "Lorem ipsum dolor sit");

  // in your draw function
  font.draw();
  ````

  If you want to update the text at every draw call, make sure
  that your call clear before calling `write()` again.  Also, if your
  viewport size changes call resize() on the font.

 */

#if defined(ROXLU_USE_FONT) && defined(ROXLU_USE_OPENGL)
#  ifndef ROXLU_USE_FONT_H
#  define ROXLU_USE_FONT_H

// ------------------------------------------------------------------------------

#define FONT_ALIGN_LEFT      1
#define FONT_ALIGN_CENTER    2
#define FONT_ALIGN_RIGHT     3

#define FONT_GL2             1
#define FONT_GL3             2

#if !defined(FONT_GL)
#  define FONT_GL FONT_GL3
#endif

#if defined(__APPLE__) && FONT_GL == FONT_GL2
#  define glGenVertexArrays glGenVertexArraysAPPLE
#  define glBindVertexArray glBindVertexArrayAPPLE
#endif

// ------------------------------------------------------------------------------

#if FONT_GL == FONT_GL2
static const char* FONT_VS = ""
  "#version 110\n"
  "uniform mat4 u_pm;"
  "attribute vec4 a_pos;"
  "attribute vec2 a_tex;"
  "attribute vec4 a_fg_color; "
  "varying vec2 v_tex;"
  "varying vec4 v_fg_color;"
  ""
  "void main() {"
  "   gl_Position = u_pm * a_pos; "
  "   v_tex = a_tex;"
  "   v_fg_color = a_fg_color;"
  "}"
  "";

static const char* FONT_FS = ""
  "#version 110\n"
  "uniform sampler2DRect u_font_tex;"
  "varying vec2 v_tex;"
  "varying vec4 v_fg_color;"
  ""
  "void main() {"
  "  float col = texture2DRect(u_font_tex, v_tex).r;"
  "  gl_FragColor = col * v_fg_color * v_fg_color.a;"
  "}"
  "";
#endif

// ------------------------------------------------------------------------------

#if FONT_GL == FONT_GL3
static const char* FONT_VS = ""
  "#version 150\n"
  "uniform mat4 u_pm;"
  "in vec4 a_pos;"
  "in vec2 a_tex;"
  "in vec4 a_fg_color; "
  "out vec2 v_tex;"
  "out vec4 v_fg_color;"
  ""
  "void main() {"
  "   gl_Position = u_pm * a_pos; "
  "   v_tex = a_tex;"
  "   v_fg_color = a_fg_color;"
  "}"
  "";

static const char* FONT_FS = ""
  "#version 150\n"
  "uniform sampler2DRect u_font_tex;"
  "in vec2 v_tex;"
  "in vec4 v_fg_color;"
  "out vec4 fragcolor;"
  ""
  "void main() {"
  "  float col = texture(u_font_tex, v_tex).r;"
  "  fragcolor = col * v_fg_color * v_fg_color.a;"
  "}"
  "";
#endif

namespace roxlu {

  // ------------------------------------------------------------------------------

  struct Character {
    Character();

    unsigned int id;
    int x;
    int y;
    int width;
    int height;
    int xoffset;
    int yoffset;
    int xadvance;
  };

  // ------------------------------------------------------------------------------

  struct CharacterVertex {
    CharacterVertex();
    CharacterVertex(float x, float y, int s, int t, float* rgba);

    float x;
    float y;
    float s;
    float t;
    float fg_color[4];
  };

  // ------------------------------------------------------------------------------

  class Font {

  public:
    Font();

    void clear();
    void write(float x, float y, std::string str, int align = FONT_ALIGN_LEFT);           /* write a complete string */
    void write(float x, float y, unsigned int id);                                        /* write a specific character */
    void write(float x, float y, Character& c);                                           /* write a specific character */
    void draw();                                                                          /* renders all the batched write calls */
    void color(float r, float g, float b, float a = 1.0);                                 /* set the color for the font */
    void resize(int winW, int winH);                                                      /* whenever the viewport changes call this; it will recalculate the ortho matrix */
    void print();                                                                         /* print some debug info */

    bool getChar(unsigned int code, Character& result);                                   /* get a specific character, returns false when the font doesn't have the char */
    float getWidth(const std::string& str);                                               /* get the width for the given string, as it would be drawn on screen */

  protected:
    bool setupGraphics();
    void setupTexture(int w, int h, unsigned char* pix);
    void updateVertices();

  public:

    /* state */
    bool needs_update;
    int win_w;
    int win_h;
    float col[4];

    /* common font info */
    int line_height;
    int scale_w;
    int scale_h;
    int pages;
    int base;

    /* characters */
    std::map<unsigned int, Character> chars;

    /* opengl */
    float pm[16];
    static GLuint prog;
    static GLuint vert;
    static GLuint frag;
    static bool is_initialized;                                                           /* indicates if the prog, vert, frag and vao are initialized */
    GLuint vao;
    GLuint vbo;

    GLuint tex;
    size_t bytes_allocated;
    std::vector<CharacterVertex> vertices;
  };

} // namespace roxlu

static uint64_t freepixel_pixel_data[] = {0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff000000,0x0,0x0,0x0,0x0,0x0,0x0,0xffff0000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffffff0000,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0xff00,0x0,0x0,0x0,0xff0000ff000000,0xff00,0xff00000000000000,0xffff000000ffffff,0x0,0xffffffffff000000,0xffffff00ff,0xff00000000ff00,0xffffffffffff,0x0,0xffff000000000000,0xff000000ffffff,0xff000000ff,0xffffffffffff0000,0xff000000,0x0,0xff0000ff000000,0xff000000ffff,0xff000000ffff00,0xff00ff00ff0000ff,0x0,0xff0000000000,0xff00000000,0xff00000000ff00,0xff0000,0x0,0xff00ff000000ffff,0xff00ff0000000000,0xff00000000,0xff00000000,0xff00ff0000,0x0,0xff00ff000000,0xff0000ff00,0xff0000ff0000ff,0xff00ff00000000ff,0xff0000,0xff0000000000,0xff000000ff000000,0xffff0000ffff00,0xff0000,0xff00000000000000,0xff00ff0000ff0000,0x0,0xffff00000000,0xff00000000,0xff000000ff00,0x0,0xffffff00,0xff000000ff00ff00,0xff00ff00ffff00,0xffff0000000000ff,0xffffffff0000ff00,0xff00000000ff,0xff00ff00000000,0xffff0000ffff00,0xff0000,0xff0000ffffffff,0xff00ff00ff00ffff,0xffffff0000000000,0xffff00000000,0xff00000000,0xff000000ff00,0x0,0xff0000ff,0xff00000000ffff00,0xff0000ff0000ff00,0xffffff,0xff00ff000000ff,0xff000000ff00,0xffffffff0000,0xff00ffff00ff00,0xff00000000ff0000,0xff00ff0000ff00,0xff00ff00ff00ffff,0xff00ffffff,0xff000000ff,0xff00000000,0xff000000ff00,0x0,0xff0000ff,0xff00ffffffffff00,0xff00ffff00,0xff000000ff0000ff,0xff00ff00000000,0xff000000ff00,0xffff00ff00000000,0xff00ffff00ff00,0xff00000000ff0000,0xff00ffffffff00,0xff00ff00ff00ffff,0xff00000000,0xffff0000ff,0xff00000000,0xff000000ff00,0x0,0xff0000ff,0xffffff0000ff,0xff0000ff,0xff0000ff0000ff,0xff00ff0000ffff,0xff000000ff00,0xff00ff00ff000000,0xff0000ff00ff00,0xff00000000ff0000,0xff0000000000ff00,0xff00ff0000ff0000,0xff00000000,0xff000000ff,0xff00000000,0xff000000ff00,0x0,0xff0000ff,0xff000000,0xff000000ffff00,0xff00ff00ff0000ff,0xff00ff00ff0000,0xff000000ff00,0xffffff0000ff0000,0xff00000000ff00,0xff00000000ff0000,0xff00,0xff00ff000000ffff,0xff00000000,0xff000000ff,0xff00000000,0xff00ff0000,0xffffffffffffff,0xffffff00,0xffffff000000,0xff00000000000000,0xff00000000ffffff,0xff00ff00ff0000,0xff000000ff00,0xff00000000000000,0xff00000000ff00,0xff0000,0xffffffff,0xffff000000000000,0xffffff0000ffffff,0xffffffff00000000,0xff000000ff,0xff000000,0x0,0x0,0x0,0x0,0xff,0xffff,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff00,0x0,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffffff,0x0,0xff000000000000,0xff0000ff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffffffff0000,0xff0000,0x0,0x0,0x0,0x0,0x0,0xff0000ff000000,0x0,0xff000000000000,0xff00ff00000000ff,0xff00000000,0xff0000000000,0xff0000,0xffffff000000ff,0x0,0xff00ffffff00,0xff000000ffff0000,0xff000000ff000000,0x0,0xff00000000,0xff00ff00,0xffff000000ff,0xff00ff000000,0xffffff00,0xff0000ffffff00,0xff00ff00000000ff,0xff00000000,0xff0000000000,0xffff000000ff0000,0xff0000ff0000ffff,0x0,0xff00ffffff00,0xff0000ff0000ff00,0xffff0000ff000000,0x0,0xff00000000,0xff00ff00,0xff0000ff0000ff,0xff00ff0000,0xff0000ff00,0xff00ff0000ff00,0xff00ff00000000ff,0xff00000000,0xff000000000000,0xff00,0xff000000ff,0xff,0xff00ffffff00,0xff00ff00000000ff,0xff000000ff000000,0xff000000,0xff0000000000,0xff0000ff000000ff,0xff000000ff000000,0x0,0xff000000ff00,0xff0000000000ff,0xff0000ff0000ff00,0xff00ff00000000,0xff0000ff000000,0xffffff000000ff00,0xff000000ff,0xff000000ff00ff,0xff00ffffff00,0xff00000000000000,0xff000000ff000000,0xffff000000ff0000,0xff000000ffff,0xff0000ff000000ff,0xff00ff0000ff0000,0xff000000ff00,0xff000000ff00,0xff,0xff0000ff0000ff00,0xff00ff0000ff00,0xff000000ff000000,0xff000000ff,0xffffff0000ff,0xff000000ff00ff,0xff00ffff0000,0xff00000000000000,0xff000000ff000000,0xff0000000000ff00,0xffff000000ff0000,0xffff0000000000,0xff00ffff00ff0000,0xff000000ff00,0xff00ff0000ffffff,0xffffffff,0xff0000ffffffff00,0xff00ff00ffff00,0xff000000ff0000ff,0xff00000000,0xff000000ff,0xff000000ff00ff,0xff00ff000000,0xff00000000000000,0xff000000,0xffff0000ffff00ff,0xff000000ffffff,0xff0000000000,0xff00ffff00ff0000,0xff000000ff00,0xff000000ff00,0xff,0xff000000ffff0000,0xff00ff00ffff00,0xff000000ff00ffff,0xff00000000,0xff000000ff,0xff000000ff00ff,0xff00ff000000,0xff00000000000000,0xff000000ff000000,0xff00ff00ff00ff00,0xff000000000000,0xff0000000000,0xff000000ff0000,0xff000000ff00,0xff000000ff00,0xff,0xffff0000,0xff000000ff0000ff,0xff00000000ff0000,0xff0000ff00000000,0xff0000ff000000ff,0xffff00ffff0000,0xff00ff000000,0xff00000000000000,0xff0000ff000000,0xff00ff00ffffff00,0xff000000000000,0xff0000000000,0xff000000,0xffff0000ff00,0xff0000ff00,0xff,0xff0000,0xff000000ff0000ff,0xff00000000ff0000,0xffff0000000000,0xffffff000000ff,0xff0000ff00ff0000,0xff00ff000000,0x0,0xffffff,0xffff0000ff000000,0xff000000ffffff,0xff0000000000,0xff0000ff000000,0xff00ffff0000,0xff000000ffffff00,0xffffffffff,0x0,0x0,0x0,0x0,0x0,0xff0000,0xff00ff000000,0x0,0x0,0x0,0x0,0x0,0xffff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff0000,0x0,0xff00,0x0,0x0,0x0,0x0,0xff0000ff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffff000000,0x0,0xff,0x0,0x0,0x0,0x0,0xffff0000,0x0,0x0,0x0,0x0,0x0,0xffff00ffff0000,0x0,0x0,0x0,0xff0000ff00,0xff00000000000000,0x0,0x0,0x0,0xff,0x0,0x0,0x0,0xff00ffffffff0000,0xffff00ff00000000,0xffffffff0000ffff,0xffff00ffff0000,0xff00ff00,0xff,0xffffffff0000,0xff000000ffff0000,0xff00000000ffff,0xffffff000000,0x0,0x0,0xffffff0000ff,0xff000000ffffff00,0xffffff000000ffff,0xffff00000000,0xff000000ffff0000,0xff00ff00000000,0xff00ff0000,0xffff00ffff00ff,0xff000000ff00ff00,0xff00,0xff000000ff0000,0xff000000ff0000,0xff000000ff0000,0xff000000ff0000,0x0,0xff00000000000000,0xff000000ff0000,0xff00ff000000ff,0xff00ff0000,0xffff000000ff,0xffff0000,0xff0000ff0000ff,0xff00ff0000,0xffff00ffff00ff,0xff0000ffffffffff,0xff00,0xff000000ff0000,0xff000000000000,0x0,0xff0000,0x0,0xff00000000000000,0xff000000ff0000,0xff0000000000ff,0xff0000,0xffff000000ff,0xff00ff00,0xff0000ff0000ff,0xff00ff0000,0xff,0xff00ff00,0xffff00ff000000ff,0xff000000ff0000,0xff0000ffff00ff,0xffffff0000000000,0xff0000,0xff00000000000000,0xff00000000ffff,0xffff0000ff0000,0xff,0xff00000000ff0000,0xff00ff000000,0xffffff00ff00,0xffff000000ffff00,0xff0000ffff,0xff,0xff000000ff00ff00,0xffff000000ff,0xff000000ff00ff,0xff0000ff0000ffff,0xff0000ffff,0xffffff0000ff,0xff000000000000,0xff000000ff0000,0xff00ff00ff0000,0xffffff00,0xff00000000ff00,0xff00ff000000,0xffffffff,0xff0000ffffffff,0xffffffff00ff0000,0x0,0xff0000ff00ff00,0xff00ff00ff,0xffffffff0000,0xff,0xffffffff00ff0000,0xff000000000000,0xffff00ffffffffff,0xffff,0xff0000ffff0000,0xff00000000,0xff000000000000ff,0xffffffffff00ff,0xff0000ff,0xff00000000ff00,0xff0000ff00ff0000,0x0,0xff00ffffffffff,0xff0000ff00,0xff0000ff0000,0xff,0xff0000,0xff0000000000ff,0x0,0xff0000ff0000,0xff000000ff0000,0xff0000ff00000000,0x0,0xff00000000ff,0xff0000ff,0xff0000ffffffff,0xff0000ff00ff0000,0x0,0xff0000ff00ff00,0xff0000ff00,0xff0000ff0000,0xff0000000000ff,0xff00ff0000,0xff000000ff00ff,0xff000000000000,0xff0000ff0000,0xff000000ff0000,0xff00ff000000ff,0xff0000ff00000000,0xff0000000000,0xffffff0000ff,0xffff00000000ff00,0xff0000ffff,0xff,0xff000000ff00ff00,0xff00ff00ff,0xff000000ff0000,0xff000000000000ff,0xffffff000000ffff,0xffffff000000,0xff00000000000000,0xff0000ffff,0xffffff000000,0xffff0000ffffff00,0xffff0000ffffff,0xff0000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0x0,0xff000000ff,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0x0,0xff000000000000,0x0,0x0,0x0,0xff00000000,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0x0,0xff000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0xffff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0xff00,0xff000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffffffff,0xffffffff0000ffff,0xffffff0000ff,0xff000000ffffff00,0xff0000000000ff,0xff00ff00000000,0xff000000000000,0xffffff0000ff0000,0xffffff000000,0xff00000000,0xffffff0000ffff,0xffffffffff0000,0xffffffffff,0xff0000ffff,0xffffff0000ff,0xffffff0000,0xff000000000000ff,0xff00000000,0xff000000ff00ff,0xff000000ff,0xff0000000000ff,0xff00000000,0xff000000000000,0xff00ff0000,0xff000000ff00ff,0xff0000ff00000000,0xff0000ff00ff0000,0xff0000,0xff000000000000ff,0xff00ff0000,0xff,0xff00000000,0xff0000000000ff,0x0,0xff000000ff00ff,0xff000000ff,0xff000000000000,0xff00000000,0xff000000000000,0xff00ff0000,0xff000000ff00ff,0xff00ff00000000,0xff00000000,0xff00ff,0xff0000000000ff,0xff00000000,0xff,0xff00000000,0xff0000000000ff,0xff00000000000000,0xff00ff000000,0xff0000ff000000ff,0xffffffff0000ffff,0xffffffffff00ff,0xff000000000000,0xff0000,0xff000000ff00ff,0xff00ffffffff00,0xff00000000,0xff00ff,0xff0000000000ff,0xff00000000,0xff000000ff00ff,0xff00ff00000000,0xff0000ffffffff,0xff0000000000ffff,0xff00000000,0xff00ffff0000ff,0xff000000ff0000,0xff00000000,0xff00ffffffffff,0xff00000000ff0000,0xff000000ff0000,0xff00ff000000ff,0xff00000000,0xffffffff00ff,0xff0000ffffffff,0xffffffff00000000,0xff000000ff00ff,0xff00ff00000000,0xffff00ff00000000,0xff000000ff0000,0xffff00ffff0000,0xffff00ff00000000,0xff00000000ffff,0xff00000000,0xff00ffffffffff,0xff000000ff0000,0xffffffffff0000,0xff00ff000000ff,0xff00000000,0xff00ff,0xff0000000000ff,0xff00000000,0xff000000ff00ff,0xff00000000,0xff00ff00000000,0xff000000ff0000,0xff000000ff0000,0xff00000000,0xff000000ff0000,0xff00000000,0xff000000000000,0xff000000ff0000,0xff000000ff0000,0xff00ff000000ff,0xff00000000,0xff00ff,0xff0000000000ff,0xff00ff0000,0xff000000ff00ff,0xff00000000,0xff00ff000000ff,0xff000000ff0000,0xff000000ff0000,0xff0000ff000000,0xff000000ff0000,0xff00000000,0xff000000000000,0xff0000,0xff000000ff0000,0xff0000ffff0000ff,0xff0000ff00ff0000,0xff0000,0xff000000000000ff,0xff00ff0000,0xffff0000ff00ff,0xff0000ff0000ff,0xff000000ffffff00,0xff00000000ffff,0xffffff000000,0xff00000000ffff00,0xffff00000000ffff,0xffffff000000ff,0xff00000000000000,0xff00000000ffff,0xff000000ff0000,0xff00ffff00,0xffffff0000ffff,0xffffffffff0000,0xff,0xff00ffffff,0xff00ffff0000ff,0xff000000ffff00,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xff,0x0,0x0,0x0,0x0,0x0,0xff00ff000000ff,0xff00000000,0xff000000ff,0xffffffff,0xffffffff000000ff,0xffffff000000,0xff00ff000000ff,0xffffffff00ff0000,0xff000000ff00ff,0xff000000000000ff,0xff0000ff00,0xff,0xff,0x0,0x0,0xffffff0000,0xff0000ff0000ff,0xff00000000,0xff00ff0000ff,0xff0000ff000000ff,0xff0000ff00,0xff000000ff00ff,0xff00ff000000ff,0xff0000,0xff000000ff00ff,0xff00,0xff000000ff,0xff,0xff,0x0,0x0,0xff0000,0xff000000ff00ff,0xffff00000000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff00ff,0xff0000ff000000ff,0xff00,0xff000000ff00ff,0xff00,0xff00000000,0xff,0xff,0x0,0x0,0xff0000,0xff000000ff00ff,0xff00ff00000000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff00ff,0xff0000ff000000ff,0xff0000000000ff00,0xff000000ff0000,0xff000000ff0000,0xff00ff0000,0xffff000000ff,0xff000000ffff00ff,0xff0000ffff,0xffffff0000ff,0xffffffff,0xff00000000ffff,0xff00ff00000000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xffffff0000ff,0xff000000ff,0xff0000000000ff,0xff000000ff0000,0xff000000ff0000,0xff00ff0000,0xff0000ff0000ff,0xff00ff0000ffff,0xff00ff0000,0xff000000ff00ff,0xff0000,0xff000000ffffff,0xff00ff00000000,0xff000000ff00ff,0xff0000ffffffff,0xffffffff00ff0000,0xff000000000000,0xff0000ff000000ff,0xffff000000ffff,0xff000000ff0000,0xff000000ff0000,0xff00ff0000,0xffffffff0000ff,0xff00ff000000ff,0xff00000000,0xffffffffff00ff,0xff0000,0xff000000ff00ff,0xff0000ff00000000,0xff000000ff00ff,0xff0000000000ff,0xff0000ff00ff00ff,0xff000000000000,0xff0000ff000000ff,0xff0000ff00,0xff000000ff0000,0xff0000ff000000,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00000000,0xff00ff,0xff0000,0xff0000ff0000ff,0xff00000000,0xff00ff0000ff,0xff000000000000ff,0xff0000ff0000ff00,0xff000000ff0000,0xff00ff000000ff,0xff00ff0000,0xff000000ff0000,0xff0000ff000000,0xff00ffff00,0xffff0000ff00ff,0xff00ff000000ff,0xff0000ff00ff0000,0xff00ff,0xff000000ff0000,0xffff00ff000000ff,0xff00ffffff,0xff000000ff,0xff,0xff00ff00ff,0xffffff0000ff,0xff0000ffffff00,0xffffffff00ff0000,0xffffff0000ff,0xff0000ff00000000,0xffffff0000ff00ff,0xff00ffff000000,0xff000000ffffffff,0xffff000000ffff,0xffffff0000ff,0xff000000ff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0xff00000000000000,0x0,0x0,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0x0,0x0,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff000000,0x0,0x0,0xff000000000000,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffff00000000,0xff000000000000,0x0,0x0,0xff0000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00ff0000ff00,0xff000000000000,0xffffffff00000000,0xff000000ff,0xffff0000000000ff,0xffffff,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0xff000000000000,0xffffffffff0000,0xff0000ff000000,0xff000000000000,0x0,0xff000000ff,0xff,0xff00,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0xff000000000000,0xff000000000000,0xff000000ff0000,0xff000000000000,0x0,0xff,0xff,0xff00,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0xff00ff00ffff00,0xff0000000000ffff,0xffffffffff0000,0xff000000ff,0xffff00ff000000ff,0xffffff000000,0xff000000ffff00ff,0xffff00ff00ffffff,0xffffff000000,0xff00ffffffffff,0xff00ff0000,0xff000000ff00ff,0xffff00ff000000ff,0xffffffff00ffffff,0xff00000000ff,0xffffffffff,0xffff00ffff0000ff,0xff000000ff0000,0xff000000000000,0xff0000ff,0xffff000000ff,0xff000000ff00ff,0xff00ff0000ffff,0xffff00ff0000,0xff000000ff00ff,0xff000000ff0000,0xff00ff0000,0xff000000ff00ff,0xff000000ff,0xff0000,0xff000000ff,0xff00000000,0xff00ff000000ff,0xffff0000ff0000,0xffff00000000,0xffffff,0xff000000ff,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xffffffff0000,0xff000000ff0000,0xff00ffff00ff0000,0xffffff0000ff,0xffff00ff00,0xffff00000000ffff,0xffff000000,0xffff0000,0xff00ff000000ff,0xff00ff0000,0xff00000000,0xff00000000ff00ff,0xff00000000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000000000,0xff000000ff0000,0xff00ff0000ff0000,0xff00ff000000,0xffff0000,0xff0000000000ff,0xff0000,0xff0000,0xff00ffff0000ff,0xff00ff0000,0xff000000,0xff000000ff0000ff,0xff00000000,0xff000000ff00ff,0xff00ff000000ff,0xff00ffff00,0xff000000ff0000,0xff000000ff0000,0xff00ff0000ffff00,0xff000000ff0000,0xff000000ffff0000,0xff0000000000,0xff0000,0xff00,0xff00ff00ffff00,0xffffffff00ff0000,0xffffffffff00ff,0xffff00ff000000ff,0xff00ffffff,0xffffff0000ff,0xff000000ffffffff,0xff00ff00ff,0xffffff000000,0xff0000ffffff0000,0xff000000ff00ff,0xff000000ff0000,0xffff000000ff0000,0xffffffff00ffffff,0xffffffffff00ff,0xffffffffff,0xff00000000,0x0,0x0,0x0,0x0,0x0,0xff,0xff0000,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0xff000000ff,0x0,0x0,0x0,0x0,0x0,0xff,0xff0000,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0xffffff00,0x0,0x0,0x0,0x0,0x0,0xff,0xff0000,0x0,0x0,0x0,0x0,0xffff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0xff00000000ff00,0x0,0x0,0xff,0xff000000,0xff000000,0xff,0x0,0xff00,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000,0xffffff00000000ff,0xffffff000000,0xff000000ff00ff00,0xffff,0xff00000000,0xff00000000ff0000,0xff00ff000000ffff,0xffffff000000,0xff000000ff0000,0x0,0x0,0x0,0xffff00ffff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0xff00000000,0xffffff0000ff,0xffff00ffff,0x0,0x0,0xff000000ffffff00,0xffffff000000ffff,0xffffff000000,0xff000000ffffff00,0xffff00000000ffff,0xffffffffff0000,0xffff00ffffffffff,0xffffffff00ffffff,0xff000000ff00ff,0xff000000ff0000,0xff00000000000000,0xffffff000000ffff,0xff000000ff0000,0x0,0x0,0x0,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff0000ff0000,0xff00ff,0xff0000000000ff,0xff00000000,0xff000000ff0000,0xff0000ff00ff00,0xff000000000000,0xff00ff0000ff0000,0xff0000,0x0,0x0,0x0,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff0000,0xff0000000000ff,0xff00000000,0xff0000ffff0000,0xff00ff000000ff,0xff000000000000,0xff00000000,0xffff0000ff,0x0,0x0,0xff00000000,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff0000,0xff0000000000ff,0xff00000000,0xff00ff00ff0000,0xff00ff000000ff,0xff00ff00ffff00,0xff00000000,0xffff00ff00ff,0x0,0xff00ff00ff00ff,0x0,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xffffffff0000,0xffff0000ffffffff,0xffffffff0000ffff,0xff00ff00ff0000,0xff00ff000000ff,0xff0000ffff00ff,0xffffff0000ff0000,0xff00ffffff0000,0xffff000000000000,0xffffff000000ffff,0xff000000ff,0xffff00ffffffffff,0xffffffff00ffffff,0xffffffffff00ff,0xffff00ffffffffff,0xff00ffffff,0xff0000,0xff0000000000ff,0xff00000000,0xff00ff00ff0000,0xff00ff000000ff,0xff00000000000000,0xff0000ffff,0xffff000000ff,0xff00000000000000,0xff00ff000000ff00,0xff00000000,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff0000,0xff0000000000ff,0xff00000000,0xffff0000ff0000,0xff00ff000000ff,0x0,0x0,0xff000000000000,0x0,0xff00ff00ff00ff,0xff000000,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff0000ff0000,0xff00ff,0xff0000000000ff,0xff00000000,0xff000000ff0000,0xff0000ff00ff00,0x0,0x0,0xff000000ff0000,0x0,0x0,0xff0000,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xffff000000ff0000,0xffffffffff0000,0xffff00ffffffffff,0xffffffff00ffffff,0xff000000ff00ff,0xff000000ff0000,0x0,0x0,0xffffff000000,0x0,0x0,0xff000000ff0000,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0x0,0x0,0x0,0x0,0xff000000ff0000,0x0,0x0,0x0,0x0,0xffff0000000000,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0x0,0x0,0x0,0x0,0xffffff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000,0xff,0x0,0xff00000000000000,0xff00000000000000,0xff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000ff0000,0xffffff000000ffff,0xff00ff000000,0x0,0xff0000000000ff,0xffffff000000,0xff00ff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0x0,0x0,0x0,0xff00000000,0x0,0x0,0x0,0x0,0xff000000,0xff000000,0xff,0x0,0xff0000,0xff0000000000,0xff0000000000,0xff000000ff0000,0xff0000,0xff0000000000ff,0xff00000000,0xff00ff00ff0000,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xffffff0000000000,0x0,0xff0000,0xffffff00000000ff,0xff00ff000000,0xff00ff00,0xff000000000000,0xff00000000,0xff000000ff0000,0xff000000ff00ff00,0xff00ff000000ff00,0xff00ff000000,0xff0000ff00ff00,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00000000,0xff000000ff,0xff00000000000000,0xffffff000000ff00,0xff00ff000000,0xff0000,0xff000000000000,0xff00000000,0xff0000ff00ff00,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ffff0000ff,0xff00ff0000,0xff000000ff00ff,0xffff00ff000000ff,0xff000000ff,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000000000,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff00ff00ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff0000ff0000ff00,0xffff00000000,0xffff0000,0xffff00000000ffff,0xffff00000000,0xff000000ffff0000,0xffffff000000ffff,0xffffff000000,0xffffff00,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff00ff00ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff00ff0000,0xff0000ff000000,0xff0000ff0000ff00,0xff0000ff0000,0xff0000ff0000ff,0xff00ff0000ff00,0xff00ff0000,0xff000000ff00ff,0xff000000ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff00ff00ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xffff00ff00ffff00,0xffffffff000000,0xff0000ffffffff00,0xffffff0000ffffff,0xffffffff0000ff,0xff00ffffffff00,0xffffffff00000000,0xffffffffff00ff,0xffffffffff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff0000ffff,0xff00ff0000,0xff000000ff00ff,0xffff00ff000000ff,0xff000000ff,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00000000,0xff0000,0xff0000000000ff,0xff000000ff00ff00,0xff00ff000000ff00,0xff00ff000000,0xff0000ff00ff00,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00000000,0xffff0000ff00ff,0xff00ffff0000ff,0xff0000ff00ffff00,0xffff0000ff00ff,0xff00ffff0000ff,0xff00ff0000,0xff0000,0xff0000000000ff,0xff0000,0xff0000000000ff,0xff00000000,0xff00000000ff00ff,0xffffff000000ffff,0xffffff000000,0xff0000ffffff00,0xffff00ff00000000,0xff00ffff000000,0xff0000ff00ffff00,0xffff0000ff00ff,0xff00ffff0000ff,0xff0000ff00ffff00,0xffffff000000ffff,0xffffff000000,0xff0000ffffff00,0x0,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0xff000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0xff,0x0,0xff000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffff00,0xffffff0000000000,0xff,0x0,0xff00000000000000,0x0,0xff000000,0xff000000,0xff,0x0,0xff00000000000000,0xff00000000000000,0xff00000000,0x0,0xff00,0x0,0x0,0x0,0x0,0x0,0xff00ff00,0xffffff000000ffff,0xff00000000,0xff0000,0xffffff00000000ff,0xff00ff000000,0x0,0xff0000000000ff,0xff00000000,0xff00ff00,0xff000000ff,0xff00ff000000,0xff000000ffffff00,0xffffff000000ffff,0xff0000ff000000,0xffffff00,0xffff0000ff00ff00,0xffffff00000000ff,0xff00000000,0xff00000000ff0000,0xffffff000000ff00,0xff00ff000000,0x0,0xff0000000000ff,0xff00ff000000,0xff00ff00,0xff000000ff,0xff00ff000000,0xff000000ff,0xff00000000,0xffff000000ff,0xff000000ff,0x0,0xff,0x0,0x0,0x0,0x0,0xff00000000,0x0,0x0,0x0,0xff00000000,0x0,0xff000000ff,0xff00000000,0xff,0xff000000ff,0xff000000ffffff00,0xffff00ff0000ffff,0xffffff000000,0xff000000ffffff00,0xffffff000000ffff,0xffffff000000,0xff0000ffffff00,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xffff00ff00ff0000,0xff000000ff0000,0xff000000ff,0xff0000ffff,0xffff000000ff,0xff00ff000000ff,0xff00ff000000ff,0xffff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ffff0000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xffff00ff0000,0xff000000ff00ff,0xff0000ff000000ff,0xff00ff0000,0xff0000ff0000ff,0xff00ff000000ff,0xff00ffffffffff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff00ff00ff,0xff00ff0000,0xff000000ff00ff,0xff0000ff000000ff,0xff00ffff00,0xffff00ff0000ff,0xff0000ffffffffff,0xffffffff00ffffff,0xffffffff0000ff,0xff00ffffffffff,0xff0000000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff00ff00ff,0xff00ff0000,0xff000000ff00ff,0xff000000ff,0xff0000ffff,0xffff000000ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff0000000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff0000ffff,0xff0000ff00ffff00,0xffff0000ff00ff,0xffff0000ff,0xff0000ffff,0xffff000000ff,0xff00ff000000ff,0xff00ffff00,0xffff0000ff00ff,0xff00ff000000ff,0xff000000ffffff00,0xff0000ffff,0xffffff0000ff,0xff000000ffffff00,0xffffff000000ffff,0xffffff000000,0xff000000ffffff00,0xffff0000ff00ff,0xff00ffff0000ff,0xff00ffff00,0xffffffff000000ff,0xff00000000,0xff0000ff000000ff,0xff00ff00ff,0xff00ffff0000ff,0xff00ff000000ff,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0x0,0xff000000ff,0xff00000000,0x0,0x0,0x0,0xff0000ff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff000000ff,0xff00000000,0x0,0x0,0x0,0xff0000ff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0xff000000ff,0xffff000000,0x0,0x0,0x0,0xff00ff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0xff0000ff000000,0xff000000000000,0xff00,0x0,0x0,0x0,0xff0000ff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0xffff00000000,0xff00000000000000,0xffffff00000000ff,0x0,0x0,0x0,0xffff00000000,0xff00000000000000,0xffffff,0x0,0x0,0x0,0xff00000000,0x0,0x0,0xff00000000000000,0x0,0xff0000ff00,0x0,0x0,0x0,0x0,0x0,0xff0000ff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000ffff,0xffff00000000,0xffff0000ffff0000,0xffffffff000000ff,0xffffff0000ff,0xffff000000ff0000,0xffffff,0xffffffffff0000,0xffff0000,0xffffff000000ffff,0xffff00000000,0xff000000ff0000,0xff00ff0000,0xff0000,0xff0000000000ff,0xff00000000000000,0xff000000ff0000,0xff0000ff000000,0xff000000ff0000,0xff0000ff00,0x0,0xff000000ff0000,0x0,0xff0000,0xff00000000ff0000,0xffffff0000ff0000,0xff0000ff000000,0xff000000ff0000,0xff0000ff00,0xff0000,0xff0000000000ff,0xff000000000000,0x0,0xff0000,0xff000000000000,0xff00ff0000,0x0,0xff000000000000,0x0,0xff0000,0xff000000000000,0x0,0xff0000,0xff000000000000,0xff000000ff,0xff0000,0xff0000000000ff,0xff0000ffff0000,0xffffff0000000000,0xff0000,0xff0000ffffff00,0xff00ff0000,0xffffff000000,0xff0000ffffff00,0xffffff0000000000,0xff0000,0xff0000ffffff00,0xffff0000000000,0xff00ff,0xff00ff00ffff00,0xff000000ff,0xff00ff,0xff0000000000ff,0xff00ff0000ff00,0xff00000000,0xff00ff,0xff00ff000000ff,0xffffffff00ff0000,0xff000000ff0000,0xffff00ff000000ff,0xff0000ffff,0xffffffff00ff,0xff00ff000000ff,0xff0000ff00000000,0xff00ff,0xffff00ffff0000ff,0xff0000ff00000000,0xff0000,0xff0000000000ff,0xff00ffffffff00,0xff00000000,0xff0000,0xff0000000000ff,0xff00ff0000,0xffffffffff0000,0xff00ffffffffff,0xffffffff00000000,0xff00ff,0xff00ffffffffff,0xff00000000,0xff00ff,0xffff00ff000000ff,0xffffff000000ff,0xff0000,0xff0000000000ff,0xff00ff000000ff,0xff00000000,0xff0000,0xff0000000000ff,0xff00ff0000,0xff0000,0xff0000000000ff,0xff00000000,0xff0000,0xff0000000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff000000ff,0xff0000,0xff,0xff0000ffff0000ff,0xff00ff0000,0xff0000ff0000ff,0xff00ff000000ff,0xff0000ff00,0xff0000,0xff0000000000ff,0xff00000000,0xff0000,0xff000000000000ff,0xff0000ff00ff0000,0xff0000ff0000ff,0xff00ffff0000ff,0xff0000ff0000ff00,0xff0000,0xff0000000000ff,0xff00ffff00,0xffffff000000ffff,0xffff00000000,0xffff0000ffffff00,0xffffffff000000ff,0xffffff0000ff,0xffff0000ffffff00,0xffffff0000ffffff,0xffffffffff00ff,0xffffff00,0xffff0000ffffff,0xffffff000000ff,0xff00ff00ffff00,0xff00ff0000,0xffffffffff00ff,0xff00ffffffffff,0xff000000,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000ff00,0x0,0x0,0x0,0xff,0xff00000000,0x0,0x0,0x0,0xff000000,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000ff00,0x0,0x0,0xff00000000,0xff000000ff,0xff000000ff,0xff0000000000ff,0x0,0xff0000,0xff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000,0xff,0x0,0xffffff0000000000,0xff00000000,0xffffff00,0xff0000000000ff,0x0,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0x0,0xff00000000000000,0xff0000,0x0,0x0,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0x0,0xff,0x0,0x0,0xffff,0xffffff000000,0x0,0xff0000000000ff,0x0,0xffff00,0x0,0x0,0xff00000000000000,0x0,0x0,0xff000000,0xff000000,0xff00000000000000,0x0,0x0,0xff0000000000,0xff,0x0,0x0,0xff0000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff0000ff,0xff000000ff0000,0xff000000ff0000,0xff000000000000,0xffff000000ff0000,0xff00000000,0xffff0000ffffff00,0xffffffff00ffffff,0xff00000000,0xffff0000ffffffff,0xffffffff0000,0x0,0xffff000000000000,0xffff,0xff00ff000000ff00,0xffff0000,0xff000000ff00ff,0xff000000ff0000,0xff000000ff0000,0xff000000000000,0xff000000ff0000,0xff00ff000000,0xff000000000000,0xff00000000,0xff000000ff,0xff00,0xff000000,0xff00000000ff,0xff00000000000000,0x0,0xff00ff000000ff00,0xff000000,0xffff0000000000ff,0xff0000,0xff0000ffff0000,0xffff000000000000,0xff0000,0xff000000ff0000,0xff000000000000,0xff00000000,0xff,0xff00,0xff000000,0xff00ffff00000000,0xff0000000000ff00,0x0,0xffff,0xff000000,0xff0000000000ff,0xffff00ff00ff00ff,0xff00ff00ff0000,0xff0000ffff00ff,0xffff00ff00ff00ff,0xff000000ff0000,0xff0000ffffff00,0xff00000000,0xffff00ff00ff,0xffff000000ff00,0xff000000ff000000,0xffff00000000ff,0xff0000000000ff00,0xffff000000,0xffff000000ff00,0xff000000,0xff0000000000ff,0xffff00ff00ff,0xff00ff00ff00ff,0xff00ff0000ffff,0xffff00ff00ff,0xff000000ff00ff,0xffff00ff000000ff,0xff0000ffff,0xff0000ffff00ff,0xff00000000ff00,0xff000000,0xff00ffff000000ff,0xff0000000000ff00,0xff00000000,0xff00000000ff00,0xff000000,0xff0000000000ff,0xff00ff00ff,0xff00ff00ff00ff,0xff00ff000000ff,0xff00ff00ff,0xff000000ff00ff,0xff00ff000000ff,0xffffffff00000000,0xff0000,0xff00000000ff00,0xff000000,0xff00000000ff,0xff0000ffffffff00,0xff00000000,0xff00000000ff00,0xff000000,0xff0000000000ff,0xff00ffff00,0xffff0000ff00ff,0xff00ff000000ff,0xff00ffff00,0xff000000ff00ff,0xff00ff000000ff,0xff0000ff00000000,0xff0000,0xff00000000ff00,0xff000000,0xff,0xff0000000000ff00,0xff00000000,0xff00000000ff00,0xff000000,0xff0000000000ff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff00ff0000ff,0xff00ff000000ff,0xff0000ff00000000,0xff0000,0xff00000000ff00,0xff000000,0xff,0xff0000000000ff00,0xff00000000,0xff00000000ff00,0xff000000,0xff00ffffffffff,0xff00ff0000,0xff000000ff00ff,0xff00ff000000ff,0xff00ff0000,0xff000000ff,0xffff0000ffffff00,0xff00ffffff,0xff00ff,0xffffff00ffffffff,0xff00ffffffff00ff,0xffffff,0xffff000000000000,0xffffffff00ffff,0xffffff00ffffffff,0xffffff0000ff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0xff00,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0xff00,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff,0x0,0x0,0xff,0xff00000000000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000000000,0x0,0x0,0xff00000000000000,0xffff,0xff00000000000000,0xffff000000ff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000,0x0,0x0,0x0,0xffff00000000,0xff000000000000,0x0,0x0,0x0,0xff0000,0xff0000000000,0xffffff00,0xffffff0000000000,0xff000000000000,0xffff0000ffff00,0xff00ffffffff0000,0xff00ff,0xffff000000ffff00,0xffff,0xffff0000000000,0xff0000ff00000000,0xff0000ffffff,0xffff000000000000,0xffffffff00ffff,0xffff00,0xff000000,0xff0000000000,0xff00,0xff00000000000000,0xff0000000000,0xff0000ff000000,0xff000000,0xffff,0xff00000000ff0000,0xff00000000,0xff0000ff000000,0xff00000000,0xff00000000ff,0xff00000000000000,0xff00000000,0xff0000ff,0xff000000,0xff00ff00000000,0xff00,0xff00000000000000,0xff0000000000,0xff0000ffffff00,0xff000000,0xff00000000ff,0xff000000ffff0000,0x0,0xff000000000000,0xff00000000,0xff,0xff00000000000000,0xff00000000,0xff0000ff,0x0,0x0,0xff0000000000ff00,0xff0000000000ff00,0xff0000000000,0xff0000ff0000ff,0xff000000,0xff00000000ff,0xff00000000ff0000,0xffff000000,0xff0000ffffff00,0xff00000000,0xffff000000ff,0xff000000ff000000,0xff00000000,0xff0000ff,0xff000000ffff0000,0xffff00ff0000,0xff00000000ff00,0xff00000000ff0000,0xff0000000000,0xff0000ffffff00,0xff000000,0xffffffff000000ff,0xff00000000ff0000,0xff00000000,0xff0000ff000000,0xff00000000,0xff00000000ff,0xff00000000000000,0xff00000000,0xffff00,0xff000000,0xff000000ffff,0xff00000000ff00,0xff00000000ff0000,0xff0000000000,0xff0000ffffffff,0xff000000,0xff00000000ff,0xff00000000ffff00,0xff00000000,0xff0000ff000000,0xffffff0000,0xff00000000ff,0xff0000ffffffff00,0xff00000000,0xffffff00ffffffff,0xff0000ff,0xff000000ffff,0xffff000000ff00,0xff000000ffff0000,0xff0000000000,0xff000000000000,0xff000000,0xff,0xff00000000ff0000,0xff00000000,0xff0000ff000000,0xff00000000,0xff00000000ff,0xff000000ff000000,0xff00000000,0x0,0xff000000ff000000,0xff0000ff0000,0xff00000000ff00,0xff00000000ff0000,0xff0000000000,0xff000000000000,0xff000000,0xffffffff000000ff,0xff00000000ff0000,0xff00000000,0xff0000ff000000,0xff00000000,0xff00000000ff,0xff00000000000000,0xff00000000,0x0,0xff000000,0xff0000000000,0xff0000000000ff00,0xff0000000000ff00,0xff0000000000,0xffff000000000000,0xffffffff00ff,0xffffff,0xffff00ffffff0000,0xffffffff00ffff,0xffff0000ff000000,0xff0000ff000000ff,0xffffffff00ffffff,0xffff000000000000,0xffffffff00ffff,0x0,0xffffffff0000,0xffffffff00000000,0xff00,0xff00000000000000,0xff0000000000,0x0,0x0,0x0,0x0,0x0,0xff000000,0xff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0xff00000000000000,0xff0000000000,0xff00000000000000,0x0,0x0,0x0,0x0,0xff0000ff,0xffff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0xffffff00,0xffffff0000000000,0xff000000000000,0xff000000000000,0x0,0x0,0x0,0x0,0xffff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0xff00000000,0xffff0000000000,0xffff00000000ff00,0xff00ffff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xffffff00ff00ff,0xff0000000000,0xff000000000000,0xff0000000000ffff,0xff00ff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00ff,0xff0000000000,0xff000000000000,0xff0000000000ff00,0xff00ff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff00,0xff0000000000,0xff000000000000,0xff00000000ff00,0xff0000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff0000ff000000,0xffff00000000ffff,0xffff00,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff0000ff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff000000000000,0x0,0xff000000000000,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff000000000000,0x0,0x0,0xff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff0000ff0000ff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff0000ff0000ff,0xff00000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000000000,0xff0000ff0000ff,0xffff000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0xff0000ff00000000,0xffff00ffff00ff,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
static int freepixel_char_data[] = {32,121,187,3,16,-1,0,7,33,126,153,1,16,3,0,7,34,26,34,5,16,1,0,7,35,32,34,5,16,1,0,7,36,30,0,6,16,1,0,7,37,37,0,6,16,1,0,7,38,38,34,5,16,1,0,7,39,126,17,1,16,3,0,7,40,125,187,2,16,3,0,7,41,12,204,2,16,2,0,7,42,92,170,4,16,2,0,7,43,97,170,4,16,3,0,7,44,15,204,2,16,2,0,7,45,80,34,5,16,1,0,7,46,126,68,1,16,3,0,7,47,92,34,5,16,1,0,7,48,98,34,5,16,1,0,7,49,112,170,4,16,2,0,7,50,110,34,5,16,1,0,7,51,116,34,5,16,1,0,7,52,122,34,5,16,1,0,7,53,0,51,5,16,1,0,7,54,6,51,5,16,1,0,7,55,12,51,5,16,1,0,7,56,18,51,5,16,1,0,7,57,24,51,5,16,1,0,7,58,126,51,1,16,3,0,7,59,18,204,2,16,2,0,7,60,109,187,3,16,2,0,7,61,48,51,5,16,1,0,7,62,113,187,3,16,2,0,7,63,60,51,5,16,1,0,7,64,98,17,6,16,1,0,7,65,66,51,5,16,1,0,7,66,14,34,5,16,1,0,7,67,78,51,5,16,1,0,7,68,84,51,5,16,1,0,7,69,90,51,5,16,1,0,7,70,96,51,5,16,1,0,7,71,102,51,5,16,1,0,7,72,108,51,5,16,1,0,7,73,10,187,4,16,2,0,7,74,120,51,5,16,1,0,7,75,0,68,5,16,1,0,7,76,6,68,5,16,1,0,7,77,65,0,6,16,1,0,7,78,12,68,5,16,1,0,7,79,18,68,5,16,1,0,7,80,24,68,5,16,1,0,7,81,30,68,5,16,1,0,7,82,36,68,5,16,1,0,7,83,42,68,5,16,1,0,7,84,114,0,6,16,1,0,7,85,48,68,5,16,1,0,7,86,0,17,6,16,1,0,7,87,7,17,6,16,1,0,7,88,54,68,5,16,1,0,7,89,21,17,6,16,1,0,7,90,60,68,5,16,1,0,7,91,105,187,3,16,2,0,7,92,72,68,5,16,1,0,7,93,117,187,3,16,2,0,7,94,56,17,6,16,1,0,7,95,0,0,7,16,0,0,7,96,126,85,1,16,3,0,7,97,90,68,5,16,1,0,7,98,96,68,5,16,1,0,7,99,102,68,5,16,1,0,7,100,72,51,5,16,1,0,7,101,114,68,5,16,1,0,7,102,120,68,5,16,1,0,7,103,0,85,5,16,1,0,7,104,6,85,5,16,1,0,7,105,35,187,4,16,2,0,7,106,40,187,4,16,1,0,7,107,24,85,5,16,1,0,7,108,45,187,4,16,2,0,7,109,44,0,6,16,1,0,7,110,36,85,5,16,1,0,7,111,42,85,5,16,1,0,7,112,48,85,5,16,1,0,7,113,54,85,5,16,1,0,7,114,60,85,5,16,1,0,7,115,66,85,5,16,1,0,7,116,72,85,5,16,1,0,7,117,78,85,5,16,1,0,7,118,84,85,5,16,1,0,7,119,14,17,6,16,1,0,7,120,90,85,5,16,1,0,7,121,96,85,5,16,1,0,7,122,102,85,5,16,1,0,7,123,50,187,4,16,1,0,7,124,126,102,1,16,3,0,7,125,21,204,2,16,2,0,7,126,0,102,5,16,1,0,7,160,8,204,3,16,-1,0,7,161,126,136,1,16,3,0,7,162,6,102,5,16,1,0,7,163,119,17,6,16,1,0,7,164,12,102,5,16,1,0,7,165,7,34,6,16,1,0,7,166,126,119,1,16,3,0,7,167,18,102,5,16,1,0,7,168,24,102,5,16,1,0,7,169,23,0,6,16,1,0,7,170,0,187,4,16,2,0,7,171,30,102,5,16,1,0,7,172,40,204,1,16,5,0,7,173,85,187,4,16,1,0,7,174,86,0,6,16,1,0,7,175,4,204,3,16,2,0,7,176,0,204,3,16,2,0,7,177,20,187,4,16,2,0,7,178,30,204,2,16,2,0,7,179,33,204,2,16,2,0,7,180,36,204,1,16,3,0,7,181,42,17,6,16,1,0,7,182,49,17,6,16,1,0,7,183,38,204,1,16,3,0,7,184,27,204,2,16,2,0,7,185,24,204,2,16,2,0,7,186,80,187,4,16,2,0,7,187,36,102,5,16,1,0,7,188,70,17,6,16,1,0,7,189,16,0,6,16,1,0,7,190,58,0,6,16,1,0,7,191,42,102,5,16,1,0,7,192,48,102,5,16,1,0,7,193,54,102,5,16,1,0,7,194,60,102,5,16,1,0,7,195,66,102,5,16,1,0,7,196,72,102,5,16,1,0,7,197,78,102,5,16,1,0,7,198,0,34,6,16,1,0,7,199,84,102,5,16,1,0,7,200,90,102,5,16,1,0,7,201,96,102,5,16,1,0,7,202,102,102,5,16,1,0,7,203,108,102,5,16,1,0,7,204,70,187,4,16,2,0,7,205,30,187,4,16,2,0,7,206,55,187,4,16,2,0,7,207,75,187,4,16,2,0,7,208,112,17,6,16,1,0,7,209,114,102,5,16,1,0,7,210,120,102,5,16,1,0,7,211,0,119,5,16,1,0,7,212,6,119,5,16,1,0,7,213,12,119,5,16,1,0,7,214,18,119,5,16,1,0,7,215,95,187,4,16,2,0,7,216,24,119,5,16,1,0,7,217,30,119,5,16,1,0,7,218,36,119,5,16,1,0,7,219,42,119,5,16,1,0,7,220,48,119,5,16,1,0,7,221,84,17,6,16,1,0,7,222,54,119,5,16,1,0,7,223,60,119,5,16,1,0,7,224,66,119,5,16,1,0,7,225,72,119,5,16,1,0,7,226,78,119,5,16,1,0,7,227,84,119,5,16,1,0,7,228,90,119,5,16,1,0,7,229,96,119,5,16,1,0,7,230,77,17,6,16,1,0,7,231,102,119,5,16,1,0,7,232,108,119,5,16,1,0,7,233,114,119,5,16,1,0,7,234,120,119,5,16,1,0,7,235,0,136,5,16,1,0,7,236,90,187,4,16,2,0,7,237,60,187,4,16,2,0,7,238,100,187,4,16,2,0,7,239,117,170,4,16,2,0,7,240,6,136,5,16,1,0,7,241,12,136,5,16,1,0,7,242,18,136,5,16,1,0,7,243,24,136,5,16,1,0,7,244,30,136,5,16,1,0,7,245,36,136,5,16,1,0,7,246,42,136,5,16,1,0,7,247,65,187,4,16,1,0,7,248,48,136,5,16,1,0,7,249,54,136,5,16,1,0,7,250,60,136,5,16,1,0,7,251,66,136,5,16,1,0,7,252,72,136,5,16,1,0,7,253,78,136,5,16,1,0,7,254,84,136,5,16,1,0,7,255,90,136,5,16,1,0,7,256,96,136,5,16,1,0,7,257,102,136,5,16,1,0,7,258,108,136,5,16,1,0,7,259,114,136,5,16,1,0,7,260,120,136,5,16,1,0,7,261,0,153,5,16,1,0,7,262,6,153,5,16,1,0,7,263,12,153,5,16,1,0,7,268,18,153,5,16,1,0,7,269,24,153,5,16,1,0,7,270,30,153,5,16,1,0,7,271,8,0,7,16,0,0,7,272,35,17,6,16,1,0,7,273,28,17,6,16,1,0,7,274,36,153,5,16,1,0,7,275,42,153,5,16,1,0,7,278,54,170,5,16,1,0,7,279,48,153,5,16,1,0,7,280,54,153,5,16,1,0,7,281,60,153,5,16,1,0,7,282,66,153,5,16,1,0,7,283,72,153,5,16,1,0,7,286,78,153,5,16,1,0,7,287,84,153,5,16,1,0,7,290,90,153,5,16,1,0,7,291,96,153,5,16,1,0,7,298,72,170,4,16,2,0,7,299,77,170,4,16,2,0,7,302,82,170,4,16,2,0,7,303,87,170,4,16,2,0,7,304,102,170,4,16,2,0,7,305,107,170,4,16,2,0,7,310,102,153,5,16,1,0,7,311,108,153,5,16,1,0,7,313,114,153,5,16,1,0,7,314,122,170,4,16,2,0,7,315,120,153,5,16,1,0,7,316,5,187,4,16,2,0,7,317,0,170,5,16,1,0,7,318,15,187,4,16,2,0,7,321,107,0,6,16,1,0,7,322,25,187,4,16,2,0,7,323,6,170,5,16,1,0,7,324,12,170,5,16,1,0,7,325,18,170,5,16,1,0,7,326,24,170,5,16,1,0,7,327,30,170,5,16,1,0,7,328,36,170,5,16,1,0,7,332,42,170,5,16,1,0,7,333,48,170,5,16,1,0,7,336,121,0,6,16,1,0,7,337,100,0,6,16,1,0,7,338,93,0,6,16,1,0,7,339,79,0,6,16,1,0,7,340,60,170,5,16,1,0,7,341,66,170,5,16,1,0,7,342,20,34,5,16,1,0,7,343,44,34,5,16,1,0,7,344,50,34,5,16,1,0,7,345,56,34,5,16,1,0,7,346,62,34,5,16,1,0,7,347,68,34,5,16,1,0,7,350,74,34,5,16,1,0,7,351,86,34,5,16,1,0,7,352,104,34,5,16,1,0,7,353,30,51,5,16,1,0,7,354,72,0,6,16,1,0,7,355,36,51,5,16,1,0,7,356,51,0,6,16,1,0,7,357,42,51,5,16,1,0,7,362,54,51,5,16,1,0,7,363,114,51,5,16,1,0,7,366,66,68,5,16,1,0,7,367,78,68,5,16,1,0,7,368,63,17,6,16,1,0,7,369,105,17,6,16,1,0,7,370,84,68,5,16,1,0,7,371,108,68,5,16,1,0,7,376,91,17,6,16,1,0,7,377,12,85,5,16,1,0,7,378,18,85,5,16,1,0,7,379,30,85,5,16,1,0,7,380,108,85,5,16,1,0,7,381,114,85,5,16,1,0,7,382,120,85,5,16,1,0,7};

class PixelFont : public roxlu::Font {
 public:

 PixelFont()
   :Font()
    {
      if(!setupGraphics()) {
        printf("Error: cannot setup the graphics for FreePixel.\n");
        ::exit(EXIT_FAILURE);
      }

      line_height = 16;
      scale_w = 128;
      scale_h = 256;
      pages = 1;
      base = 12;

      int num_chars = 276;
      int max_els = num_chars * 8;
      int i = 0;

      while(i < max_els) {
        roxlu::Character c;
        c.id       = freepixel_char_data[i++];
        c.x        = freepixel_char_data[i++];
        c.y        = freepixel_char_data[i++];
        c.width    = freepixel_char_data[i++];
        c.height   = freepixel_char_data[i++];
        c.xoffset  = freepixel_char_data[i++];
        c.yoffset  = freepixel_char_data[i++];
        c.xadvance = freepixel_char_data[i++];
        chars[c.id] = c;
      }

      setupTexture(scale_w, scale_h, (unsigned char*)&freepixel_pixel_data[0]);
    }
};

#  endif // ROXLU_USE_FONT_H
#endif // #if defined(ROXLU_USE_FONT) && defined(ROXLU_USE_OPENGL)

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ M A T H
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_MATH)
#  ifndef ROXLU_USE_OPENGL_MATH_H
#  define ROXLU_USE_OPENGL_MATH_H

struct VertexP {
  VertexP(){};
  VertexP(vec3 p):pos(p){}
  void set(vec3 p) { pos = p; }
  float* ptr() { return pos.ptr(); }
  vec3 pos;
};

struct VertexPC {
  VertexPC(){}
  VertexPC(vec3 pos, vec4 col):pos(pos),col(col){}
  void set(vec3 p, vec4 c) { pos = p; col = c; }
  float* ptr() { return pos.ptr(); }
  vec3 pos;
  vec4 col;
};

struct VertexPT {
  VertexPT(){}
  VertexPT(vec3 p, vec2 t):pos(p),tex(t){}
  void set(vec3 p, vec2 t) { pos = p; tex = t; }
  float* ptr() { return pos.ptr(); }
  void print() { printf("x: %f, y: %f, z: %f, u: %f, v: %f\n", pos.x, pos.y, pos.z, tex.x, tex.y); }
  vec3 pos;
  vec2 tex;
};

struct VertexPT3 {    /* Position Texcoord 3 */
  VertexPT3(){}
  VertexPT3(vec3 p, vec3 t):pos(p),tex(t){}
  void set(vec3 p, vec3 t) { pos = p; tex = t; }
  float* ptr() { return pos.ptr(); }
  void print() { printf("x: %f, y: %f, z: %f, u: %f, v: %f, q: %f\n", pos.x, pos.y, pos.z, tex.x, tex.y, tex.z); }
  vec3 pos;
  vec3 tex;
};

struct VertexPTN {
  VertexPTN() {}
  VertexPTN(vec3 p, vec2 t, vec3 n):pos(p),tex(t),norm(n){}
  void set(vec3 p, vec2 t, vec3 n) { pos = p; tex = t; norm = n; }
  float* ptr() { return pos.ptr(); }
  vec3 pos;
  vec2 tex;
  vec3 norm;
};

struct VertexPN {
  VertexPN(){}
  VertexPN(vec3 p, vec3 n):pos(p),norm(n){}
  void set(vec3 p, vec3 n) { pos = p; norm = n; }
  float* ptr() { return pos.ptr(); }
  vec3 pos;
  vec3 norm;
};

/* Position, Texcoord, Tangent, Normal */
struct VertexPTTN {
  VertexPTTN(){};
  VertexPTTN(vec3 p, vec2 tex, vec4 tan, vec3 norm):pos(p),tex(tex),tan(tan),norm(norm) { }
  void set(vec3 vp, vec2 vtex, vec4 vtan, vec3 vnorm) { pos = vp; tex = vtex; tan = vtan; norm = vnorm; }
  float* ptr() { return pos.ptr(); }
  vec3 pos;
  vec2 tex;
  vec4 tan;      /* w-coordinate defines handedness */
  vec3 norm;
};

class OBJ {
 public:
  struct TRI { int v, t, n, tan; }; /* v = vertex index, t = texcoord index, n = normal index, tan = tangent index */
  struct FACE { TRI a, b, c; };
  struct XYZ {  float x, y, z; };
  struct TEXCOORD { float s, t; };

  bool load(std::string filepath);

  bool hasNormals();
  bool hasTexCoords();
  bool hasTangents();

  template<class T>
    bool copy(T& result);

  void push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexP>& verts);
  void push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPTN>& verts);
  void push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPT>& verts);
  void push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPN>& verts);
  void push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPTTN>& verts);

 private:
  void calculateTangents();

 public:
  std::vector<vec3> vertices;
  std::vector<vec3> normals;
  std::vector<vec2> tex_coords;
  std::vector<vec4> tangents;
  std::vector<OBJ::FACE> faces;
  std::vector<int> indices;
  bool has_texcoords;
  bool has_normals;
  bool has_tangents;
}; // OBJ

template<class T>
inline bool OBJ::copy(T& result) {
  for(std::vector<FACE>::iterator it = faces.begin(); it != faces.end(); ++it) {
    FACE& f = *it;
    push_back(vertices[f.a.v], normals[f.a.n], tex_coords[f.a.t], tangents[f.a.tan], result) ;
    push_back(vertices[f.b.v], normals[f.b.n], tex_coords[f.b.t], tangents[f.b.tan], result) ;
    push_back(vertices[f.c.v], normals[f.c.n], tex_coords[f.c.t], tangents[f.c.tan], result) ;
  }
  return true;
}

inline void OBJ::push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexP>& verts) {
  verts.push_back(VertexP(vert));
}

inline void OBJ::push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPTN>& verts) {
  verts.push_back(VertexPTN(vert, tc, norm));
}

inline void OBJ::push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPT>& verts) {
  verts.push_back(VertexPT(vert, tc));
}

inline void OBJ::push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPN>& verts) {
  verts.push_back(VertexPN(vert, norm));
}

inline void OBJ::push_back(vec3 vert, vec3 norm, vec2 tc, vec4 tan, std::vector<VertexPTTN>& verts) {
  verts.push_back(VertexPTTN(vert, tc, tan, norm));
}

inline bool OBJ::hasNormals() {
  return has_normals;
}

inline bool OBJ::hasTexCoords() {
  return has_texcoords;
}

inline bool OBJ::load(std::string filepath) {

  // are unset below
  has_normals = true;
  has_texcoords = true;
  has_tangents = true;

  std::ifstream ifs;
  ifs.open(filepath.c_str());
  if(!ifs.is_open()) {
    printf("Error: Cannot find .obj file: %s\n", filepath.c_str());
    return false;
  }

  char c;
  std::string line;
  while(std::getline(ifs, line)) {
    std::stringstream ss(line);
    ss >> c;
    if(c == 'v') {
      if(line[1] == ' ') {
        vec3 p;
        ss >> p.x >> p.y >> p.z;
        vertices.push_back(p);
      }
      else if(line[1] == 'n') {
        vec3 p;
        ss >> c;
        ss >> p.x >> p.y >> p.z;
        normals.push_back(p);
      }
      else if (line[1] == 't') {
        vec2 t;
        ss >> c;
        ss >> t.x >> t.y;
        t.y = 1.0f - t.y;
        tex_coords.push_back(t);
      }
    }
    else if(c == 'f') {
      std::string part;
      std::vector<OBJ::TRI> tris;
      while(ss >> part) {
        std::stringstream fss;
        std::string indices[3];
        int dx = 0;
        for(int i = 0; i < part.size(); ++i) {
          if(part[i] == '/') {
            dx++;
            continue;
          }
          indices[dx].push_back(part[i]);
        }
        TRI tri;
        tri.v = atoi(indices[0].c_str()) - 1;
        tri.t = atoi(indices[1].c_str()) - 1;
        tri.n = atoi(indices[2].c_str()) - 1;
        tri.tan = 0;
        tris.push_back(tri);
      }
      if(tris.size() == 3) {
        OBJ::FACE face;
        face.a = tris[0];
        face.b = tris[1];
        face.c = tris[2];
        faces.push_back(face);
      }
      else {
        printf("Error: wrong face indices.\n");
      }
    }
  }

  // we need to have a reserved tangents in either case ...
  tangents.assign(vertices.size(), vec4());

  // create empty tangents when we don't have texcoord / normals that we need to calculate them.
  if(!normals.size() || !tex_coords.size()) {
    has_tangents = false;
  }
  else {
    calculateTangents();
  }

  // create empty texcoords/normals when not found so we just return invalid values but wont crash
  if(!normals.size()) {
    normals.assign(vertices.size(), vec3());
    has_normals = false;
  }
  if(!tex_coords.size()) {
    tex_coords.assign(vertices.size(), vec2());
    has_texcoords = false;
  }

  return true;
} // OBJ::load


/* from: Mathematics for 3D Game Programming and Computer Graphics, 3rd edition, Eric Lengyel */
inline void OBJ::calculateTangents() {

  if(!normals.size() || !tex_coords.size()) {
    printf("Error: cannot calculate tangents because we have no normals and/or texcoords.\n");
    return;
  }

  vec3* tan1 = new vec3[vertices.size() * 2];
  vec3* tan2 = tan1 + vertices.size();
  memset(tan1, 0x00, sizeof(vec3) * vertices.size() * 2);

  for(size_t i = 0; i < faces.size(); ++i) {
    OBJ::FACE& face = faces[i];

    int i1 = face.a.v;
    int i2 = face.b.v;
    int i3 = face.c.v;

    vec3& v1 = vertices[face.a.v];
    vec3& v2 = vertices[face.b.v];
    vec3& v3 = vertices[face.c.v];

    vec2& c1 = tex_coords[face.a.t];
    vec2& c2 = tex_coords[face.b.t];
    vec2& c3 = tex_coords[face.c.t];

    float x1 = v2.x - v1.x;
    float x2 = v3.x - v1.x;

    float y1 = v2.y - v1.y;
    float y2 = v3.y - v1.y;

    float z1 = v2.z - v1.z;
    float z2 = v3.z - v1.z;

    float s1 = c2.x - c1.x;
    float s2 = c3.x - c1.x;

    float t1 = c2.y - c1.y;
    float t2 = c3.y - c1.y;

    float r = 1.0f / (s1 * t2 - s2 * t1);

    vec3 sdir((t2 * x1 - t1 * x2) * r,
              (t2 * y1 - t1 * y2) * r,
              (t2 * z1 - t1 * z2) * r);

    vec3 tdir((s1 * x2 - s2 * x1) * r,
              (s1 * y2 - s2 * y1) * r,
              (s1 * z2 - s2 * z1) * r);

    tan1[i1] += sdir;
    tan1[i2] += sdir;
    tan1[i3] += sdir;

    tan2[i1] += tdir;
    tan2[i2] += tdir;
    tan2[i3] += tdir;
  }

  for(size_t i = 0; i < vertices.size(); ++i) {
    const vec3& n = normalized(normals[i]);
    const vec3& t = tan1[i];
    vec3 tangent = normalized(t - n * dot(n, t));
    float w = (dot(cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;
    tangents[i].set(tangent.x, tangent.y, tangent.z, w);
  }

  delete[] tan1;
  tan1 = NULL;
  tan2 = NULL;

} // OBJ::calculateTangents

/*
  Painter
  --------

  The Painter class is a simple wrapper that mimics intermediate
  mode of openGL draw calls. This Painter class is not meant to be
  used in super optimized applications as using this approach is
  is just not the correct one for optimized draw calls. Though this
  class is probably quite fast as we will use vbo's. Goal of is class
  is to provide a simple way to draw lines, rects, circles etc.. when
  you just want to quickly draw something.

  Contexts
  --------
  The painter is build around the concept of contexts, where a
  context takes care of drawing particular vertex data. At this time
  we only have a VertexPC renderer, which can draw vertices with color.
  When ready, we will also have a VertexPT context that can render
  textures.

  TODO
  ----
  At this point the API might change...

 */

#define PAINTER_CONTEXT_TYPE_PC 1   /* a context that can draw position + colors */
#define PAINTER_CONTEXT_TYPE_PT 2   /* a context that can draw position + texcoords (not yet implemented) */

#define PAINTER_STATE_NONE 0x0000   /* default state; no-fill */
#define PAINTER_STATE_FILL 0x0001   /* draw everything filled */

// -----------------------------------------------------

static const char* PAINTER_VERTEX_PC_VS  = ""
  "#version 330\n"
  ""
  "layout( std140 ) uniform Shared { "
  "  mat4 u_pm; "
  "};"
  ""
  "layout( location = 0 ) in vec4 a_pos; "
  "layout( location = 1 ) in vec4 a_col; "
  "out vec4 v_col;"
  ""
  "void main() {"
  "  gl_Position = u_pm * a_pos; "
  "  v_col = a_col; "
  "}"
  "";

static const char* PAINTER_VERTEX_PC_FS = ""
  "#version 330\n"
  "in vec4 v_col; "
  "layout (location = 0) out vec4 fragcolor; "
  ""
  "void main() {"
  " fragcolor = v_col; "
  "}"
  "";

// -----------------------------------------------------

static const char* PAINTER_VERTEX_PT_VS = ""
  "#version 330\n"
  ""
  "layout( std140 ) uniform Shared { "
  "  mat4 u_pm; "
  "};"
  ""
  "layout( location = 0 ) in vec4 a_pos; "
  "layout( location = 1 ) in vec2 a_tex; "
  "out vec2 v_tex;"
  ""
  "void main() {"
  "   gl_Position = u_pm * a_pos; "
  "   v_tex = a_tex; "
  "}"
  "";

static const char* PAINTER_VERTEX_PT_SAMPLER2D_FS = ""
  "#version 330\n"
  "uniform sampler2D u_tex; "
  "in vec2 v_tex; "
  "layout( location = 0 ) out vec4 fragcolor; "
  ""
  "void main() {"
  "  fragcolor = texture(u_tex, v_tex);"
  "}"
  "";

// -----------------------------------------------------

class PainterCommand {
 public:
  GLenum type;
  int offset;
  int count;
  GLuint tex; /* the texture, in case we need to draw a texture */
};

// -----------------------------------------------------

class Painter;

class PainterContextPC {

public:
  PainterContextPC(Painter& painter);                             /* a PainterContext is used to render specific vertex data. This context renders only colors */
  void clear();                                                   /* clear all vertices */
  void update();                                                  /* updates the vbo if necessary */
  void draw();                                                    /* draws the buffers and commands to screen */
  void rect(float x, float y, float w, float h);                  /* create a rectangular shape */
  void circle(float x, float y, float radius);                    /* create a circle */
  void line(float x0, float y0, float x1, float y1);              /* create a line */
  void command(GLenum cmd, std::vector<VertexPC>& vertices);      /* adds a command (Painter calls this when using begin()/end() */

public:
  Painter& painter;                                               /* reference to the main Painter object */
  std::vector<PainterCommand> commands;                           /* the command that we render */
  std::vector<VertexPC> vertices;                                 /* the vertices... also stored in vbo */
  size_t allocated;                                               /* number of bytes allocated in our vbo */
  bool needs_update;                                              /* is set to true whenever our vbo needs to be updated */
  GLuint vao;                                                     /* the vao */
  GLuint vbo;                                                     /* vbo that holds our data on the gpu */
  GLuint vert;                                                    /* our vertex shader, see PAINTER_VERTEX_PC_VS */
  GLuint frag;                                                    /* our fragment shader, see PAINTER_VERTEX_PC_FS */
  GLuint prog;                                                    /* the shader program, used to render our vbo */
};

// -----------------------------------------------------

class PainterContextPT {

 public:
  PainterContextPT(Painter& painter);                             /* a PainterContextPT is used to render textures */
  void clear();                                                   /* clear all vertices */
  void update();                                                  /* updates the vbo with VertexPT data */
  void draw();                                                    /* draws the texture */
  void texture(GLuint tex, float x, float y, float w, float h);   /* add anther texture that should be drawn */

 public:
  Painter& painter;                                               /* reference to the main Painter object */
  std::vector<PainterCommand> commands;                           /* the commands/texture draw calls. the type field contains the texture type; at this moment of writing only GL_TEXTURE_2D is supported */
  std::vector<VertexPT> vertices;                                 /* the vertices we draw */
  size_t allocated;                                               /* number of bytes we've allocated in the vbo */
  bool needs_update;                                              /* is set to true whenever we need to udpate the vbo; is set in texture() */
  GLuint vao;                                                     /* our vao, that contains the vertex info */
  GLuint vbo;                                                     /* reference to our gpu vbo */
  GLuint vert;                                                    /* vertex shader for vertexpt data */
  GLuint frag;                                                    /* fragment shader for the vertexpt data */
  GLuint prog;                                                    /* the shader program that draws the texture */
};

// -----------------------------------------------------

// Shared GL data
struct PainterShared {
  mat4 pm;
};

// -----------------------------------------------------

class Painter {

 public:
  Painter();
  void clear();                                                                     /* clear all vertices */
  void draw();                                                                      /* draw all the added lines, circles, textures etc.. */
  void rect(float x, float y, float w, float h);                                    /* draw a rectangle */
  void circle(float x, float y, float radius);                                      /* draw a circle, see resolution() to change the resolution of the circle */
  void line(float x0, float y0, float x1, float y1);                                /* draw a single line, see begin()/end() if you want to draw line strips */
  void texture(GLuint tex, float x, float y, float w, float h);                     /* draw a texture, at the time of writing only GL_TEXTURE_2D is supported */

  void begin(GLenum type);                                                          /* begin a batch of vertices, just like the old days  */
  void vertex(float x, float y);                                                    /* add a vertex to the current batch */
  void end();                                                                       /* end the current batch; this will flush the added vertices into the correct context */

  void color(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0);           /* set the color of lines, circles, rectangles */
  void fill();                                                                      /* enable fill mode */
  void nofill();                                                                    /* disable fill mode */
  void resize(int w, int h);                                                        /* whenever your viewport changes, call this so we can recalculate the projection matrix */
  void resolution(int n);                                                           /* set the circle resolution */

  int width();                                                                      /* returns the last set/calculated viewport width */
  int height();                                                                     /* returns the last set/calculated viewport height */

 public:
  PainterContextPC context_pc;                                                      /* context used to draw VertexPC vertices (color) */
  PainterContextPT context_pt;                                                      /* context used to draw VertexPT vertices (textures) */
  int circle_resolution;                                                            /* the last set circle resolution */
  vec4 col;                                                                         /* the color we use to draw with */
  int state;                                                                        /* keeps state of the painter; e.g. fill/nofill */
  GLuint ubo;                                                                       /* shared uniform buffer object with matrice(s) */
  PainterShared ubo_data;                                                           /* the actual data for our ubo */
  std::vector<vec2> circle_data;                                                    /* pre-calculated sin/cos values for our circle; just a tiny bit of optimization */
  int win_w;                                                                        /* the last calculated or set viewport width */
  int win_h;                                                                        /* the last calculated or set viewport height */

  /* begin() - end() handling */
  GLenum command_type;                                                              /* when begin() is called we store the command type. the next call to vertex defines what PainterContext we will use; for now only default is used */
  int context_type;                                                                 /* when begin()/end() is used, a call to one of the overloaded vertex() function will define what context will be used. in end() we add the added vertices to the correct context */
  std::vector<VertexPC> vertices_pc;                                                /* VertexPC vertices for our PC context */
};

#  endif // ROXLU_USE_OPENGL_MATH_H
#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_MATH)

// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ P N G
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_PNG)
#  ifndef ROXLU_USE_OPENGL_PNG_H
#  define ROXLU_USE_OPENGL_PNG_H

extern GLuint rx_create_texture(std::string filepath, int internalFormat = -1, int format = -1, int type = -1);
extern bool rx_create_png_screenshot(std::string filepath);

#  endif // ROXLU_USE_OPENGL_PNG_H
#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_PNG)


// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ J P G
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_JPG)
#  ifndef ROXLU_USE_OPENGL_JPG_H
#  define ROXLU_USE_OPENGL_JPG_H

extern bool rx_create_jpg_screenshot(std::string filepath, int quality = 80);

#  endif // ROXLU_USE_OPENGL_JPG_H
#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_JPG)


// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ A U D I O
// ------------------------------------------------------------------------------------

#if defined(ROXLU_USE_AUDIO)
#  ifndef ROXLU_USE_AUDIO_H
#  define ROXLU_USE_AUDIO_H
#  include <sndfile.h>
#  include <cubeb/cubeb.h>
#  if defined(__APPLE__) or defined(__linux)
#    include <pthread.h>
#  endif

/*

  AudioPlayer
  -----------

  The audio player uses libsndfile and libcubeb (Firefox' audio output layer) to
  load and playback audio files. At this moment we only support audio files with
  a samplerate of 44100, 2 channels.

  On Mac you must link with:
  --------------------------
  - cubeb (https://github.com/kinetiknz/cubeb)
  - pthreads
  - AudioUnit framework
  - CoreAudio framework
  - AudioToolbox framework

  On Linux you must link with (depending how you compiled libsndfile):
  -------------------------------------------------------------------
  -lz
  -lpthread
  -logg
  -lvorbis
  -lvorbisenc
  -lFLAC
  -ldl
  -lasound


  High level description:
  -----------------------
  AudioPlayer:         interface that is supposed to use to add audio files and play them
  AudioFile:           holds a buffer (data) with float (2-channel) audio
  AudioPlaybackState:  a user can play the same audio file multiple times; this object keeps
                       track of the current read position in the AudioFile.data member

  ````c++
  AudioPlayer player;

  player.add(0, "boing.wav");
  player.add(1, "bleep.wav");
  player.add(2, "zweeep.wav");

  AudioPlaybackState* state = player.play(0);
  if(state) {
     state->volume(0.5);
  }
  ````

*/

class AudioPlayer;

long audioplayer_data_cb(cubeb_stream* stm, void* user, void* buffer, long nframes);      /* The callback that gets called by cubeb that whenever it wants some more audio data */
void audioplayer_state_cb(cubeb_stream* stm, void* user, cubeb_state state);              /* Is called whenever a stream starts or stops */

/* ----------------------------------- */

class AudioFile {                                                                         /* An AudioFile will hold the raw loaded audio data */
 public:
  AudioFile();
  bool load(std::string filepath);                                                        /* Loads the given audio filepath usign libsnd file */

 public:
  SNDFILE* sf;                                                                            /* Handle to the sound file */
  SF_INFO info;                                                                           /* Contains information about the loaded sound file. See libsndfile documentation for more info */
  std::vector<float> data;                                                                /* The raw audio data */
};

/* ----------------------------------- */

class AudioPlaybackState {                                                                /* Everytime you call AudioPlayer::play() we create a new AudioPlaybackState instance that keeps track of the read position for this sample */
 public:
  AudioPlaybackState(int name, AudioFile* file);                                          /* C'tor; pass the name of the audio file and a pointer to the actual AudioFile itself that contains the audio data */
  ~AudioPlaybackState();
  void volume(float level) ;                                                              /* Set the volume: 0 - off, 1.0 full */

 public:
  AudioFile* file;                                                                        /* Pointer to the audio file */
  size_t read_pos;                                                                        /* The current read position in the file->data member. */
  float volume_level;                                                                     /* Each playstate keeps track of it's own volume */
  int name;                                                                               /* The name of the audio file that we play (a redundant, easy helper) */
};

/* ----------------------------------- */

class AudioPlayer {                                                                       /* The AudioPlayer manages all the audio files and states. */
 public:
  AudioPlayer();                                                                          /* C'tor, creates the necessary cubeb context and stream. */
  ~AudioPlayer();                                                                         /* D'tor, cleans up all allocated members. */
  bool add(int name, std::string filepath);                                               /* Add a new audio file for the given name and filepath. Note that we use libsndfile to load the audio files. */
  AudioPlaybackState* play(int name, float volume = 1.0);                                 /* Playback the given audio name. We return the state object on which you can set the volume. On error we return NULL. */
  void lock();                                                                            /* Locks a mutex; used to sync access to the play_states member. */
  void unlock();                                                                          /* Unlocks a mutex; "" "" "" */

 public:
  cubeb* cube_ctx;                                                                        /* The cubeb state */
  cubeb_stream* cube_stream;                                                              /* The cubeb_stream to playback the audio samples. We only have one stream. */
  std::map<int, AudioFile*> files;                                                        /* The added files */
  std::vector<AudioPlaybackState*> play_states;                                           /* The playback states; each state keeps track of what frames to play and the volume */

#if defined(__APPLE__) or defined(__linux)
  pthread_mutex_t mutex;                                                                  /* Used to lock the mutex to protect the play_states member. */
#endif
};

#  endif // ROXLU_USE_AUDIO_H
#endif // defined(ROXLU_USE_AUDIO)


// ------------------------------------------------------------------------------------
//                              R O X L U _ U S E _ L O G
// ------------------------------------------------------------------------------------
#if defined(ROXLU_USE_LOG)
#  ifndef ROXLU_USE_LOG_H
#  define ROXLU_USE_LOG_H

#define RX_LOG_LEVEL_ALL 4
#define RX_LOG_LEVEL_ERROR  1
#define RX_LOG_LEVEL_WARNING 2
#define RX_LOG_LEVEL_VERBOSE 3

#if defined(_MSC_VER)
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); }
#  define RX_WARNING(fmt, ...) { rx_warning(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); }
#  define RX_ERROR(fmt, ...) { rx_error(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); }
#else
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); }
#  define RX_WARNING(fmt, ...) { rx_warning(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); }
#  define RX_ERROR(fmt, ...) { rx_error(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); }
#endif

/* --------------------------------------------------------------------------------- */

int rx_log_init(std::string path = "");
void rx_log_disable_stdout();
void rx_log_enable_stdout();
void rx_log_set_level(int level);
void rx_verbose(int line, const char* function, const char* fmt, ...);
void rx_warning(int line, const char* function, const char* fmt, ...);
void rx_error(int line, const char* function, const char* fmt, ...);

/* --------------------------------------------------------------------------------- */

class Log {
 public:
  Log();
  ~Log();
  int open(std::string filepath);
  void log(int level, int line, const char* function, const char* fmt, va_list args);

 public:
  bool write_to_stdout;
  int level;                   /* what level we should log. */

 private:
  std::string filepath;        /* filepath where we save the log file. */
  std::ofstream ofs;           /* the output file stream */
};

/* --------------------------------------------------------------------------------- */

extern Log rx_log;

#  endif // ROXLU_USE_LOG_H
#endif // defined(ROXLU_USE_LOG)


// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                          I M P L E M E N T A T I O N
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// ====================================================================================
//                              T I N Y L I B
// ====================================================================================

#if defined(ROXLU_IMPLEMENTATION)

#if defined(_WIN32)
extern std::string rx_get_exe_path() {
  char buffer[MAX_PATH];

  // Try to get the executable path with a buffer of MAX_PATH characters.
  DWORD result = ::GetModuleFileNameA(nullptr, buffer, static_cast<DWORD>(MAX_PATH));
  if(result == 0) {
    return "";
  }

  std::string::size_type pos = std::string(buffer).find_last_of( "\\/" );

  return std::string(buffer).substr(0, pos) +"\\";
}
#elif defined(__APPLE__)
extern std::string rx_get_exe_path() {
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
#elif defined(__linux)
extern std::string rx_get_exe_path() {
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
#endif // rx_get_exe_path() / win32

extern bool rx_file_exists(std::string filepath) {

#if defined(_WIN32)
  char* lptr = (char*)filepath.c_str();
  DWORD dwattrib = GetFileAttributes(lptr);
  return (dwattrib != INVALID_FILE_ATTRIBUTES && !(dwattrib & FILE_ATTRIBUTE_DIRECTORY));

#elif defined(__APPLE__)
  int res = access(filepath.c_str(), R_OK);
  if(res < 0) {
    return false;
  }
#else
  struct stat buffer;
  return (stat(filepath.c_str(), &buffer) == 0);
#endif

  return true;
}

#if !defined(WIN32)
extern bool rx_is_dir(std::string filepath) {
  struct stat st;
  int result = stat(filepath.c_str(), &st);

  if(result < 0) {
    if(errno == EACCES) {
      printf("EACCESS: no permission for: %s", filepath.c_str());
    }
    else if(errno == EFAULT) {
      printf("EFAULT: bad address, for: %s", filepath.c_str());
    }
    else if(errno == ELOOP) {
      printf("ELOOP: too many links, for: %s", filepath.c_str());
    }
    else if(errno == ENAMETOOLONG) {
      printf("ENAMETOOLONG: for: %s", filepath.c_str());
    }
    else if(errno == ENOENT) {
      // we expect this when the dir doesn't exist
      return false;
    }
    else if(errno == ENOMEM) {
      printf("ENOMEM: for: %s", filepath.c_str());
    }
    else if(errno == ENOTDIR) {
      printf("ENOTDIR: for: %s", filepath.c_str());
    }
    else if(errno == EOVERFLOW) {
      printf("EOVERFLOW: for: %s", filepath.c_str());
    }

    return false;
  }

#if defined(__APPLE__) or defined(__linux__)
  return S_ISDIR(st.st_mode);
#else
  return result == 0;
#endif
}
#endif // !defined(WIN32) for rx_is_dir()

extern std::string rx_to_data_path(const std::string filename) {
  std::string exepath = rx_get_exe_path();

#if defined(__APPLE__)
  if(rx_is_dir(exepath +"data")) {
    exepath += "data/" +filename;
  }
  else if(rx_is_dir(exepath +"../MacOS")) {
    exepath += "../../../data/" +filename;
  }
  else {
    exepath += filename;
  }
#else
  exepath += "data/" +filename;
#endif

  return exepath;
}

/* See http://stackoverflow.com/questions/4021479/getting-file-modification-time-on-unix-using-utime-in-c */
/* returns the unix epoc time stamp in nano seconds */
extern uint64_t rx_get_file_mtime(std::string filepath) {
#if defined(_WIN32)
  struct _stat statbuf;
  if (_stat(filepath.c_str(), &statbuf) == -1) {
    return 0;
  }
  return statbuf.st_mtime * 1e6;
#else
  struct stat statbuf;
  if (stat(filepath.c_str(), &statbuf) == -1) {
    return 0;
  }
  return statbuf.st_mtime * 1e6;
#endif
}

extern std::string rx_string_replace(std::string str, std::string from, std::string to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos) {
    return str;
  }
  str.replace(start_pos, from.length(), to);
  return str;
}

extern std::string rx_string_replace(std::string str, char from, char to) {
  std::replace(str.begin(), str.end(), from, to);
  return str;
}

extern std::string rx_strip_filename(std::string path) {
  std::string directory;
  path = rx_string_replace(path, '\\', '/');
  path = rx_string_replace(path, "//", "/");
  const size_t last_slash_idx = path.rfind('/');

  if(std::string::npos != last_slash_idx) {
    directory = path.substr(0, last_slash_idx + 1);
  }

#if defined(_WIN32)
  directory = rx_string_replace(directory, '/', '\\');
#endif

  return directory;
}

extern std::string rx_strip_file_ext(std::string path) {
  std::string ext = "." +rx_get_file_ext(path);
  std::string result = rx_string_replace(path, ext, "");
  return result;
}

extern std::string rx_strip_dir(std::string path) {
  std::string filename;;
  path = rx_string_replace(path, '\\', '/');
  path = rx_string_replace(path, "//", "/");

  const size_t last_slash_idx = path.rfind('/');
  if(last_slash_idx == std::string::npos) {
    return path;
  }

  if(std::string::npos != last_slash_idx) {
    filename = path.substr(last_slash_idx + 1, path.size());
  }

  return filename;
}

extern bool rx_create_dir(std::string path) {
#ifdef _WIN32
  if(_mkdir(path.c_str()) != 0) {
    if(errno == ENOENT) {
      printf("Cannot create directory: %s (ENOENT)\n", path.c_str());
      return false;
    }
    else if(errno == EEXIST) {
      printf("Cannot create directory: %s (EEXIST)\n", path.c_str());
    }
  }
  return true;

#else
  if(mkdir(path.c_str(), 0777) != 0) {
    return false;
  }
  return true;
#endif
}

extern std::string rx_get_file_ext(std::string filepath) {
  size_t pos = filepath.rfind(".");
  std::string result = "";

  if(pos == std::string::npos) {
    return result;
  }

  std::string ext = filepath.substr(pos + 1, (filepath.size() - pos));
  return ext;
} // rx_get_file_ext()

extern bool rx_create_path(std::string path) {

#ifdef _WIN32
  std::string drive;
  for(int i = 0; i < path.size()-1; ++i) {
    if(path[i] == ':' && path[i + 1] == '\\') {
      break;
    }
    drive.push_back(path[i]);
  }
  path = path.substr(drive.size() + 2);
  drive = drive + ":";
#endif

  std::vector<std::string> dirs;
  while(path.length() > 0) {


#ifdef _WIN32
    int index = path.find('\\'); // win
#else
    int index = path.find('/'); // posix
#endif
    std::string dir = (index == -1 ) ? path : path.substr(0, index);

    if(dir.length() > 0) {
      dirs.push_back(dir);
    }
    if(index + 1 >= path.length() || index == -1) {
      break;
    }
    path = path.substr(index + 1);
  }

  struct stat s;
  std::string dir_path;

#ifdef _WIN32
  dir_path = drive;
#endif

  for(unsigned int i = 0; i < dirs.size(); i++) {

#ifdef _WIN32
    dir_path += "\\";
#else
    dir_path += "/";
#endif

#if !defined(_WIN32)
    dir_path += dirs[i];
    if(stat(dir_path.c_str(), &s) != 0) {
      if(!rx_create_dir(dir_path.c_str())) {
        printf("ERROR: cannot create directory: %s\n", dir_path.c_str());
        return false;
      }
    }
#endif
  }

#if defined(_WIN32)
   printf("WARNING: We have not yet implemented rx_create_path() on windows.\n");
#endif

  return true;
} // rx_create_path()

/* using * for ext will return all images */
extern std::vector<std::string> rx_get_files(std::string path, std::string ext) {
  std::vector<std::string> result;
#if !defined(_WIN32)
  DIR* dir;
  struct dirent* ent;
  if((dir = opendir(path.c_str())) != NULL) {
    while((ent = readdir(dir)) != NULL) {
      if(ent->d_type == DT_REG) {
        std::string file_path = path +"/" +ent->d_name;

        if(ext != "*" && ext.size()) {
          std::string file_ext = rx_get_file_ext(file_path);
          if(file_ext != ext) {
            continue;
          }
        }

        result.push_back(file_path);
      }
    }
    closedir(dir);
  }
#else
  printf("WARNING: We have not yet implemented rx_get_files on windows.\n");
#endif
  return result;
} // rx_get_files()

extern std::string rx_norm_path(std::string path) {
#if defined(_WIN32)
  std::string from = "/";
  std::string to = "\\";
  size_t start_pos = 0;
  while((start_pos = path.find(from, start_pos)) != std::string::npos) {
    path.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return path;
#else
  return path;
#endif
} // rx_norm_path()


extern int rx_to_int(const std::string& v) {
  int r = 0;
  std::stringstream ss(v);
  ss >> r;
  return r;
}

extern float rx_to_float(const std::string& v) {
  float r = 0.0f;
  std::stringstream ss(v);
  ss >> r;
  return r;
}

extern std::string rx_int_to_string(const int& v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

extern std::string rx_float_to_string(const float& v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

extern std::vector<std::string> rx_split(std::string str, char delim) {
  std::string line;
  std::stringstream ss(str);
  std::vector<std::string> result;
  while(std::getline(ss, line, delim)) {
    result.push_back(line);
  }
  return result;
}

extern uint64_t rx_hrtime() {
#if defined(__APPLE__)
  mach_timebase_info_data_t info;
  if(mach_timebase_info(&info) != KERN_SUCCESS) {
    abort();
  }
  return mach_absolute_time() * info.numer / info.denom;

#elif defined(__linux)
  static clock_t fast_clock_id = -1;
  struct timespec t;
  clock_t clock_id;

  if(fast_clock_id == -1) {
    if(clock_getres(CLOCK_MONOTONIC_COARSE, &t) == 0 && t.tv_nsec <= 1 * 1000 * 1000LLU) {
      fast_clock_id = CLOCK_MONOTONIC_COARSE;
    }
    else {
      fast_clock_id = CLOCK_MONOTONIC;
    }
  }

  clock_id =  CLOCK_MONOTONIC;
  if(clock_gettime(clock_id, &t)) {
    return 0;
  }

  return t.tv_sec * (uint64_t)1e9 +t.tv_nsec;

#elif defined(_WIN32)
  LARGE_INTEGER timer_freq;
  LARGE_INTEGER timer_time;
  QueryPerformanceCounter(&timer_time);
  QueryPerformanceFrequency(&timer_freq);
  static double freq = (double)timer_freq.QuadPart/(double)1000000000;
  return (uint64_t)((double)timer_time.QuadPart / freq);
#endif
};

extern float rx_millis() {
  static uint64_t start = rx_hrtime();
  int64_t d = (rx_hrtime() - start);
  return d / 1000000000.0;
}

extern std::string rx_read_file(std::string filepath) {
  std::ifstream ifs(filepath.c_str(), std::ios::in);
  if(!ifs.is_open()) {
    return "";
  }
  std::string str((std::istreambuf_iterator<char>(ifs)) , std::istreambuf_iterator<char>());
  return str;
}

extern std::string rx_strftime(const std::string fmt) {
  time_t t;
  struct tm* info;
  char buf[4096]; // must be enough..
  time(&t);
  info = localtime(&t);
  strftime(buf, 4096, fmt.c_str(), info);
  std::string result(buf);
  return result;
}

extern std::string rx_get_time_string() {
  double millis = double(rx_hrtime()) / 1000000000.0;
  uint64_t intpart = (millis - (int)millis) * 1000;
  std::string millis_str;
  std::stringstream ss;
  ss << intpart;
  ss >> millis_str;
  return rx_strftime("%Y.%m.%d_%H.%M.%S_") +millis_str;
}

extern std::string rx_get_date_string() {
  return rx_strftime("%Y.%m.%d");
}

extern int rx_get_year() {
  return rx_to_int(rx_strftime("%Y"));
}

extern int rx_get_month() {
  return rx_to_int(rx_strftime("%m"));
}

extern int rx_get_day() {
  return rx_to_int(rx_strftime("%d"));
}

extern int rx_get_hour() {
  return rx_to_int(rx_strftime("%H"));
}

extern int rx_get_minute() {
  return rx_to_int(rx_strftime("%M"));
}

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
|| defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
+(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

extern uint32_t rx_string_id(const std::string& v) {
  const char* data = v.c_str();
  int len = v.size();
  uint32_t hash = len, tmp;
  int rem;

  if (len <= 0 || data == NULL) {
    return 0;
  }

  rem = len & 3;
  len >>= 2;

  /* Main loop */
  for (;len > 0; len--) {
    hash  += get16bits (data);
    tmp    = (get16bits (data+2) << 11) ^ hash;
    hash   = (hash << 16) ^ tmp;
    data  += 2*sizeof (uint16_t);
    hash  += hash >> 11;
  }

  /* Handle end cases */
  switch (rem) {
    case 3: hash += get16bits (data);
      hash ^= hash << 16;
      hash ^= data[sizeof (uint16_t)] << 18;
      hash += hash >> 11;
      break;
    case 2: hash += get16bits (data);
      hash ^= hash << 11;
      hash += hash >> 17;
      break;
    case 1: hash += *data;
      hash ^= hash << 10;
      hash += hash >> 1;
  }

  /* Force "avalanching" of final 127 bits */
  hash ^= hash << 3;
  hash += hash >> 5;
  hash ^= hash << 4;
  hash += hash >> 17;
  hash ^= hash << 25;
  hash += hash >> 6;

  return hash;
}

#endif // defined(ROXLU_IMPLEMENTATION)

// ====================================================================================
//                              R O X L U _ U S E _ M A T H
// ====================================================================================
#if defined(ROXLU_USE_CURL) && defined(ROXLU_IMPLEMENTATION)

size_t rx_curl_write_string_data(void* ptr, size_t size, size_t nmemb, void* str) {
  std::string* s = static_cast<std::string*>(str);
  std::copy((char*)ptr, (char*)ptr + (size * nmemb), std::back_inserter(*s));
  return size * nmemb;
}

size_t rx_curl_write_file_data(void* ptr, size_t size, size_t nmemb, void* fpptr) {
  FILE* fp = static_cast<FILE*>(fpptr);
  size_t written = fwrite(ptr, size, nmemb, fp);
  return written;
}

bool rx_fetch_url(std::string url, std::string& result) {

  CURL* curl = NULL;
  CURLcode res;

  curl = curl_easy_init();
  if(!curl) {
    printf("Error: cannot intialize curl.\n");
    return false;
  }

  res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  RX_CHECK_CURLCODE(res, "Cannot set url");

  res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  RX_CHECK_CURLCODE(res, "Cannot set follow location");

  res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);
  RX_CHECK_CURLCODE(res, "Cannot set write data");

  res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rx_curl_write_string_data);
  RX_CHECK_CURLCODE(res, "Cannot set write function");

  res = curl_easy_perform(curl);
  RX_CHECK_CURLCODE(res, "Cannot easy perform");

  curl_easy_cleanup(curl);
  curl = NULL;

  return true;
}

bool rx_download_file(std::string url, std::string filepath) {

  FILE* fp = NULL;
  CURL* curl = NULL;
  CURLcode res;

  fp = fopen(filepath.c_str(), "wb");
  if(!fp) {
    printf("Error: cannot open file: %s\n", filepath.c_str());
    return false;
  }

  curl = curl_easy_init();
  if(!curl) {
    printf("Error: cannot initialize cur.\n");
    return false;
  }

  res = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  RX_CHECK_CURLCODE(res, "Cannot set url");

  res = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  RX_CHECK_CURLCODE(res, "Cannot set follow location");

  res = curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
  RX_CHECK_CURLCODE(res, "Cannot set write data");

  res = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, rx_curl_write_file_data);
  RX_CHECK_CURLCODE(res, "Cannot set write function");

  res = curl_easy_perform(curl);
  RX_CHECK_CURLCODE(res, "Cannot easy perform");

  curl_easy_cleanup(curl);
  curl = NULL;

  fclose(fp);
  fp = NULL;

  return true;
}

#endif

// ====================================================================================
//                              R O X L U _ U S E _ M A T H
// ====================================================================================
#if defined(ROXLU_USE_MATH) && defined(ROXLU_IMPLEMENTATION)

extern float rx_random(float max) {
  return max * rand() / (RAND_MAX + 1.0f);
}

extern float rx_random(float x, float y) {
  float high = 0;
  float low = 0;
  float result = 0;

  high = std::max<float>(x,y);
  low = std::min<float>(x,y);
  result = low + ((high-low) * rand()/(RAND_MAX + 1.0));
  return result;
}

// all in range 0 - 1
extern void rx_rgb_to_hsv(float r, float g, float b, float& h, float& s, float& v) {
  float K = 0.f;

  if(g < b) {
    std::swap(g, b);
    K = -1.f;
  }

  if(r < g) {
    std::swap(r, g);
    K = -2.f / 6.f - K;
  }

  float chroma = r - std::min<float>(g, b);
  h = fabs(K + (g - b) / (6.f * chroma + 1e-20f));
  s = chroma / (r + 1e-20f);
  v = r;
}

extern void rx_rgb_to_hsv(vec3 rgb, vec3& hsv) {
  rx_rgb_to_hsv(rgb.x, rgb.y, rgb.z, hsv.x, hsv.y, hsv.z);
}

extern void rx_rgb_to_hsv(vec3 rgb, float* hsv) {
  rx_rgb_to_hsv(rgb.x, rgb.y, rgb.z, hsv[0], hsv[1], hsv[2]);
}

extern void rx_rgb_to_hsv(float* rgb, float* hsv) {
  rx_rgb_to_hsv(rgb[0], rgb[1], rgb[2], hsv[0], hsv[1], hsv[2]);
}

extern void rx_hsv_to_rgb(float h, float s, float v, float& r, float& g, float& b) {
  float tmp_r = CLAMP((-1.0f + fabs(6.0f * h - 3.0f)), 0,1);
  float tmp_g = CLAMP(( 2.0f - fabs(6.0f * h - 2.0f)), 0,1);
  float tmp_b = CLAMP(( 2.0f - fabs(6.0f * h - 4.0f)), 0,1);
  float p = 1.0f - s;
  r = v * (p + tmp_r * s);
  g = v * (p + tmp_g * s);
  b = v * (p + tmp_b * s);
}

extern void rx_hsv_to_rgb(vec3 hsv, vec3& rgb) {
  rx_hsv_to_rgb(hsv.x, hsv.y, hsv.z, rgb.x, rgb.y, rgb.z);
}

extern void rx_hsv_to_rgb(vec3 hsv, float* rgb) {
  rx_hsv_to_rgb(hsv.x, hsv.y, hsv.z, rgb[0], rgb[1], rgb[2]);
}

extern void rx_hsv_to_rgb(float* hsv, float* rgb) {
  rx_hsv_to_rgb(hsv[0], hsv[1], hsv[2], rgb[0], rgb[1], rgb[2]);
}


#endif // defined(ROXLU_USE_MATH) && defined(ROXLU_IMPLEMENTATON)

// ====================================================================================
//                              R O X L U _ U S E _ J P G
// ====================================================================================

#if defined(ROXLU_USE_JPG) && defined(ROXLU_IMPLEMENTATION)

/*

  @param std::string filepath      - The file to laod
  @param unsigned char** pix       - A reference to the buffer that will be set to the
                                     memory we allocate OR that we will reallocate (if needed).
                                     We will only reallocate when the *allocated parameter is
                                     passed.
  @param int& width                - Will be set to the width of the image.
  @param int& height               - Will be set to the height of the image
  @param int& nchannels            - Will be set to the number of color channels in the image
  @param int* allocated (NULL)     - Can be NULL, if not it must be set to the number of bytes in *pix.
                                     When the number of allocated bytes is less then what we need, we will
                                     reallocate the buffer.
  @return int                      - The number of bytes of the image buffer (doesn't have to be the same
                                     as the number of bytes that was previously allocated!).
                                   - on error we return a negative value.

 */
int rx_load_jpg(std::string filepath, unsigned char** pix, int& width, int& height, int& nchannels, int* allocated) {

  struct jpeg_error_mgr jerr;
  struct jpeg_decompress_struct cinfo;
  FILE* fp;
  JSAMPARRAY buffer;
  int stride = 0;
  int num_bytes = 0;
  unsigned char* pixels = NULL;
  unsigned char* tmp = NULL;

  if( (fp = fopen(filepath.c_str(), "rb")) == NULL ) {
    printf("Error: cannot load %s\n", filepath.c_str());
    return -1;
  }

  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, fp);
  jpeg_read_header(&cinfo, TRUE);
  jpeg_start_decompress(&cinfo);

  stride = cinfo.output_width * cinfo.output_components;
  nchannels = cinfo.output_components;
  width = cinfo.output_width;
  height = cinfo.output_height;
  num_bytes = width * height * nchannels;

  /* Reallocate when a buffer size is given */
  if (NULL != allocated) {
    if (0 == *allocated) {
      /* not allocated yet */
      pixels = (unsigned char*)malloc(num_bytes);
      if (pixels) {
        *allocated = num_bytes;
      }
      else {
        printf("Error: cannot allocate a buffer for the jpg.\n");
      }
    }
    else if (num_bytes > *allocated) {
      /* already allocated, try to reallocate if necessary */
      tmp = (unsigned char*)realloc(*pix, num_bytes);
      if (NULL == tmp) {
        printf("Error: cannot reallocate the pixel buffer for the jpg.");
        pixels = NULL;
      }
      else {
        pixels = tmp;
        *allocated = num_bytes;
      }
    }
    else {
      pixels = *pix;
    }
  }
  else {
     pixels = new unsigned char[num_bytes];
  }

  if(!pixels) {
    printf("Error: cannot allocate pixel buffer for jpg.\n");
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(fp);
    return -1;
  }

  size_t dest_row = 0;
  buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, stride, 1);
  while(cinfo.output_scanline < cinfo.output_height) {
    jpeg_read_scanlines(&cinfo, buffer, 1);
    memcpy(pixels + (dest_row * stride), buffer[0], stride);
    dest_row++;
  }

  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  fclose(fp);
  *pix = pixels;
  return num_bytes;
}

bool rx_save_jpg(std::string filepath, unsigned char* pix, int width, int height, int nchannels, int quality, bool flip, J_COLOR_SPACE colorSpace, J_DCT_METHOD dctMethod) {

  if(!pix) {
    printf("Error: cannot save jpg, invalid pixels.\n");
    return false;
  }

  if(width <= 0 || height <= 0) {
    printf("Error: cannot save jpg, invalid size: %d x %d\n", width, height);
    return false;
  }

  if(quality < 0) {
    quality = 0;
  }
  else if(quality > 100) {
    quality = 100;
  }

  FILE* fp = fopen(filepath.c_str(), "wb");
  if(!fp) {
    printf("Cannot open the file: `%s`", filepath.c_str());
    fp = NULL;
    return false;
  }

  struct jpeg_compress_struct cinfo;
  struct jpeg_error_mgr jerr;
  JSAMPROW row_pointer[1];

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_compress(&cinfo);

  jpeg_stdio_dest(&cinfo, fp);

  // compression parameters
  cinfo.image_width = width;
  cinfo.image_height = height;
  cinfo.input_components = nchannels;
  cinfo.in_color_space = colorSpace;

  jpeg_set_defaults(&cinfo); // after setting the default we can set our custom settings

  cinfo.dct_method = dctMethod;

  jpeg_set_quality(&cinfo, quality, TRUE /* limit to jpeg baseline values */);

  jpeg_start_compress(&cinfo, TRUE /* write complete data stream */);

  int stride = width * nchannels;

  if(flip) {
    while(cinfo.next_scanline < cinfo.image_height) {
      row_pointer[0] = &pix[ (cinfo.image_height - 1 - cinfo.next_scanline) * stride];
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
  }
  else {
    while(cinfo.next_scanline < cinfo.image_height) {
      row_pointer[0] = &pix[cinfo.next_scanline * stride];
      jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }
  }

  jpeg_finish_compress(&cinfo);

  fclose(fp);
  fp = NULL;

  jpeg_destroy_compress(&cinfo);
  return true;
}

#endif // defined(ROXLU_USE_JPG) && defined(ROXLU_IMPLEMENTATON)


// ====================================================================================
//                              R O X L U _ U S E _ P N G
// ====================================================================================

#if defined(ROXLU_USE_PNG) && defined(ROXLU_IMPLEMENTATION)

extern bool rx_save_png(std::string filepath,
                        unsigned char* pixels,
                        int w,
                        int h,
                        int channels = 3,
                        bool flip = false)
{

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

  //  unsigned char* p = (h * w * channels);
  //  int stride = -(w * channels);
  png_bytep* row_ptrs = new png_bytep[h];
  if(flip) {
    for(size_t j = 0; j < h; ++j) {
      row_ptrs[h-(j+1)] = pixels + (j * (w * channels));
    }
  }
  else {
    for(size_t j = 0; j < h; ++j) {
      row_ptrs[j] = pixels + (j * (w * channels));
    }
  }

  png_init_io(png_ptr, fp);
  png_set_rows(png_ptr, info_ptr, row_ptrs);
  png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  png_destroy_write_struct(&png_ptr, &info_ptr);

  delete[] row_ptrs;
  row_ptrs = NULL;

  fclose(fp);

  return true;
}

/*

  @param std::string filepath      - The file to laod
  @param unsigned char** pix       - A reference to the buffer that will be set to the
                                     memory we allocate OR that we will reallocate (if needed).
                                     We will only reallocate when the *allocated parameter is
                                     passed.
  @param int& width                - Will be set to the width of the image.
  @param int& height               - Will be set to the height of the image
  @param int& nchannels            - Will be set to the number of color channels in the image
  @param int* allocated (NULL)     - Can be NULL, if not it must be set to the number of bytes in *pix.
                                     When the number of allocated bytes is less then what we need, we will
                                     reallocate the buffer.
  @param int flags                 - This allows you to tell the loader, to convert a e.g. RGB
                                     png into a RGBA, wich may be handy if you need to upload
                                     the pixels to the GPU.

  @return int                      - The number of bytes of the image buffer (doesn't have to be the same
                                     as the number of bytes that was previously allocated!).
                                   - on error we return a negative value.

 */
extern int rx_load_png(std::string filepath,
                        unsigned char** pixels,
                        int& width,
                        int& height,
                        int& nchannels,
                        int* allocated,
                        int flags)
{
  png_structp png_ptr;
  png_infop info_ptr;
  unsigned char* tmp = NULL;

  FILE* fp = fopen(filepath.c_str(), "rb");
  if(!fp) {
    printf("Error: cannot load the png file: %s\n", filepath.c_str());
    fp = NULL;
    return -1;
  }

  unsigned char sig[8];
  size_t r = 0;
  r = fread(sig, 1, 8, fp);
  if(r != 8) {
    printf("Error: invalid png signature (not enough bytes read) in: %s.\n", filepath.c_str());
    fclose(fp);
    fp = NULL;
    return  -2;
  }

  if(!png_check_sig(sig, 8)) {
    printf("Error: invalid png signature (wrong siganture) in: %s.\n", filepath.c_str());
    fclose(fp);
    fp = NULL;
    return -3;
  }

  png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if(!png_ptr) {
    printf("Error: cannot create png read struct: %s\n", filepath.c_str());
    fclose(fp);
    fp = NULL;
    return -4;
  }

  info_ptr = png_create_info_struct(png_ptr);
  if(!info_ptr) {
    png_destroy_read_struct(&png_ptr, NULL, NULL);
    printf("Error: cannot create png info struct for: %s\n", filepath.c_str());
    fclose(fp);
    fp = NULL;
    return -5;
  }

#if !defined(_WIN32)
  if(setjmp(png_jmpbuf(png_ptr))) {
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;
    return -6;
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
    return -7;
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
    default: {
      /*printf("Warning: unsupported color type: %d.\n", color_type);*/
      break;
    }
  };

  // When transparency is set convert it to a full alpha channel
  if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
    png_set_tRNS_to_alpha(png_ptr);
    nchannels += 1;
  }

  /* When flag is set to load as RGBA, we need to the info struct */

  if ((flags & RX_FLAG_LOAD_AS_RGBA) == RX_FLAG_LOAD_AS_RGBA) {
    if (color_type == PNG_COLOR_TYPE_RGB ||
        color_type == PNG_COLOR_TYPE_GRAY ||
        color_type == PNG_COLOR_TYPE_PALETTE)
    {
      png_set_filler(png_ptr, 0xFF, PNG_FILLER_AFTER);
    }

    if(color_type == PNG_COLOR_TYPE_GRAY ||
       color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    {
      png_set_gray_to_rgb(png_ptr);
    }

    png_read_update_info(png_ptr, info_ptr);

    nchannels = 4;
  }

  stride = width * bit_depth * nchannels / 8;
  num_bytes = width * height * bit_depth * nchannels / 8;

  /* Reallocate when a buffer size is given */
  unsigned char* pixbuf = NULL;
  if (NULL != allocated) {
    if (0 == *allocated) {
      /* not allocated yet */
      *pixels = (unsigned char*)malloc(num_bytes);
      if (*pixels) {
        *allocated = num_bytes;
        pixbuf = *pixels;
      }
      else {
        printf("Error: cannot allocate buffer for png image, %d bytes.\n", num_bytes);
      }
    }
    else if (num_bytes > *allocated) {
      /* already allocated, try to reallocate if necessary */
      tmp = (unsigned char*)realloc(*pixels, num_bytes);
      if (NULL == tmp) {
        printf("Error: cannot reallocate the pixel buffer for the jpg.");
      }
      else {
        *pixels = tmp;
        *allocated = num_bytes;
        pixbuf = *pixels;
      }
    }
    else {
      pixbuf = *pixels;
    }
  }
  else {
     pixbuf = new unsigned char[num_bytes];
  }

  if(!pixbuf) {
    printf("Error: image is to big: %s\n", filepath.c_str());
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;
    return -8;
  }

   /* set the outgoing pixel buffer. */
  *pixels = pixbuf;

  png_bytep* row_ptrs = new png_bytep[height];
  if(!row_ptrs) {
    printf("Error: image is to big: %s\n", filepath.c_str());
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);
    fp = NULL;

    /* free allocations */
    delete[] *pixels;
    *pixels = NULL;
    if (NULL != allocated) {
      *allocated = 0;
    }
    return -9;
  }

  for(size_t i = 0; i < height; ++i) {
    row_ptrs[i] = (png_bytep)(*pixels) +(i * stride);
  }

  png_read_image(png_ptr, row_ptrs);

  delete[] row_ptrs;
  row_ptrs = NULL;
  png_destroy_read_struct(&png_ptr, &info_ptr, 0);
  fclose(fp);
  return num_bytes;
}

#endif //  defined(ROXLU_USE_PNG) && defined(ROXLU_IMPLEMENATION)

// ====================================================================================
//                              R O X L U _ U S E _ O P E N G L
// ====================================================================================

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_IMPLEMENTATION)

extern void rx_print_shader_link_info(GLuint shader) {
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

extern void rx_print_shader_compile_info(GLuint shader) {
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

extern GLuint rx_create_program(GLuint vert, GLuint frag, bool link) {
  GLuint prog = glCreateProgram();
  glAttachShader(prog, vert);
  glAttachShader(prog, frag);

  if(link) {
    glLinkProgram(prog);
    rx_print_shader_link_info(prog);
  }
  return prog;
}

extern GLuint rx_create_shader(GLenum type, const char* src) {
  GLuint s = glCreateShader(type);
  glShaderSource(s, 1, &src,  NULL);
  glCompileShader(s);
  rx_print_shader_compile_info(s);
  return s;
}


extern GLuint rx_create_shader_from_file(GLenum type, std::string filepath) {

  std::string source = rx_read_file(filepath);
  if(!source.size()) {
    printf("Error: cannot open the shader source: %s\n", filepath.c_str());
    return -1;
  }

  return rx_create_shader(type, source.c_str());
}

extern GLuint rx_create_program_with_attribs(GLuint vert,
                                             GLuint frag,
                                             int nattribs,
                                             const char** attribs)
{
  GLuint prog = rx_create_program(vert, frag);

  if(attribs) {
    for(int i = 0; i < nattribs; ++i) {
      glBindAttribLocation(prog, i, attribs[i]);
    }
  }

  glLinkProgram(prog);
  rx_print_shader_link_info(prog);

  return prog;
}

extern GLint rx_get_uniform_location(GLuint prog, std::string name) {
#if !defined(NDEBUG)
  GLint loc = glGetUniformLocation(prog, name.c_str());
  if(loc < 0) {
    printf("Error: cannot find the uniform: %s\n", name.c_str());
  }
#else
  GLint loc = glGetUniformLocation(prog, name.c_str());
#endif

  return loc;
}

extern void rx_uniform_1i(GLuint prog, std::string name, GLint v) {
  glUniform1i(rx_get_uniform_location(prog, name), v);
}

extern void rx_uniform_1f(GLuint prog, std::string name, GLfloat v) {
  glUniform1f(rx_get_uniform_location(prog, name), v);
}

extern void rx_uniform_mat4fv(GLuint prog,
                              std::string name,
                              GLsizei count,
                              GLboolean transpose,
                              const GLfloat* value)
{
  glUniformMatrix4fv(rx_get_uniform_location(prog, name), count, transpose, value);
}

Program::~Program() {
  for(std::vector<Shader*>::iterator it = created_shaders.begin(); it != created_shaders.end(); ++it) {
    delete *it;
  }
  shaders.clear();
  created_shaders.clear();
}

Program& Program::add(Shader* s) {

  if(id < 0) {
    id = glCreateProgram();
  }

  s->compile();

  if(s->id < 0) {
    printf("Error: a shader is not compiled yet!\n");
    ::exit(EXIT_FAILURE);
  }

  glAttachShader(id, s->id);

  shaders.push_back(s);
  return *this;
}

Program& Program::create(GLenum type,
                         std::string filepath,
                         std::string extraSource)
{
  Shader* s = new Shader();
  s->load(type, filepath, extraSource);
  created_shaders.push_back(s);
  return add(s);
}

Program& Program::link(int nattribs,
                       const char** atts,
                       int nfrags,
                       const char** fraglocs)
{

  if(id < 0) {
    printf("Error: first add a shader before linking.\n");
    ::exit(EXIT_FAILURE);
  }

  if(nattribs) {
    attribs.clear();

    for(int i = 0; i < nattribs; ++i) {
      glBindAttribLocation(id, i, atts[i]);
      attribs.push_back(atts[i]);
    }
  }

  if(nfrags) {
    frags.clear();
    for(int i = 0; i < nfrags; ++i) {
      glBindFragDataLocation(id, i, fraglocs[i]);
      frags.push_back(fraglocs[i]);
    }
  }

  glLinkProgram(id);
  rx_print_shader_link_info(id);

  return *this;
}

Program& Program::recompile() {

  if(id < 0) {
    printf("Error: cannot recompile the program because we've not been created yet.\n");
    ::exit(EXIT_FAILURE);
  }

  for(size_t i = 0; i < shaders.size(); ++i) {
    shaders[i]->reload();
    shaders[i]->compile();
  }

  for(size_t i = 0; i < attribs.size(); ++i) {
    glBindAttribLocation(id, i, attribs[i].c_str());
  }

  for(size_t i = 0; i < frags.size(); ++i) {
    glBindFragDataLocation(id, i, frags[i].c_str());
  }

  glLinkProgram(id);
  rx_print_shader_link_info(id);

  return *this;
}

Shader& Shader::load(GLenum t,
                     std::string fp,
                     std::string extraSource)
{

  file_source = rx_read_file(fp);

  if(!file_source.size()) {
    printf("Error: cannot get contents for shader file: %s\n", fp.c_str());
    ::exit(EXIT_FAILURE);
  }

  extra_source = extraSource;
  filepath = fp;
  type = t;
  return *this;
}

Shader& Shader::compile() {

  if(id < 0) {
    id = glCreateShader(type);
  }

  if(!file_source.size()) {
    printf("Error: The source of the shader is empty, cannot compile.\n");
    ::exit(EXIT_FAILURE);
  }

  std::string combined_source = extra_source +"\n" +file_source;
  const char* src = combined_source.c_str();

  glShaderSource(id, 1, &src, NULL);
  glCompileShader(id);
  rx_print_shader_compile_info(id);
  return *this;
}

Shader& Shader::reload() {
  return load(type, filepath, extra_source);
}

#endif //  defined(ROXLU_USE_OPENGL) && defined(ROXLU_IMPLEMENTATION)

// ====================================================================================
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ M A T H
// ====================================================================================
#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_MATH) && defined(ROXLU_IMPLEMENTATION)

PainterContextPT::PainterContextPT(Painter& painter)
  :painter(painter)
  ,vao(0)
  ,vbo(0)
  ,vert(0)
  ,frag(0)
  ,prog(0)
  ,allocated(0)
  ,needs_update(false)
{
  vert = rx_create_shader(GL_VERTEX_SHADER, PAINTER_VERTEX_PT_VS);
  frag = rx_create_shader(GL_FRAGMENT_SHADER, PAINTER_VERTEX_PT_SAMPLER2D_FS);
  prog = rx_create_program(vert, frag);
  glLinkProgram(prog);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // tex

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*) 0); // pos
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexPT), (GLvoid*) 12); // tex

  // bind ubo to binding 0
  glUseProgram(prog);
  GLint block_dx = glGetUniformBlockIndex(prog, "Shared");
  glUniformBlockBinding(prog, block_dx, 0);
  glUniform1i(glGetUniformLocation(prog, "u_tex"), 0);
}

void PainterContextPT::texture(GLuint tex, float x, float y, float w, float h) {

  PainterCommand cmd;
  cmd.type = GL_TEXTURE_2D;
  cmd.count = 6;
  cmd.offset = vertices.size();
  cmd.tex = tex;
  commands.push_back(cmd);

  VertexPT a(vec3(x,     y + h, 0.0), vec2(0.0, 1.0)); // bottom left
  VertexPT b(vec3(x + w, y + h, 0.0), vec2(1.0, 1.0)); // bottom right
  VertexPT c(vec3(x + w,     y, 0.0), vec2(1.0, 0.0)); // top right
  VertexPT d(vec3(x,         y, 0.0), vec2(0.0, 0.0)); // top left

  vertices.push_back(a);
  vertices.push_back(b);
  vertices.push_back(c);
  vertices.push_back(a);
  vertices.push_back(c);
  vertices.push_back(d);

  needs_update = true;
}

void PainterContextPT::clear() {
  vertices.clear();
  commands.clear();
}

void PainterContextPT::update() {
  if(!needs_update) {
    return;
  }

  size_t needed = sizeof(VertexPT) * vertices.size();
  if(needed == 0) {
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  if(needed > allocated) {
    allocated = needed;
    glBufferData(GL_ARRAY_BUFFER, needed, vertices[0].ptr(), GL_STREAM_DRAW);
  }
  else {
    glBufferSubData(GL_ARRAY_BUFFER, 0, needed, vertices[0].ptr());
  }

  needs_update = false;
}

void PainterContextPT::draw() {

  if(vertices.size() == 0) {
    return;
  }

  glDisable(GL_DEPTH_TEST);
  glUseProgram(prog);
  glBindVertexArray(vao);

  glActiveTexture(GL_TEXTURE0);

  for(std::vector<PainterCommand>::iterator it = commands.begin(); it != commands.end(); ++it) {
    PainterCommand& cmd = *it;
    glBindTexture(cmd.type, cmd.tex);
    glDrawArrays(GL_TRIANGLES, cmd.offset, cmd.count);
  }
}

// -----------------------------------------------------

PainterContextPC::PainterContextPC(Painter& painter)
  :vao(0)
  ,vbo(0)
  ,vert(0)
  ,frag(0)
  ,prog(0)
  ,painter(painter)
  ,needs_update(true)
  ,allocated(0)
{
  vert = rx_create_shader(GL_VERTEX_SHADER, PAINTER_VERTEX_PC_VS);
  frag = rx_create_shader(GL_FRAGMENT_SHADER, PAINTER_VERTEX_PC_FS);
  prog = rx_create_program(vert, frag);
  glLinkProgram(prog);

  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  glEnableVertexAttribArray(0); // pos
  glEnableVertexAttribArray(1); // col

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (GLvoid*) 0); // pos
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexPC), (GLvoid*) 12); // col

  // bind ubo to binding 0
  glUseProgram(prog);
  GLint block_dx = glGetUniformBlockIndex(prog, "Shared");
  glUniformBlockBinding(prog, block_dx, 0);
}

void PainterContextPC::rect(float x, float y, float w, float h) {

  VertexPC a(vec3(x,     y + h, 0.0), painter.col); // bottom left
  VertexPC b(vec3(x + w, y + h, 0.0), painter.col); // bottom right
  VertexPC c(vec3(x + w,     y, 0.0), painter.col); // top right
  VertexPC d(vec3(x,         y, 0.0), painter.col); // top left

  PainterCommand cmd;
  cmd.offset = vertices.size();

  if(painter.state & PAINTER_STATE_FILL) {

    cmd.type = GL_TRIANGLES;

    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
    vertices.push_back(a);
    vertices.push_back(c);
    vertices.push_back(d);
  }
  else {

    cmd.type = GL_LINE_LOOP;
    vertices.push_back(a);
    vertices.push_back(b);
    vertices.push_back(c);
    vertices.push_back(d);
  }

  cmd.count = vertices.size() - cmd.offset;
  commands.push_back(cmd);
  needs_update = true;
}

void PainterContextPC::clear() {
  vertices.clear();
  commands.clear();
}

void PainterContextPC::circle(float x, float y, float radius) {

  PainterCommand cmd;
  cmd.offset = vertices.size();

  if(painter.state & PAINTER_STATE_FILL) {
    cmd.type = GL_TRIANGLE_FAN;
    vertices.push_back(VertexPC(vec3(x, y, 0), painter.col));
  }
  else {
    cmd.type = GL_LINE_STRIP;
  }

  cmd.offset = vertices.size();

  for(std::vector<vec2>::iterator it = painter.circle_data.begin(); it != painter.circle_data.end(); ++it) {
    vec2& v= *it;
    vertices.push_back(VertexPC(vec3(x + v.x * radius, y + v.y * radius, 0), painter.col));
  }

  cmd.count = vertices.size() - cmd.offset;

  commands.push_back(cmd);
  needs_update = true;
}

void PainterContextPC::line(float x0, float y0, float x1, float y1) {

  PainterCommand cmd;
  cmd.type = GL_LINES;
  cmd.offset = vertices.size();
  cmd.count = 2;
  commands.push_back(cmd);

  VertexPC a(vec3(x0, y0, 0), painter.col);
  VertexPC b(vec3(x1, y1, 0), painter.col);
  vertices.push_back(a);
  vertices.push_back(b);

  needs_update = true;
}

void PainterContextPC::command(GLenum type, std::vector<VertexPC>& v) {

  PainterCommand command;
  command.type = type;
  command.offset = vertices.size();
  command.count = v.size();

  std::copy(v.begin(), v.end(), std::back_inserter(vertices));

  commands.push_back(command);

  needs_update = true;
}

void PainterContextPC::update() {

  if(!needs_update) {
    return;
  }

  size_t needed = sizeof(VertexPC) * vertices.size();
  if(needed == 0) {
    return;
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  if(needed > allocated) {
    allocated = needed;
    glBufferData(GL_ARRAY_BUFFER, needed, vertices[0].ptr(), GL_STREAM_DRAW);
  }
  else {
    glBufferSubData(GL_ARRAY_BUFFER, 0, needed, vertices[0].ptr());
  }

  needs_update = false;
}

void PainterContextPC::draw() {

  if(vertices.size() == 0) {
    return;
  }

  glUseProgram(prog);
  glBindVertexArray(vao);

  for(std::vector<PainterCommand>::iterator it = commands.begin(); it != commands.end(); ++it) {
    PainterCommand& cmd = *it;
    glDrawArrays(cmd.type, cmd.offset, cmd.count);
  }
}

// -----------------------------------------------------

Painter::Painter()
  :context_pc(*this)
  ,context_pt(*this)
  ,state(PAINTER_STATE_NONE)
  ,circle_resolution(8)
  ,ubo(0)
{

  col[0] = 1.0f;
  col[1] = 0.0f;
  col[2] = 0.0f;
  col[3] = 1.0f;

  glGenBuffers(1, &ubo);
  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(PainterShared), NULL, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  resize(viewport[2], viewport[3]);

  resolution(circle_resolution);
}

void Painter::clear() {
  context_pc.clear();
  context_pt.clear();
}

void Painter::resolution(int n) {

  float a = TWO_PI / float(n);
  for(int i = 0; i <= n; ++i) {
    circle_data.push_back(vec2(cosf(a * i), sinf(a * i)));
  }
}

void Painter::rect(float x, float y, float w, float h) {
  context_pc.rect(x, y, w, h);
}

void Painter::circle(float x, float y, float radius) {
  context_pc.circle(x, y, radius);
}

void Painter::line(float x0, float y0, float x1, float y1) {
  context_pc.line(x0, y0, x1, y1);
}

void Painter::texture(GLuint tex, float x, float y, float w, float h) {
  context_pt.texture(tex, x, y, w, h);
}

void Painter::color(float r, float g, float b, float a) {

  col[0] = r;
  col[1] = g;
  col[2] = b;
  col[3] = a;
}

void Painter::draw() {

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  context_pc.update();
  context_pc.draw();

  context_pt.update();
  context_pt.draw();
}

void Painter::resize(int w, int h) {

  ubo_data.pm.ortho(0, w, h, 0, 0.0f, 100.0f);

  glBindBuffer(GL_UNIFORM_BUFFER, ubo);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(PainterShared), ubo_data.pm.ptr());

  win_w = w;
  win_h = h;
}

void Painter::fill() {
  state |= PAINTER_STATE_FILL;
}

void Painter::nofill() {
  state &= ~PAINTER_STATE_FILL;
}

void Painter::begin(GLenum type) {
  command_type = type;
}

void Painter::vertex(float x, float y) {
  context_type = PAINTER_CONTEXT_TYPE_PC;
  vertices_pc.push_back(VertexPC(vec3(x, y, 0), col));
}

void Painter::end() {

  if(context_type == PAINTER_CONTEXT_TYPE_PC) {
    context_pc.command(command_type, vertices_pc);
    vertices_pc.clear();
  }
}

int Painter::width() {
  return win_w;
}

int Painter::height() {
  return win_h;
}

#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_MATH) && defined(ROXLU_IMPLEMENTATION)

// ====================================================================================
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ P N G
// ====================================================================================

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_PNG) && defined(ROXLU_IMPLEMENTATION)

extern GLuint rx_create_texture(std::string filepath,
                                int internalFormat,
                                int format,
                                int type)
{

  int w, h, n;
  unsigned char* pix;

  if(!rx_load_png(filepath, &pix, w, h, n)) {
    printf("Error: cannot find: %s\n", filepath.c_str());
    ::exit(EXIT_FAILURE);
  }

  if(format == -1) {
    switch(n) {
      case 1: format = GL_RED;  break;
      case 2: format = GL_RG;   break;
      case 3: format = GL_RGB;  break;
      case 4: format = GL_RGBA; break;
      default: {
        printf("Unhandled number of channels for texture :%d\n", n);
        ::exit(EXIT_FAILURE);
      }
    }
  }

  if(internalFormat == -1) {
    switch(n) {
      case 1: internalFormat = GL_R8;    break;
      case 2: internalFormat = GL_RG8;   break;
      case 3: internalFormat = GL_RGB8;  break;
      case 4: internalFormat = GL_RGBA8; break;
      default: {
        printf("Unhandled number of channels for texture :%d\n", n);
        ::exit(EXIT_FAILURE);
      }
    }
  }

  if(type == -1) {
    type = GL_UNSIGNED_BYTE;
  }

  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexImage2D(GL_TEXTURE_2D, 0, (GLenum)internalFormat, w, h, 0, (GLenum)format, (GLenum)type, pix);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  delete[] pix;
  pix = NULL;
  return tex;
}

// save a PNG, note that we do not free the allocated pixel buffer.
// we don't allocate the pixel buffer because reallocating is a cpu-heavy
// operation. This mean we might leak some memory if you want to use this
// function.
bool rx_create_png_screenshot(std::string filepath) {
  static unsigned char* pixels = NULL;
  static int width = 0;
  static int height = 0;

  GLint viewport[4] = { 0 };
  glGetIntegerv(GL_VIEWPORT, viewport);
  int nbytes_needed = viewport[2] * viewport[3] * 3;
  int nbytes_allocated = width * height * 3;

  // check if we need to (re)-allocate
  if(nbytes_needed > nbytes_allocated) {

    if(pixels != NULL) {
      delete[] pixels;
    }

    pixels = new unsigned char[nbytes_needed];
    if(!pixels) {
      printf("Error: cannot allocate pixels for screenshot.\n");
      return false;
    }
  }

  if(viewport[2] != width) {
    width = viewport[2];
  }

  if(viewport[3] != height) {
    height = viewport[3];
  }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_PACK_ROW_LENGTH, width);

  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  return rx_save_png(filepath, pixels, width, height, 3, true);
}

#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_PNG) && defined(ROXLU_IMPLEMENATION)


// ====================================================================================
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ J P G
// ====================================================================================

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_JPG) && defined(ROXLU_IMPLEMENTATION)

bool rx_create_jpg_screenshot(std::string filepath, int quality) {
  static unsigned char* pixels = NULL;
  static int width = 0;
  static int height = 0;

  GLint viewport[4] = { 0 };
  glGetIntegerv(GL_VIEWPORT, viewport);
  int nbytes_needed = viewport[2] * viewport[3] * 3;
  int nbytes_allocated = width * height * 3;

  // check if we need to (re)-allocate
  if(nbytes_needed > nbytes_allocated) {

    if(pixels != NULL) {
      delete[] pixels;
    }

    pixels = new unsigned char[nbytes_needed];
    if(!pixels) {
      printf("Error: cannot allocate pixels for screenshot.\n");
      return false;
    }
  }

  if(viewport[2] != width) {
    width = viewport[2];
  }

  if(viewport[3] != height) {
    height = viewport[3];
  }

  glPixelStorei(GL_PACK_ALIGNMENT, 4);
  glPixelStorei(GL_PACK_ROW_LENGTH, width);

  glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  return rx_save_jpg(filepath, pixels, width, height, 3, quality, true, JCS_RGB, JDCT_FASTEST);
}

#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_JPG) && defined(ROXLU_IMPLEMENTATION)


// ====================================================================================
//                              R O X L U _ U S E _ O P E N G L
//                              R O X L U _ U S E _ F O N T
// ====================================================================================

#if defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_FONT) && defined(ROXLU_IMPLEMENTATION)

namespace roxlu {

  GLuint Font::prog = 0;
  GLuint Font::vert = 0;
  GLuint Font::frag = 0;
  bool Font::is_initialized = false;

  // ------------------------------------------------------------------------------

  static GLuint font_create_program(GLuint vert, GLuint frag, int natts = 0, const char** atts = NULL) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vert);
    glAttachShader(prog, frag);

    for(int i = 0; i < natts; ++i) {
      glBindAttribLocation(prog, i, atts[i]);
    }

    glLinkProgram(prog);
    return prog;
  }

  static GLuint font_create_shader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src,  NULL);
    glCompileShader(s);
    return s;
  }

  static void font_ortho(float l, float r, float b, float t, float n, float f, float* m) {
    memset((char*)m, 0x0, sizeof(float) * 16);
    float rml = r - l;
    float fmn = f - n;
    float tmb = t - b;
    m[0]  = 2.0f / rml;
    m[5]  = 2.0f / tmb;
    m[10] = -2.0f / fmn;
    m[12] = -(r+l)/rml;
    m[13] = -(t+b)/tmb;
    m[14] = -(f+n)/fmn;
    m[15] = 1.0f;
  }

  // -----------------------------------------------------------------------------

  Font::Font()
    :needs_update(false)
    ,win_w(0)
    ,win_h(0)
    ,vbo(0)
    ,vao(0)
    ,tex(0)
    ,bytes_allocated(0)
    ,line_height(0)
    ,scale_w(0)
    ,scale_h(0)
    ,pages(0)
    ,base(0)
    {
      col[0] = col[1] = col[2] = col[3] = 1.0f;
    }

  bool Font::setupGraphics() {

    if(!Font::is_initialized) {

      const char* atts[] = { "a_pos", "a_tex", "a_fg_color" } ;
      vert = font_create_shader(GL_VERTEX_SHADER, FONT_VS);
      frag = font_create_shader(GL_FRAGMENT_SHADER, FONT_FS);
      prog = font_create_program(vert, frag, 3, atts);

      GLint viewport[4] = { 0 } ;
      glGetIntegerv(GL_VIEWPORT, viewport);
      font_ortho(0.0f, viewport[2], viewport[3], 0.0f, 0.0f, 100.0f, pm);

      glUseProgram(prog);
      glUniformMatrix4fv(glGetUniformLocation(prog, "u_pm"), 1, GL_FALSE, pm);
      glUniform1i(glGetUniformLocation(prog, "u_font_tex"), 0);

      Font::is_initialized = true;
    }

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glEnableVertexAttribArray(0);  // pos
    glEnableVertexAttribArray(1);  // tex
    glEnableVertexAttribArray(2);  // fg_color

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex), (GLvoid*)0); // pos
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex), (GLvoid*)8); // tex
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(CharacterVertex), (GLvoid*)16); // fg_color

    return true;
  }

  void Font::setupTexture(int w, int h, unsigned char* pix) {
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_RECTANGLE, tex);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, pix);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }

  void Font::resize(int winW, int winH) {

    if(!winW || !winH) {
      return;
    }

    win_w = winW;
    win_h = winH;

    font_ortho(0.0f, win_w, win_h, 0.0f, 0.0, 100.0, pm);
    glUseProgram(prog);
    glUniformMatrix4fv(glGetUniformLocation(prog, "u_pm"), 1, GL_FALSE, pm);
  }

  void Font::color(float r, float g, float b, float a) {
    col[0] = r;
    col[1] = g;
    col[2] = b;
    col[3] = a;
  }

  void Font::clear() {
    vertices.clear();
  }

  void Font::write(float x, float y, std::string str, int align) {

    float xoffset = x;
    float yoffset = y;

    if(align == FONT_ALIGN_RIGHT) {
      xoffset -= getWidth(str);
    }

    for(size_t i = 0; i < str.size(); ++i) {

      std::map<unsigned int, Character>::iterator it = chars.find(str[i]);
      if(it == chars.end()) {
        printf("character: %c not found.\n", str[i]);
        continue;
      }

      Character& c = it->second;

      write(xoffset, yoffset, c);

      xoffset += c.xadvance;
    }

  }

  void Font::write(float x, float y, unsigned int id)  {

    std::map<unsigned int, Character>::iterator it = chars.find(id);

    if(it == chars.end()) {
      printf("Character for id: %u not found.\n", id);
      return;
    }

    write(x, y, it->second);
  }

  void Font::write(float x, float y, Character& c) {

    float x0 = x + c.xoffset;
    float y0 = y + c.yoffset;
    float x1 = x0 + c.width;
    float y1 = c.height + y + c.yoffset;
    float u0 = c.x;
    float u1 = c.x + c.width;
    float v0 = c.y;
    float v1 = v0 + c.height;

    CharacterVertex va(x0, y1, u0, v1, col); // bottom left
    CharacterVertex vb(x1, y1, u1, v1, col); // bottom right
    CharacterVertex vc(x1, y0, u1, v0, col); // top right
    CharacterVertex vd(x0, y0, u0, v0, col); // top left

    vertices.push_back(va);
    vertices.push_back(vb);
    vertices.push_back(vc);
    vertices.push_back(va);
    vertices.push_back(vc);
    vertices.push_back(vd);

    needs_update = true;

    //printf("x0: %f, y0: %f, x1: %f, y1: %f u0: %f, u1: %f, v0: %f, v1: %f\n", x0, y0, x1, y1, u0, u1, v0, v1);
  }

  void Font::draw() {

    if(!vertices.size()) {
      return;
    }

    updateVertices();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE, tex);

    glUseProgram(prog);
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, vertices.size());
  }

  void Font::updateVertices() {

    if(!needs_update) {
      return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    size_t needed = sizeof(CharacterVertex) * vertices.size();
    if(!needed) {
      return;
    }

    if(needed > bytes_allocated) {
      glBufferData(GL_ARRAY_BUFFER, needed, &vertices[0].x, GL_DYNAMIC_DRAW);
      bytes_allocated = needed;
    }
    else {
      glBufferSubData(GL_ARRAY_BUFFER, 0, needed, &vertices[0].x);
    }

    needs_update = false;
  }

  bool Font::getChar(unsigned int code, Character& result) {

    std::map<unsigned int, Character>::iterator it = chars.find(code);

    if(it == chars.end()) {
      return false;
    }

    result = it->second;

    return true;
  }

  float Font::getWidth(const std::string& str) {
    float w = 0.0f;
    Character ch;

    for(size_t i = 0; i < str.size(); ++i) {
      if(getChar(str[i], ch)) {
        w += ch.xadvance;
      }
    }

    return w;
  }

  void Font::print() {
    printf("font.line_height: %d\n", line_height);
    printf("font.scale_w: %d\n", scale_w);
    printf("font.scale_h: %d\n", scale_h);
    printf("font.pages: %d\n", pages);
    printf("font.base: %d\n", base);
  }

  // -----------------------------------------------------------------------------

  Character::Character()
    :id(0)
    ,x(0)
    ,y(0)
    ,width(0)
    ,height(0)
    ,xoffset(0)
    ,yoffset(0)
    ,xadvance(0)
    {
    }

  // -----------------------------------------------------------------------------

  CharacterVertex::CharacterVertex()
    :x(0.0f)
    ,y(0.0f)
    ,s(0.0f)
    ,t(0.0f)
    {
      fg_color[0] = fg_color[1] = fg_color[2] = fg_color[3] = 1.0f;
    }

  CharacterVertex::CharacterVertex(float x, float y, int s, int t, float* rgba)
    :x(x)
    ,y(y)
    ,s(s)
    ,t(t)
  {
    fg_color[0] = rgba[0];
    fg_color[1] = rgba[1];
    fg_color[2] = rgba[2];
    fg_color[3] = rgba[3];
  }

} // namespace roxlu

#endif // defined(ROXLU_USE_OPENGL) && defined(ROXLU_USE_FONT) && defined(ROXLU_IMPLEMENTATION)

// ====================================================================================
//                              R O X L U _ U S E _ A U D I O
// ====================================================================================

#if defined(ROXLU_USE_AUDIO) && defined(ROXLU_IMPLEMENTATION)

/* AUDIO FILE */
/* ----------------------------------- */
AudioFile::AudioFile()
:sf(NULL)
{
}

bool AudioFile::load(std::string filepath) {

  if(sf != NULL) {
    printf("Error: already loaded.\n");
    return false;
  }

  info.format = 0;
  sf = sf_open(filepath.c_str(), SFM_READ, &info);

  if(sf == NULL) {
    printf("Erorr: cannot open the audio file: %s\n", filepath.c_str());
    return false;
  }

  if(info.samplerate != 44100) {
    printf("Error: we only support a samplerate of 44100 now.\n");
    if(sf_close(sf) != 0) {
      printf("Error: cannot close the just opened sound file.\n");
    }
    sf = NULL;
    return false;
  }

  if(info.channels != 2) {
    printf("Error: we only support 2 channels of audio atm.\n");
    if(sf_close(sf) != 0) {
      printf("Error: cannot close the just opened sound file.\n");
    }
    sf = NULL;
    return false;
  }

  // read all audio data as floats.
  sf_count_t nread = 0;
  int chunk_size = 1024 * 1024;
  int read_items = info.channels * chunk_size;
  float* tmp = new float[read_items];

  do {
    nread = sf_readf_float(sf, tmp, chunk_size);
    std::copy(tmp, tmp + (nread * info.channels), std::back_inserter(data));
  } while(nread > 0);

  delete[] tmp;

  if(sf_close(sf) != 0) {
    printf("Error: cannot close the file.\n");
    return false;
  }

#if 0
  printf("Samplerate: %d\n", info.samplerate);
  printf("Channels: %d\n", info.channels);
  printf("Data.size: %ld\n", data.size());
#endif

  return true;
}

/* PLAYBACK STATE */
/* ----------------------------------- */
AudioPlaybackState::AudioPlaybackState(int name, AudioFile* file)
  :name(name)
  ,file(file)
  ,volume_level(1.0)
  ,read_pos(0)
{
}

AudioPlaybackState::~AudioPlaybackState() {
  name = -1;
  file = NULL;
  volume_level = 0.0f;
  read_pos = 0;
}

void AudioPlaybackState::volume(float level) {
  volume_level = level;
}

/* AUDIO PLAYER */
/* ----------------------------------- */

AudioPlayer::AudioPlayer()
:cube_ctx(NULL)
,cube_stream(NULL)
{

#if defined(__APPLE__) or defined(__linux)
  int status = pthread_mutex_init(&mutex, NULL);
  if(status != 0) {
    printf("Error: cannot initialize the mutex for the audio player.\n");
    ::exit(EXIT_FAILURE);
  }
#endif

  if(cubeb_init(&cube_ctx, "AudioPlayer") != CUBEB_OK) {
    printf("Error: cannot initialize cubeb.\n");
    ::exit(EXIT_FAILURE);
  }

  cubeb_stream_params params;
  params.format = CUBEB_SAMPLE_FLOAT32LE;
  params.rate = 41000;
  params.channels = 2;

  unsigned int latency_ms = 250;
  cubeb_stream_init(cube_ctx, &cube_stream, "stream", params,
                    latency_ms, audioplayer_data_cb, audioplayer_state_cb, this);
}

AudioPlayer::~AudioPlayer() {

  if(play_states.size() > 0 && cube_stream != NULL) {
    cubeb_stream_stop(cube_stream);
    cube_stream = NULL;
  }

  if(cube_ctx != NULL) {
    cubeb_destroy(cube_ctx);
    cube_ctx = NULL;
  }

  // free all play states.
  for(size_t i = 0; i < play_states.size(); ++i) {
    delete play_states[i];
  }
  play_states.clear();

  // free all files.
  for(size_t i = 0; i < files.size(); ++i) {
    delete files[i];
  }
  files.clear();

  // free the mutex.
#if defined(__APPLE__) or defined(__linux)
  int status = pthread_mutex_destroy(&mutex);
  if(status != 0) {
    printf("Error: cannot destroy the mutex.\n");
  }
#endif
}

bool AudioPlayer::add(int name, std::string filepath) {

  AudioFile* af = new AudioFile();
  if(!af->load(filepath)) {
    printf("Error: cannot open the audio file: %s\n", filepath.c_str());
    delete af;
    af = NULL;
    return false;
  }

  files[name] = af;

  return true;
}

void AudioPlayer::lock() {
#if defined(__APPLE__) or defined(__linux)
  int status = pthread_mutex_lock(&mutex);
  if(status != 0) {
    printf("Error: cannot lock the mutex.\n");
  }
#endif
}

void AudioPlayer::unlock() {
#if defined(__APPLE__) or defined(__linux)
  int status = pthread_mutex_unlock(&mutex);
  if(status != 0) {
    printf("Error: cannot unlock the mutex.\n");
  }
#endif
}

AudioPlaybackState* AudioPlayer::play(int name, float volume) {

  // find the file or stop when not found.
  std::map<int, AudioFile*>::iterator it = files.find(name);
  if(it == files.end()) {
    printf("Error: cannot find the file for name: %d\n", name);
    return NULL;
  }

  // Start the stream if this is the first playback.
  lock();
  {
    if(play_states.size() == 0) {
      if(cubeb_stream_start(cube_stream) != CUBEB_OK) {
        printf("Error: cannot start the cubeb stream.\n");
        unlock();
        return NULL;
      }
    }
  }
  unlock();

  // add a new playback state that is used in the data callback
  AudioPlaybackState* play_state = new AudioPlaybackState(name, it->second);
  play_state->volume(volume);

  lock();
  {
    play_states.push_back(play_state);
  }
  unlock();

  return play_state;
}

long audioplayer_data_cb(cubeb_stream* stm, void* user, void* buffer, long nframes) {

  // at this moment we only support 2 channel audio
  AudioPlayer* player = static_cast<AudioPlayer*>(user);
  float* buf = (float*)buffer;
  int nchannels = 2;

  // clear
  memset(buf, 0x00, (nframes * nchannels * sizeof(float)));

  // copy the states over so we don't interfere with the cross-thread data
  std::vector<AudioPlaybackState*> states;
  player->lock();
  {
    std::copy(player->play_states.begin(), player->play_states.end(), std::back_inserter(states));
  }
  player->unlock();

  // loop over all playback states and mix the audio together.
  for(size_t i = 0; i < states.size(); ++i) {

    AudioPlaybackState* play_state = states[i];
    AudioFile* file = play_state->file;

    if(play_state->read_pos < file->data.size()) {

      int nelems_needed = nframes * 2; // 2 channels
      int nelems_left = file->data.size() - play_state->read_pos;
      int nelems_read = std::min<int>(nelems_needed, nelems_left);

      for(size_t i = 0; i < nelems_read; ++i) {
        int read_dx = play_state->read_pos + i;
        buf[i] += (file->data[read_dx] * play_state->volume_level);
      }

      play_state->read_pos += nelems_read;
    }
  }

  // remove play states that are ready.
  player->lock();
  {
    std::vector<AudioPlaybackState*>::iterator it = player->play_states.begin();
    while(it != player->play_states.end()) {
      AudioPlaybackState* state = *it;
      if(state->read_pos >= state->file->data.size()) {
        it = player->play_states.erase(it);
        delete state;
        state = NULL;
      }
      else {
        ++it;
      }
    }

    // check if we need to stop the stream for now.
    if(player->play_states.size() == 0) {
      if(cubeb_stream_stop(player->cube_stream) != CUBEB_OK) {
        printf("Error: something went wrong while trying to stop the audio stream.\n");
      }
    }
  }
  player->unlock();

  return nframes;
}

void audioplayer_state_cb(cubeb_stream* stm, void* user, cubeb_state state) {
}

#endif //  defined(ROXLU_USE_AUDIO) && defined(ROXLU_IMPLEMENTATION)


// ====================================================================================
//                              R O X L U _ U S E _ M A T H
// ====================================================================================
#if defined(ROXLU_USE_LOG) && defined(ROXLU_IMPLEMENTATION)

/* --------------------------------------------------------------------------------- */

Log rx_log;

/* --------------------------------------------------------------------------------- */

Log::Log()
  :write_to_stdout(true)
  ,level(RX_LOG_LEVEL_ALL)
{
}

Log::~Log() {
  if (ofs.is_open()) {
    ofs.close();
  }
}

int Log::open(std::string filep) {

  if (0 != filepath.size()) {
    printf("Error: trying to open the log file but it's already open? Calling rx_log_init() twice?\n");
    return -1;
  }

  filepath = filep;

  if (0 == filepath.size()) {
    printf("Error: cannot open the log filepath because the string is empty.\n");
    return -2;
  }

  ofs.open(filepath.c_str(), std::ios::out | std::ios::app);
  if (!ofs.is_open()) {
    printf("Error: cannot open the log file. No permission? %s\n", filepath.c_str());
    return -3;
  }

  return 0;
}

void Log::log(int inlevel, int line, const char* function, const char* fmt, va_list args) {

  if (inlevel > level) {
    return;
  }

  //  std::stringstream ss;
  static char buffer[1024 * 8]; /* should be big enough ;-) */
  std::string slevel;

  if (false == ofs.is_open()) {
    printf("Error: cannot log because the file hasn't been opened. Did you call rx_log_init()?\n");
    return;
  }

  vsprintf(buffer, fmt, args);

  ofs << rx_get_time_string() << " " ;

  if (inlevel == RX_LOG_LEVEL_VERBOSE) {
    slevel = " verbose ";
    ofs << slevel;
  }
  else if (inlevel == RX_LOG_LEVEL_WARNING) {
    slevel =  " warning ";
    ofs << slevel;
  }
  else if (inlevel == RX_LOG_LEVEL_ERROR) {
    slevel = " <<ERROR>> ";
    ofs << slevel;
  }

  ofs << " [" << function << ":" << line << "] = " <<  buffer << "\n";

  if (write_to_stdout) {
    printf("%s:%s[%s:%d] = %s \n", rx_get_time_string().c_str(), slevel.c_str(), function, line, buffer);
  }

  ofs.flush();
}


/* --------------------------------------------------------------------------------- */
int rx_log_init(std::string path) {
  std::string filepath = "";

  if (0 == path.size()) {
    filepath = rx_get_exe_path();
  }
  else {
    if (false == rx_is_dir(path)) {
      printf("Error: the given path for the log is invalid: %s\n", path.c_str());
      return -1;
    }

    filepath = path;
  }

  return rx_log.open(filepath +"/log-" +rx_get_date_string() +".log");
}

void rx_log_disable_stdout() {
  rx_log.write_to_stdout = false;
}

void rx_log_enable_stdout() {
  rx_log.write_to_stdout = false;
}

void rx_log_set_level(int level) {
  rx_log.level = level;
}

void rx_verbose(int line, const char* function, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  rx_log.log(RX_LOG_LEVEL_VERBOSE, line, function, fmt, args);
  va_end(args);
}

void rx_warning(int line, const char* function, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  rx_log.log(RX_LOG_LEVEL_WARNING, line, function, fmt, args);
  va_end(args);
}

void rx_error(int line, const char* function, const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  rx_log.log(RX_LOG_LEVEL_ERROR, line, function, fmt, args);
  va_end(args);
}

/* --------------------------------------------------------------------------------- */

#endif // defined(ROXLU_USE_LOG) && defined(ROXLU_IMPLEMENTATION)


#undef ROXLU_IMPLEMENTATION
