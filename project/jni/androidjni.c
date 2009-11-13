/*
 * Global file for android jni call
 *
 * Created by cytown
 * Last edited 2009.11.13
 */
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <assert.h>
#include "androidjni.h"

//#define DEBUG

#ifdef DEBUG
    #include <android/log.h>
    #define TAG_JNI = "jnireg"
#endif


/*
 * Register several native methods for one class.
 */
static int registerNativeMethods(JNIEnv* env, const char* className,
    JNINativeMethod* gMethods, int numMethods)
{
    jclass clazz;

    clazz = (*env)->FindClass(env, className);
    if (clazz == NULL) {
#ifdef DEBUG
        __android_log_print(ANDROID_LOG_DEBUG, TAG_JNI, "class not exist!");
#endif
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
#ifdef DEBUG
        __android_log_print(ANDROID_LOG_DEBUG, TAG_JNI, "method not exist!");
#endif
        return JNI_FALSE;
    }

    return JNI_TRUE;
}


/*
 * Register native methods for all classes we know about.
 */
static int registerNatives(JNIEnv* env)
{
    if (!registerNativeMethods(env,
            JNIREG_CLASS,
            gMethods, sizeof(gMethods) / sizeof(gMethods[0])))
        return JNI_FALSE;

    return JNI_TRUE;
}

/*
 * Set some test stuff up.
 *
 * Returns the JNI version on success, -1 on failure.
 */
JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved)
{
    JNIEnv* env = NULL;
    jint result = -1;

#ifdef DEBUG
    __android_log_print(ANDROID_LOG_DEBUG, TAG_JNI, "onload");
#endif

    if ((*vm)->GetEnv(vm, (void**) &env, JNI_VERSION_1_4) != JNI_OK) {
        return -1;
    }
    assert(env != NULL);
    
    if (!registerNatives(env)) {
        return -1;
    }
    /* success -- return valid version number */
    result = JNI_VERSION_1_4;
 
    return result;
}


