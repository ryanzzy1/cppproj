#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

// 获取支持的摄像头格式
#include <sys/ioctl.h>
#include <linux/videodev2.h>

#include <string.h>

int main()
{
    int fd = open("/dev/video0", O_RDWR);
    if (fd < 0)
    {
        std::cout << "open device failed" << std::endl;

        return -1;
    }
    // 获取支持的摄像头格式, ioctl(文件描述符，命令，与命令对应的结构体)
    struct v4l2_format vfmt;      //配置摄像头信息需要使用该结构体
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;    // 摄像头采集
    /* 配置摄像头*/
    vfmt.fmt.pix.width = 640;   // 设置摄像头采集参数，不可任意设置
    vfmt.fmt.pix.height = 480;  
    vfmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;   // 设置视频采集格式，根据上一步测得，格式有YUYV和YYUV区别
    
    int ret = ioctl(fd, VIDIOC_S_FMT, &vfmt);   // 设置格式命令
    if(ret < 0)
    {
        perror("set format failed 1");
    }

    memset(&vfmt, 0, sizeof(vfmt));
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = ioctl(fd, VIDIOC_G_FMT, &vfmt);

    if (ret < 0)
    {
        perror("set format failed 2");
    }

    if (vfmt.fmt.pix.width == 640 && vfmt.fmt.pix.height == 480 && vfmt.fmt.pix.pixelformat == V4L2_PIX_FMT_YUYV)
    {
        std::cout << "set format successfully.\n"; 
    } else
    {
        std::cout << "set format failed 3.\n";
    }
/*
    // 获取支持的摄像头格式, ioctl(文件描述符，命令，与命令对应的结构体)
    // struct v4l2_fmtdesc vfmt;   // 获取摄像头支持的格式使用
    vfmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;    // 摄像头采集
    int i = 0;
    while(1)
    {
        vfmt.index = i;
        i++;
        int ret = ioctl(fd, VIDIOC_ENUM_FMT, &vfmt);
        std::cout << "ret: " << ret << std::endl;
        if (ret < 0)
        {
            perror("get cam failed.");
            break;
        }

        std::cout << "index=" << vfmt.index << "\nflags=" << vfmt.flags
                  << "\ndiscription=" << vfmt.description << std::endl;
        
        unsigned char *p = (unsigned char*)&vfmt.pixelformat;
        std::cout << "pixelformat="<< p[0] << p[1] << p[2] << p[3] << std::endl;
        std::cout << "reserved=" << vfmt.reserved[0] << std::endl;
    }
*/
    close(fd);

    return 0;
}