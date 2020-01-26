#ifndef write_buf_hh
#define write_buf_hh write_buf_ff

#include <fmt.hh>
#include <c_str.hh>
#include <string.h>
#include <memory>

void check_print(const c_str &str);
void call_write(fd_t fd, const c_str &str);
template<size_t pages=1, size_t page_size=4096>
struct write_buf
{
  typedef fmt::fmt_t fmt_t;
  enum { size = pages * page_size };
  struct body_t {
    int fd;
    size_t pos;
    size_t tot;
    char buf[size-1];
    char end[1];
    body_t(int fd)
      : fd(fd), pos(0), tot(0)
    {
      memset(buf,0,end-buf);
    };
    size_t room() const {
      return size-pos;
    };
    void flush()
    {
      if(!pos)
        return;
      tot+=pos;
      write(fd,buf,pos);
      pos=0;
      memset(&buf,0,end-buf);
    };
    void put(c_str str)
    {
      // this while will repeat if there are zeroes in the str.
      while(str) {
        size_t room=this->room();
        if(!room) {
          flush();
        } else if ( str.len() > room ) {
          char ch;
          while(str && room--)
            if(ch=*str++)
              buf[pos++]=ch;
        } else {
          char ch;
          while(str)
            if(ch=*str++)
              buf[pos++]=ch;
        };
      };
      if(!room())
        flush();
    };
    void putln(c_str str)
    {
      put(str);
      switch(room()) {
        case 0:
          flush();
          // fall through
        case 1:
          buf[pos++]='\n';
          flush();
          break;
        default:
          buf[pos++]='\n';
          break;
      };
    };
  };
  std::shared_ptr<body_t> body;

  write_buf(int fd)
    : body(new body_t(fd))
  {
  };
  ~write_buf()
  {
    body->flush();
  };

  size_t room() const {
    return body->room();
  };
  void flush()
  {
    body->flush();
  };
  bool between(char min, char val, char max) {
    return min<=val && val<=max;
  };
  void putln(const c_str &str) {
    body->putln(str);
  };
  void put(const c_str &str) {
    body->put(str);
  };
  void putln(const fmt_t &fmt) {
    c_str str=fmt.get();
    putln(str);
  };
  void put(const fmt_t &fmt) {
    c_str str=fmt.get();
    put(str);
  };
  template<size_t n>
    void putln(const char (&str)[n]){
      putln(c_str(str,n-1));
    };
  template<size_t n>
    void put(const char (&str)[n]){
      put(c_str(str,n-1));
    };
  void put(const char *str)
  {
    body->put(c_str(str));
  };
  void put(char *str)
  {
    body->put(c_str(str));
  };
  template <typename val_t>
    void fmtln(const val_t &val)
    {
      fmt_t fmt(val);
      body->putln(fmt.get());
    };
  template <typename val_t>
    void fmt(const val_t &val)
    {
      fmt_t fmt(val);
      body->put(fmt.get());
    };
};
#endif
