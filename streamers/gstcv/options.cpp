//
//  options.cpp
//  gst_appsrc
//
//  Created by Ting Cao on 1/27/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "options.h"
/**
 * This utility tells if the passing string contains only ascii '0' - '9'
 */
int IsNumber( char* value )
{
   int is_number = 0;
   char c;
   
   while ( 0 != ( c = *value++ ) )
   {
      is_number = 1;
      if ( ( '0' > c ) || ( '9' < c ) )
      {
         is_number = 0;
         break;
      }
   }
   
   return is_number;
}

void InitOptions( GstAppParam* app, const char* name )
{
   strcpy( app->name, name );
   strcpy( app->input_image, "input.jpg" );
   strcpy( app->host, "127.0.0.1");
   app->port = 5000;
   app->framerate = 25;
   app->width = 320;
   app->height = 240;
   app->bit_per_sample = 8;
   app->iwidth = app->iheight = 0;
   app->fpga = false;
   
   app->flags = 0;
   app->timer_elapsed = 0.0;
}

void Usage( const char* exec )
{
   printf( "Usage: %s\n", exec );
   printf( "\t--input=<input image>, default=input.jpg\n" );
   printf( "\t--host=<streaming destination host ip address>, default=127.0.0.1\n" );
   printf( "\t--port=<streaming destination host ip port>, default=5000\n" );
   printf( "\t--framerate=<streaming frame rate>, default=25\n" );
   printf( "\t--width=<displaying screen width>, default=320\n");
   printf( "\t--height=<displaying screen height>, default=240\n");
   printf( "\t--bpp=<bit per pixel component>, default=8\n" );
   printf( "\t--fpga, display fpga generated images, default=false\n");
   printf( "\twhen host and/or port is specified, it uses rtp\n" );
   printf( "\t--HELP (or -H), this help\n");
}

//https://dev.mobileread.com/svn/iliados/upstream/glib-2.6.6/tests/option-test.c

/**
 * This routine parses user options
 *
 * @return negative - there is problem with input options
 *    0 - continue but displaying usage
 *    positive - continue
 */
bool ParseOptions( int argc, char* argv[], GstAppParam* app, const char* name )
{
   bool success = true;

   InitOptions( app, name );

   char* host = NULL, *input = NULL;
   bool help = false;
   int port = 0;
   int fpga = 0;
   GOptionEntry options[] =
   {
      { "input", 'i', 0, G_OPTION_ARG_FILENAME, &input, "Input Image File", "input.jpg" },
      { "host", 'o', 0, G_OPTION_ARG_STRING, &host, "Streaming Destination IP Name or Address", "127.0.0.1" },
      { "port", 'p', 0, G_OPTION_ARG_INT, &port, "Streaming Destination IP", "5000" },
      { "framerate", 'r', 0, G_OPTION_ARG_INT, &app->framerate, "Frame Rate", "25" },
      { "width", 'x', 0, G_OPTION_ARG_INT, &app->width, "Displaying Screen Width", "320" },
      { "height", 'y', 0, G_OPTION_ARG_INT, &app->height, "Displaying Screen Height", "240" },
      { "bpp", 'b', 0, G_OPTION_ARG_INT, &app->bit_per_sample, "Bit per Pixel Component", "8" },
      { "fpga", 'f', 0, G_OPTION_ARG_NONE, &fpga, "Display FPGA Generated Images", NULL },
      { "HELP", 'H', 0, G_OPTION_ARG_NONE, &help, "Help", NULL },
      { NULL }
   };
   GError * err = NULL;
   GOptionContext* ctx = g_option_context_new( name );
   g_option_context_add_main_entries (ctx, options, NULL );
   g_option_context_add_group (ctx, gst_init_get_option_group ());
   if (!g_option_context_parse (ctx, &argc, &argv, &err))
   {
      g_print ("Error initializing: %s\n", GST_STR_NULL (err->message));
      success = false;
   }

   // input is specified
   if ( NULL != input )
   {
      strncpy( app->input_image, input, sizeof(app->input_image) );
      g_free( input );
   }

   // host is specified
   if ( NULL != host )
   {
      app->flags |= GST_APP_RTP;
      strncpy( app->host, host, sizeof(app->host) );
      g_free( host );
   }

   // port is specified
   if ( 0 != port )
   {
      app->port = port;
      app->flags |= GST_APP_RTP;
   }

   if ( 0 != fpga )
   {
      app->fpga = true;
   }
   
   g_option_context_free (ctx);

   // force to display application help
   if ( help )
   {
      success = false;
   }
   return success;
}

int ParseOptions0( int argc, const char* argv[], GstAppParam* app, const char* name )
{
   int opt = 0, index = 0, v = 0, ret = 0;
   const struct option long_opts[] = {
      { "input", required_argument, NULL, 0 },
      { "host", required_argument, NULL, 0 },
      { "port", required_argument, NULL, 0 },
      { "width", required_argument, NULL, 0 },
      { "height", required_argument, NULL, 0 },
      { "bpp", required_argument, NULL, 0 },
      { "help", no_argument, NULL, 'h' },
      { NULL, no_argument, NULL, 0 }
   };
   
   InitOptions( app, name );
   /*
    int	getopt_long(int, char * const *, const char *,
    const struct option *, int *);
    int	getopt_long_only(int, char * const *, const char *,
    const struct option *, int *);
    */
   while ( -1 != ( opt = getopt_long_only( argc, (char**)argv, "h?", long_opts, &index ) ) )
   {
      ret = 1;
      switch( opt )
      {
         case 0:
         {
            const struct option *o = &long_opts[index];
            if ( o->has_arg )
            {
               char* ptr = optarg;
               // the string options
               if ( !strcmp( o->name, "host" ) )
               {
                  // rtp
                  strncpy( app->host, ptr, sizeof(app->host) );
                  app->flags |= GST_APP_RTP;
               }
               else if ( !strcmp( o->name, "input" ) )
               {
                  strncpy( app->input_image, ptr, sizeof(app->input_image) );
               }
               else
               {
                  // the number options
                  int* iptr = NULL;
                  if ( !strcmp( o->name, "port" ))
                  {
                     // rtp
                     iptr = &app->port;
                     app->flags |= GST_APP_RTP;
                  }

                  if ( ( NULL == iptr ) && ( !strcmp( o->name, "width")) )
                  {
                     iptr = &app->width;
                  }
                  
                  if ( ( NULL == iptr ) && ( !strcmp( o->name, "height")) )
                  {
                     iptr = &app->height;
                  }
                  
                  if ( ( NULL == iptr ) && ( !strcmp( o->name, "bpp")) )
                  {
                     iptr = &app->bit_per_sample;
                  }
                  
                  if ( NULL != iptr )
                  {
                     if ( 0 == ( v = atoi( ptr ) ) )
                     {
                        if ( !IsNumber( ptr ))
                        {
                           fprintf( stderr, "Please specify integer value for %s\n", ptr );
                           return -1;
                        }
                     }
                     
                     *iptr = v;
                  }
                  else
                  {
                     ret = 0;
                     fprintf( stderr, "Unknow option: %s\n", argv[optind] );
                  }
               }
            }
         }
            break;
            
         case 'h':
            ret = 0;
            break;
            
         case '?':
         default:
            ret = 0;
            fprintf( stderr, "Unknow option: %s\n", argv[optind] );
            break;
      }
   }

   return ret;
}
