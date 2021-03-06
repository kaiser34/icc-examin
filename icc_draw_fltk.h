/*
 * ICC Examin ist eine ICC Profil Betrachter
 * 
 * Copyright (C) 2004  Kai-Uwe Behrmann 
 *
 * Autor: Kai-Uwe Behrmann <ku.b@gmx.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 * -----------------------------------------------------------------------------
 *
 * Zeichenroutinen für Diagramme.
 * 
 */

// Date:      Mai 2004

#ifndef ICC_DRAW_FLTK_H
#define ICC_DRAW_FLTK_H

#include "icc_utils.h"
# define BG FL_BACKGROUND_COLOR //177,177,177  //FL_BACKGROUND2_COLOR
# define VG FL_FOREGROUND_COLOR //225,225,225  //FL_BACKGROUND_COLOR
# define DIAG VG
#if 0
//#ifdef __APPLE__
# define BG FL_DARK3
# define VG FL_DARK2
# define DIAG FL_DARK2-4
//#else
#if 0
# define BG FL_GRAY
# define VG FL_LIGHT2
# define DIAG FL_LIGHT1
#else
# define BG FL_DARK2
# define VG FL_BACKGROUND_COLOR
# define DIAG VG
#endif
//#endif
#endif

#endif //ICC_DRAW_FLTK_H

