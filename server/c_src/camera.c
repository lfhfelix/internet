#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <jpeglib.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "include/camera.h"
#include "include/msg.h"

#define	REQBUFS_COUNT	4

//#define DEBUG

int image_size = 50 * 1024;
char img_buf[50 * 1024];

struct cam_buf {
	void *start;
	size_t length;
};

struct v4l2_requestbuffers reqbufs;
struct cam_buf bufs[REQBUFS_COUNT];

int img_capture(void *arg)
{
	bzero(&bufs, 0);
	bzero(&reqbufs, 0);
	
	int fd;
	unsigned int width;
	unsigned int height;
	unsigned int size;
	unsigned int index;
	char *yuv;
	char *rgb;

	unsigned char buf[50 * 1024] = {0};
	unsigned int jpg_size;

	//char bmp[640*480*3 + 54] = {0};
	
	int ret;

	width = 640;
	height = 480;
	//width = 480;
	//height = 320;
	fd = camera_init(CAMERA_USB, &width, &height, &size);
	if(fd < 0) {
		return -1;
	}
	printf("cam_init sucess\n");
	rgb = malloc(width * height * 3);
	camera_start(fd);
	//conv_rgb_to_jpeg_init();
	
	printf("cam into prepared\n");
	bzero(img_buf,0);
	while(1){
		ret = camera_dqbuf(fd, (void **)&yuv, &size, &index);
		if(ret < 0) {
			goto end;
		}
		conv_rgb_to_jpeg_init();
		yuv_to_rgb(yuv, rgb, width, height);
		jpg_size = conv_rgb_to_jpeg_work(rgb, buf, width, height, 24, 80);
		
#ifdef DEBUG
		printf("size: %d\n", jpg_size);
#endif
		//图片小于10k时，不拷贝到缓存中
		if(jpg_size > 10*1024) {
			memcpy(img_buf, buf, jpg_size);
			image_size = jpg_size;
		}
		
		ret = camera_eqbuf(fd, index);
		if(ret < 0) {
			goto end;
		}
		conv_rgb_to_jpeg_exit();
	}

end:
	//conv_rgb_to_jpeg_exit();
	free(rgb);
	camera_stop(fd);
	camera_exit(fd);
}

int camera_init(char *devpath, unsigned int *width, unsigned int *height, unsigned *size)
{
	int i;
	int fd;
	int ret;
	struct v4l2_buffer vbuf;
	struct v4l2_format format;
	struct v4l2_capability capability;

	fd = open(devpath, O_RDWR);
	if (fd == -1) {
		perror("cam init");
		return -1;
	}
	printf("open cam sucess\n");
	ret = ioctl(fd, VIDIOC_QUERYCAP, &capability);
	if (ret == -1) {
		perror("cam init");
		return -1;
	}
	printf("ioctl1 cam sucess\n");
	if(!(capability.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		printf("device can not support V4L2_CAP_VIDEO_CAPTURE\n");
		close(fd);
		return -1;
	}
	
	if(!(capability.capabilities & V4L2_CAP_STREAMING)) {
		printf("device can not support V4L2_CAP_STREAMING\n");
		close(fd);
		return -1;
	}
	printf("open2 cam sucess\n");
	memset(&format, 0, sizeof(format));
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
	format.fmt.pix.width = *width;
	format.fmt.pix.height = *height;
	format.fmt.pix.field = V4L2_FIELD_ANY;
	ret = ioctl(fd, VIDIOC_S_FMT, &format);
	if(ret == -1) {
		perror("cam init");
		return -1;
	}

	ret = ioctl(fd, VIDIOC_G_FMT, &format);
	if (ret == -1) {
		perror("cam init");
		return -1;
	}
	printf("open3 cam sucess\n");
	memset(&reqbufs, 0, sizeof(struct v4l2_requestbuffers));
	reqbufs.count	= REQBUFS_COUNT;
	reqbufs.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbufs.memory	= V4L2_MEMORY_MMAP;
	ret = ioctl(fd, VIDIOC_REQBUFS, &reqbufs);	
	if (ret == -1) {	
		perror("cam init");
		close(fd);
		return -1;
	}

	for (i = 0; i < reqbufs.count; i++)
	{
		printf("%d----%d\n", i, reqbufs.count);
		memset(&vbuf, 0, sizeof(struct v4l2_buffer));
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		vbuf.index = i;
		ret = ioctl(fd, VIDIOC_QUERYBUF, &vbuf);
		if (ret == -1) {
			perror("cam init");
			close(fd);
			return -1;
		}

		bufs[i].length = vbuf.length;
		bufs[i].start = mmap(NULL, vbuf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, vbuf.m.offset);
		if (bufs[i].start == MAP_FAILED)
		{
			perror("cam init");
			close(fd);
			return -1;
		}

		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
		if (ret == -1) {
			perror("cam init");
			close(fd);
			return -1;
		}
	}

	*width = format.fmt.pix.width;
	*height = format.fmt.pix.height;
	*size = bufs[0].length;

	return fd;
}

int camera_start(int fd)
{
	int ret;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ret = ioctl(fd, VIDIOC_STREAMON, &type);
	if (ret == -1) {
		perror("cam start");
		return -1;
	}
	printf("start capture\n");

	return 0;
}

int camera_dqbuf(int fd, void **buf, unsigned int *size, unsigned int *index)
{
	int ret;
	fd_set fds;
	struct timeval timeout;
	struct v4l2_buffer vbuf;
	int flags;

	FD_ZERO(&fds);
	FD_SET(fd, &fds);
	timeout.tv_sec = 3;
	timeout.tv_usec = 0;
	
	//设置文件描述符属性为非阻塞
	//flags = fcntl(fd, F_GETFL, 0); 
	//fcntl(fd, F_SETFL, flags | O_NONBLOCK);
	//ret = select(fd + 1, &fds, NULL, NULL, NULL);
	ret = select(fd + 1, &fds, NULL, NULL, &timeout);
	if (ret == -1) {
		perror("cam dqbuf");
		return -1;
	} else if (ret == 0) {
		printf("dequeue buffer timeout\n");
		return -1;
	} else {
		vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		vbuf.memory = V4L2_MEMORY_MMAP;
		ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
		if (ret == -1) {
			perror("cam dqbuf");
			return -1;
		}
		*buf = bufs[vbuf.index].start;
		*size = bufs[vbuf.index].length;
		*index = vbuf.index;

		return 0;
	}
}

int camera_eqbuf(int fd, unsigned int index)
{
	int ret;
	struct v4l2_buffer vbuf;

	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;
	vbuf.index = index;
	ret = ioctl(fd, VIDIOC_QBUF, &vbuf);
	if (ret == -1) {
		perror("cam eqbuf");
		return -1;
	}

	return 0;
}

int camera_stop(int fd)
{
	int ret;
	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	ret = ioctl(fd, VIDIOC_STREAMOFF, &type);
	if (ret == -1) {
		perror("cam stop");
		return -1;
	}
	printf("stop capture\n");

	return 0;
}

void camera_exit(int fd)
{
	int i;
	int ret;
	struct v4l2_buffer vbuf;

	vbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	vbuf.memory = V4L2_MEMORY_MMAP;

	for (i = 0; i < reqbufs.count; i++) {
		ret = ioctl(fd, VIDIOC_DQBUF, &vbuf);
		if (ret == -1)
			break;
	}

	for (i = 0; i < reqbufs.count; i++)
		munmap(bufs[i].start, bufs[i].length);

	close(fd);

	printf("cam exit\n");
}

void yuv_to_rgb(void *yuv, void *rgb, unsigned int width, unsigned int height)  
{  
    int           i,j;  
    unsigned char y1,y2,u,v;  
    int r1,g1,b1,r2,g2,b2;  
    unsigned char *src = yuv; 
    unsigned char *dst = rgb;
  
    for(i = 0; i < height; i++)  
    {  
    	for(j = 0; j < width / 2; j++) 
    	{  
    	y1 = *( src + (i * width / 2 + j) * 4);       
    	u  = *( src + (i * width / 2 + j) * 4 + 1);  
		y2 = *( src + (i * width / 2 + j) * 4 + 2);  
    	v  = *( src + (i * width / 2 + j) * 4 + 3);  

    	b1 = y1 + 1.042 * (v - 128);  
    	g1 = y1 - 0.34414 * (u - 128) - 0.71414 * (v - 128);  
    	r1 = y1 + 1.772 * (u - 128);  

  
    	b2 = y2 + 1.042 * (v - 128);  
    	g2 = y2 - 0.34414 * (u - 128) - 0.71414 * (v - 128);  
    	r2 = y2 + 1.772 * (u - 128);  

    	if(r1 > 255)  
    		r1 = 255;  
    	else if(r1 < 0)  
    		r1 = 0;  
  
    	if(b1 > 255)  
    		b1 = 255;  
    	else if(b1 < 0)  
    		b1 = 0;      
  
    	if(g1 > 255)  
    		g1 = 255;  
    	else if(g1 < 0)  
    		g1 = 0;      
  
    	if(r2 > 255)  
    		r2 = 255;  
    	else if(r2 < 0)  
    		r2 = 0;  
  
    	if(b2 > 255)  
    		b2 = 255;  
    	else if(b2 < 0)  
    		b2 = 0;      
  
    	if(g2 > 255)  
    		g2 = 255;  
    	else if(g2 < 0)  
    		g2 = 0;          
#if 1
    	*(dst + (i * width / 2 + j) * 6    ) = (unsigned char)b1;  
    	*(dst + (i * width / 2 + j) * 6 + 1) = (unsigned char)g1;  
    	*(dst + (i * width / 2 + j) * 6 + 2) = (unsigned char)r1;  
    	*(dst + (i * width / 2 + j) * 6 + 3) = (unsigned char)b2;  
    	*(dst + (i * width / 2 + j) * 6 + 4) = (unsigned char)g2;  
    	*(dst + (i * width / 2 + j) * 6 + 5) = (unsigned char)r2;  
#else
		*(dst + ((height - i - 1) * width / 2 + j) * 6    ) = (unsigned char)r1;  
    	*(dst + ((height - i - 1) * width / 2 + j) * 6 + 1) = (unsigned char)g1;  
    	*(dst + ((height - i - 1) * width / 2 + j) * 6 + 2) = (unsigned char)b1;  
    	*(dst + ((height - i - 1) * width / 2 + j) * 6 + 3) = (unsigned char)r2;  
    	*(dst + ((height - i - 1) * width / 2 + j) * 6 + 4) = (unsigned char)g2;  
    	*(dst + ((height - i - 1) * width / 2 + j) * 6 + 5) = (unsigned char)b2; 
#endif


    	}  
    }  
#ifdef DEBUG
    printf("change to RGB OK \n");  
#endif
}

typedef struct tagBMPFILEHEADER {
	unsigned short align;
	unsigned short bfType;
	unsigned int bfSize;
	unsigned int bfReserved;
	unsigned int bfOffBits;
}BMPFILEHEADER;

typedef struct tagBMPINFOHEADER{
	unsigned int biSize;		// must be 0x28
	unsigned int biWidth;
	unsigned int biHeight;
	unsigned short biPlanes;		// must be 1
	unsigned short biBitCount;
	unsigned int biCompression;
	unsigned int biSizeImage;
	unsigned int biXPelsPerMeter;
	unsigned int biYPelsPerMeter;
	unsigned int biClrUsed;
	unsigned int biClrImportant;
}BMPINFOHEADER;


void rgb_to_bmp(void *rgb, void *bmp, unsigned int width, unsigned int height)
{
	int len = width * height * 3;
	BMPINFOHEADER bi;
	BMPFILEHEADER bf;

	bi.biSize = 0x28;
	bi.biWidth  = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount   = 24;
	bi.biCompression= 0;
	bi.biSizeImage  = len;	/* WIDTHBYTES(bi.biWidth * bi.biBitCount) * bi.biHeight*/
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed      = 0;
	bi.biClrImportant = 0;

	bf.bfType = 0x4D42;	/* 'B''M'*/
	bf.bfSize = 54 + bi.biSizeImage;
	bf.bfReserved= 0;
	bf.bfOffBits = 54;	/* 偏移*/


	unsigned char *pbmp = bmp;
	memcpy(pbmp, &bf.bfType, 14);
	pbmp += 14;
	memcpy(pbmp, &bi, 40);
	pbmp += 40;

	unsigned char *prgb = rgb;

	memcpy(pbmp, prgb, len);

}

typedef struct {
	struct jpeg_destination_mgr pub;
	JOCTET *buffer;
	unsigned char *outbuffer;
	int outbuffer_size;
	unsigned char *outbuffer_cursor;
	int *written;
} jpeg_dest_mgr, *jpeg_dest_mgr_ptr;

struct jpeg_mgr_info {
	unsigned long written;
	JSAMPROW row_pointer[1];
	struct jpeg_error_mgr jerr;
	struct jpeg_compress_struct cinfo;
};

static struct jpeg_mgr_info jinfo;

void conv_rgb_to_jpeg_init(void)
{
	memset(&jinfo, 0, sizeof(struct jpeg_mgr_info));
	jinfo.cinfo.err = jpeg_std_error(&jinfo.jerr);
	jpeg_create_compress(&jinfo.cinfo);
}

int conv_rgb_to_jpeg_work(void *rgb, void *jpeg, unsigned int width, unsigned int height, unsigned int bpp, int quality)
{
	jinfo.written = width * height * bpp / 3;
	jpeg_mem_dest(&jinfo.cinfo, (unsigned char **)&jpeg, &jinfo.written);

	jinfo.cinfo.image_width = width;
	jinfo.cinfo.image_height = height;
	jinfo.cinfo.input_components = bpp / 8;
	jinfo.cinfo.in_color_space = JCS_RGB;
	jpeg_set_defaults(&jinfo.cinfo);
	jpeg_set_quality(&jinfo.cinfo, quality, TRUE);

	jpeg_start_compress(&jinfo.cinfo, TRUE);

	while(jinfo.cinfo.next_scanline < height) {
		jinfo.row_pointer[0] = rgb + jinfo.cinfo.next_scanline * width * bpp / 8;
		jpeg_write_scanlines(&jinfo.cinfo, jinfo.row_pointer, 1);
	}

	jpeg_finish_compress(&jinfo.cinfo);

	return (jinfo.written);
}

void conv_rgb_to_jpeg_exit(void)
{
	jpeg_destroy_compress(&jinfo.cinfo);
}

#if 0
int main(){
	//img_capture(NULL);

	return 0;
}
#endif
