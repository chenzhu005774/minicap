#改这玩意儿时遇到的问题集

##关于编译

    $MINICAP_HOME/jni/Application.mk中修改

    APP_ABI := armeabi-v7a arm64-v8a x86 x86_64

    可指定仅编译特定平台下的minicap 这里我只保留armeabi-v7a，arm64-v8a有需要的自己加上

##关于执行

    $MINICAP_HOME/run.sh脚本用于整合编译，部署等操作

    改脚本会负责编译整个minicap
    并将特定的minicap以及minicap.so推送到手机的/data/local/tmp/minicap-dev目录下

    确定手机分辨率之后，例如1080x1920
    ./run.sh -P 1080x1920/1080x1920@0
    可在手机中启动minicap进行

#关于修改

##增加-x选项，用于输出libjpge-turbo所支持的缩放比例，输出如下：
        ==============================
        Scaling Table of libjpge-turbo
        ==============================
        Scaling: 2/1 (Percentage: 2.000000)
        Scaling: 15/8 (Percentage: 1.875000)
        Scaling: 7/4 (Percentage: 1.750000)
        Scaling: 13/8 (Percentage: 1.625000)
        Scaling: 3/2 (Percentage: 1.500000)
        Scaling: 11/8 (Percentage: 1.375000)
        Scaling: 5/4 (Percentage: 1.250000)
        Scaling: 9/8 (Percentage: 1.125000)
        Scaling: 1/1 (Percentage: 1.000000)
        Scaling: 7/8 (Percentage: 0.875000)
        Scaling: 3/4 (Percentage: 0.750000)
        Scaling: 5/8 (Percentage: 0.625000)
        Scaling: 1/2 (Percentage: 0.500000)
        Scaling: 3/8 (Percentage: 0.375000)
        Scaling: 1/4 (Percentage: 0.250000)
        Scaling: 1/8 (Percentage: 0.125000)

##压缩质量
    本来想增加原始图像压缩后的质量选项，后来发现被人捷足先登了，
    使用-Q选项即可，参数为0～100

#TODO    
##为minicap所输出的jpge数据，添加比例缩放支持。
    在JpgEncoder中增加缩放比例参数
    原始的RGB图像经过tjCompress之后，创建tjtransform结构，tjDecompress为所期望的缩放比例。
    再次进行tjCompress

##调整图像的缩放比例
##调整图像像素点的大小
##支持灰度图像输出


#ISSUEs
    这东西挂着跑久了，手机用起来会变得非常慢，但是不吃内存，也没有飙高的cpu。
    启动minicap后，如果直接在apk中kill掉minicap，手机会画屏，需要关闭屏幕重新开启，方可正常。

#REMAKE
##ImageMagick是一个用来处理图片的好东西～
        http://xahlee.info/img/imagemagic.html