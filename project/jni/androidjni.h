/*
 * Global header for android jni call
 *
 * Created by cytown
 * Last edited 2009.11.13
 */

/**
 * This define the reg class for jni call
 */
#define JNIREG_CLASS "com/android/phone/location/PhoneLocation"

JNIEXPORT jstring JNICALL
getPhoneLocationJni( JNIEnv* env, jclass thiz, jstring phone );

/**
 * Table of methods associated with a single class.
 */
static JNINativeMethod gMethods[] = {
    { "getPhoneLocationJni", "(Ljava/lang/String;)Ljava/lang/String;",
            (void*) getPhoneLocationJni },
    /* <<----Functions for sync end--------------------------------- */
};

