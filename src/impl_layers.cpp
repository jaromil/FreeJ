/*  FreeJ
 *  (c) Copyright 2001-2003 Denis Roio aka jaromil <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 2 of the License,
 * or (at your option) any later version.
 *
 * This source code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * Please refer to the GNU Public License for more details.
 *
 * You should have received a copy of the GNU Public License along with
 * this source code; if not, write to:
 * Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * "$Id$"
 *
 */

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <layer.h>
#include <jutils.h>
#include <config.h>

/* software layers which don't need special loaders */
#include <gen_layer.h>

#ifdef WITH_V4L
#include <v4l_layer.h>
#endif

#ifdef WITH_AVIFILE
#include <avi_layer.h>
#endif

#ifdef WITH_AVCODEC
#include <video_layer.h>
#endif

#ifdef WITH_PNG
#include <png_layer.h>
#endif

#ifdef WITH_FT2
#include <txt_layer.h>
#endif

#ifdef WITH_XHACKS
#include <xhacks_layer.h>
#endif

const char *layers_description =
#ifdef WITH_V4L
" .  - Video4Linux devices as of BTTV cards and webcams\n"
" .    you can specify the size  /dev/video0%160x120\n"
#endif
#ifdef WITH_AVIFILE
" .  - AVI,ASF,WMA,WMV movies as of codecs supported by avifile lib\n"
#endif
#ifdef WITH_AVCODEC
" .  - AVI,ASF,WMA,WMV,MPEG local and remote (http://localhost/file.mpg), dv1394 firewire devices\n"
#endif
#ifdef WITH_PNG
" .  - PNG images (also with transparency)\n"
#endif
#ifdef WITH_FT2
" .  - TXT files rendered with freetype2 library\n"
#endif
#ifdef WITH_XHACKS
" .  - xscreensaver screen hack. ex. /usr/X11R6/lib/xscreensaver/cynosure\n"
#endif
" .  - particle generator ( add layer_gen on commandline)\n"
"\n";


Layer *create_layer(char *file) {
  char *p,*pp;
  FILE *tmp;
  Layer *nlayer = NULL;
  
  /* check that file exists */
  if(strncasecmp(file,"/dev/",5)!=0 
     && strncasecmp(file,"http://",7)!=0
     && strncasecmp(file,"layer_",6)!=0) {
    tmp = fopen(file,"r");
    if(!tmp) {
      error("can't open %s to create a Layer: %s",
	    file,strerror(errno));
      return NULL; 
    } else fclose(tmp);
  }
  /* check file type, add here new layer types */
  p = pp = file;
  p += strlen(file);
//  while(*p!='\0' && *p!='\n') p++; *p='\0';


  /* ==== Video4Linux */
  if(strncasecmp(pp,"/dev/",5)==0 && strncasecmp((pp+5),"ieee1394/",9)!=0) {
#ifdef WITH_V4L
    unsigned int w=320, h=240;
    while(p!=pp) {
      if(*p!='%') p--;
      else { /* size is specified */
	*p='\0'; p++;
	sscanf(p,"%ux%u",&w,&h);
	p = pp; }
    }
    nlayer = new V4lGrabber();
    if(nlayer->open(pp)) {
      ((V4lGrabber*)nlayer)->init_width = w;
      ((V4lGrabber*)nlayer)->init_heigth = h;
    } else {
      error("create_layer : V4L open failed");
      delete nlayer; nlayer = NULL;
    }
#else
    error("Video4Linux layer support not compiled");
    act("can't load %s",pp);
#endif

  } else /* AVI LAYER */
    if( strncasecmp((p-4),".avi",4)==0
	| strncasecmp((p-4),".asf",4)==0
	| strncasecmp((p-4),".asx",4)==0
	| strncasecmp((p-4),".wma",4)==0
	| strncasecmp((p-4),".wmv",4)==0
	| strncasecmp((p-4),".mov",4)==0
	| strncasecmp((p-5),".mpeg",5)==0
	| strncasecmp((p-4),".mpg",4)==0 
        | strncasecmp(pp,"/dev/ieee1394/",14)==0) {
#ifdef WITH_AVCODEC 
      nlayer = new VideoLayer();
      if(!nlayer->open(pp)) {
	error("create_layer : VIDEO open failed");
	delete nlayer; nlayer = NULL;
      }
#elif WITH_AVIFILE 
      if( strncasecmp(pp,"/dev/ieee1394/",14)==0) 
	  nlayer=NULL;
      nlayer = new AviLayer();
      if(!nlayer->open(pp)) {
	error("create_layer : AVI open failed");
	delete nlayer; nlayer = NULL;
      }
#else
      error("VIDEO and AVI layer support not compiled");
      act("can't load %s",pp);
#endif
    } else /* PNG LAYER */
      if(strncasecmp((p-4),".png",4)==0) {
#ifdef WITH_PNG
	nlayer = new PngLayer();
	if(!nlayer->open(pp)) {
	  error("create_layer : PNG open failed");
	  delete nlayer; nlayer = NULL;
	}
#else
	error("PNG layer support not compiled");
	act("can't load %s",pp);
#endif
	
      } else /* TXT LAYER */
	if(strncasecmp((p-4),".txt",4)==0) {
#ifdef WITH_FT2
	  nlayer = new TxtLayer();
	  if(!nlayer->open(pp)) {
	    error("create_layer : TXT open failed");
	    delete nlayer; nlayer = NULL;
	  }
#else
	  error("TXT layer support not compiled");
	  act("can't load %s",pp);
	  return(NULL);
#endif
	} else
	  if(strstr(pp,"xscreensaver")) { /* XHACKS_LAYER */
#ifdef WITH_XHACKS
	    nlayer = new XHacksLayer();
	    if (!nlayer->open(pp)) {
	      error("create_layer : XHACK open failed");
	      delete nlayer; nlayer = NULL;
	    }
#else
	    error("no xhacks layer support");
	    act("can't load %s",pp);
	    return(NULL);
#endif	  
	} else
	  if(strncasecmp(pp,"layer_gen",9)==0) {
	    nlayer = new GenLayer();
	  }
  
  if(!nlayer)
    error("can't create a layer with %s",file);
  else
    func("create_layer succesful, returns %p",nlayer);
  return nlayer;
}