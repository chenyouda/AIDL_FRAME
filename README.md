# AIDL_FRAME
# STEP 1:
创建AIDL文件，在frameworks/base/core/java/android目录下创建my_mouse目录，可以参考frameworks/base/core/java/android其他服务类，如hardware，os等服务。该目录下我们要实现自己IMyMouseService，MyMouseService，MyMouseManager。
(1)IMyMouseService是AIDL接口类，定义了void setMouseEnble(boolean enble)方法。
(2)另外MyMouseService继承IMyMouseService.Stub，并真正实现void setMouseEnble(boolean enble)方法。
(3)MyMouseManager作为一个管理类出现，内部存放IMyMouseService对象，通过代理来调用服务端的方法。
# STEP 2:
已经创建my_mouse的服务目录，创建了AIDL文件，那么我们还需要告诉编译系统，以便编译出我们需要的对应的IMyMouseService.JAVA文件。此文件包含了接口及我们所要的IMyMouseService.Stub远程服务代理对象。并且添加到系统服务中，在开机启动systemservice时候添加到ServiceManager中。并且将MyMouseManager注册到SystemService中。
（1）在frameworks/base编辑mk文件，LOCAL_SRC_FILES += \中添加core/java/android/my_mouse/IMyMouseService.aidl \
（2）添加到系统服务在frameworks/base/services/java/com/android/server/SystemServer.java 中 public void initAndLoop()函数里添加
           //add by cyd begin
           try {
                Slog.i(TAG, "MyMouseService Service");
                MyMouseService myMouseService= new MyMouseService();
                ServiceManager.addService(Context.MY_MOUSE_SERVICE, myMouseService);
            } catch (Throwable e) {
                reportWtf("starting Ethernet Service", e);
            }
           //add by cyd end
           
（3）注册环境变量Context.MY_MOUSE_SERVICE及MyMouseManager。
     在抽象类frameworks/base/core/java/android/content/Context.jave中，添加变量
    // add by cyd begin
    public static final String MY_MOUSE_SERVICE = "MyMouseService";
    // add by cyd end
    
    在frameworks/base/core/java/android/app/ContextImpl.java中静态函数模块注册Service。
    //add by cyd begin
         registerService(MY_MOUSE_SERVICE, new ServiceFetcher() {
                public Object createService(ContextImpl ctx) {
                    IBinder b = ServiceManager.getService(MY_MOUSE_SERVICE);
                    IMyMouseService service = IMyMouseService.Stub.asInterface(b);
                    return new MyMouseManager(ctx, service);
                }});
        //add by cyd end
# STEP 3:
编译对应模块
#source build/envsetup.sh
#lunch x
frameworks/base#mm
将\out\target\common\obj\JAVA_LIBRARIES\framework_intermediates\class.jar拷贝出来，导入app project。app调用即可。
