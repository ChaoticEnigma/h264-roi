/*

  MATH 
  --------------------------------------------------------------------------------------------------------
  - rx_clamp(v, low, high)                      - clamp a value between the given two low and high
  - rx_map(v, min, max, minOut, maxOut)         - map from one value to another
  - rx_smoothstep(edge0, edge1, t)              - smoothstep function
  - rx_ortho(l,r,b,t,n,r,dest*)                 - create a orthographic projection matrix in dest
  - rx_ortho_top_left(w,h,n,r,dest*)            - create a orthogrpahic projection matrix with 0,0 in the top left
  - rx_is_power_of_two(uint val)                - check if the given value is a power of two

  STRING
  --------------------------------------------------------------------------------------------------------
  - rx_str_pad(src, len, char)                  - padd a couple of characters after `src`
  - rx_join<T>(vector<T>, sep)                  - join a vector by the given separator
  - rx_string_replace(str, charFrom, charTo)    - replace a character in 'str' into charTo

  FILESYSTEM
  --------------------------------------------------------------------------------------------------------
  - rx_get_file_ext(filepath)                   - get the file extension for the given filepth
  - rx_get_exe_path()                           - get the path to the executable (as string)
  - rx_get_files(path, ext = "")                - get all the files from the given path (and extension)
  - rx_to_data_path(filename)                   - get the datapath for the given filename
  - rx_create_dir(path)                         - create a directory
  - rx_create_path(path)                        - create the path and all of its subdirectories 
  - rx_is_dir(path)                             - check if the given string is a directory (and e.g. not a file)
  - rx_load_file(file, datapath, vector<char>)  - load the contents of a file from file (in data path or not) as binary into the given vector
  - rx_get_file_contents(filepath, datapath)    - returns the contents of the file as string
  - rx_put_file_contents(filepath, data, datapath, binary) - save the given `data` into a file as text or binary
  - rx_file_exists(filepath)                    - check if the given file exists
  - rx_get_file_size(filepath)                  - return the file's size
  - rx_file_remove(filepath)                    - remove the given file
  - rx_rename_file(old, new)                    - rename a file
  - rx_strip_filename(path)                     - remove the filename from the given path
  - rx_strip_dir(path)                          - remove the directories from the given path returning only the filename
  - rx_norm_path(str)                           - creates a normalized path string

  TIME
  --------------------------------------------------------------------------------------------------------
  - rx_strftime(timestr)                        - create a time string
  - rx_millis()                                 - get the current time in millis
  - rx_time()                                   - get the current epoch timestamp
  - rx_get_date_time_string()                   - get the current date as a string
  - rx_sleep_millis()                           - sleep x millis

  CONVERSION
  --------------------------------------------------------------------------------------------------------
  - rx_string_to_sizet(str)                     - convert the given string to a size_t 
  - rx_string_to_int(str)                       - convert the given string to an integer
  - rx_int_to_string(num)                       - convert the given integer to a string

  LOG
  --------------------------------------------------------------------------------------------------------
  - RX_VERBOSE("%s", "some message")            - log a verbose message (removed when not in debug mode)
  - RX_WARNING("%s", "some message")            - log a warning message 
  - RX_ERROR("%s", "some message")              - log an error message
  - rx_log_set_level(RX_LOG_LEVEL_ERROR)        - set the log level

  <example>
      Log log;
      if(!log.setup("application")) {
        ::exit(EXIT_FAILURE);
      }
      log.mini();
  </example>

 */
#ifndef ROXLU_UTILSH
#define ROXLU_UTILSH

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <vector>
#include <cmath>                                    /* for pow() */
#include <sstream>
#include <fstream>
#include <algorithm>                                /* std::replace() */

#if defined(_WIN32)
#  include <utils/dirent.h>                         /* for stat() */  
#  include <direct.h>                               /* _mkdir */
#  include <Shlwapi.h>
#  include <stdarg.h>
#else
#  include <dirent.h>                               /* for stat() */
#  include <errno.h>                                /* for errno */
#endif

#if defined(__APPLE__)
#  include <libgen.h>                               /* dirname */
#  include <CoreFoundation/CFRunLoop.h>
#  include <mach/mach.h>
#  include <mach/mach_time.h>
#  include <mach-o/dyld.h>                          /* _NSGetExecutablePath */
#  include <sys/resource.h>
#  include <sys/sysctl.h>
#  include <sys/stat.h>                             /* stat() */
#  include <unistd.h>                               /* sysconf */
#elif defined(__linux) 
#  include <unistd.h>                               /* readlink(), getcwd() */
#  include <sys/time.h>                             /* timeofday */
#  include <libgen.h>                               /* dirname */
#  include <sys/stat.h>
#  include <stdarg.h>
#  define MAX_PATH 4096
#endif

extern "C" {
#  include <uv.h>                                    /* we depend on libuv */
}


// LOG DEFINES
// ---------------------------------------------------------------
#define RX_LOG_NO_COLOR

#define RX_LOG_LEVEL_NONE 0 
#define RX_LOG_LEVEL_ERROR  1
#define RX_LOG_LEVEL_WARNING 2
#define RX_LOG_LEVEL_VERBOSE 3
#define RX_LOG_LEVEL_ALL 4

#if defined(_WIN32)
#  define ANSI_VERBOSE   "\x1b[32;1m"
#  define ANSI_WARNING   "\x1b[35;1m"
#  define ANSI_ERROR     "\x1b[31;1m"
#else 
#  define ANSI_VERBOSE   "\x1b[32m"
#  define ANSI_WARNING   "\x1b[35m"
#  define ANSI_ERROR     "\x1b[31m"
#endif

#if defined(_MSC_VER)
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); } 
#  define RX_WARNING(fmt, ...) { rx_warning(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); } 
#  define RX_ERROR(fmt, ...)   { rx_error(__LINE__, __FUNCSIG__, fmt, ##__VA_ARGS__); } 
#else                                                                             
#  define RX_VERBOSE(fmt, ...) { rx_verbose(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define RX_WARNING(fmt, ...) { rx_warning(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#  define RX_ERROR(fmt, ...)   { rx_error(__LINE__, __PRETTY_FUNCTION__, fmt, ##__VA_ARGS__); } 
#endif

#if !defined(RX_LOG_LEVEL)
#  define RX_LOG_LEVEL RX_LOG_LEVEL_VERBOSE
#endif

// unset log macros based on the -DRX_LOG_LEVEL preprocessor variable
#if RX_LOG_LEVEL == RX_LOG_LEVEL_VERBOSE
#  elif RX_LOG_LEVEL ==  RX_LOG_LEVEL_WARNING
#  undef RX_VERBOSE
#elif RX_LOG_LEVEL == RX_LOG_LEVEL_ERROR
#  undef RX_VERBOSE
#  undef RX_WARNING
#endif

#if RX_LOG_LEVEL == RX_LOG_LEVEL_NONE
#  undef RX_ERROR
#  undef RX_VERBOSE
#  undef RX_WARNING
#endif

// if not set anymore, define empties
#if !defined(RX_VERBOSE)
#  define RX_VERBOSE(fmt, ...) {}
#endif

#if !defined(RX_WARNING)
#  define RX_WARNING(fmt, ...) {}
#endif

#if !defined(RX_ERROR)
#  define RX_ERROR(fmt, ...) {}
#endif

extern "C" {

  typedef void(*roxlu_log_callback)(int level, void* user, int line, const char* function, const char* fmt, va_list args);

  extern roxlu_log_callback roxlu_log_cb;
  extern void* roxlu_log_user;
  extern int roxlu_log_level;

  void rx_log_set_callback(roxlu_log_callback cb, void* user);
  void rx_log_set_level(int level); 
  void rx_log_default_callback(int level, void* user, int line, const char* function, const char* fmt, va_list args); 

  void rx_verbose(int line, const char* function, const char* fmt, ...);
  void rx_warning(int line, const char* function, const char* fmt, ...);
  void rx_error(int line, const char* function, const char* fmt, ...);
} // extern C


// ---------------------------------------------------------------

static std::string rx_get_file_ext(std::string filepath);
static bool rx_file_exists(std::string filepath);

// as suggested: http://stackoverflow.com/questions/4100657/problem-with-my-clamp-macro
template <typename T> 
inline T rx_clamp(T value, T low, T high) {
    return (value < low) ? low : ((value > high) ? high : value);
}

template<typename T>
inline T rx_map(T value, T minIn, T maxIn, T minOut, T maxOut) {

	T range = ((value - minIn) / (maxIn - minIn) * (maxOut - minOut) + minOut);
	return range;
}

inline float rx_smoothstep(float edge0, float edge1, float t) {
  return edge0 + (pow(t,2) * (3-2*t)) * (edge1 - edge0);
}

inline void rx_sleep_millis(int millis) {
#ifdef _WIN32
  Sleep(millis);
#else
  usleep(millis * 1000);
#endif
}

inline std::string rx_str_pad(std::string src, int len, char pad) {
  if(src.size() < len) {
    for(size_t i = src.size(); i < len; ++i) {
      src.push_back(pad);
    }
  }
  return src;
}

inline std::string rx_strftime(const char* timestr) {
  time_t t;
  struct tm* info;
  char buf[4096]; // must be enough..
  time(&t);
  info = localtime(&t);
  strftime(buf, 4096, timestr, info);
  std::string result(buf);
  return result;
}

inline std::string rx_get_date_time_string() { 
  return rx_strftime("%Y.%m.%d.%H.%M.%S");
};

template<class T>
static std::string rx_join(const std::vector<T>& entries, std::string sep) {
  std::string result;
  for(typename  std::vector<T>::const_iterator it = entries.begin(); it != entries.end(); ++it) {
    std::stringstream ss; 
    ss << *it;
    result.append(ss.str());
    if(it + 1 != entries.end()) {
      result.append(sep);
    }
  }
  return result;
}


// -------------------------------------- WIN ---------------------------------------
#ifdef _WIN32

static std::string rx_get_exe_path() {
  char buffer[MAX_PATH];

  // Try to get the executable path with a buffer of MAX_PATH characters.
  DWORD result = ::GetModuleFileNameA(nullptr, buffer, static_cast<DWORD>(MAX_PATH));
  if(result == 0) {
    return "";
  }

  std::string::size_type pos = std::string(buffer).find_last_of( "\\/" );

  return std::string(buffer).substr(0, pos) +"\\";
}

static rx_int64 rx_millis(void) {
  static LARGE_INTEGER s_frequency;
  static BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
  if (s_use_qpc) {
    LARGE_INTEGER now;
    QueryPerformanceCounter(&now);
    return (1000LL * now.QuadPart) / s_frequency.QuadPart;
  } 
  else {
    return GetTickCount();
  }
}

/* returns epoch timestamp */
static time_t rx_time() {
  return time(NULL);
}


// -------------------------------------- LINUX -------------------------------------
#elif defined(__linux) 


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

static int64_t rx_millis() {
  timeval time;
  gettimeofday(&time, NULL);
  int64_t n = time.tv_usec;
  n /= 1000; // convert seconds to millis
  n += (time.tv_sec * 1000); // convert micros to millis
  return n;
}

/* returns epoch timestamp */
static time_t rx_time() {
  return time(NULL);
}

// -------------------------------------- OSX ---------------------------------------
#elif defined(__APPLE__)
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

static int64_t rx_millis(void) {
  mach_timebase_info_data_t info;
  if (mach_timebase_info(&info) != KERN_SUCCESS) {
    abort();
  }
  return (mach_absolute_time() * info.numer / info.denom) / 1000000;
}

/* returns epoch timestamp */
static time_t rx_time() {
  return time(NULL);
}

#endif

// ---------------------------------------------------------------------------------
static bool rx_is_dir(std::string filepath) {
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

static std::string rx_to_data_path(const std::string filename) {
  std::string exepath = rx_get_exe_path();

#if defined(__APPLE__)
  if(rx_is_dir(exepath +"data")) {
    exepath += "data/" +filename;
  }
  else if(rx_is_dir(exepath +"../MacOS")) {
    exepath += "../../../data/" +filename;
  }
#else 
  exepath += "data/" +filename;
#endif  

  return exepath;
}

static std::string rx_to_exe_path(std::string filename) {
  return rx_get_exe_path() +filename;
}

// read a binary file 
static bool rx_load_file(std::string filepath, bool datapath, std::vector<char>& result) {

  if(datapath) {
    filepath = rx_to_data_path(filepath);
  }

  if(!rx_file_exists(filepath)) {
    printf("Cannot find file: %s", filepath.c_str());
    return false;
  }

  std::ifstream ifs(filepath.c_str(), std::ios::in | std::ios::binary);
  if(!ifs.is_open()) {
    printf("Cannot open the file: %s", filepath.c_str());
    return false;
  }

  result.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());

  ifs.close();

  return true;
}

// read a text file line by line
static std::string rx_get_file_contents(std::string filepath, bool datapath = false) {
  if(datapath) {
    filepath = rx_to_data_path(filepath);
  }

  std::string result = "";
  std::string line = "";
  std::ifstream ifs(filepath.c_str());
  if(!ifs.is_open()) {
    printf("Cannot open file: '%s'", filepath.c_str());
    return result;
  }
  while(getline(ifs,line)) {
    result += line +"\n";
  }
  return result;
}

static bool rx_put_file_contents(std::string filename, std::string data, bool datapath = false, bool isBinary = false) {

  if(datapath) {
    filename = rx_to_exe_path(filename);
  }

  std::ofstream ofs;
  if(isBinary) {
    ofs.open(filename.c_str(), std::ios::out | std::ios::binary);
  }
  else {
    ofs.open(filename.c_str(), std::ios::out);
  }

  if(!ofs.is_open()) {
    printf("Cannot open file for output: %s", filename.c_str());
    return false;
  }

  if(!ofs.write(data.c_str(), data.size())) {
    printf("Error while writing data to: %s", filename.c_str());
    return false;
  }

  ofs.close();

  return true;
}

static bool rx_file_exists(std::string filepath) {
#if defined(_WIN32)
  char* lptr = (char*)filepath.c_str();
  DWORD dwattrib = GetFileAttributes(lptr);
  return (dwattrib != INVALID_FILE_ATTRIBUTES && !(dwattrib & FILE_ATTRIBUTE_DIRECTORY));

#elif defined(__APPLE__)
  int res = access(filepath.c_str(), R_OK);
  if(res < 0) {
    return false;
  }
#endif

  return true;
}

static size_t rx_get_file_size(std::string filepath) {
  if(!rx_file_exists(filepath)) {
    return 0;
  }
  struct stat stat_buf;
  int rc = stat(filepath.c_str(), &stat_buf);
  return rc == 0 ? stat_buf.st_size : 0;
} 

static bool rx_file_remove(std::string filepath) {
  if(!rx_file_exists(filepath)) {
    return false;
  }
  if(::remove(filepath.c_str()) != 0) {
    printf("cannot remove file: %s - %s", filepath.c_str(), strerror(errno));
    return false;
  }
  return true;
}

static bool rx_rename_file(std::string from, std::string to) {
  return rename(from.c_str(), to.c_str()) == 0;
}

/* 
   double check that your window is really APP_WIDTH and APP_HEIGHT!
   rx_ortho(0, APP_WIDTH, APP_HEIGHT, 0, -1.0, 1.0, pm);
 */
static void rx_ortho(float l, float r, float b, float t, float n, float f, float* dest) {
  dest[0] = (2.0f / (r - l));
  dest[1] = 0.0f;
  dest[2] = 0.0f;
  dest[3] = 0.0f;

  dest[4] = 0.0f;
  dest[5] = (2.0f / (t - b));
  dest[6] = 0.0f;
  dest[7] = 0.0f;
  
  dest[8] = 0.0f; 
  dest[9] = 0.0f;
  dest[10] = (-2.0f / (f - n));
  dest[11] = 0.0f;

  dest[12] = - ((r + l) / (r - l));
  dest[13] = - ((t + b) / (t - b));
  dest[14] = - ((f + n) / (f - n));
  dest[15] = 1.0f;
}

/* 
   create ortho matrix, where top left = (0.0, 0.0)
 */
static void rx_ortho_top_left(float w, float h, float n, float f, float* dest) {
  rx_ortho(0, w, h, 0, n, f, dest);
}

static size_t rx_string_to_sizet(std::string str) {
  size_t result = 0;
  std::stringstream ss;
  ss << str;
  ss >>  result;
  return result;
}

static int rx_string_to_int(std::string str) {
  int result = 0;
  std::stringstream ss;
  ss << str;
  ss >>  result;
  return result;
}


static std::string rx_int_to_string(int num) {
  std::string str;
  std::stringstream ss;
  ss << num;
  return ss.str();
}

static std::string rx_string_replace(std::string str, std::string from, std::string to) {
  size_t start_pos = str.find(from);
  if(start_pos == std::string::npos) {
    return str;
  }
  str.replace(start_pos, from.length(), to);
  return str;
}

static std::string rx_string_replace(std::string str, char from, char to) {
  std::replace(str.begin(), str.end(), from, to);
  return str;
}

static std::string rx_strip_filename(std::string path) {
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

static std::string rx_strip_dir(std::string path) {
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

static bool rx_create_dir(std::string path) {
#ifdef _WIN32
  if(_mkdir(path.c_str()) != 0) {
    if(errno == ENOENT) { 
      printf("Cannot create directory: %s (ENOENT)", path.c_str());
      return false;
    }
    else if(errno == EEXIST) {
      printf("Cannot create directory: %s (EEXIST)", path.c_str());
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

// changes forward slashes (the standard used in roxlu lib), to backward on windows
// e.g.: rx_norm_path("data/frames/2015/"), becomes: "data\frames\2015" on win, on unices it stays "data/frames/2015"
static std::string rx_norm_path(std::string path) {
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
}

// e.g.: rx_create_path(/home/roxlu/data/images/2012/12/05/")
static bool rx_create_path(std::string path) {

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

    dir_path += dirs[i];
    if(stat(dir_path.c_str(), &s) != 0) {
      if(!rx_create_dir(dir_path.c_str())) {
        printf("ERROR: cannot create directory: %s", dir_path.c_str());
        return false;
      }
    }
  }
  return true;
}

// retrieve files from the given path. if you pass an extension we only return files that have this extension. Use a value like "jpg", "flv", "txt", so w/o the dot
static std::vector<std::string> rx_get_files(std::string path, std::string ext = "") { 
  std::vector<std::string> result;
  DIR* dir;
  struct dirent* ent;
  if((dir = opendir(path.c_str())) != NULL) {
    while((ent = readdir(dir)) != NULL) {
      if(ent->d_type == DT_REG) {
        std::string file_path = path +"/" +ent->d_name;

        if(ext.size()) {
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
  return result;
}

// returns the extension of a file; e.g. "tga", "flv", "gif", etc..
static std::string rx_get_file_ext(std::string filepath) {
  size_t pos = filepath.rfind(".");
  std::string result = "";

  if(pos == std::string::npos) {
    return result;
  }

  std::string ext = filepath.substr(pos + 1, (filepath.size() - pos));
  return ext;
}

static bool rx_is_power_of_two(unsigned int x) {
  while (((x & 1) == 0) && x > 1) { /* While x is even and > 1 */
    x >>= 1;
  }
  return (x == 1);
}


// ENDIANNESS
// -------------------------------------------------------
inline uint16_t rx_get_le_u16(const unsigned char* data) {
  return (data[1] << 8)|(data[0]);
}

inline uint32_t rx_get_le_u32(const unsigned char* data) {
  return (data[3] << 24)|(data[2] << 16)|(data[1] << 8)|(data[0]);
}

inline uint64_t rx_get_le_u64(const unsigned char* data) {
  return ((uint64_t)data[7] << 56) 
    | ((uint64_t)data[6] << 48)
    | ((uint64_t)data[5] << 40) 
    | ((uint64_t)data[4] << 32)
    | ((uint64_t)data[3] << 24)
    | ((uint64_t)data[2] << 16)
    | ((uint64_t)data[1] << 8)
    | ((uint64_t)data[0]);
}

// store a uint16 into mem, Little Endian
inline void rx_put_le_u16(char* mem, uint16_t val) {
  mem[0] = val;
  mem[1] = val >> 8;
}

// store an uint32 into mem, Little Endian
inline void rx_put_le_u32(char* mem, uint32_t val) {
  mem[0] = val;
  mem[1] = val >> 8;
  mem[2] = val >> 16;
  mem[3] = val >> 24;
}

// LOG IMPLEMENTATION
// --------------------------------------------------------------
struct LogMessage {
  std::string tty_message;
  std::string file_message;
  int level;
};

class Log {
 public:
  Log();
  ~Log();

  bool setup(std::string name, std::string path = "");  /* create a new log with the name `name` in dir `data/[path]/[name].log */
  void addMessage(LogMessage msg);                          /* gets called indirectly by RX_VERBOSE/RX_WARNING/RX_ERROR */

  void mini();                                              /* log minimal info, just the message */
  void maxi();                                              /* log maximum info */

  void logFunctionName(bool flag);                          /* log the function name of the caller */
  void logLineNumber(bool flag);                            /* log the line number of the caller */
  void logTime(bool flag);                                  /* log the timestamp */
  void logLevel(bool flag);                                 /* log [verbose], [warning], [error], etc.. */
                                                         
  void useColors(bool flag);                                /* enable/disable colors in the tty output */
  void writeToFile(bool flag);                              /* enable/disable writing to a log file */
  void writeToConsole(bool flag);                           /* enable/disable writing to the tty console */
                                                         
  void rotateLog();                                          /* when the file is bigger then max_file_size (in bytes), we rename the current file to somethigng like: [logname]-2013_04_03_16_33.log */
                                                         
 private:                                               
  bool createLogFile();                                
                                                         
 public:                                                
  bool write_function_name;                                 /* when true, we add the function name to the log lines */
  bool write_line_number;                                   /* when true, we add the line number where the log function was called */
  bool use_colors;                                          /* colorify the output to tty */
  bool write_to_file;                                       /* log messages to a file */
  bool write_to_console;                                    /* write to tty */
  bool write_time;                                          /* write the current time to a log file */
  bool write_log_level;                                     /* write [verbose], [warning], [error], etc.. */
                                                         
  std::string date_format;                                  /* strftime() date format */
  std::string log_file;                                     /* the path to the current log file */
  std::string log_name;                                     /* the name of this log. e.g. `roxlu` creates `roxlu.log` */
  std::string log_path;                                     /* the path name/dirname where we store the log (in the data path), e.g. `log/` */
  size_t max_file_size;                                     /* maximum filesize of the log; if the file gets bigger we rotate it */
    
  uv_loop_t* loop;
  uv_mutex_t mutex;
  uv_tty_t tty;
  std::ofstream ofs;
};

inline void Log::logFunctionName(bool flag) {
  write_function_name = flag;
}

inline void Log::logLineNumber(bool flag) {
  write_line_number = flag;
}

inline void Log::logTime(bool flag) {
  write_time = flag;
}

inline void Log::logLevel(bool flag) {
  write_log_level = flag;
}

inline void Log::useColors(bool flag) {
  use_colors = flag;
}

inline void Log::writeToConsole(bool flag) {
  write_to_console = flag;
}

inline void Log::writeToFile(bool flag) {
  write_to_file = flag;
}


#endif // ROXLU_UTILSH
