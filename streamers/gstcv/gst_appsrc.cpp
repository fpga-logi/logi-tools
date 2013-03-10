//
//  gst_appsrc.c
//  gst_appsrc
//
//  Created by Ting Cao on 1/24/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "gst_appsrc.h"
#include "options.h"
#include "gstcv.h"

#include <string>
#include <sstream>
using namespace std;

GST_DEBUG_CATEGORY (appsrc_pipeline_debug);
#define GST_CAT_DEFAULT appsrc_pipeline_debug

#define GstAppName "gst_image_src"

GstAppParam GstApp;
typedef enum
{
   APP_PARAMETER_INT = 0,
   APP_PARAMETER_STRING,
}
APP_PARAMETER_TYPE;
typedef struct
{
   const char* name;
   void* value;
   APP_PARAMETER_TYPE type;
}
APP_PARAMETER;

APP_PARAMETER AppPipeLineTobeReplacedParameters[] = {
   {
      "$app_name",
      GstApp.name,
      APP_PARAMETER_STRING
   },
   {
      "$app_width",
      &GstApp.width,
      APP_PARAMETER_INT
   },
   {
      "$app_height",
      &GstApp.height,
      APP_PARAMETER_INT
   },
   {
      "$app_host",
      GstApp.host,
      APP_PARAMETER_STRING
   },
   {
      "$app_port",
      &GstApp.port,
      APP_PARAMETER_INT
   },
};

const char* AppPipeLineParameters[] = {
   "appsrc name=$app_name is-live=true do-timestamp=true ",
   "! video/x-raw-rgb,width=$app_width,height=$app_height ",
   "! ffmpegcolorspace ",
   "! videoscale method=1 ",
   "! ximagesink",
};

const char* RtpPipeLineParameters[] = {
   "appsrc name=$app_name is-live=true do-timestamp=true ",
//   "! video/x-raw-rgb,width=$app_width,height=$app_height ",
   "! ffmpegcolorspace ",
   "! videoscale method=1 ",
   "! theoraenc bitrate=150 ",
   "! udpsink host=$app_host port=$app_port",
};

char AppPipeLine[1024] = { 0 };

int GstCVHandle = -1;

void DisplayCaps( GstCaps *caps )
{
   unsigned int s_count = gst_caps_get_size( caps );
   for( unsigned int i=0; i<s_count; i++ )
   {
      const GstStructure *st = gst_caps_get_structure (caps, i );
      char* str = gst_structure_to_string ( st );
      printf( "Caps used %d:\n%s\n", i, str );
   }
}
gboolean AppBusWatcher( GstBus* bus, GstMessage* message, GstAppParam* app )
{
   GST_DEBUG ("received message: %s\n",
              gst_message_type_get_name (GST_MESSAGE_TYPE (message)));

   switch( GST_MESSAGE_TYPE( message ))
   {
      case GST_MESSAGE_ERROR:
      {
         GError* error = NULL;
         gchar* info = NULL;
         gst_message_parse_error(message,
                                 &error,
                                 &info );
         g_printerr( "%s: %s\n",
                    GST_OBJECT_NAME(message->src),
                    error->message );
         if ( NULL != info )
         {
            g_printerr( "\tdebug: %s\n", info );
         }
         g_error_free( error );
         g_free( info );
         //g_main_loop_quit( app->loop );
      }
         break;
         
      case GST_MESSAGE_EOS:
         g_main_loop_quit( app->loop );
         break;
         
      default:
         break;
   }

   return TRUE;
}

#define MIN_THRESHOLD (10.0)
#define MAX_THRESHOLD (100.0)
GSTCV_DATA GstCV_TestData [] = {
   // original input
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },
   { 0.0, 0.0, 0.0, GSTCV_OPERATION_INPUT },

   // canny
   { 10.0, 100.0, 3.0, GSTCV_OPERATION_CANNY },
   { 15.0, 95.0, 3.0, GSTCV_OPERATION_CANNY },
   { 20.0, 90.0, 3.0, GSTCV_OPERATION_CANNY },
   { 25.0, 85.0, 3.0, GSTCV_OPERATION_CANNY },
   { 30.0, 80.0, 3.0, GSTCV_OPERATION_CANNY },
   { 35.0, 75.0, 3.0, GSTCV_OPERATION_CANNY },
   { 40.0, 70.0, 3.0, GSTCV_OPERATION_CANNY },
   { 45.0, 65.0, 3.0, GSTCV_OPERATION_CANNY },
   { 50.0, 60.0, 3.0, GSTCV_OPERATION_CANNY },
   { 55.0, 55.0, 3.0, GSTCV_OPERATION_CANNY },

   // canny inverse
   { 10.0, 100.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 15.0, 95.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 20.0, 90.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 25.0, 85.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 30.0, 80.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 35.0, 75.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 40.0, 70.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 45.0, 65.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 50.0, 60.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },
   { 55.0, 55.0, 3.0, static_cast<unsigned int>(GSTCV_OPERATION_CANNY|GSTCV_OPERATION_INVERSE) },

   // inverse input
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
   { 0.0, 0.0, 0.0, static_cast<unsigned int>(GSTCV_OPERATION_INPUT|GSTCV_OPERATION_INVERSE) },
};

gboolean GeneratePicture( GstAppParam * app )
{
   static unsigned int threshold_index = 0;
   
   gboolean status = TRUE;
   gdouble ms = g_timer_elapsed(app->timer, NULL);
   app->timer_elapsed = ms - app->timer_elapsed;
   if ( app->timer_elapsed > 1.0/app->framerate )
   {
      GstBuffer* buffer = 0;
      const GSTCV_DATA* data = &GstCV_TestData[threshold_index++];
      if ( 0 <= PerformGstCV( GstCVHandle, data, &buffer ) )
      {
         GstFlowReturn ret;
         GST_DEBUG ("feed buffer");
         GST_BUFFER_TIMESTAMP( buffer ) = (GstClockTime)(ms*1e6) ;
         GST_BUFFER_TIMESTAMP( buffer ) = GST_CLOCK_TIME_NONE;
         GST_BUFFER_DURATION( buffer ) = GST_CLOCK_TIME_NONE;
         GST_BUFFER_OFFSET( buffer ) = GST_BUFFER_OFFSET_NONE;
         GST_BUFFER_OFFSET_END( buffer ) = GST_BUFFER_OFFSET_NONE;
         
         g_signal_emit_by_name( app->appsrc, "push-buffer", buffer, &ret );
         gst_buffer_unref (buffer);
         
         if (ret != GST_FLOW_OK)
         {
            /* some error, stop sending data */
            GST_DEBUG ("flow error");
            status = FALSE;
         }
         
        // adjust thresholds data for next run
         if ( (sizeof(GstCV_TestData)/sizeof(GSTCV_DATA) ) <= threshold_index )
         {
            threshold_index = 0;
         }
      }
   }
   return status;
}

void StartGeneratePicture( GstElement * pipeline, guint size, GstAppParam *app )
{
   if ( 0 == app->sourceid )
   {
      GST_DEBUG ("generate picture");
      app->sourceid = g_idle_add ((GSourceFunc)GeneratePicture, app);
   }
}

void StopGeneratePicture( GstElement * pipeline, GstAppParam *app  )
{
   if (0 != app->sourceid ) {
      GST_DEBUG ("stop generate");
      g_source_remove (app->sourceid);
      app->sourceid = 0;
   }
}

char* BuildPipelineContent( const GstAppParam* app )
{
   // build pipeline content string
   string content;
   int parameter_count = ( app->flags & GST_APP_RTP ) ? sizeof(RtpPipeLineParameters) : sizeof(AppPipeLineParameters);
   parameter_count /= sizeof(char*);
   char** parameters = (char**)(( app->flags & GST_APP_RTP ) ? RtpPipeLineParameters : AppPipeLineParameters);
   for( int i=0; i<parameter_count; i++ )
   {
      content += parameters[i];
   }
   
   // replace parameters with user specified ones
   for( unsigned int i=0; i<sizeof(AppPipeLineTobeReplacedParameters)/sizeof(APP_PARAMETER); i++ )
   {
      string value;
      ostringstream os;
      APP_PARAMETER* parameter = &AppPipeLineTobeReplacedParameters[i];
      string tobe_replaced = parameter->name;
      if ( APP_PARAMETER_INT == parameter->type )
      {
         os <<*((int*)parameter->value);
         value = os.str();
      }
      else
      {
         value = (char*)parameter->value;
      }
      size_t pos = content.find( tobe_replaced );
      if ( string::npos != pos )
      {
         content.replace( pos, tobe_replaced.length(), value );
      }
   }
   strcpy( AppPipeLine, content.c_str());
   return AppPipeLine;
}

int main( int argc, char* argv[] )
{
   GstAppParam* app = &GstApp;
   GstBus *bus;
   GstCaps *caps;
   
   if ( !ParseOptions( argc, argv, app, GstAppName ) )
   {
      Usage( argv[0] );
      exit( -1 );
   }

   if ( 0 > ( GstCVHandle = InitGstCV(app->input_image, app->iwidth, app->iheight ) ) )
   {
      fprintf( stderr, "Can't find input image: >%s<\n", app->input_image );
      Usage( argv[0] );
      exit( -2 );
   }
   
   printf( "Options used:\n" );
   printf( "\tinput image=%s\n", app->input_image );
   if ( 0 != ( app->flags & GST_APP_RTP ) )
   {
      printf( "\thost=%s\n", app->host );
      printf( "\tport=%d\n", app->port );
   }
   printf( "\tframerate=%d\n", app->framerate );
   printf( "\timage width=%d\n", app->iwidth );
   printf( "\timage height=%d\n", app->iheight );
   if ( ( app->width != app->iwidth ) ||
       ( app->height != app->iheight ) )
   {
      printf( "\twindow size (%d,%d) is changed to image size for the current implementation\n",
             app->width,
             app->height );
      app->width = app->iwidth;
      app->height = app->iheight;
   }
   
   GST_DEBUG_CATEGORY_INIT (appsrc_pipeline_debug,
                            "appsrc-pipeline",
                            0,
                            GstAppName );

   app->loop = g_main_loop_new (NULL, TRUE);
   app->timer = g_timer_new();
   
   app->pipeline = gst_parse_launch( BuildPipelineContent( app ), NULL );
   g_assert( app->pipeline );
   
   bus = gst_pipeline_get_bus( GST_PIPELINE(app->pipeline));
   g_assert( bus );
   
   gst_bus_add_watch( bus, (GstBusFunc)AppBusWatcher, app );

   app->appsrc = gst_bin_get_by_name(GST_BIN( app->pipeline ), GstAppName);
   g_assert( app->appsrc );
   g_assert( GST_IS_APP_SRC( app->appsrc) );
   
   g_signal_connect( app->appsrc, "need-data", G_CALLBACK(StartGeneratePicture), app );
   g_signal_connect( app->appsrc, "enough-data", G_CALLBACK(StopGeneratePicture), app );

   caps = gst_caps_new_simple ("video/x-raw-rgb",
                               "width", G_TYPE_INT, app->width,
                               "height", G_TYPE_INT, app->height,
                               "bpp", G_TYPE_INT, 24,
                               "depth", G_TYPE_INT, 24,
                               "red_mask",   G_TYPE_INT, 0x00ff0000,
                               "green_mask", G_TYPE_INT, 0x0000ff00,
                               "blue_mask",  G_TYPE_INT, 0x000000ff,
                               "framerate", GST_TYPE_FRACTION, app->framerate, 1,
                               "endianness", G_TYPE_INT, G_BIG_ENDIAN/*G_LITTLE_ENDIAN*/,
                               NULL);
   
   DisplayCaps( caps );
   gst_app_src_set_caps(GST_APP_SRC(app->appsrc), caps);

   /* go to playing and wait in a mainloop. */
   gst_element_set_state (app->pipeline, GST_STATE_PLAYING);
   
   /* this mainloop is stopped when we receive an error or EOS */
   g_main_loop_run (app->loop);
   
   GST_DEBUG ("stopping");
   
   gst_element_set_state (app->pipeline, GST_STATE_NULL);
   
   gst_object_unref (bus);
   g_main_loop_unref (app->loop);

   DoneGstCV( GstCVHandle );
   
   return 0;
}