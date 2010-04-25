#ifndef LIBBLURAY_ATTRIBUTES_H_
#define LIBBLURAY_ATTRIBUTES_H_

#if __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3 )
#  define BD_ATTR_FORMAT_PRINTF(format,var) __attribute__((__format__(__printf__,format,var)))
#  define BD_ATTR_MALLOC                    __attribute__((__malloc__))
#  define BD_ATTR_PACKED                    __attribute__((packed))
#else
#  define BD_ATTR_FORMAT_PRINTF(format,var)
#  define BD_ATTR_MALLOC
#  define BD_ATTR_PACKED
#endif

#endif /* LIBBLURAY_ATTRIBUTES_H_ */
