//
//  options.h
//  gst_appsrc
//
//  Created by Ting Cao on 1/27/13.
//  Copyright (c) 2013 Ting Cao. All rights reserved.
//

#ifndef __gst_appsrc__options__
#define __gst_appsrc__options__

#include "gst_appsrc.h"

void InitOptions( GstAppParam* app, const char* name );
bool ParseOptions( int argc, char* argv[], GstAppParam* app, const char* name );
void Usage( const char* exec );

#endif /* defined(__gst_appsrc__options__) */
