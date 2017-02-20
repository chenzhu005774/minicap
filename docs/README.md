#改这玩意儿时遇到的问题集

##关于编译

    $MINICAP_HOME/jni/Application.mk中修改

    APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

    可指定仅编译特定平台下的minicap 这里我只保留armeabi-v7a，arm64-v8a有需要的自己加上

##关于执行

    $MINICAP_HOME/run.sh脚本用于整合编译，部署等操作

    改脚本会负责编译整个minicap
    并将特定的minicap以及minicap.so推送到手机的/data/local/tmp/minicap-dev目录下

##关于修改
    为minicap所输出的jpge数据，添加比例缩放支持。

    TODO
        在JpgEncoder中增加缩放比例参数
        原始的RGB图像经过tjCompress之后，创建tjtransform结构，tjDecompress为所期望的缩放比例。
        再次进行tjCompress

    TODO    
        调整图像的缩放比例
        调整图像像素点的大小
        支持灰度图像输出
