#include <syscall.hh>
#include <fmt.hh>

void write_hex(size_t val) {
	char buf[128];
	char *end=buf+sizeof(buf)-1;
	char *pos=fmt::fmt_hex(val,buf,buf+sizeof(buf)-1);
	write(2,pos,end);
};
void *malloc(size_t size)
{
	char *block=(char*)sbrk(size);
	if (block == (void*) -1)
		return nullptr;
	return block;
}
void free(void *vp) {
};
void *__curbrk;

int brk (void *addr)
{
	char *newbrk;

	asm (
			"syscall\n" 
			: "=a"(newbrk) 
			: "0"(12), "D"(addr)
			: "rcx", "r11", "memory"
			);

	__curbrk=newbrk;

  if (newbrk < addr)
    {
      set_errno (ENOMEM);
      return -1;
    }

  return 0;
}
void * sbrk (ssize_t increment)
{
  void *oldbrk;

  /* If this is not part of the dynamic library or the library is used
     via dynamic loading in a statically linked program update
     __curbrk from the kernel's brk value.  That way two separate
     instances of brk and sbrk can share the heap, returning
     interleaved pieces of it.  */
	if (!__curbrk && (brk(0)<0))
		return (void*)-1;

  if (increment == 0)
    return __curbrk;

  oldbrk = __curbrk;
  if (increment > 0
      ? ((uintptr_t) oldbrk + (uintptr_t) increment < (uintptr_t) oldbrk)
      : ((uintptr_t) oldbrk < (uintptr_t) -increment))
    {
      set_errno (ENOMEM);
      return (void *) -1;
    }

  if (brk (ostr_t(oldbrk) + increment) < 0)
    return (void *) -1;

  return oldbrk;
}
