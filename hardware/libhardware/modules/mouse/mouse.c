#include <hardware/hardware.h>
#include <hardware/mouse.h>
#include <fcntl.h>
#include <errno.h>
#include <cutils/log.h>
#include <cutils/atomic.h>


#define LOG_TAG "MouseStub"

#define DEVICE_NAME "/sys/devices/platform/Mstar-ir.0/smouse_enable"
#define MODULE_NAME "Mouse"
#define MODULE_AUTHOR "chenyouda"


//define open funtion
static int mouse_device_open(const struct hw_module_t* module,const char* name,struct hw_device_t** device); 

//define close funtion
static int mouse_device_close(struct hw_device_t* device);


//define interface
static bool setMouseEnble(struct mouse_device_t* dev,bool enbleOrNo);

//define mouse methods
static struct hw_module_methods_t mouse_module_methods = {
    open: mouse_device_open
};

// define mouse instance
struct mouse_module_t HAL_MODULE_INFO_SYM = {
    common: {
       tag: HARDWARE_MODULE_TAG,
       version_major: 1,
       version_minor: 0,
       id: MOUSE_HARDWARE_MODULE_ID,
       name: MODULE_NAME,
       author: MODULE_AUTHOR,
       methods: &mouse_module_methods,
  }
};


static int mouse_device_open(const struct hw_module_t* module,const char* name,struct hw_device_t** device)
{
    struct mouse_device_t* dev = (struct mouse_device_t*)malloc(sizeof(struct mouse_device_t));

    if(!dev){
       ALOGE("MOUSE stub: fail to alloc space!");
       return -EFAULT;
    }
    memset(dev,0,sizeof(struct mouse_device_t));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (hw_module_t*)module;
    dev->common.close = mouse_device_close;
    dev->setMouseEnble = setMouseEnble;

    if((dev->fd = open(DEVICE_NAME,O_RDWR)) == -1){
       ALOGE("mouse stub: failed to open device -- %s",strerror(errno));
       free(dev);
       return -EFAULT;
    }

    *device = &(dev->common);
    ALOGE("MOUSE stub open success !");

    return 0;

}

static int mouse_device_close(struct hw_device_t* device) 
{

    struct mouse_device_t* dev = (struct mouse_device_t*)device;
    if(dev){
      close(dev->fd);
      free(dev);
    }
    return 0;
}

static bool setMouseEnble(struct mouse_device_t* dev,bool enbleOrNo)
{
   
    if(dev){
      if(enbleOrNo)
           write(dev->fd,"1",1);
      else
           write(dev->fd,"0",1);
    }
    return 0;
}
