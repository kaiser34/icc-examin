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
 * Aufbereitung von ICC internen Informationen - werkzeugabhängig
 * 
 */

// Date:      04. 05. 2004

#if 0
  #ifndef DEBUG
   #define DEBUG
  #endif
  #define DEBUG_ICCFUNKT
#endif

#include "icc_utils.h"
#include "icc_helfer_fltk.h"
#include "icc_icc.h"
#include <Fl/Fl.H>

#ifdef HAVE_FLTK
int*
getChannel_flColours (icColorSpaceSignature color)
{
  static int flFarben[16];
  #define nFARBEN(n) for (int i = 0; i < n; i++) \
                       { \
                         flFarben[i] = 9 + i; \
                       }

  switch (color) {
    case icSigXYZData: flFarben[0] = FL_RED;
                       flFarben[1] = FL_GREEN;
                       flFarben[2] = FL_BLUE;
                       break;
    case icSigLabData: flFarben[0] = FL_WHITE;
                       flFarben[1] = FL_RED;
                       flFarben[2] = FL_BLUE;
                       break;
    case icSigLuvData: flFarben[0] = FL_WHITE;
                       flFarben[1] = FL_RED;
                       flFarben[2] = FL_BLUE;
                       break;
    case icSigYCbCrData:flFarben[0] = FL_WHITE;
                       flFarben[1] = FL_BLUE;
                       flFarben[2] = FL_RED;
                       break;
    case icSigYxyData: flFarben[0] = FL_WHITE;
                       flFarben[1] = FL_RED;
                       flFarben[2] = FL_GREEN;
                       break;
    case icSigRgbData: flFarben[0] = FL_RED;
                       flFarben[1] = FL_GREEN;
                       flFarben[2] = FL_BLUE;
                       break;
    case icSigGrayData:flFarben[0] = FL_LIGHT2;
                       break;
    case icSigHsvData: flFarben[0] = FL_RED;
                       flFarben[1] = FL_GREEN;
                       flFarben[2] = FL_WHITE;
                       break;
    case icSigHlsData: flFarben[0] = FL_RED;
                       flFarben[1] = FL_WHITE;
                       flFarben[2] = FL_GREEN;
                       break;
    case icSigCmykData:flFarben[0] = FL_CYAN;
                       flFarben[1] = FL_MAGENTA;
                       flFarben[2] = FL_YELLOW;
                       flFarben[3] = FL_BLACK;
                       break;
    case icSigCmyData: flFarben[0] = FL_CYAN;
                       flFarben[1] = FL_MAGENTA;
                       flFarben[2] = FL_YELLOW;
                       break;
    case icSig2colorData: nFARBEN(2) break;
    case icSig3colorData: nFARBEN(3) break;
    case icSig4colorData: nFARBEN(4) break;
    case icSig5colorData: nFARBEN(5) break;
    case icSig6colorData: nFARBEN(6) break;
    case icSig7colorData: nFARBEN(7) break;
    case icSig8colorData: nFARBEN(8) break;
    case icSig9colorData: nFARBEN(9) break;
    case icSig10colorData: nFARBEN(10) break;
    case icSig11colorData: nFARBEN(11) break;
    case icSig12colorData: nFARBEN(12) break;
    case icSig13colorData: nFARBEN(13) break;
    case icSig14colorData: nFARBEN(14) break;
    case icSig15colorData: nFARBEN(15) break;
    default: flFarben[0] = FL_LIGHT2; break;
  }
  return flFarben;
}

#undef nFARBEN

void
zeigFltkEvents(int event)
{
  switch(event) {
  case FL_NO_EVENT: DBG_PROG_S( "FL_NO_EVENT" ) break;
  case FL_PUSH: DBG_PROG_S( "FL_PUSH" ) break;
  case FL_RELEASE: DBG_PROG_S( "FL_RELEASE" ) break;
  case FL_ENTER: DBG_PROG_S( "FL_ENTER" ) break;
  case FL_LEAVE: DBG_PROG_S( "FL_LEAVE" ) break;
  case FL_DRAG: DBG_PROG_S( "FL_DRAG" ) break;
  case FL_FOCUS: DBG_PROG_S( "FL_FOCUS" ) break;
  case FL_UNFOCUS: DBG_PROG_S( "FL_UNFOCUS" ) break;
  case FL_KEYDOWN: DBG_PROG_S( "FL_KEYDOWN" ) break;
  case FL_KEYUP: DBG_PROG_S( "FL_KEYUP" ) break;
  case FL_CLOSE: DBG_PROG_S( "FL_CLOSE" ) break;
  case FL_MOVE: DBG_PROG_S( "FL_MOVE" ) break;
  case FL_SHORTCUT: DBG_PROG_S( "FL_SHORTCUT" ) break;
  case FL_DEACTIVATE: DBG_PROG_S( "FL_DEACTIVATE" ) break;
  case FL_ACTIVATE: DBG_PROG_S( "FL_ACTIVATE" ) break;
  case FL_HIDE: DBG_PROG_S( "FL_HIDE" ) break;
  case FL_SHOW: DBG_PROG_S( "FL_SHOW" ) break;
  case FL_PASTE: DBG_PROG_S( "FL_PASTE" ) break;
  case FL_SELECTIONCLEAR: DBG_PROG_S( "FL_SELECTIONCLEAR" ) break;
  case FL_MOUSEWHEEL: DBG_PROG_S( "FL_MOUSEWHEEL" ) break;
  case FL_DND_ENTER: DBG_PROG_S( "FL_DND_ENTER" ) break;
  case FL_DND_DRAG: DBG_PROG_S( "FL_DND_DRAG" ) break;
  case FL_DND_LEAVE: DBG_PROG_S( "FL_DND_LEAVE" ) break;
  case FL_DND_RELEASE: DBG_PROG_S( "FL_DND_RELEASE" ) break;
  default: DBG_PROG_S( "event: " << event ) break;
  }
}


#endif
