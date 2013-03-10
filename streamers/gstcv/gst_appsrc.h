//
//  gst_appsrc.h
//  gst_appsrc
//
//  Created by Ting Cao on 1/24/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//
//  It needs packages of:
//    gstreamer-0.10
//    gstreamer-app-0.10
//    gstreamer-video-0.10
//    gdk-pixbuf-2.0
//
//  To build:
//    g++ gst_appsrc.cpp -o gst_appsrc `pkg-config --cflags --libs gstreamer-0.10 gstreamer-app-0.10 gstreamer-video-0.10 opencv gdk-pixbuf-2.0`
//
// To run in rtp:
// 1) start client first:
//    gst-launch udpsrc port=5000 ! theoradec ! ffmpegcolorspace ! ximagesink
// 2) start server second:
//    gst_appsrc --host=192.168.1.2
//
// You can gst_appsrc -h
// to list all options
//
#ifndef gst_appsrc_gst_appsrc_h
#define gst_appsrc_gst_appsrc_h
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <gst/video/video.h>

#ifdef __cplusplus
extern "C" {
#endif
typedef enum
{
   GST_APP_STANDALONE = 0,
   GST_APP_RTP = 1
}
GST_APP_TYPE;
   
typedef struct
{
   GstElement *pipeline;
   GstElement *appsrc;
   GMainLoop *loop;
   guint sourceid;
   GTimer *timer;
   char name[32];
   char input_image[128];
   char host[128];
   int port;
   int width;           // window width
   int height;          // window height
   int bit_per_sample;
   int iwidth;          // image width
   int iheight;         // image height
   int framerate;
   bool fpga;
   unsigned int flags;  // internal status flags
                        // bit0 - GST_APP_TYPE
   double timer_elapsed;
}
GstAppParam;

#ifdef __cplusplus
}
#endif

#endif
