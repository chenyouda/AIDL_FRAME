package com.ctv.setenblemouse;

import java.net.ContentHandler;

import android.my_mouse.MyMouseManager;
import android.os.Bundle;
import android.os.IBinder;
import android.os.ServiceManager;
import android.app.Activity;
import android.content.Context;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.Toast;

public class SetEnbleMouseActivity extends Activity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);

		int i=0;
        final MyMouseManager mouseManager=(MyMouseManager)SetEnbleMouseActivity.this.getSystemService("MyMouseService");//MyMouseManager.getInstance();

		setContentView(R.layout.activity_set_enble_mouse);
		Button btnButton=(Button)findViewById(R.id.setEnbleMouse);
		btnButton.setOnClickListener(new OnClickListener() {
		
			@Override
			public void onClick(View v) {
				// TODO Auto-generated method stub
				mouseManager.setMouseEnble(true);
				Toast tst = Toast.makeText(SetEnbleMouseActivity.this, "setMouseEnble(true)", Toast.LENGTH_SHORT);
				tst.show();
			}
		});
	}

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.set_enble_mouse, menu);
		return true;
	}

}
