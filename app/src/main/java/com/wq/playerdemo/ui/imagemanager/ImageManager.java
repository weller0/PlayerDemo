package com.wq.playerdemo.ui.imagemanager;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.drawable.BitmapDrawable;
import android.graphics.drawable.ColorDrawable;
import android.graphics.drawable.Drawable;
import android.graphics.drawable.TransitionDrawable;
import android.media.ThumbnailUtils;
import android.os.Handler;
import android.os.Message;
import android.provider.MediaStore;
import android.util.Log;
import android.util.LruCache;
import android.widget.ImageView;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.HttpURLConnection;
import java.net.URL;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Created by qiangwang on 5/6/15.
 */
public class ImageManager {
    private static final String TAG = "ImageManager";
    private static final int SIZE_THREAD_POOL = 5;
    // 内存缓存
    private LruCache<String, Bitmap> mMemCache;
    // 文件缓存
    private FileCache fileCache;
    // 图片下载的线程池
    private ExecutorService mImgThreadPool;
    private static ImageManager imgManager;
    private static Activity mActivity;

    public static ImageManager getInstance(){
        if(imgManager == null){
            imgManager = new ImageManager(mActivity);
        }

        return imgManager;
    }

    public static void init(Activity activity){
        mActivity = activity;
    }

    public ImageManager(Activity activity){
        // 获取系统分配给应用最大的内存
        long maxMemSize = Runtime.getRuntime().maxMemory();
        // 让图片缓存占1/4
        final int maxCacheSize = (int)maxMemSize / 8;
        mMemCache = new LruCache<String, Bitmap>(maxCacheSize){
            /**
             * 必须重写该方法，获取图片的大小
             * @param key
             * @param value
             * @return
             */
            @Override
            protected int sizeOf(String key, Bitmap value) {
                return value.getByteCount();
            }
        };
        fileCache = new FileCache(activity);
    }

    /**
     * 获取线程池，涉及到并发问题，加入了同步锁
     * @return
     */
    private ExecutorService getThreadPool(){
        if(mImgThreadPool == null){
            synchronized (ExecutorService.class){
                if(mImgThreadPool == null){
                    mImgThreadPool = Executors.newFixedThreadPool(SIZE_THREAD_POOL);
                }
            }
        }
        return mImgThreadPool;
    }

    /**
     * 从内存缓存中获取图片
     * @param key
     * @return
     */
    private Bitmap getFromMemCache(String key){
        return mMemCache.get(key);
    }

    /**
     * 往内存缓存中保存图片
     * @param key
     * @param bmp
     */
    private void saveToMemCache(String key, Bitmap bmp){
        if(getFromMemCache(key)==null && bmp!=null){
            //Log.d(TAG, "####Save bitmap to memory:"+key);
            mMemCache.put(key, bmp);
        }
    }

    /**
     * 从网络上获取图片，并且重定义大小，保存文件时就已经更改了大小了
     * @param strUrl
     * @param w 不为0时生效
     * @param h
     * @return
     */
    private Bitmap getFromUrl(String strUrl, int w, int h) {
        Bitmap bmp = null;
        HttpURLConnection con = null;

        try{
            URL url = new URL(strUrl);
            con = (HttpURLConnection)url.openConnection();
            con.setRequestMethod("GET");
            con.setConnectTimeout(10*1000);
            con.setReadTimeout(10*1000);
            con.setDoInput(true);
            con.connect();

            // 把图片的流转换成数组
            InputStream is = con.getInputStream();
            byte[] data = inputStream2Bytes(is);

            // 获取图片大小
            BitmapFactory.Options opt = new BitmapFactory.Options();
            opt.inSampleSize = 1;
            if(w>0 && h>0) {
                opt.inJustDecodeBounds = true;  // 不会占用内存，仅仅获取大小
                BitmapFactory.decodeByteArray(data, 0, data.length, opt);
                if (opt.outHeight > h || opt.outWidth > w) {
                    int heightRadio = Math.round((float) opt.outHeight / (float) h);
                    int widthRadio = Math.round((float) opt.outWidth / (float) w);
                    opt.inSampleSize = heightRadio > widthRadio ? widthRadio : heightRadio;
                }
            }
            // 根据要求大小重新编码图片
            opt.inJustDecodeBounds = false;
            bmp = BitmapFactory.decodeByteArray(data, 0, data.length, opt);
            is.close();
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            if(con != null){
                con.disconnect();
            }
        }

        return bmp;
    }

    private byte[] inputStream2Bytes(InputStream is){
        if(is != null) {
            ByteArrayOutputStream output = new ByteArrayOutputStream();
            byte[] buf = new byte[1024];
            int count = 0;
            try {
                while((count=is.read(buf)) != -1){
                    output.write(buf, 0, count);
                }
                return output.toByteArray();
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
        return null;
    }

    /**
     *
     * @param url
     * @param view
     * @param width     不为0时生效，定义获取到图片的大小
     * @param height
     * @param listener
     */
    public void getBitmap(final String url,
                          final ImageView view,
                          final int width,
                          final int height,
                          final OnImageListener listener){
        // 需要使用url作为图片名字和key，但是在文件中不能出现某些字符，需要替换掉
        final String key = url.replaceAll("[^\\w]", "");
        // 获取到图片时，执行回调
        final Handler handler = new Handler() {
            @Override
            public void handleMessage(Message msg) {
                super.handleMessage(msg);
                if(listener != null){
                    listener.onImageDownload((Bitmap)msg.obj, view, url, msg.arg1>0?true:false);
                }
            }
        };

        Bitmap bmp = getBitmap(url);
        Message msg = handler.obtainMessage();
        msg.obj = bmp;
        msg.arg1 = 0;
        handler.sendMessage(msg);

        // 从网络中获取
        if(bmp == null){
            // 在线程池中获取网络图片
            getThreadPool().execute(new Runnable() {
                @Override
                public void run() {
                    Bitmap b = null;
                    // 从文件中获取
                    b = fileCache.getBitmap(key);
                    if(b != null){
                        Message msg = handler.obtainMessage();
                        msg.obj = b;
                        msg.arg1 = 0;
                        handler.sendMessage(msg);
                        Log.d(TAG, "Get bitmap from file:"+url);
                        saveToMemCache(key, b);
                        return;
                    }

                    /*// 从网络获取
                    b = getFromUrl(url, width, height);
                    if(b != null) {
                        Message msg = handler.obtainMessage();
                        msg.obj = b;
                        msg.arg1 = 1;
                        handler.sendMessage(msg);

                        Log.d(TAG, "Get bitmap from internet success:"+url);
                        try {
                            fileCache.saveBitmap(key, b);
                            saveToMemCache(key, b);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    } else{
                        Log.d(TAG, "Get bitmap from internet fail:"+url);
                    }*/
                    b = getVideoThumbnail(url);
                    if(b != null) {
                        Message msg = handler.obtainMessage();
                        msg.obj = b;
                        msg.arg1 = 1;
                        handler.sendMessage(msg);

                        //Log.d(TAG, "get video thumbnail success:"+url);
                        /*try {
                            fileCache.saveBitmap(key, b);
                            saveToMemCache(key, b);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }*/
                        saveToMemCache(key, b);
                    } else{
                        Log.d(TAG, "get video thumbnail  fail:"+url);
                    }
                }
            });
        }
    }

    /**
     * 从内存获取图片
     * @param url
     * @return
     */
    public Bitmap getBitmap(final String url){
        // 需要使用url作为图片名字和key，但是在文件中不能出现某些字符，需要替换掉
        final String key = url.replaceAll("[^\\w]", "");
        return getFromMemCache(key);
    }

    /**
     * 设置动画（网络获取到图片时）
     * @param imageView
     * @param bitmap
     * @param isTran
     */
    public void setImageBitmap(ImageView imageView, Bitmap bitmap, boolean isTran) {
        if (isTran) {
            final TransitionDrawable td = new TransitionDrawable(
                    new Drawable[] {
                            new ColorDrawable(android.R.color.transparent),
                            new BitmapDrawable(bitmap) });
            td.setCrossFadeEnabled(true);
            imageView.setImageDrawable(td);
            td.startTransition(200);
        } else {
            imageView.setImageBitmap(bitmap);
        }
    }

    /**
     * 取消下载任务（在滑动的时候）
     */
    public synchronized void cancelDownload(){
        if(mImgThreadPool != null){
            mImgThreadPool.shutdownNow();
            mImgThreadPool = null;
        }
    }

    public interface OnImageListener{
        void onImageDownload(Bitmap bmp, ImageView view, String url, boolean isTran);
    }

    /**
     * 清除文件和内存缓存
     */
    public void clear(){
        fileCache.clear();
        if(mMemCache.size() > 0) {
            mMemCache.evictAll();
        }
    }

//    private Bitmap getVideoThumbnail(String filePath) {
//        Bitmap bitmap = null;
//        MediaMetadataRetriever retriever = new MediaMetadataRetriever();
//        try {
//            retriever.setDataSource(filePath);
//            bitmap = retriever.getFrameAtTime();
//        }
//        catch(IllegalArgumentException e) {
//            e.printStackTrace();
//        }
//        catch (RuntimeException e) {
//            e.printStackTrace();
//        }
//        finally {
//            try {
//                retriever.release();
//            }
//            catch (RuntimeException e) {
//                e.printStackTrace();
//            }
//        }
//        return bitmap;
//    }

    private Bitmap getVideoThumbnail(String filePath) {
        Bitmap bitmap =null;
        bitmap = ThumbnailUtils.createVideoThumbnail(filePath, MediaStore.Images.Thumbnails.MINI_KIND);
        return bitmap;
    }
}
