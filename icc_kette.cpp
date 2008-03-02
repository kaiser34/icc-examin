/*
 * ICC Examin ist eine ICC Profil Betrachter
 * 
 * Copyright (C) 2005  Kai-Uwe Behrmann 
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
 * Die Kette von Profilen Klasse.
 * 
 */

#include "icc_kette.h"
#include "icc_info.h"
#include "icc_examin.h"

#if USE_THREADS
  #include "threads.h"
#else
  #if HAVE_FLTK
    #include <FL/Fl.H>
  #endif
#endif

ICCkette profile;

ICCkette::ICCkette  ()
{
  aktuelles_profil_ = -1;
  #if USE_THREADS
  waechter(this);
  #else
  Fl::add_timeout( 0.4, /*(void(*)(void*))*/waechter ,(void*)this);
  #endif
}

bool
ICCkette::einfuegen (const Speicher & prof, int pos)
{ DBG_PROG_START
  bool erfolg = false;

  // Ist das Profile eventuell schon geladen? -> Abbruch
  for(unsigned int i = 0; i < profile_.size(); ++i)
    if(prof.name() == profile_[i].filename())
      return erfolg;

  if(!prof.size())
    return false;

  DBG_PROG_V( pos )

  // Laden TODO: test auf Existenz der Datei (oyranos?)
  if (pos < 0 ||
      pos >= (int)profile_.size() )
  {
    pos = profile_.size();
    profile_.resize (profile_.size()+1 );
    profilnamen_.resize (profilnamen_.size()+1 );
    aktiv_.resize (aktiv_.size()+1 );
    profil_mzeit_.resize (profil_mzeit_.size()+1 );
    DBG_PROG_V( pos<<" "<<prof.name() )
  }
  DBG_PROG_V( pos )

  profile_[pos] = ICCprofile();
  ICCprofile::ICCDataType type = profile_[pos].load(prof);
  profile_[pos].filename( prof.name().c_str() );
  DBG_PROG_V( type )
  if(type == ICCprofile::ICCmeasurementDATA && pos != 0)
  {
    ICCmeasurement m;
    m.load( profile.profil() , (const char*) prof, prof.size() );
    DBG_V( profile.profil()->hasMeasurement() )
  }
  DBG_PROG_V( profile_[pos].size() )
  DBG_PROG_V( profile_[pos].filename() )
  DBG_PROG_V( prof.name() )
  std::string name = _("noName");
  if(prof.name().size())
    name = prof.name();
  else
    if(profile_[pos].hasTagName("desc"))
      name = profile_[pos].getTagText( profile_[pos].getTagByName("desc"))[0];
  profilnamen_[pos] = name ;
  aktiv_[pos] = true;
  profil_mzeit_[pos] = (double)prof.zeit();
  DBG_PROG

  if( profile_.size() ) {
    aktuelles_profil_ = 0;
    DBG_PROG_V( aktuell() )
    erfolg = true;
  } else { DBG_PROG
    icc_examin_ns::status_info(_("File not loaded!"));
  }

  /*Modell::*/benachrichtigen( pos );
  DBG_PROG_ENDE
  return erfolg;
}
#if 0
bool
ICCkette::einfuegen (std::string dateiname, int pos)
{ DBG_PROG_START
  bool erfolg = false;
  DBG_PROG_V( dateiname <<" "<< pos )
  // Ist das Profile eventuell schon geladen? -> Abbruch
  for(unsigned int i = 0; i < profile_.size(); ++i)
    if(dateiname == profile_[i].filename()) {
      DBG_PROG_S( _("File allready loaded")<< i <<": "<< dateiname )
      return erfolg;
    }

  #if 0
  // Laden TODO: test auf Existenz der Datei (oyranos?)
  int resize_b = false;
  if (pos >= 0 &&
      pos < (int)profile_.size())
  {
    profile_.insert (profile_.begin()+pos, ICCprofile());
  } else {
    profile_.resize( profile_.size() + 1 );
    pos = profile_.size()-1;
    resize_b = true;
  }

  ICCprofile::ICCDataType type = profile_[pos].load(dateiname.c_str());
  DBG_PROG_V( type )
  if(type == ICCprofile::ICCmeasurementDATA && pos != 0)
  {
      ICCtag tag;
      if(profile_[pos].hasTagName("targ"))
      {
        tag = profile_[pos].getTag( profile_[pos].getTagByName("targ"));
        profile_[aktuell()].addTag( tag );
        /*size_t groesse;
        char* c = profile_[aktuell()].saveProfileToMem( &groesse );*/
        /*Modell::*/benachrichtigen( aktuell() );
      }
      DBG_V( profile.profil()->hasMeasurement() )
  }

  DBG_PROG_V( dateiname )
  if( !resize_b )
  {
    profilnamen_.insert (profilnamen_.begin()+pos, dateiname );
    aktiv_.insert (aktiv_.begin()+pos, true);
    profil_mzeit_.insert (profil_mzeit_.begin()+pos,
                            holeDateiModifikationsZeit( dateiname.c_str() ));
  } else {
    profilnamen_.push_back( dateiname );
    aktiv_.push_back(true);
    profil_mzeit_.push_back(holeDateiModifikationsZeit( dateiname.c_str() ));
  }
  DBG_PROG

  if( profile_.size() ) {
    aktuelles_profil_ = 0;
    erfolg = true;
  } else { DBG_PROG
    icc_examin_ns::status_info(_("File not loaded!"));
  }

  /*Modell::*/benachrichtigen( pos );
  #else
  size_t size = 0;
  char* data = ladeDatei( dateiname, &size );
  Speicher s(data, size);
  s = dateiname.c_str();
  einfuegen(s, pos);
  #endif

  DBG_PROG_ENDE
  return erfolg;
}

bool
ICCkette::einfuegen (std::vector<std::string> dateinamen)
{ DBG_PROG_START
  bool erfolg = false;

  clear();

  for (unsigned int i = 0; i < dateinamen.size(); ++i)
  {
    erfolg = einfuegen (dateinamen[i], -1);
    if (erfolg && i == 0)
      icc_examin->farbraumModus(0);
    DBG_PROG_V( dateinamen[i] )
    //icc_examin_ns::fortschritt (1.0/i);
  }
  //icc_examin_ns::fortschritt (1.1);

  if( profile_.size() )
    aktuelles_profil_ = 0;

  erfolg = true;
  DBG_PROG_ENDE
  return erfolg;
}
#endif
static bool erstes_mal = true;
#if USE_THREADS
void*
#else
void
#endif
ICCkette::waechter (void* zeiger)
{
  //DBG_PROG_START
  // warte, starte einen pthread , teste alle Profile - ende 
  // Es entsteht eine Kette von threads. Faellt einer aus
  // ist die Ueberwachung beendet. - etwas labil vielleicht

  ICCkette* obj = (ICCkette*) zeiger;

  if(erstes_mal)
    erstes_mal = false;
  else {
  #if USE_THREADS
    sleep(1);
  #endif
  }

  #if USE_THREADS
  static Fl_Thread fl_t;
  int fehler = fl_create_thread( fl_t, &waechter, (void *)zeiger );
  if( fehler == EAGAIN)
  {
    WARN_S( _("Waechter Thread nicht gestartet Fehler: ")  << fehler );
  } else
  #if !APPLE && !WIN32
  if( fehler == PTHREAD_THREADS_MAX )
  {
    WARN_S( _("zu viele Waechter Threads Fehler: ") << fehler );
  } else
  #endif
  if( fehler != 0 )
  {
    WARN_S( _("unbekannter Fehler beim Start eines Waechter Threads Fehler: ") << fehler );
  }
  #else
  Fl::add_timeout( 0.4, /*(void(*)(void*))*/waechter ,(void*)zeiger);
  #endif

  for(unsigned i = 0; i < obj->profilnamen_.size(); ++i)
  {
    DBG_MEM_V( obj->profilnamen_[i] );
    double m_zeit = holeDateiModifikationsZeit( obj->profilnamen_[i].c_str() );
    DBG_MEM_V( m_zeit )
    if( m_zeit &&
        obj->aktiv_[i] &&
        obj->profil_mzeit_[i] != m_zeit
  #if USE_THREADS
     && icc_examin->frei()
  #endif
      )
    {
      obj->profile_[i].load( obj->profilnamen_[i] );
      obj->/*Modell::*/benachrichtigen( i );
      obj->profil_mzeit_[i] = m_zeit;
    }
  }

  //DBG_PROG_ENDE
  #if USE_THREADS
  return 0;
  #endif
}




