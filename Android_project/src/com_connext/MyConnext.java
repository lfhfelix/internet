package com_connext;

import java.io.DataInputStream;
import java.io.IOException;

import android.util.Log;

public class MyConnext {
	public static int Size;
	public static int Num;
	
	public static String read(DataInputStream bin, int size,int max) {
		byte[] image = new byte[size];
		int hasRead = 0;		
		while(true) {
			if(max > size - hasRead) {
			max = size - hasRead;
			 Log.i("FSLog", "max = " + max);
			
			}
			try {
				hasRead = hasRead + bin.read(image,hasRead,max);
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			if(hasRead == size) {
				break;
			}
						
		}
		String exp = new String(image);
		 Log.i("FSLog", "exp1 = "+exp);
		// 3��������Ҫ�����ַ����е����֣���Ϊ���ǻ�ȡ������ֵ�Ŀ����Ϊ�˻�ȡͼƬ����������������Ҫ���ַ��������lenȥ��
		exp = exp.substring(0,exp.lastIndexOf("|"));
//		exp = exp.substring(0,exp.lastIndexOf("|"));
		 Log.i("FSLog", "exp2 = "+exp);	        
		 return exp;	

		
	}

}
