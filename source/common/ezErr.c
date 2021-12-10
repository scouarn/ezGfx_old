#include "ezErr.h"
#include <time.h>


void ERR_stamp_val(const char* file, const char* func, unsigned int line) {

	char buffer[16];
	time_t sec = time(NULL);
	struct tm* tm_info = localtime(&sec);

	strftime(buffer, 16, "%H:%M:%S", tm_info);

	fprintf(stderr, "[%s - %s:%d in %s] ", buffer, file, line, func);
}