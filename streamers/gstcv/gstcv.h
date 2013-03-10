//
//  gstcv.h
//  gst_appsrc
//
//  Created by Ting Cao on 1/27/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//
#ifndef __gst_appsrc__gstcv__
#define __gst_appsrc__gstcv__

#include <gst/gst.h>

typedef enum
{
   GSTCV_OPERATION_INPUT = (1<<0),
   GSTCV_OPERATION_SOBEL = (1<<1),
   GSTCV_OPERATION_CANNY = (1<<2),
   // operations can be applied to the aboves
   GSTCV_OPERATION_SMOOTH = (1<<30),
   GSTCV_OPERATION_INVERSE = (1<<31),
}
GSTCV_OPERATION_TYPE;

typedef struct
{
   double data1;
   double data2;
   double data3;
   unsigned int operation;
}
GSTCV_DATA;

unsigned int InitGstCV( const char* input_image, int& width, int& height );
int PerformGstCV( unsigned int handle,
                 const GSTCV_DATA* data,
                 GstBuffer** buffer );
int DoneGstCV( unsigned int handle );
   
#endif /* defined(__gst_appsrc__gstcv__) */
