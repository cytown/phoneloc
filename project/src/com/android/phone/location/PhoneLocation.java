package com.android.phone.location;

/*
 * Usage:
 * String s = PhoneLocation.getLocationFromPhone("13501340000");
 * String[] loc = null;
 * if (s != null && !s.equals(" ")) {
 *     loc = s.split(",");
 * }
 * if (loc != null) {
 *     System.out.println("location code: " + loc[0]);
 *     System.out.println("location name: " + loc[1]);
 * }
 */

public final class PhoneLocation {
    static {
        System.loadLibrary("phoneloc-jni");
    }

    static native String getPhoneLocationJni(String phone);

    public static String getLocationFromPhone(String num) {
        return getPhoneLocationJni(num);
    }

}
