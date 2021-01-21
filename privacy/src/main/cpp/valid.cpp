#include <stdio.h>
#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include "config.h"

#define LOG_TAG  "SafePrivacyKey"
#define LOG_E(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define LOG_D(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

const char hex_code[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

char* get_sha1(JNIEnv *env, jobject context_object){
    //上下文对象
    jclass context_class = env->GetObjectClass(context_object);

    //反射获取PackageManager
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context_object, methodId);
    if (package_manager == NULL) {
        LOG_E("package_manager is NULL!!!");
        return NULL;
    }

    //反射获取包名
    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring)env->CallObjectMethod(context_object, methodId);
    if (package_name == NULL) {
        LOG_E("package_name is NULL!!!");
        return NULL;
    }
    env->DeleteLocalRef(context_class);

    //获取PackageInfo对象
    jclass pack_manager_class = env->GetObjectClass(package_manager);
    methodId = env->GetMethodID(pack_manager_class, "getPackageInfo", "(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    env->DeleteLocalRef(pack_manager_class);
    jobject package_info = env->CallObjectMethod(package_manager, methodId, package_name, 0x40);
    if (package_info == NULL) {
        LOG_E("getPackageInfo() is NULL!!!");
        return NULL;
    }
    env->DeleteLocalRef(package_manager);

    //获取签名信息
    jclass package_info_class = env->GetObjectClass(package_info);
    jfieldID fieldId = env->GetFieldID(package_info_class, "signatures", "[Landroid/content/pm/Signature;");
    env->DeleteLocalRef(package_info_class);
    jobjectArray signature_object_array = (jobjectArray)env->GetObjectField(package_info, fieldId);
    if (signature_object_array == NULL) {
        LOG_E("signature is NULL!!!");
        return NULL;
    }
    jobject signature_object = env->GetObjectArrayElement(signature_object_array, 0);
    env->DeleteLocalRef(package_info);

    //签名信息转换成sha1值
    jclass signature_class = env->GetObjectClass(signature_object);
    methodId = env->GetMethodID(signature_class, "toByteArray", "()[B");
    env->DeleteLocalRef(signature_class);
    jbyteArray signature_byte = (jbyteArray) env->CallObjectMethod(signature_object, methodId);
    jclass byte_array_input_class=env->FindClass("java/io/ByteArrayInputStream");
    methodId=env->GetMethodID(byte_array_input_class,"<init>","([B)V");
    jobject byte_array_input=env->NewObject(byte_array_input_class,methodId,signature_byte);
    jclass certificate_factory_class=env->FindClass("java/security/cert/CertificateFactory");
    methodId=env->GetStaticMethodID(certificate_factory_class,"getInstance","(Ljava/lang/String;)Ljava/security/cert/CertificateFactory;");
    jstring x_509_jstring=env->NewStringUTF("X.509");
    jobject cert_factory=env->CallStaticObjectMethod(certificate_factory_class,methodId,x_509_jstring);
    methodId=env->GetMethodID(certificate_factory_class,"generateCertificate",("(Ljava/io/InputStream;)Ljava/security/cert/Certificate;"));
    jobject x509_cert=env->CallObjectMethod(cert_factory,methodId,byte_array_input);
    env->DeleteLocalRef(certificate_factory_class);
    jclass x509_cert_class=env->GetObjectClass(x509_cert);
    methodId=env->GetMethodID(x509_cert_class,"getEncoded","()[B");
    jbyteArray cert_byte=(jbyteArray)env->CallObjectMethod(x509_cert,methodId);
    env->DeleteLocalRef(x509_cert_class);
    jclass message_digest_class=env->FindClass("java/security/MessageDigest");
    methodId=env->GetStaticMethodID(message_digest_class,"getInstance","(Ljava/lang/String;)Ljava/security/MessageDigest;");
    jstring sha1_jstring=env->NewStringUTF("SHA1");
    jobject sha1_digest=env->CallStaticObjectMethod(message_digest_class,methodId,sha1_jstring);
    methodId=env->GetMethodID(message_digest_class,"digest","([B)[B");
    jbyteArray sha1_byte=(jbyteArray)env->CallObjectMethod(sha1_digest,methodId,cert_byte);
    env->DeleteLocalRef(message_digest_class);

    //转换成char
    jsize array_size=env->GetArrayLength(sha1_byte);
    jbyte* sha1 =env->GetByteArrayElements(sha1_byte,NULL);
    char *hex_sha=new char[array_size*2+1];
    for (int i = 0; i <array_size ; ++i) {
        hex_sha[2*i]=hex_code[((unsigned char)sha1[i])/16];
        hex_sha[2*i+1]=hex_code[((unsigned char)sha1[i])%16];
    }
    hex_sha[array_size*2]='\0';

    //LOG_D("hex_sha %s ",hex_sha);
    return hex_sha;
}


char* get_package_name(JNIEnv *env, jobject context_object){
    //上下文对象
    jclass context_class = env->GetObjectClass(context_object);

    //反射获取PackageManager
    jmethodID methodId = env->GetMethodID(context_class, "getPackageManager", "()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context_object, methodId);
    if (package_manager == NULL) {
        LOG_E("package_manager is NULL!!!");
        return NULL;
    }

    //反射获取包名
    methodId = env->GetMethodID(context_class, "getPackageName", "()Ljava/lang/String;");
    jstring package_name = (jstring)env->CallObjectMethod(context_object, methodId);
    if (package_name == NULL) {
        LOG_E("package_name is NULL!!!");
        return NULL;
    }

    const char *char_package_name = env->GetStringUTFChars(package_name, JNI_FALSE);

    env->ReleaseStringUTFChars(package_name, char_package_name);
    return const_cast<char *>(char_package_name);

}

/**
 * 验证sha1值
 * @param env
 * @param sha1
 * @return
 */
jboolean verify_sign_sha1(JNIEnv *env,char *sha1){
    //比较签名
    for (auto &i : config::array_signature) {
        if (strcmp(sha1, i) == 0) {
            LOG_D("签名:验证通过");
            return true;
        }
    }

    LOG_E("签名:验证拒绝");
    return false;
}

/**
 * 验证包名 package_name
 * @param env
 * @param package_name
 * @return
 */
jboolean verify_package(JNIEnv *env,char *package_name){
    //比较签名
    for (auto &i : config::array_package) {
        if (strcmp(package_name, i) == 0) {
            LOG_D("包名:验证通过");
            return true;
        }
    }

    LOG_E("包名:验证拒绝");
    return false;
}

/**
 * 根据类型type，获取相关的PrivacyKey数据
 * @param type
 * @param isDebug
 * @return
 */
char *obtain_privacy_key_by_type(const char *type, jboolean isDebug) {
    for (int i = 0; i < sizeof(config::array_type) / sizeof(config::array_type[0]); ++i) {
        if (strcmp(config::array_type[i], type) == 0) {
            return isDebug ? config::array_key[i][0] : config::array_key[i][1];
        }
    }
    return nullptr;
}