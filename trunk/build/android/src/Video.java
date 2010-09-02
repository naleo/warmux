// This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount
package org.wormux;

import javax.microedition.khronos.opengles.GL10;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGL11;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;

import android.widget.TextView;
import java.lang.Thread;
import java.util.concurrent.locks.ReentrantLock;
import android.os.Build;

  abstract class DifferentTouchInput
  {
    public static DifferentTouchInput getInstance()
    {
      if (Integer.parseInt(Build.VERSION.SDK) <= 4)
        return SingleTouchInput.Holder.sInstance;
      else
         return MultiTouchInput.Holder.sInstance;
    }

    public abstract void process(final MotionEvent event);

    private static class SingleTouchInput extends DifferentTouchInput
    {
      private static class Holder
      {
        private static final SingleTouchInput sInstance = new SingleTouchInput();
      }
      public void process(final MotionEvent event)
      {
        int action = -1;
        if( event.getAction() == MotionEvent.ACTION_DOWN )
          action = 0;
        if( event.getAction() == MotionEvent.ACTION_UP )
          action = 1;
        if( event.getAction() == MotionEvent.ACTION_MOVE )
          action = 2;
        if ( action >= 0 )
          DemoGLSurfaceView.nativeMouse( (int)event.getX(), (int)event.getY(), action, 0,
                                         (int)(event.getPressure() * 1000.0),
                                         (int)(event.getSize() * 1000.0));
      }
    }

    private static class MultiTouchInput extends DifferentTouchInput
    {
      private static class Holder
      {
        private static final MultiTouchInput sInstance = new MultiTouchInput();
      }
      public void process(final MotionEvent event)
      {
        for( int i = 0; i < event.getPointerCount(); i++ )
        {
          int action = -1;
          if( event.getAction() == MotionEvent.ACTION_DOWN )
            action = 0;
          if( event.getAction() == MotionEvent.ACTION_UP )
            action = 1;
          if( event.getAction() == MotionEvent.ACTION_MOVE )
            action = 2;
          if ( action >= 0 )
            DemoGLSurfaceView.nativeMouse( (int)event.getX(i), (int)event.getY(i), action, event.getPointerId(i),
                                           (int)(event.getPressure(i) * 1000.0),
                                           (int)(event.getSize(i) * 1000.0));
        }
      }
    }
  }


class DemoRenderer extends GLSurfaceView_SDL.Renderer {

  public DemoRenderer(Activity _context)
  {
    context = _context;
  }

  public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    // nativeInit();
  }

  public void onSurfaceChanged(GL10 gl, int w, int h) {
    //gl.glViewport(0, 0, w, h);
    nativeResize(w, h);
  }

  public void onDrawFrame(GL10 gl) {

    nativeInitJavaCallbacks();

    // Make main thread priority lower so audio thread won't get underrun
    // Thread.currentThread().setPriority((Thread.currentThread().getPriority() + Thread.MIN_PRIORITY)/2);

    System.loadLibrary("application");
    System.loadLibrary("sdl_main");
    URLDownloader tmp = new URLDownloader();
    Settings.Apply(context);
    // Tweak video thread priority, if user selected big audio buffer
    if (Globals.AudioBufferConfig >= 2)
      Thread.currentThread().setPriority( (Thread.NORM_PRIORITY + Thread.MIN_PRIORITY) / 2 ); // Lower than normal

    nativeInit(); // Calls main() and never returns, hehe - we'll call eglSwapBuffers() from native code
    System.exit(0); // The main() returns here - I don't bother with deinit stuff, just terminate process
  }

  public int swapBuffers() // Called from native code, returns 1 on success, 0 when GL context lost (user put app to background)
  {
    synchronized (this) {
      this.notify();
    }
    //Thread.yield();
    return super.SwapBuffers() ? 1 : 0;
  }

  public void exitApp() {
     nativeDone();
  };

  private native void nativeInitJavaCallbacks();
  private native void nativeInit();
  private native void nativeResize(int w, int h);
  private native void nativeDone();

  private Activity context = null;

  private EGL10 mEgl = null;
  private EGLDisplay mEglDisplay = null;
  private EGLSurface mEglSurface = null;
  private EGLContext mEglContext = null;
}

class DemoGLSurfaceView extends GLSurfaceView_SDL {
  public DemoGLSurfaceView(Activity context) {
    super(context);
    mParent = context;
    touchInput = DifferentTouchInput.getInstance();
    setEGLConfigChooser(Globals.NeedDepthBuffer);
    accelerometer = new AccelerometerReader(context);
    mRenderer = new DemoRenderer(context);
    setRenderer(mRenderer);
  }


  @Override
  public boolean onTouchEvent(final MotionEvent event)
  {
    touchInput.process(event);
    // Wait a bit, and try to synchronize to app framerate, or event thread will eat all CPU and we'll lose FPS
    synchronized (mRenderer) {
      try {
        mRenderer.wait(300L);
      } catch (InterruptedException e) { }
    }
    return true;
  };

  public void exitApp() {
    mRenderer.exitApp();
    accelerometer.stop();
    accelerometer = null;
  };

+ @Override
  public void onPause() {
    super.onPause();
    System.exit(0); // Not implemented yet
  };

  @Override
  public void onResume() {
    super.onResume();
  };

  @Override
  public boolean onKeyDown(int keyCode, final KeyEvent event) {
    nativeKey( keyCode, 1 );
    return true;
  }

  @Override
  public boolean onKeyUp(int keyCode, final KeyEvent event) {
    nativeKey( keyCode, 0 );
    return true;
  }

  DemoRenderer mRenderer;
  Activity mParent;
  AccelerometerReader accelerometer = null;
  DifferentTouchInput touchInput = null;

  public static native void nativeMouse( int x, int y, int action, int pointerId, int pressure, int radius );
  public static native void nativeKey( int keyCode, int down );
}


