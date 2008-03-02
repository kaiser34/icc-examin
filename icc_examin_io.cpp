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


#include "icc_examin.h"
#include "icc_betrachter.h"
#include "icc_waehler.h"

using namespace icc_examin_ns;

#if USE_THREADS
#include "threads.h"
#else
// TODO: beseitige Hack
static int frei_tuen = 0;
#define frei_ frei_tuen
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

const char* dateiName(const char* name);
const char* dateiName(std::string name);

static int laden_sperren = false;


#include "icc_vrml_parser.h"

void
ICCexamin::oeffnen (std::vector<Speicher> speicher_vect)
{ DBG_START
  if(!speicher_vect.size() ||
     laden_sperren) {
    fortschritt( 1.1 );
    DBG_PROG_ENDE
    return;
  }
  laden_sperren = true;

  // Laden
  frei_ = false;
  icc_betrachter->DD_farbraum->punkte_clear();
  profile.clear();
  fortschritt( -.1 );
  for (unsigned int i = 0; i < speicher_vect.size(); ++i)
  {
    DBG_V( speicher_vect[i].size()<<" "<<speicher_vect[i].name() )
    fortschritt( 1./3.+ (double)(i)/speicher_vect.size()/3.0 );
    profile.einfuegen( speicher_vect[i], -1 );
    fortschritt( 1./3.+ (double)(i+1)/speicher_vect.size()/3.0 );
  }

  std::vector<std::string> dateinamen = profile;

  if (profile.size())
  {
      // Oberflaechenpflege
    Fl::lock();
    tag_browserText ();
    if(icc_betrachter->DD_farbraum->visible() &&
       !icc_betrachter->inspekt_html->visible() )
      icc_betrachter->DD_farbraum->flush();
    icc_betrachter->menueintrag_gamut_speichern->activate();
    icc_betrachter->menueintrag_gamut_vrml_speichern->activate();

    fortschritt( 2./3.+ 1./3. );
    icc_betrachter->measurement( profile.profil()->hasMeasurement() );
    if(farbraumModus())
    {
        // Oberflaechenpflege
      gamutAnsichtZeigen();
    }
    Fl::unlock();

      // Sortieren
    if( dateinamen.size() &&
        (dateinamen[0].find( "wrl",  dateinamen[0].find_last_of(".") )
         != std::string::npos) )
    {
      std::string d (speicher_vect[0], speicher_vect[0].size());
      //DBG_NUM_V( d <<" "<< size )
      std::vector<ICCnetz> netze = extrahiereNetzAusVRML (d);
      if( netze.size() )
      { DBG_NUM_V( netze.size() )
        for(unsigned int n = 0; n< netze.size(); ++n)
        {
          DBG_NUM_V( netze[n].punkte.size() )
          for(unsigned int i = 0; i < 10; ++i) {
           cout << netze[n].punkte[i].koord[0] << " ";
           cout << netze[n].punkte[i].koord[1] << " ";
           cout << netze[n].punkte[i].koord[2] << "  ";
           cout << netze[n].punkte[i].farbe[0] << " ";
           cout << netze[n].punkte[i].farbe[1] << " ";
           cout << netze[n].punkte[i].farbe[2] << " ";
           cout << netze[n].punkte[i].farbe[3] << endl;
          }
          DBG_NUM_V( netze[n].indexe.size()/4.0 )
        }
        for(unsigned int i = 0; i < netze.size(); ++i ) {
          netze[i].transparenz = 0.6;
          netze[i].grau = false;
          netze[i].aktiv = true;
        }
        Fl::lock();
        icc_betrachter->DD_farbraum->hineinNetze(netze);
        std::vector<std::string> texte;
        texte.push_back(_("CIE *L"));
        texte.push_back(_("CIE *a"));
        texte.push_back(_("CIE *b"));
        icc_betrachter->DD_farbraum->achsNamen(texte);
        icc_betrachter->DD_farbraum->punkte_clear();
        icc_betrachter->DD_farbraum->auffrischen();
        gamutAnsichtZeigen();
        Fl::unlock();
      } else
        WARN_S(_("kein Netz gefunden in VRML Datei"))
    } else {
      DBG_PROG
      //farbraum();
      //icc_betrachter->DD_farbraum->auffrischen();
    }

    fortschritt( 2./3.+ 2./3. );
    // ICCwaehler
      // erneuern
    static std::vector<std::string> namen_neu, namen_alt;
    bool namensgleich = false;
    namen_neu = profile;
    DBG_PROG_V( namen_neu.size() <<" "<< namen_alt.size() )
    if(namen_alt.size() == namen_neu.size()) {
      namensgleich = true;
      DBG_NUM_S( _("Anzahl gabs schon mal") )
      for(int i = 0; i < (int)namen_neu.size(); ++i)
        if(namen_neu[i] != namen_alt[i])
          namensgleich = false;
    }
    namen_alt = namen_neu;
    DBG_NUM_V( "#################### " << namensgleich << " ##############")
    if(!namensgleich)
    {
      icc_waehler_->clear();
      int anzahl = profile.size();
      DBG_PROG_V( anzahl )
      double transparenz;
      bool grau;
      std::vector<int> aktiv = profile.aktiv();
      DBG_PROG_V( aktiv.size() )
      for(int i = 0; i < anzahl; ++i) {
        DBG_PROG_V( i )

        if( i >= (int)icc_betrachter->DD_farbraum->dreiecks_netze.size() ) {
          WARN_S( _("Kein Netz gefunden. Ist Argyll installiert?") )
          break;
        }

        transparenz= icc_betrachter->DD_farbraum->dreiecks_netze[i].transparenz;
        DBG_PROG_V( transparenz )
        grau = icc_betrachter->DD_farbraum->dreiecks_netze[i].grau;
        icc_waehler_->push_back(dateiName(profile.name(i)),
                                transparenz, grau , aktiv[i]);
      }
    }

    // Fenstername setzen
    {
      detaillabel = "ICC Examin: ";
      detaillabel.insert( detaillabel.size(), dateiName(dateinamen[0]) );
      icc_betrachter->details->label(detaillabel.c_str());
      DBG_PROG_V( dateinamen[0] <<" "<< detaillabel )
    }

  }
  frei_ = true;
  laden_sperren = false;
  DBG_ENDE
}

const char*
dateiName(const char* name)
{
  const char* dateiname=0;
        // Datainame extahieren
        if(name)
          dateiname = strrchr(name,'/');

        if(!dateiname)
          dateiname = name;
        else
          ++dateiname;
  return dateiname;
}

const char*
dateiName(std::string name)
{
  return dateiName(name.c_str());
}

// Dieses Spalten in Threads fuer eine fluessige Oberflaeche
#if USE_THREADS
void*
#else
void
#endif
ICCexamin::oeffnenStatisch (void* ptr)
{ DBG_PROG_START
  DBG_MEM_V( ptr )
  if(!ptr)
    WARN_S( "kein Speichervektor uebergeben: "<< ptr )
  std::vector<Speicher> *ss = (std::vector<Speicher>*)ptr;;
  DBG_PROG_V( (*ss).size() )
  icc_examin->oeffnen(*ss);

  DBG_PROG_ENDE
  #if USE_THREADS
  return 0;
  #endif
}

void
ICCexamin::oeffnen (std::vector<std::string> dateinamen)
{ DBG_PROG_START
  if(!dateinamen.size()) {
    WARN_S("keine Dateinamen angegeben")
    fortschritt( 1.1 );
    DBG_PROG_ENDE
    return;
  }

  // Laden
  frei_ = false;
  char* data = 0;
  size_t size = 0;
  #if 0
  static std::vector<Speicher> ss;
  ss.clear();
  #else
  std::vector<Speicher> *ss = (std::vector<Speicher>*)new std::vector<Speicher>;
  #endif
  // resize benutzt copy, und erzeugt damit Referenzen auf das
  // selbe Objekt ; wir benoetigen aber neue Objekte
  // ss.resize(dateinamen.size());
  for (unsigned int i = 0; i < dateinamen.size(); ++i)
    ss->push_back(Speicher());

  fortschritt( 0.0 );
  for(unsigned int i = 0; i < dateinamen.size(); ++i)
  {
    try {
      DBG_PROG_V( dateinamen[i] )
      fortschritt( (double)i/dateinamen.size()/3.0 );
      data = ladeDatei (dateinamen[i], &size);
      fortschritt( (double)(i+1)/dateinamen.size()/3.0 );
    }
      catch (Ausnahme & a) {  // faengt alles von Ausnahme Abstammende
        DBG_NUM_V (_("Ausnahme aufgetreten: ") << a.what());
        a.report();
      }
      catch (std::exception & e) { // faengt alles von exception Abstammende
        DBG_NUM_V (_("Std-Ausnahme aufgetreten: ") << e.what());
      }
      catch (...) {       // faengt alles Uebriggebliebene
        DBG_NUM_V (_("Huch, unbekannte Ausnahme"));
      }

    if(data && size)
    {
      DBG_PROG_V( (int*)ss )
      (*ss)[i].lade(data,size);
      (*ss)[i] = dateiName(dateinamen[i]);
      free(data);
      DBG_PROG
    } else {
      status( _("Stop loading ") << dateinamen[i] <<" "<<
              (int*)data <<"|"<< size )
      fortschritt( 1.1 );
      return;
    }
  }
  frei_ = true;
  DBG_PROG
  #if USE_THREADS
  static Fl_Thread fl_t;
  int fehler = fl_create_thread( fl_t, &oeffnenStatisch, (void *)ss );
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
  Fl::add_timeout( 0.01, /*(void(*)(void*))*/oeffnenStatisch ,(void*)&ss);
  #endif

  DBG_PROG_ENDE
}

void
ICCexamin::oeffnen ()
{ DBG_PROG_START
  if(!laden_sperren)
  {
    fortschritt(0.01);
    std::vector<std::string> profilnamen = icc_betrachter->open( profile );
    oeffnen( profilnamen );
  }
  DBG_PROG_ENDE
}

void
ICCexamin::tag_browserText (void)
{
  DBG_PROG_START
  //open and preparing the first selected item

  TagBrowser *b = icc_betrachter->tag_browser;

  std::stringstream s;
  std::string text;
  std::vector<std::string> tag_list = profile.profil()->printTags();
  DBG_PROG_V( tag_list.size() <<" "<< (int*) b )

  #define add_s(stream) s << stream; b->add (s.str().c_str()); s.str("");
  #define add_          s << " ";

  b->clear();
  add_s ("@f" << _("Filename") << ":" )
  add_s ("@b    " << profile.profil()->filename() )
  add_s ("")
  if (tag_list.size() == 0) {
    add_s (_("found no content for") <<" \"" << profile.profil()->filename() << "\"")
    return;
  } else if ((int)tag_list.size() != profile.profil()->tagCount()*5 ) {
    add_s (_("Internal error") )
  }
  add_s ("@B26@t" << _("No. Tag   Type   Size Description") )
  add_s ("@t" << profile.profil()->printHeader() )
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
    b->selectItem (1);

  if (profile.profil()->hasTagName (b->selectedTagName)) {
    int item = profile.profil()->getTagByName (b->selectedTagName) + 6;
    b->selectItem (item);
    b->value(item);
  }

  status ( dateiName( profile.profil()->filename() ) << " " << _("loaded")  )
  DBG_PROG_ENDE
}

bool
ICCexamin::berichtSpeichern (void)
{ DBG_PROG_START
  frei_ = false;
  bool erfolgreich = true;
  std::string dateiname = profile.name();  DBG_PROG_V( dateiname )

  // Profilnamen ersetzen
  std::string::size_type pos=0;
  if ((pos = dateiname.find_last_of(".", dateiname.size())) != std::string::npos)
  { DBG_PROG
    dateiname.replace (pos, 5, ".html"); DBG_NUM_S( ".html gesetzt" )
  } DBG_PROG_V( dateiname )

  // FLTK Dateidialog aufrufen
  DBG_PROG_V( dateiwahl->filter() )

  std::string muster = dateiwahl->filter(); DBG_PROG
  std::string datei;
  if (dateiwahl->value())
    datei = dateiwahl->value(); DBG_PROG
  std::string titel = dateiwahl->label(); DBG_PROG

  dateiwahl->filter(_("HTML Documents (*.htm*)")); DBG_PROG
  #ifdef HAVE_FLU
  dateiwahl->cd(".");
  #endif
  dateiwahl->label(_("Save Report")); DBG_PROG
  dateiwahl->value(dateiname.c_str()); DBG_PROG

  dateiwahl->show(); DBG_PROG
  while( dateiwahl->shown() )
    Fl::wait( 0.01 );

  DBG_PROG_V( dateiwahl->filter() )
  if (dateiwahl->value())
    dateiname = dateiwahl->value();
  else
    dateiname = "";
  DBG_PROG

  dateiwahl->filter(muster.c_str()); DBG_PROG
  dateiwahl->value(datei.c_str()); DBG_PROG
  dateiwahl->label(titel.c_str()); DBG_PROG
  DBG_PROG_V( dateiwahl->filter() )

  DBG_PROG_V( dateiname )

  if (dateiname == "" ||
      dateiname == profile.name())
  { DBG_PROG_V( dateiwahl->count() << dateiname )
    fortschritt (1.1);
    DBG_PROG_ENDE
    return false;
  }

  // Bericht erzeugen
  bool export_html = true;
  std::string bericht = profile.profil()->report(export_html);
  // Speichern
  std::ofstream f ( dateiname.c_str(),  std::ios::out );
  f.write ( bericht.c_str(), bericht.size() );
  f.close();

  frei_ = true;
  DBG_PROG_ENDE
  return erfolgreich;
}

bool
ICCexamin::gamutSpeichern (IccGamutFormat format)
{
  DBG_PROG_START
  frei_ = false;
  bool erfolgreich = true;
  std::string dateiname = profile.name();  DBG_PROG_V( dateiname )

  // Profilnamen ersetzen
  std::string::size_type pos=0;
  if ((pos = dateiname.find_last_of(".", dateiname.size())) != std::string::npos)
  { DBG_PROG
    if(format == ICC_ABSTRACT)
      dateiname.replace (pos, 5, "_proof.icc"); DBG_NUM_S( "_proof.icc gesetzt")
    else if(format == ICC_VRML)
      dateiname.replace (pos, 5, ".wrl"); DBG_NUM_S( ".wrl gesetzt")
  } DBG_PROG_V( dateiname )

  // FLTK Dateidialog aufrufen
  DBG_PROG_V( dateiwahl->filter() )

  std::string muster = dateiwahl->filter(); DBG_PROG
  std::string datei;
  if (dateiwahl->value())
    datei = dateiwahl->value(); DBG_PROG
  std::string titel = dateiwahl->label(); DBG_PROG

  #ifdef HAVE_FLU
  dateiwahl->cd(".");
  #endif
  if(format == ICC_ABSTRACT) {
    dateiwahl->filter(_("ICC colour profiles (*.ic*)")); DBG_PROG
    dateiwahl->label(_("Save Gamut as Profile")); DBG_PROG
  } else if(format == ICC_VRML) {
    dateiwahl->filter(_("VRML Files (*.wrl)")); DBG_PROG
    dateiwahl->label(_("Save Gamut as VRML")); DBG_PROG
  }
  dateiwahl->value(dateiname.c_str()); DBG_PROG

  dateiwahl->show(); DBG_PROG
  while( dateiwahl->shown() )
    Fl::wait( 0.01 );

  DBG_PROG_V( dateiwahl->filter() )
  if (dateiwahl->value())
    dateiname = dateiwahl->value();
  else
    dateiname = "";
  DBG_PROG

  dateiwahl->filter(muster.c_str()); DBG_PROG
  dateiwahl->value(datei.c_str()); DBG_PROG
  dateiwahl->label(titel.c_str()); DBG_PROG
  DBG_PROG_V( dateiwahl->filter() )

  DBG_PROG_V( dateiname )

  if (dateiname == "" ||
      dateiname == profile.name())
  { DBG_PROG_V( dateiwahl->count() << dateiname )
    fortschritt (1.1);
    frei_ = true;
    DBG_PROG_ENDE
    return false;
  }

  Speicher profil;
  size_t groesse = 0;
  char* daten = 0;
  daten = profile.profil()->saveProfileToMem( &groesse );
  profil.lade( daten, groesse );
  if(format == ICC_ABSTRACT) {
    // Gamutprofil erzeugen
    Speicher speicher;
    icc_oyranos.gamutCheckAbstract( profil, speicher,
                                    profile.profil()->intent(),
                                    /*cmsFLAGS_GAMUTCHECK |*/ cmsFLAGS_SOFTPROOFING );

    // Speichern
    saveMemToFile ( dateiname.c_str(), (const char*)speicher, speicher.size() );
    speicher.clear(); DBG
  } else if(format == ICC_VRML) {
    std::string vrml;
    vrml = icc_oyranos.vrmlVonProfil ( *profile.profil(), profile.profil()->intent() );
    // Speichern
    saveMemToFile ( dateiname.c_str(), vrml.c_str(), vrml.size() );
  }
  profil.clear(); DBG

  frei_ = true;
  DBG_PROG_ENDE
  return erfolgreich;
}

