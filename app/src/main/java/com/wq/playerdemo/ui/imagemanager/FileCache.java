package com.wq.playerdemo.ui.imagemanager;

import android.app.Activity;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class FileCache {
    private static final String TAG = "FileCache";
    private String FOLDER_NAME = "/imageCache";
    private String mCachePath;

    public FileCache(Activity activity){
        mCachePath = activity.getCacheDir().getPath();
    }

    /**
     * 获取缓存图片存放路径
     * @return
     */
    private String getStorageDir(){
        return mCachePath+FOLDER_NAME;
    }

    /**
     * 保存图片到磁盘
     * @param fileName
     * @param bmp
     * @throws IOException
     */
    public File saveBitmap(String fileName, Bitmap bmp) throws IOException{
        if(bmp == null)
            return null;
        File folder = new File(getStorageDir());
        if(!folder.exists()){
            folder.mkdir();
        }

        File file = new File(getStorageDir()+File.separator+fileName);
        file.createNewFile();
        FileOutputStream fos = new FileOutputStream(file);
        bmp.compress(Bitmap.CompressFormat.PNG, 12, fos);
        fos.flush();
        fos.close();
        Log.d(TAG, "####Save bitmap to file:"+fileName);

        return file;
    }

    /**
     * 从文件中获取图片
     * @param fileName
     * @return
     */
    public Bitmap getBitmap(String fileName){
        return BitmapFactory.decodeFile(getStorageDir() + File.separator + fileName);
    }

    /**
     * 文件是否存在
     * @param fileName
     * @return
     */
    public boolean isExists(String fileName){
        return new File(getStorageDir() + File.separator + fileName).exists();
    }

    /**
     * 获取文件大小
     * @param fileName
     * @return
     */
    public long getSize(String fileName){
        return new File(getStorageDir() + File.separator + fileName).length();
    }

    /**
     * 清除文件缓存
     */
    public void clear(){
        File folder = new File(getStorageDir());
        if(!folder.exists())
            return;
        if(folder.isDirectory()){
            for(String fileName:folder.list()){
                new File(folder, fileName).delete();
            }
        }
        folder.delete();
    }
}
