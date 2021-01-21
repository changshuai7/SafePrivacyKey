package com.shuai.privacy.example.app;

import android.app.Application;
import android.os.Handler;


public class MyApplication extends Application {

    private static MyApplication sInstance;//单例
    private static int mainTid;//主线程ID
    private static Handler baseHandler;//全局Handler


    //获取单例对象
    public static MyApplication getInstance() {
        return sInstance;
    }

    //获取主线程id
    public static int getMainTid() {
        return mainTid;
    }

    //获取Handler
    public static Handler getHandler() {
        return baseHandler;
    }

    @Override
    public void onCreate() {
        super.onCreate();
        sInstance = this;
        mainTid = android.os.Process.myTid();
        baseHandler = new Handler();


    }


}
