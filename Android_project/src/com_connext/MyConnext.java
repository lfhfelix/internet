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
		// 3，我们需要的是字符串中的数字，因为我们获取这个数字的目的是为了获取图片，所以我们现在需要把字符串后面的len去掉
		exp = exp.substring(0,exp.lastIndexOf("|"));
//		exp = exp.substring(0,exp.lastIndexOf("|"));
		 Log.i("FSLog", "exp2 = "+exp);	        
		 return exp;	

		
	}

}
