# PlayerDemo
OpenGL+IJKPlayer

加入OpenCV图像处理

自行下载OpenCV库(https://nchc.dl.sourceforge.net/project/opencvlibrary/opencv-android/3.2.0/opencv-3.2.0-android-sdk.zip)

然后拷贝OpenCV-android-sdk/sdk/native/目录到jni的根目录，并从命名位open_cv


pBeanOriginal-->从图片\视频中获取帧数据，并绘制到帧缓存，源数据，未作处理，直接绘制
pBeanProcess-->从pBeanOriginal绘制到帧缓存数据做处理
pBeanDisplay-->从pBeanProcess绘制到帧缓存数据直接显示，未作处理
