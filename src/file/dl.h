#ifndef DL_H_
#define DL_H_

// We don't bother aliasing dlopen to dlopen_posix, since only one
// of the .C files will be compiled and linked, the right one for the
// platform.

// Note the dlopen takes just the name part. "aacs", internally we
// translate to "libaacs.so" "libaacs.dylib" or "aacs.dll".
void   *dl_dlopen  ( const char* path );
void   *dl_dlsym   ( void* handle, const char* symbol );
int     dl_dlclose ( void* handle );

#endif /* DL_H_ */
