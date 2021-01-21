#include <jni.h>
#include <string>
#include <android/log.h>

#include "valid.cpp"

extern "C"
JNIEXPORT jstring JNICALL
Java_com_shuai_privacy_PrivacyKeyGenerator_getPrivacyKey(
        JNIEnv *env,
        jclass,
        jobject context,
        jstring type, jboolean isDebug) {

    char *sha1 = get_sha1(env, context);
    char *package_name = get_package_name(env, context);

    if (/*isDebug || */verify_sign_sha1(env, sha1)) {
        if (verify_package(env, package_name)) {
            const char *char_type = env->GetStringUTFChars(type, JNI_FALSE);
            char *char_result = obtain_privacy_key_by_type(char_type, true);
            return env->NewStringUTF(char_result);
        }
    }
    return nullptr;
}



