#define LOG_TAG "MyMouseService"
#include "jni.h"
#include "JNIHelp.h"
#include "android_runtime/AndroidRuntime.h"
#include <utils/Log.h>
#include <utils/misc.h>
#include <hardware/hardware.h>
#include <hardware/mouse.h>
#include <stdio.h>
#include <cutils/atomic.h>

#include "android_my_mouse_MyMouseService.h"

namespace android
{
    struct mouse_device_t* dev = NULL;
    
    static inline int mouse_device_open(const hw_module_t* module,struct mouse_device_t** device)
    {
        return module->methods->open(module,MOUSE_HARDWARE_MODULE_ID,(struct hw_device_t**)device);
    }
    
    JNIEXPORT jboolean JNICALL Java_android_my_1mouse_MyMouseService_init_1native(JNIEnv *, jclass)
    {
        mouse_module_t* module;
        ALOGI("MOUSE JNI : init...");
        if(hw_get_module(MOUSE_HARDWARE_MODULE_ID,(const struct hw_module_t**)&module)==0){
            ALOGI("MOUSE JNI: mouse Stub found.");
            if(0 == mouse_device_open(&(module->common),&dev))
            {
                ALOGI("MOUSE JNI: mouse device is open.");
                return true;
            }
            ALOGE("MOUSE JNI: failed to open mouse device.");
            return false;
        }
        ALOGE("MOUSE JNI: failed to get mouse stub module.");
        return false;
    }
    
    JNIEXPORT void JNICALL Java_android_my_1mouse_MyMouseService_setMouseEnble_1native(JNIEnv *env, jclass clazz, jboolean value)
    {
        bool enble = (bool)value;
        if(NULL != dev){
            dev->setMouseEnble(dev,enble);
            ALOGE("MOUSE JNI: success to set module.");
            return;
        }
        ALOGE("MOUSE JNI: failed to set module.");
    }
    

    
    static const JNINativeMethod method_table[] = {
        {"init_native","()Z",(void*)Java_android_my_1mouse_MyMouseService_init_1native},
        {"setMouseEnble_native","(Z)V",(void*)Java_android_my_1mouse_MyMouseService_setMouseEnble_1native},
    };
    
    int register_android_server_MyMouseService(JNIEnv *env){
        return jniRegisterNativeMethods(env,"android/my_mouse/MyMouseService",method_table,NELEM(method_table));
    }
};
