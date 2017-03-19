package com.example.android_project;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.util.Timer;
import java.util.TimerTask;

import com.example.android_project.RegisterActivity.DownloadTask;
import com.fs.util.MyUtil;

import android.app.Activity;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.format.Time;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class MonitoringActivity extends Activity implements OnClickListener{
	private ImageView imageView;
	private Timer timer;
	private Handler handler = null ;

	private Button btncon;//btnmon,
	private ToggleButton tbtn,tbtnbuzz;
//	private Socket socket = null;
	private final int GET_BITMAP = 1;
	private InputStream in = null;
	private DataInputStream bin = null;

	private OutputStream out = null;
	private DataOutputStream bout = null;
	
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		this.setContentView(R.layout.activity_monitoring);
		this.handler = new MyHandler();
		this.init();

	}
	private void init() {
		this.imageView = (ImageView)findViewById(R.id.iv_Mon_act);
		imageView.setAdjustViewBounds(true);
		imageView.setScaleType(imageView.getScaleType());
//		imageView.setOnClickListener(this);
//		this.btnmon = (Button)findViewById(R.id.btn_Mon_mon);
//		btnmon.setOnClickListener(this);
		this.tbtn = (ToggleButton)findViewById(R.id.toggleButton);
		tbtn.setOnClickListener(this);
		this.tbtnbuzz= (ToggleButton)findViewById(R.id.toggleButton1);
		tbtnbuzz.setOnClickListener(this);
		this.btncon = (Button)findViewById(R.id.btn_Mon_con);
		btncon.setOnClickListener(this);

	}

	
	@Override
	public boolean onCreateOptionsMenu(Menu menu) {
		// TODO Auto-generated method stub
		return super.onCreateOptionsMenu(menu);
	}
	
	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId()) {
		case R.id.btn_Mon_con:
			Intent intent4 = new Intent(MonitoringActivity.this,ControlActivity.class);
			startActivity(intent4);
//			finish();
			break;
//		case R.id.btn_Mon_mon:
//		{
//			Log.i("FSLog", "正在监控...!");
////			while(true){
////			this.handler = new MyHandler();
//			createTimer();
////			}
//			break;
//		}
		case R.id.toggleButton:
		{
			if (tbtn.isChecked()) {
				Log.i("FSLog", "正在监控...!");
//				while(true){
//				this.handler = new MyHandler();
				createTimer();
//				}
				
			}else {
				this.timer.cancel();
				Toast.makeText(this, "close", 1).show();
			}
			break;
		}
		case R.id.toggleButton1:
		{
			if (tbtnbuzz.isChecked()) {
				Log.i("FSLog", "正在报警!");
				new DownloadTask().execute("5|BUZZ_ON");
				Toast.makeText(this, "close", 1).show();
				
			}else {
				new DownloadTask().execute("5|BUZZ_OFF");
				Toast.makeText(this, "close", 1).show();
			}
			break;
		}

			default :
				break;
		}
		
	}
	public void createTimer() {
		
		Toast.makeText(this, "open", 1).show();

		// 使用timer开启一个子线程
		this.timer = new Timer();
		this.timer.schedule(new TimerTask() {
			@Override
			public void run() {// 子线程
				System.out.println("执行run....");
				// 调用方法获取图片
				getImage();
			}
		}, 0, 100);
	}

	
	class MyHandler extends Handler {
		private Bitmap bitmap;

		@Override
		public void handleMessage(Message msg) {
			int what = msg.what;
			Log.i("FSLog","handleMessage");
			switch (what) {
			case GET_BITMAP:

				byte[] image = (byte[]) msg.obj;

				// 创建图片前，先把之前的图片内存释放
				if (bitmap != null && !bitmap.isRecycled()) {
					bitmap.recycle();
				}
				// 这个方法是 android sdk自带，按照规范直接调用。就是把字节转为bitmap
				this.bitmap = BitmapFactory.decodeByteArray(image, 0, image.length);

				// 把bitmap给ImageView，就可以显示出图片
				Log.i("FSLog","setImageBitmap");
				imageView.setImageBitmap(bitmap);
				
				break;
//				
//				default :
//					break;
			}
			super.handleMessage(msg);
		}
	}


	public  void getImage() {

//		String accept = null;
		try {
			if (background.s == null) {
				Log.i("FSLog", "连接socket");
				background.s = new Socket(
						(background.etIP).getText().toString(),
						Integer.parseInt(background.etPort.getText().toString()));

				Log.i("FSLog", "创建socket");
			}
			Log.i("FSLog","getImage");
				in = background.s.getInputStream();
				//				in = socket.getInputStream();
				DataInputStream bin = new DataInputStream(in);

				out = background.s.getOutputStream();
				
				//				out = socket.getOutputStream();
				DataOutputStream bout = new DataOutputStream(out);

				// 将数据写入流中
				byte[] data = new byte[20];
				String n = (2 + "|");
				data = n.getBytes();
				Log.i("FSLog", n);
				bout.write(data);
				bout.flush();
				// 从服务器端接收信息的BufferedInputStream
				
				getByteData(bin);
		
		} catch (NumberFormatException e) {
			// TODO Auto-generated catch block
//			NumberFormatException e
			e.printStackTrace();
			try {
			if(bin != null) {
			this.bin.close();
			}
			if(this.in != null){
			this.in.close();
			}
//			Log.i("FSLog", background.s)
//			this.socket.close();
			if(background.s != null){
			background.s.close();
			}
			timer.cancel();
			Log.i("FSLog","timer close");
			

		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		} 
//		catch (UnknownHostException e) {
//			// TODO Auto-generated catch block
//			e.printStackTrace();
//		}
		catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		

	}
	
	private void getByteData(DataInputStream bin) {
		// 1，从输入流中取6个字节，这6个字节表示当前图片的长度
		Log.i("FSLog","getByteData");
		byte[] imageSize_Byte = MyUtil.read(bin, 6, 4096);
		
		// 2，把取出来的数据转为字符串，字符串的格式是 数字+len,比如一个图片的长度是 20 ，那么现在的字符串就是"20len"
		String imageSize_String = new String(imageSize_Byte);

		Log.i("FSLog", "图片长度  "+imageSize_String);
		// 3，我们需要的是字符串中的数字，因为我们获取这个数字的目的是为了获取图片，所以我们现在需要把字符串后面的len去掉
		imageSize_String = imageSize_String.substring(0,
				imageSize_String.indexOf("|"));
		Log.i("FSLog","getByteData1");
		// 4，现在我们获取 图片长度是一个字符串，但是我们需要的是整型，所以我们现在把字符串转为整型
		int imageSize_int = new Integer(imageSize_String);
		Log.i("FSLog","imageSize_int="+imageSize_int);
		// 5，计算出图片长度后，开始获取代表图片的字节
		byte[] image = MyUtil.read(bin, imageSize_int, 4096);// 读取图片
		Message msg = handler.obtainMessage();
		msg.what = GET_BITMAP;
		msg.obj = image;
		handler.sendMessage(msg);
		
	}
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();
		if(this.timer != null){
		this.timer.cancel();
		}
		Log.i("FSLog", "this.timer.cancel()");
		Log.i("FSLog", "onPause");
//		finish();
		
	}
	
	class DownloadTask extends AsyncTask<String, Void, String> {
		String code;
		@Override
		protected void onPreExecute() {
			// TODO Auto-generated method stub
			super.onPreExecute();
			Log.i("FSLog", "onPreExecute");
		}
		@Override
		protected String doInBackground(String... params) {
			// TODO Auto-generated method stub
			code = params[0];
			sendToServer(code);
			return null;
		}

		@Override
		protected void onPostExecute(String infor) {
			// TODO Auto-generated method stub
			super.onPostExecute(infor);	

	}
	}
		public void sendToServer(String code) {
			try {
				if (background.s == null) {
					Log.i("FSLog", "连接socket");
					background.s = new Socket(
							(background.etIP).getText().toString(),
							Integer.parseInt(background.etPort.getText().toString()));

					Log.i("FSLog", "创建socket");
				}
//				in = background.s.getInputStream();
//				//				in = socket.getInputStream();
//				bin = new DataInputStream(in);

				out = background.s.getOutputStream();
				
				//				out = socket.getOutputStream();
				bout = new DataOutputStream(out);

				// 将数据写入流中
				String send = code;
				Log.i("FSLog", "sendToServer code = " + code);
				byte[] data = new byte[20];
				data = send.getBytes();
				Log.i("FSLog", send);
				bout.write(data);
				bout.flush();
				
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				try {
					if(bin != null) {
					this.bin.close();
					}
					if(this.in != null){
					this.in.close();
					}
//					Log.i("FSLog", background.s)
//					this.socket.close();
					if(background.s != null){
					background.s.close();
					}
//					timer.cancel();
//					Log.i("FSLog","timer close");
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
//			// 从服务器端接收信息的BufferedInputStream
//			
//			String accept = (MyConnext.read(bin,20,4096));
//			Log.i("FSLog", "accept="+accept);
//			return accept;
		
		}
	
}
