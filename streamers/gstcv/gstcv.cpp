//
//  gstcv.cpp
//  gst_appsrc
//
//  Created by Ting Cao on 1/27/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//
#include "gstcv.h"
#include "cv.h"
#include "highgui.h"

#define ALGORITHM_HANDLE_MAX (1)

typedef struct
{
   IplImage* input;
   IplImage* gray;
   IplImage* gray_intermediate;
   IplImage* input_intermediate;
   IplImage* output;
}
CV_IMAGES;

typedef struct
{
   unsigned int handle;
   CV_IMAGES images;
}
ALGORITHM_HANDLE;

ALGORITHM_HANDLE AlgorithHandles[1] = {
   {
      0,
      {
         NULL,
         NULL,
         NULL,
         NULL,
         NULL,
      }
   },
};

static unsigned int HandleCount = 0;

unsigned int InitGstCV( const char* input_image, int& width, int& height )
{
   ALGORITHM_HANDLE* handle = NULL;
   IplImage* img_rgb = NULL;
   if ( ( ( sizeof(AlgorithHandles)/sizeof(ALGORITHM_HANDLE) ) > HandleCount ) &&
       ( img_rgb  = cvLoadImage( input_image ) ) )
   {
      handle = &AlgorithHandles[HandleCount];
      width = img_rgb->width;
      height = img_rgb->height;
      if ( IplImage* img_gry  = cvCreateImage( cvSize( img_rgb->width,img_rgb->height ), IPL_DEPTH_8U, 1) )
      {
         cvCvtColor(img_rgb, img_gry, CV_BGR2GRAY);
         CV_IMAGES* images = &handle->images;
         images->input = img_rgb;
         images->gray = img_gry;
         images->gray_intermediate = cvCreateImage( cvSize( img_gry->width, img_gry->height ), IPL_DEPTH_8U, 1);
         images->output = cvCreateImage( cvSize( img_gry->width, img_gry->height ), IPL_DEPTH_8U, 3 );
         images->input_intermediate = cvCreateImage( cvSize(images->input->width, images->input->height), IPL_DEPTH_8U, images->input->nChannels );
         handle->handle = HandleCount++;
      }
      else
      {
         cvReleaseImage( &img_rgb );
         handle->handle = 0;
         handle = NULL;
      }
   }

   return ( NULL == handle ) ? -1 : handle->handle;
}

/**
 * If both GSTCV_OPERATION_SMOOTH and GSTCV_OPERATION_INVERSE are applied
 * do GSTCV_OPERATION_INVERSE first
 *
 * Make sure the passing input, intermediate and output have the same image dimension and components
 */
void ApplyEffects( IplImage* input, IplImage* intermediate, IplImage* output, const GSTCV_DATA* data )
{
   if ( ( GSTCV_OPERATION_SMOOTH | GSTCV_OPERATION_INVERSE ) & data->operation )
   {
      bool both = false;
      IplImage* i = input, *o = output;
      if ( ( GSTCV_OPERATION_SMOOTH & data->operation ) &&
          ( GSTCV_OPERATION_INVERSE & data->operation ) )
      {
         both = true;
         o = intermediate;
      }
      if ( GSTCV_OPERATION_INVERSE & data->operation )
      {
         cvNot( i, o );
         i = o;
         o = output;
      }
      if ( GSTCV_OPERATION_SMOOTH & data->operation )
      {
         cvSmooth( i, o );
      }
   }
}

void PerformInput( CV_IMAGES* images, const GSTCV_DATA* data )
{
   if ( ( GSTCV_OPERATION_SMOOTH | GSTCV_OPERATION_INVERSE ) & data->operation )
   {
      ApplyEffects( images->input, images->input_intermediate, images->output, data );
   }
   else
   {
      cvCopy (images->input, images->output);
   }
}

void PerformCanny( CV_IMAGES* images, const GSTCV_DATA* data )
{
   bool need_effects = ( ( GSTCV_OPERATION_SMOOTH | GSTCV_OPERATION_INVERSE ) & data->operation ) ? true : false;
   IplImage* temp = images->gray_intermediate;
   IplImage* effects = need_effects ?
      cvCreateImage( cvSize( temp->width, temp->height), IPL_DEPTH_8U, temp->nChannels )
      : temp;
   cvCanny( images->gray, temp, data->data1, data->data2, data->data3 );
   if ( need_effects)
   {
      ApplyEffects( temp, images->gray_intermediate, effects, data );
   }
   cvCvtColor( effects, images->output, CV_GRAY2RGB );

   if ( need_effects)
   {
      cvReleaseImage( &effects );
   }
}

void PerformSobel( CV_IMAGES* images, const GSTCV_DATA* data )
{
   bool need_effects = ( ( GSTCV_OPERATION_SMOOTH | GSTCV_OPERATION_INVERSE ) & data->operation ) ? true : false;
   IplImage* effects = need_effects ? images->input_intermediate : images->output;
   cvSobel( images->input, effects, data->data1, data->data2, data->data3 );
   if ( need_effects)
   {
      ApplyEffects( effects, images->input_intermediate, images->output, data );
   }
}

typedef void ( *PerformCVOperation)( CV_IMAGES* images, const GSTCV_DATA* data );
int PerformGstCV( unsigned int handle,
                 const GSTCV_DATA* data,
                 GstBuffer** buffer )
{
   int success = -1;
   if ( ( sizeof(AlgorithHandles)/sizeof(ALGORITHM_HANDLE) ) > handle )
   {
      ALGORITHM_HANDLE* h = &AlgorithHandles[handle];
      CV_IMAGES* images = &h->images;
      if ( handle == h->handle )
      {
         PerformCVOperation operation = NULL;
         if ( GSTCV_OPERATION_INPUT & data->operation )
         {
            operation = PerformInput;
         }
         
         if ( ( NULL == operation ) &&
             ( GSTCV_OPERATION_SOBEL & data->operation ) )
         {
            operation = PerformSobel;
         }

         if ( ( NULL == operation ) &&
             ( GSTCV_OPERATION_CANNY & data->operation ) )
         {
            operation = PerformCanny;
         }
         
         if ( NULL != operation )
         {
            operation( images, data );
            GstBuffer* ptr = *buffer = gst_buffer_new_and_alloc ( images->output->imageSize );
            memcpy( ptr->data, images->output->imageData, GST_BUFFER_SIZE( ptr ) );
            
            success = 1;
         }
      }
   }
   return success;
}

int DoneGstCV( unsigned int handle )
{
   int success = -1;
   if ( ( sizeof(AlgorithHandles)/sizeof(ALGORITHM_HANDLE) ) > handle )
   {
      ALGORITHM_HANDLE* h = &AlgorithHandles[handle];
      CV_IMAGES* images = &h->images;
      if ( ( handle == h->handle ) &&
          ( NULL != images->gray_intermediate ) )
      {
         //semaphore
         if ( NULL != images->input )
         {
            cvReleaseImage( &images->input);
         }
         if ( NULL != images->gray )
         {
            cvReleaseImage( &images->gray);
         }
         if ( NULL != images->gray_intermediate )
         {
            cvReleaseImage( &images->gray_intermediate);
         }
         if ( NULL != &images->input_intermediate )
         {
            cvReleaseImage( &images->input_intermediate );
         }
         if ( NULL != images->output )
         {
            cvReleaseImage( &images->output);
         }
      }
   }
   
   return success;
}

