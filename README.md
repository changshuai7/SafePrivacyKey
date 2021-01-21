# SafePrivacyKey:采用JNI实现的秘钥存储库

## 写在前边

对于Android中的一些敏感秘钥，存放于Java层代码中无疑具有重大安全隐患。为解决此问题，设计出了本框架SafePrivacyKey

框架的主要优势如下：

1. 基于JNI和CMaker,实现了将敏感秘钥存入so中。数据更安全.
2. 第一层安全校验：只有在允许范围内的应用（校验包名）才可通过包名校验。
3. 第二层安全校验：只有在允许范围的应用签名（校验签名sha1）才可通过签名校验。

多重校验，防止so库被二次打包，极大增大了安全性和破解难度。

## 发布Library

### 一、编译C++代码

#### 1、将lib中的externalNativeBuild代码注释放开

```
defaultConfig {
    minSdkVersion rootProject.ext.minSdkVersion
    targetSdkVersion rootProject.ext.targetSdkVersion
    //放开如下externalNativeBuild的注释
    externalNativeBuild {
        cmake {
            cppFlags ""
        }
    }
}
//放开如下externalNativeBuild的注释
externalNativeBuild {
    cmake {
        path "src/main/cpp/CMakeLists.txt"
        version "3.10.2"
    }
}

```

#### 2、删除jniLibs中的所有so文件

如不想删除so，也可以临时定义 jniLibs.srcDirs = ['src/main/jniLibs']为其他目录

**通过上述操作，即可进入C++编译模式，可直接运行项目**

### 二、在C++中写入秘钥

#### 1、打开config.h文件，写入相关的数据
```
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
                {(char *) "debug下的真实秘钥",(char *) "release下的真实秘钥",}
        };
```
**array_signature**：该数组中存储着允许访问的应用的签名sha1值。

**array_package**：该数组中存储着允许访问的应用包名。

只有同时满足上述两个限制的应用，才是被允许访问的应用。

**array_type**：定义秘钥类型

**array_key**：定义秘钥值（[0]=debug秘钥，[1]=release秘钥）

获取应用签名sha1的三种方法：

1. 通过命令：keytool -list -v -keystore your.keystore"
2. 通过执行：AndroidStudio - Gradle -Tasks - android - signingReport  获得
3. 应用内可执行如下代码获取：
```
/**
 * 获取签名sha1值
 *
 * @param context
 * @return
 */
public static String getSha1Value(Context context) {
    try {
        PackageInfo info = context.getPackageManager().getPackageInfo(
                context.getPackageName(), PackageManager.GET_SIGNATURES);
        byte[] cert = info.signatures[0].toByteArray();
        MessageDigest md = MessageDigest.getInstance("SHA1");
        byte[] publicKey = md.digest(cert);
        StringBuffer hexString = new StringBuffer();
        for (int i = 0; i < publicKey.length; i++) {
            String appendString = Integer.toHexString(0xFF & publicKey[i])
                    .toUpperCase(Locale.US);
            if (appendString.length() == 1)
                hexString.append("0");
            hexString.append(appendString);
        }
        String result = hexString.toString();
        return result.substring(0, result.length());
    } catch (Exception e) {
        e.printStackTrace();
    }
    return null;
}

```

#### 2、打包SO

build即可。打包完成后，在/build/intermediates/cmake/release/obj/下，找到so库，将其全部复制到src/main/jniLibs目录下

#### 3、发布Library

1. 注释掉externalNativeBuild，且删除掉临时定义的jniLibs目录。
2. 发布Library到jcenter或者公司自有仓库

### 三、使用Library

比如本例，已发布到jcenter

#### 1、引入
```
dependencies {
     implementation 'com.shuai:safe-privacy-key:0.0.1'
}
```
#### 2、使用

```
//获取类型为”type1“的秘钥
PrivacyKeyGenerator.getPrivacyKey(MainActivity.this, ”type1“, BuildConfig.DEBUG);
```

由此即可使用。

