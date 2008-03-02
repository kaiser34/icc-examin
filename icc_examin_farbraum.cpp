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

using namespace icc_examin_ns;


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



void
ICCexamin::messwertLese (int n,
                         std::vector<double> & p,
                         std::vector<float>  & f,
                         std::vector<std::string> & namen)
{
  DBG_PROG_START
  if(profile.size() > n &&
     profile.aktuell() == n &&
     profile[n]->hasMeasurement() &&
     profile[n]->getMeasurement().hasXYZ() )
    { DBG_NUM_S( "nutze Messdaten" )
      ICCmeasurement messung = profile[n]->getMeasurement();

      if(messung.valid() && profile[n]->size())
        icc_betrachter->DD_farbraum->zeig_punkte_als_messwert_paare = true;
      else
        icc_betrachter->DD_farbraum->zeig_punkte_als_messwert_paare = false;
      DBG_NUM_V( icc_betrachter->DD_farbraum->zeig_punkte_als_messwert_paare )
      icc_betrachter->DD_farbraum->zeig_punkte_als_messwerte = true;
      DBG_NUM_V( icc_betrachter->DD_farbraum->zeig_punkte_als_messwerte )

      unsigned int j;
      int n = messung.getPatchCount(); DBG_PROG_V( messung.getPatchCount() )
      for (j = 0; j < (unsigned) n; ++j)
      { // zuerst die Messwerte ...
        std::vector<double> daten = messung.getMessLab(j);
        for (unsigned i = 0; i < daten.size(); ++i)
          p.push_back(daten[i]);
        // ... dann die über das Profil errechneten Lab Werte
        if (icc_betrachter->DD_farbraum->zeig_punkte_als_messwert_paare) {
          daten = messung.getCmmLab(j);
          for (unsigned i = 0; i < daten.size(); ++i)
            p.push_back(daten[i]);
        } 

        daten = messung.getMessRGB(j);
        for (unsigned i = 0; i < daten.size(); ++i) {
          f.push_back((float)daten[i]);
        }
        f.push_back(1.0);
        if (icc_betrachter->DD_farbraum->zeig_punkte_als_messwert_paare)
        { daten = messung.getCmmRGB(j);
          for (unsigned i = 0; i < daten.size(); ++i)
            f.push_back(daten[i]);
          f.push_back(1.0);
        } 
      }
      namen = messung.getFeldNamen();
    }

  DBG_PROG_ENDE
}

void
ICCexamin::netzLese (int n,
                     std::vector<ICCnetz> *netz)
{
  DBG_PROG_START
  if( profile.size() == 0 && farbraumModus()) {
    netz->resize(1);
    return;
  }

  std::vector<ICCnetz> netz_temp;

  {
    if(farbraumModus() && n == 1)
      intent_ = profile.profil()->intent();
    else
      intent_ = 3;

    netz_temp = icc_oyranos. netzVonProfil(  *(profile[n]), intent_ );
    if(netz_temp.size())
    {
      netz_temp[0].transparenz = (*netz)[n].transparenz;
      netz_temp[0].grau = (*netz)[n].grau;
      netz_temp[0].aktiv = (*netz)[n].aktiv;
      if(n >= (int)netz->size())
        netz->resize( n+1 );
      DBG_PROG_V( netz->size() <<" "<< netz_temp.size() )
      (*netz)[n] = netz_temp[0];
      (*netz)[n].name = profile[n]->filename();
      // Dateiname extrahieren
      std::string & dateiname = (*netz)[n].name;
      if( dateiname.find_last_of("/") != std::string::npos)
        dateiname = dateiname.substr( dateiname.find_last_of("/")+1,
                                    dateiname.size() );
      DBG_NUM_V( (*netz)[n].transparenz <<" "<< (*netz)[n].umriss.size() )
    } else {
      (*netz)[n].punkte.clear();
      (*netz)[n].indexe.clear();
      (*netz)[n].umriss.clear();
      (*netz)[n].name.clear();
    }
  }
  for(int i = 0; i < (int)netz->size(); ++i)
    DBG_PROG_V( (*netz)[n].aktiv <<" "<< (*netz)[i].transparenz <<" "<< (*netz)[i].umriss.size() );
  DBG_PROG_ENDE
}

void
ICCexamin::farbenLese (int n,
                       std::vector<double> & p,
                       std::vector<float>  & f)
{
  DBG_PROG_START
  // benannte Farben darstellen
  if( profile.size() > n )
  {
    DBG_PROG
    p = profile[n]->getTagNumbers (profile[n]->getTagByName("ncl2"),
                                         ICCtag::MATRIX);
    if(p.size())
    {
      DBG_NUM_V( p[0] <<" "<< p.size() )
      f.resize( p.size() * 4/3);
    }
    DBG_NUM_V( f.size() )
    // ncl2 Farben -> Bildschirm
    unsigned int n_farben = p.size()/3;
    double *lab = new double [n_farben*3],
           *rgb=0;
    for(unsigned i = 0; i < n_farben*3; ++i)
      lab[i] = p[i];
    rgb = icc_oyranos. wandelLabNachBildschirmFarben(lab, n_farben,
                                 icc_examin->intent(),
                                 icc_examin->gamutwarn()?cmsFLAGS_GAMUTCHECK:0);
    DBG_NUM_V( n_farben )
    if(!rgb)  WARN_S( _("RGB Ergebnis nicht verfuegbar") )
    for(unsigned i = 0; i < n_farben; ++i) {
      f[i*4+0] = rgb[i*3+0];
      f[i*4+1] = rgb[i*3+1];
      f[i*4+2] = rgb[i*3+2];
      f[i*4+3] = 1.0;
    }
    if(lab) delete [] lab;
    if(rgb) delete [] rgb;
    icc_betrachter->DD_farbraum->zeig_punkte_als_messwert_paare = false;
    icc_betrachter->DD_farbraum->zeig_punkte_als_messwerte = false;
  }

  DBG_PROG_ENDE
}

void
ICCexamin::farbraum (int n)
{
  DBG_PROG_START

  std::vector<std::string> texte, namen;

  texte.push_back(_("CIE *L"));
  texte.push_back(_("CIE *a"));
  texte.push_back(_("CIE *b"));

  std::vector<double> p;
  std::vector<float>  f;
  DBG_PROG_V( n <<" "<< profile.size()<<" "<<profile.aktuell() )
  DBG_PROG_V( profile[n]->filename() )

  // Messwerte
  int messwerte=false;
  FREI_(false);
  if(profile.size() > n &&
     profile.aktuell() == n &&
     profile[n]->hasMeasurement() &&
     profile[n]->getMeasurement().hasXYZ() )
    {
      DBG_PROG
      messwertLese(n, p,f,namen);
      messwerte = true;
    }
  FREI_(true);

  int ncl2_profil = profile[n]->hasTagName("ncl2");

  // Oeffnen
  if(lade_)
  {
    farbraumModus( profile.aktuell() );
    if(farbraumModus())
    {
        // Oberflaechenpflege
      gamutAnsichtZeigen();
    }
  }

  // benannte Farben darstellen
  if( profile.size() > n && ncl2_profil )
  {
    DBG_PROG
    farbenLese(n, p,f);
  }

  bool neues_netz = false;
  if( n >= (int)icc_betrachter->DD_farbraum-> dreiecks_netze.size() )
    neues_netz = true;

  if(n == 0)
    icc_betrachter->DD_farbraum->hineinPunkte( p, f, namen, texte );

  if((int)icc_betrachter->DD_farbraum-> dreiecks_netze .size() <= n)
    icc_betrachter->DD_farbraum-> dreiecks_netze .resize( n + 1 );

  std::vector<ICCnetz> *netz = &icc_betrachter->DD_farbraum->dreiecks_netze;
  DBG_PROG_V( icc_betrachter->DD_farbraum-> dreiecks_netze.size() <<" "<< n )


  if( profile.size() > n && !ncl2_profil )
    netzLese(n, netz);

  DBG_PROG_V( n <<" "<< netz->size() <<" "<< ncl2_profil )

  if(netz->size() && neues_netz)
  {
    if(((n == 0 && ncl2_profil)
        || (n == 1 && farbraumModus())
        || profile.size() == 1 ) &&
       !messwerte )
    {
      (*netz)[n].transparenz = 0.25;
      (*netz)[n].grau = false;
    } else {
      (*netz)[n].transparenz = 0.3;
      (*netz)[n].grau = true;
    }

    icc_betrachter->DD_farbraum->achsNamen( texte );

    std::string moni = icc_oyranos.moni_name();
    if(profile[n]->filename() == moni)
      profile.passiv(n);
    DBG_PROG_V( n <<" "<< profile.aktiv(n) )
  }
  if(icc_betrachter->DD_farbraum->dreiecks_netze[n].name == "") {
    icc_betrachter->DD_farbraum->dreiecks_netze[n].name =
                                                         profile[n]->filename();
    // Dateiname extrahieren
    std::string & dateiname = icc_betrachter->DD_farbraum->dreiecks_netze[n].name;
    if( dateiname.find_last_of("/") != std::string::npos)
      dateiname = dateiname.substr( dateiname.find_last_of("/")+1,
                                    dateiname.size() );
    DBG_PROG_V( icc_betrachter->DD_farbraum->dreiecks_netze[n].name )
  }

  FREI_(true);
  DBG_PROG_ENDE
}

void
ICCexamin::farbraum ()
{
  DBG_PROG_START
  FREI_(false);

  if((int)icc_betrachter->DD_farbraum -> dreiecks_netze.size() > profile.size())
    icc_betrachter->DD_farbraum -> dreiecks_netze.resize(profile.size());
  DBG_PROG_V( icc_betrachter->DD_farbraum -> dreiecks_netze.size() )
  FREI_(true);

  for(int i = 0; i < profile.size(); ++i)
  {
    while(!profile[i] || profile[i]->changing())
      icc_examin_ns::sleep(0.05);
    farbraum(i);
  }

  /*if(icc_betrachter->DD_farbraum -> dreiecks_netze.size())
    icc_betrachter->DD_farbraum ->
      dreiecks_netze [icc_betrachter->DD_farbraum->dreiecks_netze.size()-1]
        . transparenz = 0.7;*/

  DBG_PROG_V( profile.size() )

  DBG_PROG_ENDE
}

void
ICCexamin::farbraumModus (int profil)
{
  DBG_PROG_START
  FREI_(false);

  farbraum_modus_ = false;
  if(profile.size() && profile.profil()->hasTagName("ncl2")) {
    farbraum_modus_ = true;
    DBG_PROG_S( "setzte Farbraum Modus" )
  } else if(!profile.size()) {
    WARN_S( "zu früh mit " << profile.size() << " Profilen" )
  }

  DBG_PROG_V( farbraum_modus_ )

  FREI_(true);
  DBG_PROG_ENDE
}


