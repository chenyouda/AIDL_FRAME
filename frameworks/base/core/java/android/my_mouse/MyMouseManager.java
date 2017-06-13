package android.my_mouse;
import android.my_mouse.IMyMouseService;
import android.my_mouse.MyMouseService;
import android.content.Context;
import android.os.RemoteException;
import android.util.Log;
import android.os.ServiceManager;
import android.os.IBinder;

/**
 * @author chenyouda
 * 
 */
public class MyMouseManager {
	IMyMouseService service;
        static MyMouseManager myMouseManager;
        String TAG="MyMouseManager";
        boolean DBG=true;


        public static MyMouseManager getInstance() {
	if(myMouseManager==null){
	        IBinder IBin=ServiceManager.getService("MyMouseService");
                if(IBin==null) {
                   Log.i(TAG,"IBin==null ERROR");
                }
                myMouseManager =new MyMouseManager(IMyMouseService.Stub.asInterface(IBin));
	        }
		return myMouseManager;
	}

	public IMyMouseService getService() {
		return service;
	}

	public MyMouseManager(IMyMouseService service) {
		super();
		this.service = service;
	}

	public MyMouseManager(Context context,IMyMouseService service) {
		super();
		this.service = service;
	}
	
	public void setMouseEnble(boolean enble) {
		if(service != null) {
			try {
                                if (DBG) Log.d(TAG, "MyMouseManager setMouseEnble "+enble);
				service.setMouseEnble(enble);
			} catch (RemoteException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
	}
}
