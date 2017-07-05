#ifndef ANDROID_MOUSE_INTERFACE_H
#define ANDROID_MOUSE_INTERFACE_H
#include <hardware/hardware.h>
//#include <sys/types.h>
//#include <sys/cdefs.h>
//#include <strings.h>
//#include <stdint.h>
#include <cutils/bitops.h>


__BEGIN_DECLS

//define module id
#define MOUSE_HARDWARE_MODULE_ID "mouse"

//define mouse module public from hw_module_t
struct mouse_module_t {
    struct hw_module_t common;
};

//define mouse device interface public form hw_device_t
struct mouse_device_t {
    struct hw_device_t common;
    int fd;
    bool (*setMouseEnble)(struct mouse_device_t dev,bool enbleOrNo);
};

__END_DECLS
#endif
