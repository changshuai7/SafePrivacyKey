package com.shuai.privacy;

import android.content.Context;


public class PrivacyKeyGenerator {

    static {
        System.loadLibrary("privacy-key");
    }

    /**
     * 获取秘钥Key
     *
     * @param context 上下文
     * @param type    秘钥类型
     * @param isDebug 是否是Debug模式
     * @return
     */
    public static native String getPrivacyKey(Context context, String type, boolean isDebug);


}
