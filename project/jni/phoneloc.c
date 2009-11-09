/*
 * Phone Location of China
 *
 * Base on code from Geesun
 * Modified by cytown
 * Last edited 2009.11.06
 */
#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <assert.h>

//#define DEBUG

#ifdef DEBUG
#include <android/log.h>
const static char* TAG = "phonelocjni";
#endif

const static char* LOC_FILE = "/data/phoneloc.dat";
const static char* KNOWN_PREFIX[] = {"0086", "106", "12520", "17951", "17909", "12593"};
const static int KNOWN_PREFIX_LEN = 6;
const static jstring SPACESTR = " ";
const static jstring EMPTYSTR = "";

static int exists = 0;

int file_exists(const char * filename) {
    if (exists != 0) return exists > 0 ? 0 : -1;
#ifdef DEBUG
     __android_log_print(ANDROID_LOG_DEBUG, TAG, "check data file");
#endif
    FILE * file;
    if (file = fopen(filename, "r")) {
        fclose(file);
        exists = 1;
        return 0;
    }
#ifdef DEBUG
     __android_log_print(ANDROID_LOG_ERROR, TAG, "DATA FILE (%s) NOT EXIST!", LOC_FILE);
#endif
    exists = -1;
    return exists;
}

int isInterPhone(char * phone, int len) {
    if (strncmp(phone, "00", 2) == 0) {
        return 0;
    }
    return -1;
}

char* formatPhone(char* phone, int len) {
    char nphone[48];
    memset(nphone, 0x00, 48);
    // shouldn't length over 40!
    if (len > 40) len = 40;
    strncpy(nphone, phone, len);
    char* pch = strchr(nphone, '-');
    while (pch != NULL) {
        int pos = pch - nphone;
        memmove(nphone + pos, nphone + pos + 1, len - pos);
        pch = strchr(nphone, '-');
    }

    if (nphone[0] == '+') {
        if (strncmp(nphone, "+00", 3) != 0) {
            memmove(nphone + 2, nphone + 1, len);
            memmove(nphone, "00", 2);
        } else {
            memmove(nphone, nphone + 1, len);
        }
    }
    int i;
    for (i = 0; i < KNOWN_PREFIX_LEN; i++) {
        int l = strlen(KNOWN_PREFIX[i]);
        if (strncmp(nphone, KNOWN_PREFIX[i], l) == 0) {
            memmove(nphone, nphone+l, len);
            break;
        }
    }
    if (pch=strchr(nphone, '#')) {
        pch[0] = 0x00;
    }
    if (pch=strchr(nphone, '*')) {
        pch[0] = 0x00;
    }
#ifdef DEBUG
     __android_log_print(ANDROID_LOG_DEBUG, TAG, "after format: %s", nphone);
#endif
    char* ret = nphone;
    return ret;
}

JNIEXPORT jstring JNICALL
getPhoneLocationJni( JNIEnv* env, jclass thiz, jstring phone ) {
    char* phone2;
    jboolean is_copy;
    phone2 = (*env)->GetStringUTFChars (env, phone, &is_copy);
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "called [%s]", phone2);
#endif
    if (phone2 == NULL) return SPACESTR;
    int len = strlen(phone2);
    if (len <= 3) return SPACESTR;
    char* nphone = formatPhone(phone2, len);
    len = strlen(nphone);
    if (len <= 3) return SPACESTR;

    char location[48];
    char locationCode[48];
    memset(locationCode,0x00,48);
    memset(location,0x00,48);

    if (isInterPhone(nphone, len) >= 0) {
        int pos = len > 6 ? 6 : len;
        char m[8];
        memset(m, 0x00, 8);
        int i;
        for (i = 0; i < 7-pos; i++) {
            m[i] = '9';
        }
        strncpy(m+7-pos, nphone, pos);
        for (; pos >= 3; pos--) {
            int num = atol(&m[0]);
            if (getLocationInfoEx(num, location, locationCode) >= 0) {
                return (*env)->NewStringUTF(env, locationCode);
            }
            memmove(m + 1, m, pos);
            m[7] = 0x00;
        }
        return SPACESTR;
    }
    if (nphone[0] == '0') {
        if (nphone[1] == '1' || nphone[1] == '2') {
            nphone[3] = 0x00;
        } else if (len > 4) {
            nphone[4] = 0x00;
        } else {
            return SPACESTR;
        }
    } else {
        if (len >= 7) {
            nphone[7] = 0x00;
        }
    }
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "find %s", nphone);
#endif
    int num = atol(nphone);
    if (getLocationInfoEx(num, location, locationCode) >= 0) {
        return (*env)->NewStringUTF(env, locationCode);
    }
    return EMPTYSTR;
}

int getLocationInfoEx(int num, char * location, char * locationCode) {
    if (file_exists(LOC_FILE) < 0) {
#ifdef DEBUG
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "data file not exist!");
#endif
        return -1;
    }

    getLocationInfo(LOC_FILE, num, location, locationCode);
    if (location[0] == ' ' && location[1] == 0x00) return -1;
    strcat(locationCode, ",");
    strcat(locationCode, location);
#ifdef DEBUG
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "[%d] == %s", num, locationCode);
#endif
    return 0;
}

/*
#ifdef DEBUG
    jstring sss = getPhoneLocationJni(env, thiz, "+88613501330000");
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", sss[0]);
    sss = getPhoneLocationJni(env, thiz, "0085213501330-000");
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", sss);
    sss = getPhoneLocationJni(env, thiz, "008613501330-000");
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", sss);
    sss = getPhoneLocationJni(env, thiz, "+8613501330000");
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", sss);
    sss = getPhoneLocationJni(env, thiz, "125-20-15-01037-0000#888");
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "%s", sss);
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "===============");
#endif
*/

/**
 * Table of methods associated with a single class.
 */
static JNINativeMethod gMethods[] = {
    { "getPhoneLocationJni", "(Ljava/lang/String;)Ljava/lang/String;",
            (void*) getPhoneLocationJni },
    /* <<----Functions for sync end--------------------------------- */
};


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
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "class not exist!");
#endif
        return JNI_FALSE;
    }
    if ((*env)->RegisterNatives(env, clazz, gMethods, numMethods) < 0) {
#ifdef DEBUG
        __android_log_print(ANDROID_LOG_DEBUG, TAG, "method not exist!");
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
           "com/android/phone/location/PhoneLocation",
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
    __android_log_print(ANDROID_LOG_DEBUG, TAG, "onload");
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


