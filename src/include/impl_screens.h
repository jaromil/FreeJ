/*  FreeJ
 *  (c) Copyright 2009 Denis Roio <jaromil@dyne.org>
 *
 * This source code is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Public License as published 
 * by the Free Software Foundation; either version 3 of the License,
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
 */

#ifndef __IMPL_SCREENS_H__
#define __IMPL_SCREENS_H__

#include <config.h>

#include <sdl_screen.h>
#include <soft_screen.h>

#ifdef WITH_OPENGL
#include <sdlgl_screen.h>
#include <gl_screen.h>
#endif

#ifdef WITH_AALIB
#include <aa_screen.h>
#endif


#endif
