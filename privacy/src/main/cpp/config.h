namespace config {

    /**
     * 应用签名的SHA1值（去除连接符"-"），可通过以下方法获得
     * "1. 通过命令：keytool -list -v -keystore your.keystore"
     *  2. 通过执行：AndroidStudio - Gradle -Tasks - android - signingReport  获得
     */
    //设置允许访问的签名sha1值
    char *array_signature[] =
            {
                    (char *) "C8C1DAFF93A5029C25DE7E2593C7BE6CF560AB07",
                    (char *) "你的签名SHA1值",

            };

    //设置允许访问的应用包名
    char *array_package[] =
            {
                    (char *) "com.shuai.privacy.example.app",
                    (char *) "你的应用包名",

            };

    //存储的秘钥类型，注意，必须和下方的array_key保持一一对应。
    char *array_type[] =
            {
                    (char *) "type1",
                    (char *) "你的秘钥类型",

            };

    //存储的的真实秘钥：[0]=debug秘钥，[1]=release秘钥
    char *array_key[][2] =
            {
                    {(char *) "debug_real_key_by_type1", (char *) "release_real_key_by_type1",},
                    {(char *) "debug下的真实秘钥",             (char *) "release下的真实秘钥",}
            };

}

