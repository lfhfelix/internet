package com.example.android_project;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.PrivateKey;

import javax.security.auth.PrivateCredentialPermission;

import android.app.Activity;
import android.content.Intent;
import android.os.AsyncTask;
import android.os.Bundle;
import android.util.Log;
import android.view.Display;
import android.view.Menu;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.EditText;

public class RegisterActivity extends Activity implements OnClickListener {
	Button btn1, btn2;
	EditText etAC, etPW;
//	private Socket socket = null;
	private InputStream in = null;
	private DataInputStream bin = null;
	private OutputStream out = null;
	private BufferedOutputStream bout = null;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_register);
		WindowManager m = this.getWindowManager();
		Display d = m.getDefaultDisplay();
		WindowManager.LayoutParams p = getWindow().getAttributes();
		p.height = (int) (d.getHeight() * 0.5);
		p.width = (int) (d.getWidth() * 0.5);
		p.alpha = 1.0f;
		p.dimAmount = 0.1f;

		getWindow().setAttributes(p);

		btn1 = (Button) findViewById(R.id.btn_register_yes);
		btn1.setOnClickListener(this);
		btn2 = (Button) findViewById(R.id.btn_register_no);
		btn2.setOnClickListener(this);
		etAC = (EditText) findViewById(R.id.ET_register_account);
		etPW = (EditText) findViewById(R.id.ET_register_password);
	}

	@Override
	public boolean onCreatePanelMenu(int featureId, Menu menu) {
		// TODO Auto-generated method stub
		return super.onCreatePanelMenu(featureId, menu);
	}

	@Override
	public void onClick(View v) {
		// TODO Auto-generated method stub
		switch (v.getId()) {
		case R.id.btn_register_yes: {
			Log.i("FSLog", "正在登陆...!");
			new DownloadTask().execute("1");
			break;

		}
		case R.id.btn_register_no: {
			// 给主界面返回1
			setResult(2);
			// 销毁子界面
			finish();
			break;
		}
		default :
			break;
		}

	}

	class DownloadTask extends AsyncTask<String, Void, String> {
		private String code;

		@Override
		protected void onPreExecute() {
			// TODO Auto-generated method stub
			super.onPreExecute();
			Log.i("FSLog", "onPreExecute");
		}

		protected String doInBackground(String... params) {
			// TODO Auto-generated method stub
			code = params[0];
			Log.i("FSLog", "doInBackground");
			String infor = sendToServer(code);
			return infor;

		}

		@Override
		protected void onPostExecute(String infor) {
			// TODO Auto-generated method stub
			String code = "ok";
			Log.i("FSLog", "onPostExecute");
			Log.i("FSLog", infor);
			Log.i("FSLog", "正在登陆1...!");
//			infor.equals字符串比较用
			if((infor != null )&&(infor.equals(code))){
				
			Log.i("FSLog", "登陆完成...!");
			Intent intent3 = new Intent(RegisterActivity.this,MonitoringActivity.class);
			startActivity(intent3);
			finish();
			}
			super.onPostExecute(infor);
			} 
			
		

	}

	public String sendToServer(String code) {

		String accept = null;
		try {
			if (background.s == null) {
				Log.i("FSLog", "连接socket");
				background.s = new Socket(
						(background.etIP).getText().toString(),
						Integer.parseInt(background.etPort.getText().toString()));

				Log.i("FSLog", "创建socket");
			}
				in = background.s.getInputStream();
				DataInputStream bin = new DataInputStream(in);

				bout = new BufferedOutputStream(background.s.getOutputStream());
				// BufferedOutputStream bout = new BufferedOutputStream(out);

				// 将数据写入流中
				byte[] data = new byte[20];
				String n = 1 + "|" + etAC.getText().toString() + "|"
						+ etPW.getText().toString();
				data = n.getBytes();
				Log.i("FSLog", n);
				bout.write(data);
				bout.flush();
				// 从服务器端接收信息的BufferedInputStream
				byte[] buff = new byte[2];
				int i = bin.read(buff);
				String src = new String(buff, 0, i);

				accept = src;
				Log.i("FSLog", accept);
//			}
		} catch (NumberFormatException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (UnknownHostException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		Log.i("FSLog", accept);
		return accept;
	}

}
