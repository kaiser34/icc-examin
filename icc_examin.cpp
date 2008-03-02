/*
 * ICC Examin ist eine ICC Profil Betrachter
 * 
 * Copyright (C) 2004-2005  Kai-Uwe Behrmann 
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
 * Die zentrale Klasse.
 * 
 */


#include "icc_utils.h"
#include "icc_betrachter.h"
#include "icc_draw.h"
#include "icc_examin.h"
#include "icc_examin_io.h"
#include "icc_gl.h"
#include "icc_helfer.h"
#include "icc_helfer_ui.h"
#include "icc_draw_fltk.h"
#include "icc_helfer_fltk.h"
#include "icc_fenster.h"
#include "icc_info.h"
#include "icc_kette.h"
#include "icc_waehler.h"
#include "fl_i18n/fl_i18n.H"

#if APPLE
#include <Carbon/Carbon.h>
#endif

#include <FL/x.H>

using namespace icc_examin_ns;

#if USE_THREADS
#include "threads.h"
#else
#endif

//#define DEBUG_EXAMIN
#ifdef DEBUG_EXAMIN
#define DBG_EXAMIN_START DBG_PROG_START
#define DBG_EXAMIN_ENDE DBG_PROG_ENDE
#define DBG_EXAMIN_V( texte ) DBG_NUM_V( texte )
#define DBG_EXAMIN_S( texte ) DBG_NUM_S( texte )
#else
#define DBG_EXAMIN_START
#define DBG_EXAMIN_ENDE
#define DBG_EXAMIN_V( texte )
#define DBG_EXAMIN_S( texte )
#endif

ICCexamin *icc_examin;


ICCexamin::ICCexamin ()
{ DBG_PROG_START
  icc_examin_ns::lock(__FILE__,__LINE__);

  //Fl::scheme("plastic");

  icc_betrachter = new ICCfltkBetrachter;
  io_ = new ICCexaminIO;
  profile.init();

  _item = -1;
  _mft_item = -1;
  for(int i = 0; i < 4; ++i)
    statlabel[i] = "";
  status_ = false;
  intent_ = 3;
  intent_selection_ = 0;
  farbraum_modus_ = 0;
  bpc_ = 0;
  intent_alt_ = intent_;
  bpc_alt_ = bpc_;
  gamutwarn_ = 0;
  vcgt_cb_laeuft_b_ = 0;
  DBG_PROG_ENDE
}

void
ICCexamin::quit ()
{ DBG_PROG_START
  status_ = false;
  delete icc_betrachter;
  delete io_;
  DBG_PROG_ENDE
  exit(0);
}

void
ICCexamin::clear ()
{ DBG_PROG_START

  std::vector<ICCnetz> d_n;
  profile.clear();
  icc_betrachter->DD_farbraum->hineinNetze (d_n);
  DBG

  DBG_PROG_ENDE
}


void
resize_fuer_menubar(Fl_Widget* w)
{
# if APPLE
  w->resize( w->x(), w->y()-25, w->w(), w->h()+25 );
# endif
}

# if HAVE_X
# include "icc_examin.xpm" // icc_examin_xpm
# endif

void oeffnen_cb(const char* filenames);

void
ICCexamin::start (int argc, char** argv)
{ DBG_PROG_START

  kurven.resize(MAX_VIEWER);
  punkte.resize(MAX_VIEWER);
  texte.resize(MAX_VIEWER);
  kurve_umkehren.resize(MAX_VIEWER);

  fl_translate_menue( icc_betrachter->menu_menueleiste );
  fl_translate_menue( icc_betrachter->menu_DD_menueleiste );

  icc_betrachter->init( argc, argv );

  icc_betrachter->mft_gl->init(1);
  icc_betrachter->DD_farbraum->init(2);
  icc_waehler_ = new  ICCwaehler(485, 110, _("Gamut selector"));
  icc_waehler_->resize(icc_betrachter->details->x(),
                       icc_betrachter->details->y() - icc_waehler_->h(),
                       icc_waehler_->w()+20, icc_waehler_->h());
  if(!icc_waehler_) WARN_S( "icc_waehler_ nicht reservierbar" )
  icc_betrachter->DD_farbraum->begin();
    icc_waehler_->set_non_modal(); // gehoert zum "details" Hauptfenster
  icc_betrachter->DD_farbraum->end();
  icc_waehler_->hide();

  // Die TagViewers registrieren und ihre Variablen initialisieren
  icc_betrachter->tag_viewer->id = TAG_VIEWER;
  icc_betrachter->mft_viewer->id = MFT_VIEWER;
  icc_betrachter->vcgt_viewer->id = VCGT_VIEWER;

  // Fuer eine Fl_Sys_Menu_Bar
# if 0
  resize_fuer_menubar( icc_betrachter->DD_farbraum );
  resize_fuer_menubar( icc_betrachter->examin );
  resize_fuer_menubar( icc_betrachter->inspekt_html );
# endif
  DBG_PROG

  // Oberflaechenpflege
# if HAVE_X || APPLE
  icc_betrachter->menueintrag_vcgt->show();
#   if APPLE
    icc_betrachter->vcgt_set_button->deactivate();
    icc_betrachter->vcgt_reset_button->deactivate();
#   endif
# if HAVE_X
  setzeIcon( icc_betrachter->details, icc_examin_xpm );
# endif
  DBG_PROG_S( "Zeige vcgt" )
# else
  DBG_PROG_S( "Zeige vcgt nicht" )
# endif

# if APPLE
  // osX Resourcen
  IBNibRef nibRef;
  OSStatus err;
  err = CreateNibReference(CFSTR("main"), &nibRef);
  require_noerr( err, CantGetNibRef );
  // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
  // object. This name is set in InterfaceBuilder when the nib is created.
  err = SetMenuBarFromNib(nibRef, CFSTR("MenuBar"));
  require_noerr( err, CantSetMenuBar );
  // We don't need the nib reference anymore.
  DisposeNibReference(nibRef);
  CantSetMenuBar:
  CantGetNibRef:

  // osX DnD behandeln
  fl_open_callback( oeffnen_cb );

# endif // APPLE

  if(!icc_debug)
  {
    icc_betrachter->menueintrag_testkurven->hide();
    icc_betrachter->menueintrag_lang->hide();
  }

  DBG_PROG

  modellDazu( /*ICCkette*/&profile ); // wird in nachricht ausgewertet

  Fl::add_handler(tastatur);

  // Behandle Kommandozeilenargumente
      if (argc>1) {
        std::vector<std::string>profilnamen;
        for (int i = 1; i < argc; i++) {
          DBG_PROG_S( i <<" "<< argv[i] )
          // keine process serial number in osX
          if(std::string(argv[i]).find("-psn_") == std::string::npos)
          {
            if(i == 1)
              status( argv[1] << " " << _("loaded") )
            profilnamen.push_back( argv[i] );
          }
        }
        oeffnen (profilnamen);
      } else {
        status(_("Ready"))
      }


  // zur Benutzung freigeben
  status_ = 1;
  frei(true);

  // receive events
# if 0
  Fl_Widget* w = dynamic_cast<Fl_Widget*>(icc_betrachter->details);
  if (w) {
      Fl::pushed(w);
      DBG_PROG_S( "pushed("<< w <<") "<< Fl::pushed() )
  }
# endif

# if USE_THREADS
  static Fl_Thread fl_t;
  DBG_THREAD_V( fl_t )
  int fehler = fl_create_thread( fl_t, &ICCexaminIO::oeffnenStatisch_, (void *)this );
# if HAVE_PTHREAD_H
  icc_thread_liste[THREAD_LADEN] = fl_t;
# ifdef CWDEBUG
  Debug(myproject::debug::init_thread());
# endif
# endif
  if( fehler == EAGAIN)
  {
    WARN_S( "Waechter Thread nicht gestartet Fehler: "  << fehler );
  } else
# if !APPLE && !WIN32 && PTHREAD_THREADS_MAX
  if( fehler == PTHREAD_THREADS_MAX )
  {
    WARN_S( "zu viele Waechter Threads Fehler: " << fehler );
  } else
# endif
  if( fehler != 0 )
  {
    WARN_S( "unbekannter Fehler beim Start eines Waechter Threads Fehler: " << fehler );
  }
# else
  Fl::add_timeout( 0.01, /*(void(*)(void*))*/ICCexaminIO::oeffnenStatisch_ ,(void*)this);
# endif

  icc_betrachter->run();

  DBG_PROG_ENDE
}

void
ICCexamin::zeig3D ()
{ DBG_PROG_START

  Fl_Double_Window *w = icc_betrachter->DD;
  Fl_Widget *wid = icc_betrachter->DD_farbraum;

  int lx = wid->x(),
      ly = wid->y(),
      lw = wid->w(),
      lh = wid->h();

  const char* title = icc_betrachter->details->label();
  char* t = (char*) malloc(strlen(title)+128);

  sprintf(t, "%s - %s", title, _("Gamut View"));

  if(!w)
  {
  w =
    new Fl_Double_Window( lx+lw, ly, lw, lh, t );
    w->user_data((void*)(0));
      Fl_Group *g = new Fl_Group(0,0,lw,lh);
      g->end();
      //wid->resize(0,0, lw,lh);
      g->add(wid);
    w->end();
    w->resizable(g);
  }
  w->label(t);
  w->show();
  wid->show();

  if(!icc_waehler_->visible())
    icc_waehler_->show();

  DBG_PROG_ENDE
}

void
ICCexamin::zeigPrueftabelle ()
{ DBG_PROG_START

  Fl_Double_Window *details = icc_betrachter->details;
  Fl_Help_View *inspekt_html = icc_betrachter->inspekt_html;

  { bool export_html = false;
    icc_betrachter->tag_text->inspekt_topline = inspekt_html->topline();
    inspekt_html->value( profile.profil()->report(export_html).c_str());
    inspekt_html->topline( icc_betrachter->tag_text->inspekt_topline);
  }

  Fl_Widget *wid = inspekt_html;
  if(wid->window() != details &&
     wid->window()->visible())
    return;

  int lx = details->x(),
      ly = details->y(),
      lw = details->w(),
      lh = details->h();

  const char* title = details->label();
  char* t = (char*) malloc(strlen(title)+128);

  sprintf(t, "%s - %s", title, _("Compare Measurement <-> Profile Colours"));

  Fl_Double_Window *w =
    new Fl_Double_Window( lx+lw, ly, lw, lh, t );
    w->user_data((void*)(0));
      Fl_Group *g = new Fl_Group(0,0,lw,lh);
      g->end();
      wid->resize(0,0, lw,lh);
      g->add( wid );
      wid->show();
    w->end();
    w->set_non_modal();
    w->xclass("Fl_Window");
    w->resizable(w);
    //w->resizable(g);
    w->show();

  DBG_PROG_ENDE
}

void
ICCexamin::zeigCGATS()
{
  DBG_PROG_START
  icc_examin_ns::lock(__FILE__,__LINE__);
  // CGATS in Fenster praesentieren
  icc_examin_ns::nachricht(profile.profil()->cgats_max());
  icc_examin_ns::unlock(this, __FILE__,__LINE__);
  DBG_PROG_ENDE
}

void
ICCexamin::zeigMftTabellen (int lx, int ly, int lw, int lh)
{ DBG_PROG_START
  const char* title = icc_betrachter->mft_gl->window()->label();
  char* t = (char*) malloc(strlen(title)+20);
  int   item = tag_nr();
  std::vector<std::string> tag_info =
       profile.profil()->printTagInfo(item);

  sprintf(t, "%d:%s - %s", item + 1,
          tag_info[0].c_str(),
          title);

  std::vector<std::string> out_names =
      profile.profil()->getTagChannelNames (icc_betrachter->tag_nummer,
                                            ICCtag::TABLE_OUT);
  Fl_Double_Window *w = NULL;
  for(int i = 0; i < (int)out_names.size(); ++i)
  {
    if(!w)
      w = new Fl_Double_Window( lx, ly, lw, lh, t);
    else
      w = new Fl_Double_Window( w->x()+lw, w->y(), lw, lh, t);

      w->user_data((void*)(0));
      Fl_Group *g = new Fl_Group(0,0,lw,lh);
        GL_Ansicht *gl = 
          new GL_Ansicht (*icc_betrachter->mft_gl); //(0,0,lw,lh);

        GL_Ansicht::getAgv(gl, icc_betrachter->mft_gl);
        gl->init( icc_betrachter->mft_gl->id() );
        gl->kanal = i;
      g->end();
    w->end();
    w->set_non_modal();
    w->xclass("Fl_Window");
    w->resizable(w);
    w->show();
  }
  DBG_PROG_ENDE
}

/** virtual aus icc_examin_ns::Beobachter:: */
void
ICCexamin::nachricht( Modell* modell , int info )
{
  DBG_PROG_START
  if(!frei()) {
    WARN_S("icc_examin ist nicht frei")
    //DBG_PROG_ENDE
    //return;
  }

  DBG_THREAD_V( info )
  // Modell identifizieren
  ICCkette* k = dynamic_cast<ICCkette*>(modell);
  if(k && (k->size() > info))
  {
    DBG_PROG_S( "Nachricht von ICCkette" )
    DBG_PROG_S( "Auffrischen von Profil Nr.: " << info )
    if(info>=0)
    {
      DBG_PROG_V( (int*)(*k)[info] )
      if ((*k)[info])
      if((*k)[info]->changing()) {
        DBG_PROG_S( "veraendert sich gerade: " << info )
        //icc_examin_ns::sleep( 0.1 );
      }
      DBG_PROG_S( "lade: " << info )

      { DBG_PROG
        {
          int interactive = 0;
          intentGet(&interactive);
          if(farbraumModus() && !interactive)
            intent( -1 );

          // ncl2 ?
          DBG_PROG_V( profile.aktuell() );
          if((info == 0 && farbraumModus()) &&
             intentGet(NULL) != intent_alt_)
            farbraum();
          else
            farbraum (info);
          intent_alt_ = intentGet(NULL);
          icc_examin->fortschrittThreaded(0.5);
        }

        if(k->aktiv(info)) // momentan nicht genutzt
        { if (info < (int)icc_betrachter->DD_farbraum->dreiecks_netze.size())
            icc_betrachter->DD_farbraum->dreiecks_netze[info].aktiv = true;

        } else if (info < (int)icc_betrachter->DD_farbraum->dreiecks_netze.size()) {
          icc_betrachter->DD_farbraum->dreiecks_netze[info].aktiv = false;
        }

          // Oberflaechenpflege - Aktualisieren
        icc_examin->fortschrittThreaded(0.6);
        if(profile[info]->tagCount() <= _item)
          _item = (-1);
        DBG_PROG_V( _item )
 
        if(icc_betrachter->DD_farbraum->visible())
        {
          icc_betrachter->DD_farbraum->damage(FL_DAMAGE_ALL);
        }
        icc_examin->fortschrittThreaded(0.7);
        if(icc_betrachter->menueintrag_inspekt->active() &&
           profile[info]->hasMeasurement() )
          setzMesswerte();
        else if(icc_betrachter->examin->visible())
          waehleTag(_item);
        icc_examin->fortschrittThreaded(0.9);
      }
    }
  }

  Beobachter::nachricht(modell, info);
  icc_examin->fortschrittThreaded(1.0);
  icc_examin->fortschrittThreaded(1.1);
  DBG_PROG_ENDE
}

void
ICCexamin::setzMesswerte()
{
  DBG_PROG_START
  bool export_html = false;
  if(icc_betrachter->menueintrag_inspekt->active()) {
    icc_examin_ns::lock(__FILE__,__LINE__);
    icc_betrachter->inspekt_html->value(profile.profil()->report(export_html).c_str());
    icc_betrachter->inspekt_html->topline(icc_betrachter->tag_text->inspekt_topline);
    icc_examin_ns::unlock(this, __FILE__,__LINE__);
  }
  DBG_PROG_ENDE
}


void
ICCexamin::testZeigen ()
{ DBG_PROG_START

# if HAVE_X || APPLE
  std::vector<std::vector<std::pair<double,double> > > kurven2;
  kurven2.resize(8);
  kurven2[0].resize(4);
  kurven2[1].resize(3);
  kurven2[2].resize(56);
  kurven2[3].resize(56);
  kurven2[4].resize(56);
  kurven2[5].resize(56);
  kurven2[6].resize(56);
  kurven2[7].resize(56);
  for(unsigned int i = 0; i < kurven2.size(); ++i)
    for(unsigned int j = 0; j < kurven2[i].size(); ++j) {
      kurven2[i][j].first = sin(i) * 3.2 - 0.5* (cos(j*2)+0.1);
      kurven2[i][j].second = i * -0.2 + 0.05 * (sin(j/10.0)+2.7);
    }
  std::vector<std::string> txt;
  txt.resize(8);
  txt[0] = "ein Bild";
  txt[1] = "Gemaelde";
  txt[2] = "fast HDR";
  txt[3] = "2 fast HDR";
  txt[4] = "3 fast HDR";
  txt[5] = "4 fast HDR";
  txt[6] = "5 fast HDR";
  txt[7] = "6 fast HDR";
  icc_betrachter->vcgt_viewer->hide();
  icc_betrachter->vcgt_viewer->show();
  icc_betrachter->vcgt_viewer->hineinDaten ( kurven2, txt );
  icc_betrachter->vcgt_viewer->kurve_umkehren = true;
# endif

  // TODO: osX
  DBG_PROG_ENDE
}

void
beobachte_vcgt(void *ICCexamina)
{
  ICCexamin *ie = (ICCexamin*) ICCexamina;

  ie->vcgtZeigen();

  if (ie->icc_betrachter->vcgt->visible())
    Fl::add_timeout( 0.33, (void(*)(void*))beobachte_vcgt ,(void*)ie);
  else
    ie->vcgtStoppen();
}  

void
ICCexamin::vcgtStoppen ()
{ DBG_PROG_START
  vcgt_cb_laeuft_b_ = false;
  icc_betrachter->vcgt->hide();
  DBG_PROG_ENDE
}

void
ICCexamin::vcgtZeigen ()
{ DBG_PROG_START
  kurve_umkehren[VCGT_VIEWER] = true;

  if (!vcgt_cb_laeuft_b_) {
    vcgt_cb_laeuft_b_ = true;
    beobachte_vcgt( (void*)this );
  }

  frei(false);
# if HAVE_X || APPLE
  std::string display_name = "";
  int x = icc_betrachter->vcgt->x() + icc_betrachter->vcgt->w()/2;
  int y = icc_betrachter->vcgt->y() + icc_betrachter->vcgt->h()/2;
  kurven[VCGT_VIEWER] = leseGrafikKartenGamma (display_name,texte[VCGT_VIEWER], x,y);
  if (kurven[VCGT_VIEWER].size()) {
    icc_betrachter->vcgt_viewer->hide();
    icc_betrachter->vcgt_viewer->show();
    icc_betrachter->vcgt_viewer->hineinKurven( kurven[VCGT_VIEWER],
                                               texte [VCGT_VIEWER] );
    icc_betrachter->vcgt_viewer->kurve_umkehren = true;
  } else {
    std::vector<std::vector<double> > leer;
    icc_betrachter->vcgt_viewer->hide();
    icc_betrachter->vcgt_viewer->show();
    leer.resize(3);
    icc_betrachter->vcgt_viewer->hineinKurven( leer,
                                               texte [VCGT_VIEWER] );
    icc_betrachter->vcgt_viewer->kurve_umkehren = true;
  }
# endif

  frei(true);
  // TODO: osX
  DBG_PROG_ENDE
}

bool
ICCexamin::berichtSpeichern (void)
{ return io_->berichtSpeichern();
}
bool
ICCexamin::gamutSpeichern (icc_examin_ns::IccGamutFormat format)
{ return io_->gamutSpeichern(format);
}
void
ICCexamin::oeffnen ()
{ io_->oeffnen();
}
void
ICCexamin::oeffnen (std::vector<std::string> dateinamen)
{ io_->oeffnen( dateinamen );
}
bool
ICCexamin::lade ()
{ return io_->lade();
}
void
ICCexamin::lade (std::vector<Speicher> & neu)
{ io_->lade(neu);
}
int
ICCexamin::erneuern()
{ return io_->erneuern();
}
void
ICCexamin::erneuern(int pos)
{ io_->erneuern(pos);
}

void
ICCexamin::moniHolen ()
{ DBG_PROG_START
  //frei(false);
  fortschritt( 0.01 );
  int x = icc_betrachter->vcgt->x() + icc_betrachter->vcgt->w()/2;
  int y = icc_betrachter->vcgt->y() + icc_betrachter->vcgt->h()/2;

  static std::vector<Speicher> ss;
  ss.clear();
  ss.push_back(icc_oyranos.moni(x,y));
  size_t size = ss[0].size();
  const char *moni_profil = ss[0];
  if(!moni_profil || !size) {
    //frei(true);
    DBG_PROG_ENDE
    return;
  }

  int erfolg = false;
  while(!erfolg) {
    if(!lade()) {
      lade(ss);
      erfolg = true;
    } else {
      // kurze Pause 
      DBG_THREAD_S( "muss warten" )
      icc_examin_ns::sleep(0.05);
    }
  }

  // TODO: X notification event
# if 0
  saveMemToFile("/tmp/vcgt_temp.icc", moni_profil, size);
  system ("xcalib /tmp/vcgt_temp.icc");
  remove ("/tmp/vcgt_temp.icc");
# endif
  vcgtZeigen();

  fortschritt( 1.1 );
  DBG_PROG_ENDE
}

std::string
ICCexamin::moniName ()
{ DBG_PROG_START
  //frei(false);
  int x = icc_betrachter->vcgt->x() + icc_betrachter->vcgt->w()/2;
  int y = icc_betrachter->vcgt->y() + icc_betrachter->vcgt->h()/2;

  DBG_PROG_ENDE
  return icc_oyranos.moni_name( x,y );
}

void
ICCexamin::moniSetzen ()
{ DBG_PROG_START
  frei(false);
  int x = icc_betrachter->vcgt->x() + icc_betrachter->vcgt->w()/2;
  int y = icc_betrachter->vcgt->y() + icc_betrachter->vcgt->h()/2;

  if( profile.size() && profile.profil()->filename() &&
      strlen( profile.profil()->filename() ) ) { DBG_PROG
    icc_oyranos.setzeMonitorProfil( profile.profil()->filename(), x,y );
    frei(true);
    vcgtZeigen();
    frei(false);
  }
  frei(true);
  DBG_PROG_ENDE
}

void
ICCexamin::standardGamma ()
{ DBG_PROG_START
  frei(false);
# if HAVE_X
  int x = icc_betrachter->vcgt->x() + icc_betrachter->vcgt->w()/2;
  int y = icc_betrachter->vcgt->y() + icc_betrachter->vcgt->h()/2;

  frei(true);
  vcgtZeigen();
  frei(false);
  icc_oyranos.setzeMonitorProfil( 0, x,y );
# endif

  // TODO: osX
  frei(true);
  DBG_PROG_ENDE
}

void
ICCexamin::gamutAnsichtZeigen ()
{
      icc_examin_ns::lock(__FILE__,__LINE__);
      icc_betrachter->menueintrag_3D->set();
      icc_betrachter->menueintrag_huelle->set();
      icc_betrachter->widget_oben = ICCfltkBetrachter::WID_3D;
      farbraum_angezeigt_ = true;
      icc_betrachterNeuzeichnen(icc_betrachter->DD_farbraum);

      if(icc_betrachter->DD_farbraum->window() != icc_betrachter->details)
        icc_betrachter->details->hide();

      icc_examin_ns::unlock(this, __FILE__,__LINE__);
      DBG_PROG_S("icc_betrachterNeuzeichnen DD_farbraum")
}

void
ICCexamin::intent( int intent_neu )
{
  int intent_alt = intent_;
  if(intent_neu < 0)
  {
    if(farbraumModus())
      intent_ = profile.profil()->intent();
    else
      intent_ = 3;
      intent_selection_ = 0;
  } else {
    intent_ = intent_neu;
    intent_selection_ = 1;
  }

  if(intent_alt != intent_neu) {
    std::vector<std::string> profilnamen = profile;
    oeffnen( profilnamen );
  }
}

void
ICCexamin::bpc( int bpc_neu )
{
  if(bpc_ != bpc_neu) {
    std::vector<std::string> profilnamen = profile;
    oeffnen( profilnamen );
  }
  bpc_ = bpc_neu;
}

void
ICCexamin::erneuerTagBrowserText_ (void)
{
  DBG_PROG_START
  //open and preparing the first selected item

  if(!icc_betrachter->details->visible())
    return;

  TagBrowser *b = icc_betrachter->tag_browser;

  std::stringstream s;
  std::string text;
  std::vector<std::string> tag_list = profile.profil()->printTags();
  DBG_PROG_V( tag_list.size() <<" "<< (int*) b )

# define add_s(stream) s << stream; b->add (s.str().c_str()); s.str("");
# define add_          s << " ";

  b->clear();
  const char *file_type_name = _("Filename (ICC data type)");
  DBG_PROG_V( profile.profil()->data_type )
  if(profile.profil()->data_type != ICCprofile::ICCprofileDATA)
    file_type_name = _("Filename (other data type)");
  if(profile.profil()->data_type == ICCprofile::ICCcorruptedprofileDATA)
    file_type_name = _("Filename (corrupted ICC data type)");
  add_s ("@f" << file_type_name << ":" )
  add_s ("@b    " << profile.profil()->filename() )
  add_s ("")
  if (tag_list.size() == 0) {
    add_s (_("found no content for") <<" \"" << profile.profil()->filename() << "\"")
    return;
  } else if ((int)tag_list.size() != profile.profil()->tagCount()*5 ) {
    add_s (_("Internal error") )
  }
  add_s ("@B26@t" << _("No. Tag   Type   Size Description") )
  if(profile.profil()->data_type == ICCprofile::ICCprofileDATA ||
     profile.profil()->data_type == ICCprofile::ICCcorruptedprofileDATA) {
    add_s ("@t" << profile.profil()->printHeader() )
  } else {
    add_s("")
  }
  std::vector<std::string>::iterator it;
  for(int i = 0; i < (int)tag_list.size(); ++i)
    ;//DBG_PROG_V( i <<" "<< tag_list[i] )
  int anzahl = 0;
  for (it = tag_list.begin() ; it != tag_list.end(); ++it) {
    s << "@t";
    // Nummer
    int Nr = atoi((*it).c_str()) + 1;
    std::stringstream t; t << Nr;
    for (int i = t.str().size(); i < 3; i++) {s << " ";} s << Nr; *it++; ++anzahl; s << " ";
    // Name/Bezeichnung
    s << *it; for (int i = (*it++).size(); i < 6; i++) {s << " ";} ++anzahl;
    // Typ
    s << *it; for (int i = (*it++).size(); i < 5; i++) {s << " ";} ++anzahl;
    // Größe
    for (int i = (*it).size(); i < 6; i++) {s << " ";} s << *it++; s << " "; ++anzahl;
    // Beschreibung
    add_s (*it)
  }
  DBG_PROG_V( anzahl )
  if (b->value())
    b->selectItem (b->value()); // Anzeigen
  else
    if(profile.profil()->data_type == ICCprofile::ICCprofileDATA ||
       profile.profil()->data_type == ICCprofile::ICCcorruptedprofileDATA) {
      b->selectItem (1);
    } else {
      b->selectItem (6);
    }

  if (profile.profil()->hasTagName (b->selectedTagName)) {
    int item = profile.profil()->getTagIDByName (b->selectedTagName) + 6;
    b->selectItem (item);
    b->value(item);
  }

  status ( dateiName( profile.profil()->filename() ) << " " << _("loaded")  )
  DBG_PROG_ENDE
}

void
ICCexamin::icc_betrachterNeuzeichnen (void* z)
{ DBG_PROG_START
  Fl_Widget *wid = (Fl_Widget*)z;
  static int item;

  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->DD_farbraum)->visible())
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->inspekt_html)->visible())
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->examin)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->tag_browser)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->tag_text)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->tag_viewer)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_choice)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_text)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_gl_group)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_viewer)->visible() )

  enum {ZEIGEN, VERSTECKEN, NACHRICHT, KEINEn};
# define widZEIG(zeigen,widget,dbg) { \
     if        (zeigen ==                  VERSTECKEN && widget->visible()) { \
         widget->                          hide(); \
         if(dbg==NACHRICHT) DBG_PROG_S( "verstecke " << #widget ); \
  \
     } else if (zeigen ==                  ZEIGEN     && !widget->visible()) { \
         widget->                          show(); \
         if(dbg==NACHRICHT) DBG_PROG_S( "zeige     " << #widget ); \
     } \
  }

  // oberstes Widget testen
# if 0
  DBG_PROG_V( icc_betrachter->widget_oben )
  enum { DD_ZEIGEN, INSPEKT_ZEIGEN, TAG_ZEIGEN };
  int oben;
  if (icc_betrachter->menueintrag_3D->value() &&
      (   icc_betrachter->widget_oben == ICCfltkBetrachter::WID_3D
       || icc_betrachter->widget_oben == ICCfltkBetrachter::WID_0) )
    oben = DD_ZEIGEN;
  else if (icc_betrachter->menueintrag_inspekt->value() &&
      (   icc_betrachter->widget_oben == ICCfltkBetrachter::WID_INSPEKT
       || icc_betrachter->widget_oben == ICCfltkBetrachter::WID_0) )
    oben = INSPEKT_ZEIGEN;
  else
    oben = TAG_ZEIGEN;
  DBG_PROG_V( oben )

  // inhaltliches Zurueckschalten auf tiefere Ebene
  bool waehle_tag = false;
  if( oben == TAG_ZEIGEN &&
      (icc_betrachter->DD_farbraum->visible()
    || icc_betrachter->inspekt_html->visible()) )
    waehle_tag = true;


  // die oberste Ebene zeigen/verstecken
  if(oben == DD_ZEIGEN) {
    if(!icc_betrachter->DD_farbraum->visible()) {
      DBG_PROG_S( "3D Histogramm zeigen" )
      icc_betrachter->DD_farbraum->show();
      widZEIG(ZEIGEN, icc_waehler_ ,NACHRICHT)
      widZEIG(VERSTECKEN, icc_betrachter->examin ,NACHRICHT)
    }
  } else {
    if(icc_betrachter->DD_farbraum->visible()) {
      DBG_PROG_S( "3D hist verstecken" )
      icc_betrachter->DD_farbraum->hide();
      icc_betrachter->box_stat->color( fl_rgb_color( VG ) );
      icc_betrachter->box_stat->labelcolor(FL_BLACK);
      icc_betrachter->box_stat->redraw();
    }
    if(icc_waehler_->visible())
#   ifdef APPLE
      icc_waehler_->hide();
#   else
      icc_waehler_->iconize();
#   endif
  }

  if(icc_betrachter->inspekt_html->window() == icc_betrachter->details)
  {
    if(oben == INSPEKT_ZEIGEN)
      widZEIG(ZEIGEN, icc_betrachter->inspekt_html ,NACHRICHT)
    else
      widZEIG(VERSTECKEN, icc_betrachter->inspekt_html ,NACHRICHT)
  }

  if(oben == TAG_ZEIGEN) {
    widZEIG(ZEIGEN, icc_betrachter->examin ,NACHRICHT)
    widZEIG(ZEIGEN, icc_betrachter->tag_browser ,NACHRICHT)
    widZEIG(ZEIGEN, icc_betrachter->ansichtsgruppe ,NACHRICHT)
  } else {
    widZEIG(VERSTECKEN, icc_betrachter->examin ,NACHRICHT)
    widZEIG(VERSTECKEN, icc_betrachter->tag_browser ,NACHRICHT)
    widZEIG(VERSTECKEN, icc_betrachter->ansichtsgruppe ,NACHRICHT)
  }
# else

  if(wid == icc_betrachter->DD_farbraum) { 
    if(!icc_betrachter->DD_farbraum->visible_r()) {
      zeig3D();
    }
  }
  if(!icc_betrachter->DD_farbraum->visible_r()) {
      if(icc_waehler_->visible())
#   ifdef APPLE
        icc_waehler_->hide();
#   else
        icc_waehler_->iconize();
#   endif
  }

# endif

  // Bereinigen - hier?
  if (wid == icc_betrachter->tag_viewer ||
      wid == icc_betrachter->mft_viewer) {
    wid->clear_visible(); DBG_PROG_V( item << _item )
  }

  // Tabellenkompanion
  if (wid == icc_betrachter->mft_text ||
      wid == icc_betrachter->mft_gl_group ||
      wid == icc_betrachter->mft_viewer)
  { icc_betrachter->mft_choice->show(); DBG_PROG_S( "mft_choice zeigen" ) 
  } else
    icc_betrachter->mft_choice->hide();

# define SichtbarkeitsWechsel(widget) \
  { Fl_Widget *w = dynamic_cast<Fl_Widget*> (icc_betrachter->widget); \
    if (w != wid && w->visible()) \
    { DBG_PROG_S( #widget << " verstecken" ) \
      w->hide(); \
      Fl_Group *g = dynamic_cast<Fl_Group*> (w); \
      if( g ) \
        for(int i = 0; i < g->children(); ++i) \
          g->child(i)->hide(); \
      if(w->visible()) { \
        WARN_S( #widget << " ist noch sichbar" ); \
        w->hide(); \
      } \
    } else if(w == wid) \
    { DBG_PROG_S( #widget << " zeigen" ) \
      if (!w->visible_r()) \
      { \
        w->show(); \
        Fl_Group *g = dynamic_cast<Fl_Group*> (w); \
        if( g ) \
          for(int i = 0; i < g->children(); ++i) \
            g->child(i)->show(); \
        item = _item; \
      } \
      if (!w->visible_r()) \
        w->window()->show(); \
    } \
  }

  SichtbarkeitsWechsel(mft_viewer)
  SichtbarkeitsWechsel(mft_gl_group)
  SichtbarkeitsWechsel(mft_text)
  SichtbarkeitsWechsel(tag_viewer)
  SichtbarkeitsWechsel(tag_text)

# if 0
  // wenigstens ein Widget zeigen
  if(oben == TAG_ZEIGEN &&
     !icc_betrachter->mft_choice ->visible() &&
     !icc_betrachter->tag_viewer ->visible() )
    icc_betrachter->tag_text->show();

  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->DD_farbraum)->visible())
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->inspekt_html)->visible())
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->examin)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->tag_browser)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->ansichtsgruppe)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->tag_text)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->tag_viewer)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_choice)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_text)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_gl_group)->visible() )
  DBG_PROG_V( dynamic_cast<Fl_Widget*>(icc_betrachter->mft_viewer)->visible() )

  // Inhalte Erneuern
  if(waehle_tag) {
    //icc_examin_ns::unlock(this, __FILE__,__LINE__);
    waehleTag(_item);
    //icc_examin_ns::lock(__FILE__,__LINE__);
  }
# endif

  DBG_PROG_ENDE
}

void
ICCexamin::fortschritt(double f)
{ DBG_PROG_START
  if(0.0 < f && f <= 1.0) {
    if(!icc_betrachter->load_progress->visible())
      icc_betrachter->load_progress-> show();
    icc_betrachter->load_progress-> value( f );
    DBG_PROG_V( f )
  } else if (1.0 < f) {
    icc_betrachter->load_progress-> hide();
    DBG_PROG_V( f )
  } else {
    icc_betrachter->load_progress-> show();
    DBG_PROG_V( f )
  }
  DBG_PROG_ENDE
}

void
ICCexamin::fortschrittThreaded(double f)
{ DBG_PROG_START
  icc_examin_ns::lock(__FILE__,__LINE__);
  if(0.0 < f && f <= 1.0) {
    if(!icc_betrachter->load_progress->visible())
      icc_betrachter->load_progress-> show();
    icc_betrachter->load_progress-> value( f );
    DBG_PROG_V( f )
  } else if (1.0 < f) {
    icc_betrachter->load_progress-> hide();
    DBG_PROG_V( f )
  } else {
    icc_betrachter->load_progress-> show();
    DBG_PROG_V( f )
  }
  icc_betrachter->load_progress-> damage(FL_DAMAGE_ALL);
  icc_examin_ns::unlock(this, __FILE__,__LINE__);
  DBG_PROG_ENDE
}

void
ICCexamin::statusFarbe(double & CIEL, double & CIEa, double & CIEb)
{ DBG_PROG_START
  double lab[3] = {CIEL, CIEa, CIEb},
         *rgb = 0;
  DBG_PROG_V( lab[0]<<" "<<lab[1]<<" "<<lab[2] )
  rgb = icc_oyranos. wandelLabNachBildschirmFarben(lab, 1,
                                icc_examin->intentGet(NULL),
                                (icc_examin->gamutwarn()?cmsFLAGS_GAMUTCHECK:0)|
                                (icc_examin->bpc()?cmsFLAGS_BLACKPOINTCOMPENSATION:0));
  Fl_Color colour = fl_rgb_color( (int)(rgb[0]*255),
                                  (int)(rgb[1]*255), (int)(rgb[2]*255) );
  if (CIEL < .5)
    icc_betrachter->DD_box_stat->labelcolor( fl_rgb_color( VG ) );
  else
    icc_betrachter->DD_box_stat->labelcolor(FL_BLACK);
  icc_betrachter->DD_box_stat->color(colour);
  icc_betrachter->DD_box_stat->damage(FL_DAMAGE_ALL);
  //Fl::add_timeout(0.2, fl_delayed_redraw, icc_betrachter->DD_box_stat);
  Fl::add_idle(fl_delayed_redraw, icc_betrachter->DD_box_stat);
  Fl::awake();
  DBG_PROG_ENDE
}

void
ICCexamin::statusAktualisieren()
{ DBG_PROG_START
  icc_betrachter->box_stat->label(statlabel[0].c_str());
  icc_betrachter->DD_box_stat->label(statlabel[1].c_str());
  DBG_PROG_ENDE
}

int
tastatur(int e)
{ //DBG_PROG_START
  int gefunden = 0;
  static int dnd_kommt = false;

  switch (e)
  {
  case FL_SHORTCUT:
      if(Fl::event_key() == FL_Escape) {
        gefunden = 1;
        DBG_NUM_S("FL_Escape")
      } else
      if(Fl::event_key() == 'q'
       && Fl::event_state() == FL_CTRL) {
        DBG_NUM_S("FL_CTRL+Q")
        icc_examin->quit();
        gefunden = 1;
      } else
      if(Fl::event_key() == 'o'
       && Fl::event_state() == FL_COMMAND) {
        DBG_NUM_S("FL_COMMAND+O")
        icc_examin->oeffnen();
        gefunden = 1;
      }
    break;
  case FL_DND_ENTER:
    DBG_PROG_S( "FL_DND_ENTER" )
    fortschritt(0.01);
    return 1;
    break;
  case FL_DND_DRAG:
    DBG_PROG_S( "FL_DND_DRAG dnd_text_ops(" <<Fl::dnd_text_ops() <<")" )
    return 1;
    break;
  case FL_DND_LEAVE:
    DBG_PROG_S( "FL_DND_LEAVE" )
    fortschritt(1.1);
    return 1;
    break;
  case FL_DND_RELEASE:
    {
    DBG_PROG_S( "FL_DND_RELEASE " << Fl::event_length())
    icc_examin->icc_betrachter->details->take_focus();
    dnd_kommt = true;
    return 1;
    }
    break;
  case FL_PASTE:
    {
    DBG_PROG_S( "FL_PASTE " << Fl::event_length() )
      std::string adresse, suchen = "%20", ersetzen = " ";
      int pos;
#     if APPLE_
      if(dnd_kommt &&
         Fl::event_length())
      {
        DBG_PROG_S( Fl::event_text() );
        char *temp = (char*)malloc(Fl::event_length()+1),
             *text;
        sprintf(temp, Fl::event_text());
        std::vector<std::string>profilnamen;
        while((text = strrchr(temp,'\n')) != 0)
        {
          profilnamen.push_back(text+1);
          pos = profilnamen.size()-1;
          if(profilnamen[pos].size())
            icc_parser::suchenErsetzen(profilnamen[pos], suchen, ersetzen, 0);
          text[0] = 0;
        }
        profilnamen.push_back(temp);
        pos = profilnamen.size()-1;
        if(profilnamen[pos].size())
          icc_parser::suchenErsetzen(profilnamen[pos], suchen, ersetzen, 0);
        icc_examin->oeffnen(profilnamen);
        free(temp);
      }
      dnd_kommt = false;
#     else
      if(dnd_kommt &&
         Fl::event_length())
      {
        {
          int len = Fl::event_length();
          DBG_PROG_V( len )
          char *temp = (char*)malloc(MAX_PATH*64/*Fl::event_length()+1*/),
               *text;
          memcpy(temp, Fl::event_text(), Fl::event_length());
          temp[len]=0;
          // sprintf macht Probleme
          //sprintf(temp, Fl::event_text());
          DBG_PROG_V( Fl::event_text() )
          DBG_PROG_V( temp )
          std::vector<std::string>profilnamen;
          while((text = strrchr(temp,'\n')) != 0)
          { DBG_PROG_V( (int*)text<<" "<<text+1 )
            if(strlen(text+1))
              profilnamen.push_back(text+1);
            text[0] = 0;
          }
          profilnamen.push_back(temp);
          // Korrekturen
          for(unsigned int i = 0; i < profilnamen.size(); ++i) {
            const char *filter_a = "file:";
            DBG_PROG_V( profilnamen[i] )
            if(strstr(profilnamen[i].c_str(), filter_a)) {
              int len_neu = len-strlen(filter_a);
              char *txt = (char*)malloc(profilnamen[i].size()+1);
              memcpy(txt, &(profilnamen[i].c_str())[strlen(filter_a)],
                     len_neu);
              txt[len_neu]=0;
              // Wagenruecklauf beseitigen
              char *zeiger = strchr(txt, '\r');
              if(zeiger)
                zeiger[0] = 0;
              profilnamen[i] = txt;
              free(txt);
            }
            DBG_PROG_V( profilnamen[i] )
            // Leerzeichen filtern
            pos = i;
            if(profilnamen[pos].size())
              icc_parser::suchenErsetzen(profilnamen[pos], suchen, ersetzen, 0);
            DBG_PROG_S( i <<" "<< profilnamen[i] );
          }
          icc_examin->oeffnen(profilnamen);
          free(temp);
          dnd_kommt = false;
        }
      }
#     endif
    }
    break;
  case FL_RELEASE:
    DBG_PROG_S( "FL_RELEASE " << Fl::event_length() )
    break;
  case FL_DRAG:
    DBG_PROG_S( "FL_DRAG "<< Fl::event_length() )
    break;
  case FL_CLOSE:
    DBG_S( "FL_CLOSE " )
    break;
  default: 
    {
      //if(Fl::event_length())
        DBG_MEM_S( dbgFltkEvent(e) )
        DBG_MEM_S( Fl::event_length() << " bei: "<<Fl::event_x()<<","<<Fl::event_y() );
    }
    break;
  }
  
  icc_examin->icc_betrachter->DD_farbraum->tastatur(e);
  //DBG_PROG_ENDE
  return gefunden;
}


