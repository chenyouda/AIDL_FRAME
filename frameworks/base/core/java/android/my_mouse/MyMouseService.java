package android.my_mouse;
import android.my_mouse.IMyMouseService.Stub;
import android.content.Intent;
import android.os.IBinder;
import android.os.RemoteException;
import android.util.Log;


public class MyMouseService extends IMyMouseService.Stub {
        
        boolean DBG=true;
        String TAG="MyMouseService";
        public MyMouseService() {
            init_native();
        }
	@Override
	public void setMouseEnble(boolean enble) throws RemoteException {
		// TODO Auto-generated method stub
		if (DBG) Log.d(TAG, "setMouseEnble "+enble);
                setMouseEnble_native(enble);
	}

        private static native boolean init_native();
        private static native void setMouseEnble_native(boolean enble);
}
