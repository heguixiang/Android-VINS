package com.hezhaoxin.solomon.vins_android;

/**
 * Created by solomon on 2017/9/14.
 */

public class NDKloader {
   static{
       System.loadLibrary("native-lib");
      // System.loadLibrary("ceres");
       //System.loadLibrary("opencv_java3");
   }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public static native String stringFromJNI();

    public static native String validate(long matAddrGr, long matAddrRgba);

    //图像处理
    public static native int[] getGrayImage(int[] pixels, int w, int h);
}