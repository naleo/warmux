// This string is autogenerated by ChangeAppSettings.sh, do not change spaces amount
package org.wormux;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.view.MotionEvent;
import android.view.KeyEvent;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;
import android.content.res.Configuration;


public class MainActivity extends Activity {
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    // fullscreen mode
    requestWindowFeature(Window.FEATURE_NO_TITLE);
    getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN,
           WindowManager.LayoutParams.FLAG_FULLSCREEN);

    _tv = new TextView(this);
    _tv.setText(R.string.init);
    setContentView(_tv);
    if(mAudioThread == null) // Starting from background (should not happen)
    {
      mLoadLibraryStub = new LoadLibrary();
      mAudioThread = new AudioThread(this);
      Settings.Load(this);
    }
  }

  public void startDownloader()
  {
    if( downloader == null )
      downloader = new DataDownloader(this, _tv);
  }

  public void initSDL()
  {
    if(sdlInited)
      return;
    sdlInited = true;
    mGLView = new DemoGLSurfaceView(this);
    setContentView(mGLView);
    // Receive keyboard events
    mGLView.setFocusableInTouchMode(true);
    mGLView.setFocusable(true);
    mGLView.requestFocus();
  }

  @Override
  protected void onPause() {
    if( downloader != null ) {
      synchronized( downloader ) {
        downloader.setParent(null, null);
      }
    }
    if( mGLView != null )
      mGLView.onPause();
    super.onPause();
  }

  @Override
  protected void onResume() {
    super.onResume();
    if( mGLView != null )
      mGLView.onResume();
    else
    if( downloader != null ) {
      synchronized( downloader ) {
        downloader.setParent(this, _tv);
        if( downloader.DownloadComplete )
          initSDL();
      }
    }
  }

  @Override
  protected void onDestroy()
  {
    if( downloader != null ) {
      synchronized( downloader ) {
        downloader.setParent(null, null);
      }
    }
    if( mGLView != null )
      mGLView.exitApp();
    super.onDestroy();
    System.exit(0);
  }

  @Override
  public boolean onKeyDown(int keyCode, final KeyEvent event) {
    // Overrides Back key to use in our app
    if( mGLView != null )
       mGLView.nativeKey( keyCode, 1 );
    else
    if( keyCode == KeyEvent.KEYCODE_BACK && downloader != null )
    {
      if( downloader.DownloadFailed )
        System.exit(1);
      if( !downloader.DownloadComplete )
        System.exit(1);
    }
     return true;
  }

  @Override
  public boolean onKeyUp(int keyCode, final KeyEvent event) {
     if( mGLView != null )
       mGLView.nativeKey( keyCode, 0 );
     return true;
  }

  @Override
  public boolean dispatchTouchEvent(final MotionEvent ev) {
    if(mGLView != null)
      mGLView.onTouchEvent(ev);
    return true;
  }

  @Override
  public void onConfigurationChanged(Configuration newConfig) {
    super.onConfigurationChanged(newConfig);
    // Do nothing here
  }

  public void setText(final String t)
  {
    class Callback implements Runnable
    {
      public TextView Status;
      public String text;
      public void run()
      {
        if(Status != null)
          Status.setText(text);
      }
    }
    Callback cb = new Callback();
    cb.text = new String(t);
    cb.Status = _tv;
    this.runOnUiThread(cb);
  }

  private static DemoGLSurfaceView mGLView = null;
  private static LoadLibrary mLoadLibraryStub = null;
  private static AudioThread mAudioThread = null;
  private static DataDownloader downloader = null;
  private TextView _tv = null;
  private boolean sdlInited = false;
}
