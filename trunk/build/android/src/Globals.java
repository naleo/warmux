// This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount anywhere
package org.wormux;

import android.app.Activity;
import android.content.Context;

class Globals {
 public static String ApplicationName = "Wormux";

 // Should be zip file
 public static String DataDownloadUrl = "http://download.gna.org/wormux/android-data-0.9.3.zip"; // This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount

 // Set DownloadToSdcard to true if your app data is bigger than 5 megabytes.
 // It will download app data to /sdcard/alienblaster then,
 // otherwise it will download it to /data/data/de.schwardtnet.alienblaster/files -
 // set this dir in jni/Android.mk in SDL_CURDIR_PATH
 public static boolean DownloadToSdcard = true;

 // Set this value to true if you're planning to render 3D using OpenGL - it eats some GFX resources, so disabled for 2D
 public static boolean NeedDepthBuffer = false;

 // Set this value to true if you're planning to render 3D using OpenGL - it eats some GFX resources, so disabled for 2D
 public static boolean HorizontalOrientation = true;

 // Readme text to be shown on download page
 public static String ReadmeText = "^Please wait while data is being downloaded.".replace("^","\n");

 public static boolean AppUsesMouse = true;

 // We have to use accelerometer as arrow keys
 public static boolean AppNeedsArrowKeys = true;

 public static boolean AppUsesJoystick = false;

 public static boolean AppUsesMultitouch = false;

 public static boolean PhoneHasTrackball = false;
}

class LoadLibrary {
 public LoadLibrary() { System.loadLibrary("sdl"); System.loadLibrary("sdl_mixer"); System.loadLibrary("sdl_image"); System.loadLibrary("sdl_net"); System.loadLibrary("sdl_gfx"); };
}
