// #include <iostream>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <string.h>
#include <sys/mman.h>
#include <jpeglib.h>
#include <linux/fb.h>


// read_JPEG_file 为封装好的函数，需要转换格式的时候直接调用，无需深入理解
int read_JPEG_file(const char* jpegData, char *rgbdata, int size)
{
    struct jpeg_error_mgr jerr;
    struct jpeg_decompress_struct cinfo;
    cinfo.err = jpeg_std_error(&jerr);

    // 1. 创建解码对象并且初始化
    jpeg_create_decompress(&cinfo);
    // 2. 装备解码的数据
    // jpeg_stdio_src(&cinfo, infile);
    jpeg_mem_src(&cinfo, jpegData, size);

    // 3. 获取jpeg图片文件的参数
    (void) jpeg_read_header(&cinfo, TRUE);

    // 4. set parameters for decompression
    // 5. 开始解码
    (void) jpeg_start_decompress(&cinfo);

    // 6. 申请存储一行数据的内存空间
    int row_stride = cinfo.output_width * cinfo.output_components;
    unsigned char *buffer = (unsigned char *)malloc(row_stride);
    int i = 0;
    while(cinfo.output_scanline < cinfo.output_height) {
        // std::cout << "i=" << i << std::endl;
        // printf("index=%d\n",i);
        (void) jpeg_read_scanlines(&cinfo, &buffer, 1);
        memcpy(rgbdata+i*640*3, buffer, row_stride);
        i++;
    }

    // 7. 解码完成
    (void) jpeg_finish_decompress(&cinfo);

    // 8. 释放解码对象
    jpeg_destroy_decompress(&cinfo);

    return 1;
}

int fd_fb;
static struct fb_var_screeninfo var;     // LCD 可变参数
static unsigned int *fb_base = NULL;     // Framebuffer 映射基地址
int lcd_w = 800, lcd_h = 480;            // 定义显示器分辨率

// 将数据流以3字节为单位拷贝到rgb显存中

void lcd_show_rgb(unsigned char *rgbdata, int w, int h)
{
    unsigned int *ptr = fb_base;
    for (int i = 0; i < h; i++) {
        for (int j = 0; i < w; j++) {
            rgbdata[j*3] ^= rgbdata[j*3+2];
            rgbdata[j*3+2] ^= rgbdata[j*3];
            rgbdata[j*3] ^= rgbdata[j*3+2];
            memcpy(ptr+j, rgbdata+j*3, 3);
        }
        ptr += lcd_w;
        rgbdata += w*3;
    }
}

int main(void)
{
    fd_fb = open("/dev/fb0", O_RDWR);   // 打开LCD文件
    if (fd_fb < 0)
    {
        perror("/dev/fb0");
        exit(-1);
    }
    if (ioctl(fd_fb, FBIOGET_VSCREENINFO, &var))
    {
        printf("can't get fb_var_screeninfo.\n");
        goto err1;
    }

    // 虚拟机-ubuntu
    lcd_w = var.xres_virtual; // xres_virtual 参数可以自动获取当前虚拟机显示器的分辨率
    lcd_h = var.yres_virtual;

    // 建立显示器fb内存映射 方便控制
    fb_base = (unsigned int*) mmap(NULL, lcd_w*lcd_h*4, PROT_READ|PROT_WRITE, MAP_SHARED, fd_fb, 0);
    if (fb_base == NULL)
    {
        printf("can't mmap Framebuffer.\n");
        goto err1;
    }

    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0)
    {
        perror("open device failed.");
        return -1;
    }

    struct v4l2_format vfmt;
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;    //摄像头采集
    vfmt.fmt.pix.width = 640;   //设置摄像头采集参数，不可以任意设置
    vfmt.fmt.pix.height = 480;
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;  //设置视频采集格式为mjpeg格式

    int ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);
    if (ret < 0)
    {
        perror("设置格式失败1");
    }

    // 申请内核空间
    struct v4l2_requestbuffers reqbuffer;
    reqbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    reqbuffer.count = 4;    // 申请4个缓冲区
    reqbuffer.memory = V4L2_MEMORY_MMAP;    //映射方式

    ret = ioctl(fd, VIDIOC_REQBUFS, &reqbuffer);
    if (ret < 0)
    {
        perror("申请空间失败");
    }

    //映射
    unsigned char *mptr[4]; // 保存映射后用户空间的首地址
    unsigned int size[4];
    struct v4l2_buffer mapbuffer;

    // 初始化type 和index
    mapbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    for(int i = 0; i < 4; i++) {
        mapbuffer.index = i;
        ret = ioctl(fd, VIDIOC_QUERYBUF, &mapbuffer);   // 从内核空间查询一个空间做映射
        if (ret < 0)
        {
            perror("查询内核空间失败");
        }

        // 映射到用户空间
        mptr[i] = (unsigned char *)mmap(NULL, mapbuffer.length, PROT_READ|PROT_WRITE, MAP_SHARED, fd, mapbuffer.m.offset);
        size[i] = mapbuffer.length;     // 保存映射长度用于后期释放

        // 查询后通知内核已经放回
        ret = ioctl(fd, VIDIOC_QBUF, &mapbuffer);
        if (ret < 0)
        {
            perror("放回失败");
        }
    }

    // 开始采集
    int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ret = ioctl(fd, VIDIOC_STREAMON, &type);
    if (ret < 0)
    {
        perror("开启失败");
    }

    //定义一个空间存储解码后的rgb
    unsigned char rgbdata[640 * 480 *3];
    while(1)
    {
        // 从队列提取一帧数据
        struct v4l2_buffer readbuffer;
        readbuffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  // 每个结构体都需要设置type 为这个参数
        ret = ioctl(fd, VIDIOC_DQBUF, &readbuffer);
        if (ret < 0)
        {
            perror("读取数据失败");
        }

        // 显示在lcd上
        {
            read_JPEG_file(mptr[readbuffer.index], rgbdata, readbuffer.length);  //把jpeg数据解码为rgb数据
            lcd_show_rgb(rgbdata, 640, 480);
        }

        // 通知内核使用完毕
        ret = ioctl(fd, VIDIOC_QBUF, &readbuffer);
        if(ret < 0)
        {
            perror("放回队列失败");
        }
    }

    // 停止采集
    ret = ioctl(fd, VIDIOC_STREAMOFF, &type);

    // 释放映射
    for(int i = 0; i < 4; i++){
        munmap(mptr[i], size[i]);
    }

    close(fd);  //关闭文件
    return 0;

err1:
    close(fd_fb);

    return -1;
}