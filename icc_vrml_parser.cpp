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
 * Farbumfang betrachten im WWW-3D Format vrml: Parser
 * 
 */

// Beginn Datum:      Februar 2005

#include <sstream>
#include <string>
#include <vector>

#include "icc_helfer.h"
#include "icc_vrml_parser.h"

#define lp {l+=0.1; icc_examin->fortschritt(l);}

using namespace icc_parser;

class ICCvrmlParser
{
    std::string original_;
    std::string arbeit_;
    std::vector<ICCnetz> netze_;
  public:
    void lade ( std::string & vrml )   { DBG_PROG original_ = vrml; }
    std::vector<ICCnetz> zeigeNetze () { DBG_PROG lesen_(); return netze_; }
  private:
    void lesen_()
{
  int debug_alt = icc_debug;
  icc_debug = 1;
  DBG_PROG_START
  int netz_n = 0;                       // das n-the Netz
  std::vector<std::string> zeilen;      // Editierzeilen
  std::vector<ZifferWort> werte;        // Zwischenrückgabewert
  char trennzeichen[12];                // zu verwendende Trennzeichen
  trennzeichen[0] = ',';
  sprintf(&trennzeichen[1], leer_zeichen);
  const bool anfuehrungstriche = false; // keine Anführungsstriche setzen
  unsigned int dimensionen;             // Anzahl der zusammmengehörenden Werte
  int achse;                            // ausgewählter Wert aus dimensionen
  DBG_PROG_V( original_.size() )

  std::string::size_type pos=0, netz_pos=0;
  std::string::size_type ende, netz_ende;

  // nach Netzen in der vrml Datei suchen
  while( (netz_pos = original_.find( "IndexedFaceSet", netz_pos )) !=
        std::string::npos )
  {
    ++netz_pos;
    // Bereich des Netzes eingrenzen
    if( (netz_ende = original_.find( "IndexedFaceSet", netz_pos )) ==
        std::string::npos )
      netz_ende = original_.size();
    if( netz_pos == std::string::npos )
      break;
    DBG_NUM_S( "IndexedFaceSet gefunden auf Position " << netz_pos <<"-"<< netz_ende );

    arbeit_ = original_.substr (netz_pos, netz_ende-netz_pos+1);
    zeilen = zeilenNachVector (arbeit_);
    DBG_PROG_V( zeilen.size() <<"|"<< arbeit_.size() )

    // ab nun zeilenweise
    bool in_punkte = false;
    bool in_farben = false;
    bool in_indexe = false;
    netze_.resize( netze_.size()+1 );
    int endnetz = netze_.size()-1;
    std::string zeile;
    int geschweifte_klammer = 0;
    int punkt_n = 0, wert_n;
    for(unsigned int z = 0; z < zeilen .size(); z++)
    {
      // Hole eine kommentarfreie Zeile
      zeile = zeilen[z].substr( 0, zeilen[z].find( "#" ) );

      pos = 0;
      while( (pos = zeile.find("{",pos)) != std::string::npos ) {
        //DBG_NUM_V( zeilen[z] )
        ++geschweifte_klammer;
        ++pos;
      }
      pos = 0;
      while( (pos = zeile.find("}",pos)) != std::string::npos ) {
        //DBG_NUM_V( zeilen[z] )
        --geschweifte_klammer;
        ++pos;
      }

      #define VRMLBereichsTest(schalter, schluesselauf, schluesselzu) \
      if( zeile.find(schluesselauf) != std::string::npos ) { \
        DBG_NUM_S( z << schluesselauf ) \
        wert_n = punkt_n = 0; \
        schalter = true; DBG_NUM_V( schalter ) \
      } \
      if( zeile.find(schluesselzu) != std::string::npos && \
          schalter ) { \
        DBG_NUM_S( z << schluesselzu ) \
        schalter = false; \
      }

      if(geschweifte_klammer >= 1) {
        VRMLBereichsTest ( in_punkte, "point", "]" )
        VRMLBereichsTest ( in_farben, "Color", "}" )
        VRMLBereichsTest ( in_indexe, "coordIndex", "]" )
      }
      //DBG_NUM_S( z <<" "<< geschweifte_klammer <<" "<< in_punkte <<" "<< in_farben <<" "<< in_indexe )
      achse = 0;
      if( in_punkte )
      {
        werte =
          unterscheideZiffernWorte( zeile, anfuehrungstriche, trennzeichen );

        // ausgelesene Werte sortieren
        for(unsigned int w = 0; w < werte.size(); ++w)
        { //DBG_NUM_S( "w [" << w <<"]" << werte.size() <<" Netz: "<< endnetz <<" punkte "<< netze_[endnetz].punkte.size() )
          if(werte[w].zahl.first)
          {
            dimensionen = 3;
            achse = wert_n%dimensionen;
            //DBG_NUM_S( "w%3 " << wert_n%dimensionen <<" p_s "<< netze_[endnetz].punkte.size() <<" w/3 "<< wert_n/dimensionen )
            if(achse == 0)
            {
              if(netze_[endnetz].punkte.size() <= wert_n/dimensionen)
                netze_[endnetz].punkte. push_back (ICCnetzPunkt());

              punkt_n = wert_n/dimensionen;
            }

            //DBG_NUM_S( "w " << w <<" "<< werte[w].zahl.second )
            netze_[endnetz].punkte[punkt_n].koord[achse] =
                werte[w].zahl.second;

            //DBG_NUM_S( endnetz<<" punkt_n "<<punkt_n<<" achse "<<achse<<" "<< netze_[endnetz].punkte[punkt_n].koord[achse] )

            ++wert_n;
          } //else WARN_S(_("keine Zahl?? " << werte[w].zahl.second))
        }
      } else
      if( in_farben )
      {
        werte =
          unterscheideZiffernWorte( zeile, anfuehrungstriche, trennzeichen );

        // ausgelesene Werte sortieren
        for(unsigned int w = 0; w < werte.size(); ++w)
        {
          if(werte[w].zahl.first)
          {
            dimensionen = 3;
            achse = wert_n % dimensionen;
            if(achse == 0)
            {
              if(netze_[endnetz].punkte.size() <= wert_n/dimensionen)
                netze_[endnetz].punkte. push_back (ICCnetzPunkt());

              punkt_n = wert_n/dimensionen;
            } else if (achse == 2)
              netze_[endnetz].punkte[punkt_n].farbe[achse+1] = 1;

            netze_[endnetz].punkte[punkt_n].farbe[achse] =
                werte[w].zahl.second;

            ++wert_n;
          } //else WARN_S(_("keine Zahl?? " << werte[w].zahl.second))
        }
      } else // in_farben
      if( in_indexe )
      {
        werte =
          unterscheideZiffernWorte( zeile, anfuehrungstriche, trennzeichen );
        dimensionen = 4;

        // ausgelesene Werte sortieren
        for(unsigned int w = 0; w < werte.size(); ++w)
          if(werte[w].ganz_zahl.first)
            netze_[endnetz].indexe. push_back (werte[w].ganz_zahl.second);
      } // in_indexe
    }
    netz_n++;
  }
  DBG_PROG_ENDE
  icc_debug = debug_alt;
}

};



std::vector<ICCnetz>
extrahiereNetzAusVRML (std::string & vrml)
{
  ICCvrmlParser vrml_parser;
  vrml_parser.lade(vrml);
  return vrml_parser.zeigeNetze();
}



