package com.fs.util;

import java.io.DataInputStream;
import java.io.IOException;

public class MyUtil {
	
//	public static int size;
	
	public static byte[] read(DataInputStream bin, int size,int max ){
		
		byte[] image = new byte[size];
		int hasRead = 0;
		while(true) {
			if(max > size - hasRead) {
			max = size - hasRead;	
			
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

		 return image;	
	}
	


}
