package com.hezhaoxin.solomon.vins_android;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;
import android.widget.TextView;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.OpenCVLoader;
import org.opencv.core.Mat;

import java.io.File;

public class MainActivity extends AppCompatActivity {
 //   private static String TAG = "OpenCVLoader";
    private ImageView show_image;
    CameraBridgeViewBase cameraBridgeViewBase;//opencv打开相机



    protected static final String TAG = "AndroCLActivity";
    static boolean bCanProcess = true;
    private final Context mContext = MainActivity.this;
    enum AclStatus {
        ACL_STATUS_SUCCESS,
        ACL_STATUS_OCL_SO_LOAD_FAILED,
        ACL_STATUS_ACL_SO_LOAD_FAILED,
        ACL_STATUS_ACL_CL_LOAD_FAILED,
    }
    private AclStatus mStatus;
    // Used to load the 'native-lib' library on application startup.
    /*
    static {
        System.loadLibrary("Androidocl");
    }*/
    private boolean loadNativeLib(final String lib) {
        boolean ret = true;
        try {
            System.load(lib);
            Log.d( TAG, "Loaded file =" + lib);
        } catch (UnsatisfiedLinkError err) {
            ret = false;
            Log.e( TAG, "Failed to load file =" + lib);
        }
        return ret;
    }
    private boolean loadNativeOpenCL() {
        return (loadNativeLib("/system/vendor/lib/libOpenCL.so") ||
                loadNativeLib("/system/vendor/lib/libGLES_mali.so") ||
                loadNativeLib("/system/vendor/lib/libPVROCL.so"));
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //打开相机
        initView();
  //      loadNativeOpenCL();
        grayPic();
    }

    private void initView() {
        TextView textView = (TextView)findViewById(R.id.sample_text);

          textView.setText(NDKloader.stringFromJNI());
        show_image = (ImageView) findViewById(R.id.show_image);
        show_image.setImageResource(R.drawable.cat);
        findViewById(R.id.pic).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //恢复
                backPic();
            }
        });
        findViewById(R.id.gray_pic).setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                //变灰
                grayPic();
            }
        });
    }
    private void backPic(){
        show_image.setImageResource(R.drawable.cat);
    }
    private void grayPic(){
        Bitmap bmp = BitmapFactory.decodeResource(getResources(),R.drawable.cat);
        int w = bmp.getWidth();
        int h = bmp.getHeight();
        int[] pixels = new int[w*h];
        bmp.getPixels(pixels, 0, w, 0, 0, w, h);
        //recall JNI
        int[] resultInt = NDKloader.getGrayImage(pixels, w, h);
        Bitmap resultImg = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
        resultImg.setPixels(resultInt, 0, w, 0, 0, w, h);
        show_image.setImageBitmap(resultImg);
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (OpenCVLoader.initDebug()) {
            Log.i(TAG, "OpenCV initialize success");
        } else {
            Log.i(TAG, "OpenCV initialize failed");
        }
    }
}
