package com.example.android_project;


import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;



import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.res.Configuration;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;

import android.util.Log;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

public class background extends Activity implements OnClickListener{
	TextView tv,tvshow;
	public static  EditText etIP,etPort;
	Button btn;
	public  static Socket s;
	public static final int MSG_UPDATE_STATUES = 100;//����״̬
	public static final int MSG_UPDATE_ENABLE = 200;//����
	public static final int MSG_UPDATE_INTENT = 300;//��������״̬
	public static final int MSG_UPDATE_STATUES_INTERNET = 400;//������
	public static final int MSG_UPDATE_STATUES_internet = 500;//������
	Handler mHandler = new Handler(){

		@Override
		public void handleMessage(Message msg) {
			// TODO Auto-generated method stub
			super.handleMessage(msg);
			switch(msg.what) {
			case MSG_UPDATE_STATUES:
				tvshow.setText((String)msg.obj);
				break;
			case MSG_UPDATE_ENABLE:
				btn.setEnabled(false);
				break;
			case MSG_UPDATE_INTENT:
				alertdialog();
//				Intent intent1 = new Intent(background.this,MainActivity.class);
//				startActivity(intent1);
				break;
			case MSG_UPDATE_STATUES_INTERNET:
				Toast .makeText(background.this, "����������", 1).show();
				break;
			case MSG_UPDATE_STATUES_internet:
				Toast .makeText(background.this, "����û����", 1).show();

				break;
			}
		}
		
		
	};
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_background);
//		Intent intent1 = new Intent(background.this,MainActivity.class);
//		startActivityForResult(intent1, 100);
		this.init();
//		new ConnectThread().start();
	}
	private void init() {
		tv = (TextView)findViewById(R.id.tv_background_act);
		tv.setOnClickListener(this);
		etIP = (EditText)findViewById(R.id.et_ip_background);
		String ip =etIP.getText().toString().trim();
		etPort = (EditText)findViewById(R.id.et_port_background);
		tvshow =(TextView)findViewById(R.id.tv_background_client);
		btn = (Button)findViewById(R.id.btn_background_act);
		btn.setOnClickListener(this);	
	}


	@Override
	public void onConfigurationChanged(Configuration newConfig) {
		// TODO Auto-generated method stub
		super.onConfigurationChanged(newConfig);
		if(this.getResources().getConfiguration().orientation == Configuration.ORIENTATION_LANDSCAPE) {
			setContentView(R.layout.activity_background);
			findViewById(R.id.background_act);
//			set
		
		}
	}


//	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch(v.getId()){
		
		case R.id.btn_background_act:
		new ConnectThread().start();
		break;
		case R.id.tv_background_act:
			alertdialog();
		break;
		default:
			break;
		}
	}

	class ConnectThread extends Thread {
		
		@Override
		public void run() {
			// TODO Auto-generated method stub
			super.run();
			//����serversocket���󣬲��ȴ�����
			checkNetworkInfo();
			int port = Integer.parseInt(etPort.getText().toString());
			if(port < 10000 || port > 65535) {
				updateStatus("�˿ںŲ���10000-65535֮��");
				return;
			} 
			
			//���ip�����ʹ��������ʽ
			String ip = etIP.getText().toString();
			
			try {
				//��.��Ϊ�ָ������ַ�����
				String[] segment = ip.split("\\.");
				int s0 = Integer.parseInt(segment[0]);
				int s1 = Integer.parseInt(segment[1]);
				int s2 = Integer.parseInt(segment[2]);
				int s3 = Integer.parseInt(segment[3]);
				
				if(s0 < 0 || s0 > 255 ||
						s1 < 0 || s1 > 255 || 
						s2 < 0 || s2 > 255 ||
						s3 < 0 || s3 > 255 ) {
					updateStatus("ip���Ϸ�");
					return;
				}
			} catch(Exception e) {
				updateStatus("ip���Ϸ�");
				return;
			}
			
			try {
				updateStatus("��������...");
				//����socket����
				s = new Socket(ip, port);
				if((s!= null) && (s.isConnected())) {
					updateStatus("���ӳɹ���");
//					Toast.makeText(background.this, "���ӳɹ�", 1);
					Log.i("FSLog", "���ӳɹ�");
					//btn.setEnabled(false);
					UpdateStatus();
					updatestatus();
					
//					//��ȡ���������
//					is = s.getInputStream();
//					os = s.getOutputStream();
					
					
				} else {
					Log.i("FSLog", "����ʧ��");
					updateStatus("����ʧ�ܣ�");
				}
				
			} 
			catch (Exception e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
				updateStatus("����ʧ�ܣ�");
				return;
			} 
		}

		
	}
	

	public void updateStatus(String str) {
		Message msg = Message.obtain();
		msg.what = MSG_UPDATE_STATUES;
		msg.obj = str;
		mHandler.sendMessage(msg);
	}
	public void UpdateStatus() {
		Message msg = Message.obtain();
		msg.what = MSG_UPDATE_ENABLE;
		mHandler.sendMessage(msg);
	}
	public void updatestatus() {
		Message msg = Message.obtain();
		msg.what = MSG_UPDATE_INTENT;
		mHandler.sendMessage(msg);
	}
	public void updateinternetstatus() {
		Message msg = Message.obtain();
		msg.what = MSG_UPDATE_STATUES_internet;
		mHandler.sendMessage(msg);
	}
	public void updatINTERNETestatus() {
		Message msg = Message.obtain();
		msg.what = MSG_UPDATE_STATUES_INTERNET;
		mHandler.sendMessage(msg);
	}
	
	private void checkNetworkInfo(){
		
		 ConnectivityManager connec = (ConnectivityManager)getSystemService(Context.CONNECTIVITY_SERVICE);
		    NetworkInfo info = connec.getActiveNetworkInfo();
		    if(info == null){
		    	updateinternetstatus();
		    	
		    }else{
		    	updatINTERNETestatus();
		    	
		    }
	}
		
	
	private void alertdialog() {
//		1. ����AlertDialog.Builder
//		2. ����title��message��button
//		3. ͨ��Builder.create()������Dialog
//		4. show()
		
		//��������
		AlertDialog.Builder builder = new AlertDialog.Builder(background.this);
		//����ԭ����
		builder.setTitle("��¼����");
		builder.setIcon(0x7f020017);
//		builder.
		builder.setCancelable(false);
		builder.setMessage("�Ƿ��¼��");
		builder.setNegativeButton("��", new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				// TODO Auto-generated method stub
				//ignore
			
				Intent intent1 = new Intent(background.this,RegisterActivity.class);
				startActivity(intent1);}
		});
		
		builder.setPositiveButton("��", new DialogInterface.OnClickListener() {
			
			@Override
			public void onClick(DialogInterface dialog, int which) {
				// TODO Auto-generated method stub
				//����activity
//				finish();
	}
		});
		AlertDialog dialog = builder.create();
//		dialog.setCancelable(false);
		
		//��ʾdialog
		dialog.show();
	}
}

