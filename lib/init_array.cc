typedef unsigned long size_t;
typedef int fd_t;
typedef long ssize_t;
typedef void(* atexit_func_t)(void);
typedef unsigned char uint8_t;

extern void (*__preinit_array_start []) (void) __attribute__((weak));
extern void (*__preinit_array_end []) (void) __attribute__((weak));
extern void (*__init_array_start []) (void) __attribute__((weak));
extern void (*__init_array_end []) (void) __attribute__((weak));
extern void (*__fini_array_start []) (void) __attribute__((weak));
extern void (*__fini_array_end []) (void) __attribute__((weak));

extern "C" {
	void _init();
	void _fini();
	void _exit(int return_code) __attribute__((noreturn));
	inline void _exit(int res)
	{
		int exit_val;
		exit_val=res&0xff;
		asm (
				"syscall\n" 
				: "=a"(res) 
				: "a"(60), "D"(exit_val)
				: "rcx", "r11", "memory"
				);
		while(1);
	}


	static void __libc_init_array() {
		size_t count, i;

		count = __preinit_array_end - __preinit_array_start;
		for (i = 0; i < count; i++)
			__preinit_array_start[i]();

		//_init();

		count = __init_array_end - __init_array_start;
		for (i = 0; i < count; i++)
			__init_array_start[i]();
	}

	static void __libc_fini_array() {
		size_t count, i;

		count = __preinit_array_end - __preinit_array_start;
		for (i = count - 1; i >= 0; i--)
			__fini_array_start[i]();

		_fini();
	}

#define MAX_ATEXIT 32

	static volatile uint8_t atexit_count = 0;
	static volatile atexit_func_t atexit_funcs[MAX_ATEXIT];

	int atexit(atexit_func_t func) {
		uint8_t pos = __sync_fetch_and_add(&atexit_count, 1);

		if (pos >= MAX_ATEXIT) {
			atexit_count = 32;
			return -1;
		}

		atexit_funcs[pos] = func;
		return 0;
	}

	void exit(int return_code) {
		uint8_t i;

		for (i = 0; i < atexit_count; i++) {
			atexit_funcs[i]();
		}

		__libc_fini_array();
		_exit(return_code);
	}

	void libc_init() {
		__libc_init_array();
	}
}
