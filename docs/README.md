#用之前请先耐心的看完这份README，谢谢

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
##jpeg输出
    在增加了JpgEncoder中增加了ScalingFactor以及Resizer用于调整jpeg图像的输出。

###用于调整缩放比例的-x选项，该参数必须与libjpge-turbo所支持的缩放比例匹配（默认：1.00）
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

###用于调整图片质量的-Q选项（默认：80）
    本来想增加原始图像压缩后的质量选项，后来发现被人捷足先登了，
    使用-Q选项即可，参数为0～100

###用于调整图片采样方式的-z选项（默认：2）
    TJSAMP_440   0
    TJSAMP_422   1
    TSAMP_420    2
    TSAMP_GRAY   3
    TSAMP_440    4
    TSAMP_411    5

##yuv输出
    为了将抓取的屏幕图像，转换为h264流，因此在JpgEncoder中增加原始的RGBA_8888转换为YUV的功能。

###用于调整缩放比例的-x选项（默认：1.00）
    该选项在yuv输出下依然有效，只不过少了libjpge-turbo中只能缩放特定比例的限制。
    -x选项的所指定的缩放比例，直接关系到所输出的yuv格式图片的分辨率。
    且该分辨率与硬件编码器所支持的Profile以及level息息相关。
    因此，在显示指定该参数时，请先确认目标设备的硬件编码器是否有相关Profile的支持。

###用于指定特定的YUV格式
    安卓平台上的硬编码接口统一采用OpenMax规范，但安卓上的硬编码器来自众多不同的硬件厂商，其实现各有不同。
    参考http://bigflake.com/mediacodec/ 中的FQA5时才发现，不同硬编码器所能接收的输入各不相同。
    因此修改过的minicap，支持两种yuv输出，既I420以及NV12两种，能够适应大部分硬编码器的场景。

##交互方式
由于从Android5.0开始，限制了LocalSocket的使用，因此考虑将LocalSocket替换成普通的TCPSocket。


#关于submodules文件夹
请使用git submodules来初始化该文件夹下的项目。
##broadway
一个可以在浏览器中使用的h264的解码器，不过只能软解Baseline Profile类型的h264流。
##h264-live-player
一个基于broadway的播放器。example改一改就能得到一个能与安卓设备交互的server。

#关于原始项目
原始的minicap版本采用LocalSocket与外部进行交互。openstf中使用adb forward命令将minicap所开启的unix domain socket转发出来，进行连接。
jni目录下minicap-shared/aosp/中的所有文件都不会参与编译。如果需要修改这部分代码，建议使用docker搭建对应api level的开发环境进行编译。
具体的操作可以参考jni/minicap-shared/aosp/目录下的README.md文件。
这部分代码全都是预编译的。预先生成好.o文件，放在minicap根目录的prebuilt下。


#REMAKE
##ImageMagick是一个用来处理图片的好东西～
http://xahlee.info/img/imagemagic.html
常用的调试命令
    identity用于识别图片的类型，以及输出图片的详细参数
    display，调用xwindows来显示指定的图片

