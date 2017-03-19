package com.example.android_project;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.Random;
import java.util.Timer;
import java.util.TimerTask;

import com.example.android_project.RegisterActivity.DownloadTask;
import com.fs.util.MyUtil;
import com.spring.progressview.SpringProgressView;

import com_connext.MyConnext;

import android.app.Activity;
import android.content.Intent;
import android.graphics.drawable.AnimationDrawable;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.Switch;
import android.widget.TextView;
import android.widget.Toast;
import android.widget.ToggleButton;

public class ControlActivity extends Activity implements OnClickListener{

	private Button btntemp,btnhum,btnback;
	private ImageView imageViewF,imageViewL,imageViewB;
	private AnimationDrawable animationDrawable;
	private ToggleButton tbtnfan,tbtnLed;
	private TextView tvtemp,tvhum,tvillum,tvenv;
	private Timer timer;
	private SpringProgressView pVtemp,pVhum,pVillum;
//	private final int GET_BITMAP = 1;
	private InputStream in = null;
	private DataInputStream bin = null;
//
	private OutputStream out = null;
	private DataOutputStream bout = null;
	
    private MyHandler handler = null;
    private Random random = new Random(System.currentTimeMillis());
//    private Handler handler1 = null;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_control);
		this.handler = new MyHandler();
		this.init();
	}

	@Override
	public void onOptionsMenuClosed(Menu menu) {
		// TODO Auto-generated method stub
		super.onOptionsMenuClosed(menu);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.btn_temperature_button:
			createTimer();
			break;
		case R.id.btn_humidty_button:
			pVtemp.setCurrentCount(random.nextInt(100));//显示温度函数
			tvtemp.setText("最大温度值"+pVtemp.getMaxCount()+"℃"+"   当前温度值"+pVtemp.getCurrentCount()+"℃");
			pVhum.setCurrentCount(random.nextInt(100));//显示温度函数
			tvhum.setText("最大湿度值"+pVhum.getMaxCount()+"   当前湿度值"+pVhum.getCurrentCount());
			pVillum.setCurrentCount(random.nextInt(2000));//显示温度函数
			tvillum.setText("最大光照强度"+pVillum.getMaxCount()+"lx"+"   当前光照强度"+pVillum.getCurrentCount()+"lx");
//			createTimer1();
			break;
		case R.id.toggleButton1:
		{
			if (tbtnfan.isChecked()) {
				new DownloadTask().execute("5|FAN_ON");

				
			}else {
				new DownloadTask().execute("5|FAN_OFF");

			}
			break;
		}
		case R.id.toggleButton2:
			if (tbtnLed.isChecked()) {
				new DownloadTask().execute("5|LED_ON");
			}else{
				new DownloadTask().execute("5|LED_OFF");
			}
			break;
		case R.id.back_button:
			Intent intent = new Intent(ControlActivity.this,MonitoringActivity.class);
			startActivity(intent);
			finish();
			break;
			default :
			break;

		}

		
	}
	
	@Override
	protected void onPause() {
		// TODO Auto-generated method stub
		super.onPause();		
		if(this.timer != null){
		this.timer.cancel();
		}
		Log.i("FSLog", "onPause");


	}

	private void init() {
//		ImageView imageView = (ImageView)findViewById(R.id.animotion_ImageView);
		this.pVtemp = (SpringProgressView)findViewById(R.id.temperature_progress_view);
		this.pVhum = (SpringProgressView)findViewById(R.id.humidty_progress_view);
		this.pVillum = (SpringProgressView)findViewById(R.id.illuminations_progress_view);
		this.tvtemp = (TextView)findViewById(R.id.tv_temp_con);
		this.tvhum = (TextView)findViewById(R.id.tv_hum_con);
		this.tvillum = (TextView)findViewById(R.id.tv_illuminations_con);
		this.tvenv = (TextView)findViewById(R.id.tv_illuminations_con);
		pVtemp.setMaxCount(100.0f);//温度
		pVhum.setMaxCount(100.0f);//湿度
		pVillum.setMaxCount(3000.0f);//光照强度
		this.btntemp = (Button)findViewById(R.id.btn_temperature_button);
		btntemp.setOnClickListener(this);
		this.btnhum = (Button)findViewById(R.id.btn_humidty_button);
		btnhum.setOnClickListener(this);
		this.btnback = (Button)findViewById(R.id.back_button);
		btnback.setOnClickListener(this);
		this.imageViewF = (ImageView )findViewById(R.id.animotion_ImageView_fan);
//		imageViewF.setBackgroundResource(R.drawable.animation);
		this.imageViewB = (ImageView )findViewById(R.id.animotion_ImageView_buzz);
//		imageViewB.setBackgroundResource(R.drawable.animation);
		this.imageViewL = (ImageView )findViewById(R.id.animotion_ImageView_led);
		this.tbtnfan = (ToggleButton)findViewById(R.id.toggleButton1);
		tbtnfan.setOnClickListener(this);
		this.tbtnLed = (ToggleButton)findViewById(R.id.toggleButton2);
		tbtnLed.setOnClickListener(this);
	}
	
	public String sendToServer(String code) {
		try {
			if (background.s == null) {
				Log.i("FSLog", "连接socket");
				background.s = new Socket(
						(background.etIP).getText().toString(),
						Integer.parseInt(background.etPort.getText().toString()));

				Log.i("FSLog", "创建socket");
			}
			in = background.s.getInputStream();
			//				in = socket.getInputStream();
			bin = new DataInputStream(in);

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
//				Log.i("FSLog", background.s)
//				this.socket.close();
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
		// 从服务器端接收信息的BufferedInputStream
		
		String accept = (MyConnext.read(bin,20,4096));
		Log.i("FSLog", "accept="+accept);
		return accept;
	
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
			sendToServer_1(code);
			return null;
		}

		@Override
		protected void onPostExecute(String infor) {
			// TODO Auto-generated method stub
			super.onPostExecute(infor);	

	}
	
	}
	public void sendToServer_1(String code) {
		try {
			if (background.s == null) {
				Log.i("FSLog", "连接socket");
				background.s = new Socket(
						(background.etIP).getText().toString(),
						Integer.parseInt(background.etPort.getText().toString()));

				Log.i("FSLog", "创建socket");
			}
			in = background.s.getInputStream();
			//				in = socket.getInputStream();
			bin = new DataInputStream(in);

//			out = background.s.getOutputStream();
//			
		out = background.s.getOutputStream();
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
//				Log.i("FSLog", background.s)
//				this.socket.close();
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
//		// 从服务器端接收信息的BufferedInputStream
//		
//		String accept = (MyConnext.read(bin,20,4096));
//		Log.i("FSLog", "accept="+accept);
//		return accept;
	
	}
	   class MyHandler extends Handler {
	    	@Override
	    	public void handleMessage(Message msg) {//UI
				String[] segment = ((String) msg.obj).split("\\|");
				int i = segment.length;
				Log.i("FSLog", "i="+i);
				switch (Integer.parseInt(segment[0])){
				case 4:	
				pVtemp.setCurrentCount(Float.parseFloat(segment[1]));//显示温度函数
				tvtemp.setText("最大温度值"+pVtemp.getMaxCount()+"℃"+"   当前温度值"+pVtemp.getCurrentCount()+"℃");
				pVhum.setCurrentCount(Float.parseFloat(segment[2]));//显示温度函数
				tvhum.setText("最大湿度值"+pVhum.getMaxCount()+"%rh"+"   当前湿度值"+pVhum.getCurrentCount()+"%rh");
				pVillum.setCurrentCount(Float.parseFloat(segment[3]));//显示温度函数
				tvillum.setText("最大光照强度"+pVillum.getMaxCount()+"cd"+"   当前光照强度"+pVillum.getCurrentCount()+"cd");
				if(Integer.parseInt(segment[4]) > 0) {
//					tvenv.setText("LED已打开" + "\n");
//					imageViewL.setImageResource(R.drawable.animation_2);
//					animationDrawable = (AnimationDrawable) imageViewL.getDrawable();
//					animationDrawable.start();
					imageViewL.setImageResource(R.drawable.leda);
					Toast.makeText(ControlActivity.this, "Led open", 1).show();
				}else {
//					tvenv.setText("LED已关闭" + "\n");
//					animationDrawable = (AnimationDrawable) imageViewL.getDrawable();
//					animationDrawable.stop();
//					animationDrawable.selectDrawable(R.drawable.ledd);
					imageViewL.setImageResource(R.drawable.ledd);
					Toast.makeText(ControlActivity.this, "Led close", 1).show();
				}
				if(Integer.parseInt(segment[5]) > 0) {
//					tvenv.setText("风扇已打开" + "\n");
					imageViewF.setImageResource(R.drawable.animation);
					animationDrawable = (AnimationDrawable) imageViewF.getDrawable();
					animationDrawable.start();
					Toast.makeText(ControlActivity.this, "fan open", 1).show();
				}else {
//					tvenv.setText("风扇已关闭" + "\n");
					animationDrawable = (AnimationDrawable) imageViewF.getDrawable();
					animationDrawable.stop();
					Toast.makeText(ControlActivity.this, "fan close", 1).show();
				}
				if(Integer.parseInt(segment[6]) > 0) {
//					tvenv.setText("报警已打开" + "\n");
//					imageViewB.setImageResource(R.drawable.animation_1);
//					animationDrawable = (AnimationDrawable) imageViewB.getDrawable();
//					animationDrawable.start();
					imageViewB.setImageResource(R.drawable.buzc);
					Toast.makeText(ControlActivity.this, "buzz open", 1).show();
				}else {
//					tvenv.setText("报警已关闭" + "\n");
//					animationDrawable = (AnimationDrawable) imageViewB.getDrawable();
//					animationDrawable.stop();
					imageViewB.setImageResource(R.drawable.buzd);
					Toast.makeText(ControlActivity.this, "buzz close", 1).show();
				}
				break;
				
				default :
					break;
				} 
				super.handleMessage(msg);
	    	}
	    	
	    }


	public void createTimer() {
		
		Toast.makeText(this, "显示当前环境信息", 1).show();
		// 使用timer开启一个子线程
		this.timer = new Timer();
		this.timer.schedule(new TimerTask() {
			@Override
			public void run() {// 子线程
				System.out.println("执行run....");
				// 调用方法获取图片
//				String infor = sendToServer("4|");
				//使用UI线程中创建的handler对象发送Message
				Message msg = Message.obtain();
				String s=sendToServer("4|");
				msg.obj=s;
				Log.i("FSLog", "s = "+s);
				handler.sendMessage(msg);//发数据到UI线程中
			}
		}, 0, 2000);
	}	
	
}
