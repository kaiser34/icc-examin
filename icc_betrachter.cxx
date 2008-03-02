// generated by Fast Light User Interface Designer (fluid) version 1.0106

#include "icc_betrachter.h"
#include "icc_draw.h"
#include "icc_kette.h"
#include "icc_oyranos.h"
#include "fl_oyranos.h"
#include "icc_gl.h"
using namespace icc_examin_ns;

char* iccReadInfo(char* filename) {
  DBG_PROG_START
  char systemBefehl[1024];
  char *textfile = "/tmp/icc_temp.txt";


  sprintf (systemBefehl, "iccdump \"%s\" > %s",
                                    filename, textfile);
  system (systemBefehl);

  return textfile;
  DBG_PROG_ENDE
}

TagBrowser::TagBrowser(int X,int Y,int W,int H,char* start_info) : Fl_Hold_Browser(X,Y,W,H,start_info), X(X), Y(Y), W(W), H(H) {
}

void TagBrowser::reopen() {
  DBG_PROG_START
  //open and preparing the first selected item

  std::stringstream s;
  std::string text;
  std::vector<std::string> tag_list = profile.profil()->printTags();

  #define add_s(stream) s << stream; add (s.str().c_str()); s.str("");
  #define add_          s << " ";

  clear();
  add_s ("@fDateiname:")
  add_s ("@b    " << profile.profil()->filename() )
  add_s ("")
  if (tag_list.size() == 0) {
    add_s ("keine Inhalte gefunden f�r \"" << profile.profil()->filename() << "\"")
    return;
  }
  add_s ("@B26@tNr. Bezeichner  Typ         Gr��e Beschreibung")
  add_s ("@t" << profile.profil()->printHeader() )
  DBG_PROG
  std::vector<std::string>::iterator it;
  for (it = tag_list.begin() ; it != tag_list.end(); ++it) {
    s << "@t";
    // Nummer
    int Nr = atoi((*it).c_str()) + 1;
    std::stringstream t; t << Nr;
    for (int i = t.str().size(); i < 3; i++) {s << " ";} s << Nr; *it++; s << " "; 
    // Name/Bezeichnung
    s << *it; for (int i = (*it++).size(); i < 12; i++) {s << " ";}
    // Typ
    s << *it; for (int i = (*it++).size(); i < 12; i++) {s << " ";}
    // Gr��e
    for (int i = (*it).size(); i < 5; i++) {s << " ";} s << *it++; s << " ";
    // Beschreibung
    add_s (*it)
  }
  DBG_PROG
  if (value())
    selectItem (value()); // Anzeigen
  else
    selectItem (1);

  if (profile.profil()->hasTagName (selectedTagName)) {
    int item = profile.profil()->getTagByName (selectedTagName) + 6;
    selectItem (item);
    value(item);
  }

  std::string::size_type pos=0 , max = 0;
  std::string data = profile.profil()->filename(); DBG_NUM_S( data )
  while ((pos = data.find ("/", pos)) != std::string::npos) {
    if (pos > max) max = pos; pos++; max++;
  }
  data.erase (0, max); DBG_NUM_S( max << data )

  s.clear(); s << "ICC Details: " << data;
  status ((const char*) s.str().c_str() );
  DBG_PROG_ENDE
}

void TagBrowser::selectItem(int item) {
  DBG_PROG_START
  //Auswahl aus tag_browser

  status("")

  item -= 6;
  DBG_PROG_S( item << ". Tag " )
  
  std::string text = icc_examin->waehleTag(item);
  if (text != "")
    selectedTagName = text;
  DBG_PROG_V( text );
  DBG_PROG_ENDE
}

TagTexts::TagTexts(int X,int Y,int W,int H,char* start_info) : Fl_Hold_Browser(X,Y,W,H,start_info), X(X), Y(Y), W(W), H(H) {
}

void TagTexts::hinein(std::string text) {
  DBG_PROG_START
  //Text aus tag_browser anzeigen

      this->clear();

      std::vector <std::string> texte = icc_parser::zeilenNachVector( text );
      for (unsigned int i = 0; i < texte.size(); i++)
        this->add( texte[i].c_str(), 0);


      this->topline(0);
      this->textfont(FL_COURIER);
      this->textsize(14);
  DBG_PROG_ENDE
}
#include <FL/fl_draw.H>

MftChoice::MftChoice(int X,int Y,int W,int H,char* start_info) : Fl_Choice(X,Y,W,H,start_info), X(X), Y(Y), W(W), H(H) {
  gewaehlter_eintrag = 0;
}

void MftChoice::profilTag(int _tag, std::string text) {
  DBG_PROG_START
  icc_examin->icc_betrachter->tag_nummer = _tag;

// = profile.profil()->printTagInfo(icc_examin->icc_betrachter->tag_nummer);
    sprintf (&typ[0], profile.profil()->printTagInfo(icc_examin->icc_betrachter->tag_nummer)[1].c_str());

    DBG_PROG_V( profile.profil()->printTagInfo(icc_examin->icc_betrachter->tag_nummer)[1].c_str() )

    Info = icc_parser::zeilenNachVector (text);

    if ( strstr (typ,"mft2") != 0 )
    { DBG_PROG
      Fl_Menu_Item *mft_menue = (Fl_Menu_Item *)calloc (sizeof (Fl_Menu_Item), 6);

      mft_menue[0].text = Info[0].c_str();
      mft_menue[1].text = Info[4].c_str();
      mft_menue[2].text = Info[5].c_str();
      mft_menue[3].text = Info[6].c_str();
      mft_menue[4].text = Info[7].c_str();
      mft_menue[5].text = 0;
      icc_examin->icc_betrachter->mft_choice->menu(mft_menue);
    } else { DBG_PROG
      Fl_Menu_Item *mft_menue = (Fl_Menu_Item *)calloc (sizeof (Fl_Menu_Item), 6);

      mft_menue[0].text = Info[0].c_str();
      mft_menue[1].text = Info[4].c_str();
      mft_menue[2].text = "lineare Eingangskurve mit 256 Stufungen";
      mft_menue[3].text = Info[5].c_str();
      mft_menue[4].text = "lineare Ausgangskurve mit 256 Stufungen";
      mft_menue[5].text = 0;
      icc_examin->icc_betrachter->mft_choice->menu(mft_menue);
    }

    icc_examin->icc_betrachter->mft_choice->value( gewaehlter_eintrag );

  //auswahlCb();
  DBG_PROG_ENDE
}

void MftChoice::auswahlCb(void) {
  DBG_PROG_START
  //Auswahl aus mft_choice

  status("")

  Fl_Menu_* mw = (Fl_Menu_*)this;
  const Fl_Menu_Item* m = mw->mvalue();
  if (!m) {
    DBG_PROG_S("NULL \n")
  } else if (m->shortcut()) {
    DBG_PROG_S("%s - %s \n" << m->label() << fl_shortcut_label(m->shortcut()))
  } else {
    DBG_PROG_S("%s \n" << m->label())
  }

  icc_examin->waehleMft( mw->value() );

  DBG_PROG
  DBG_PROG_ENDE
}

inline void ICCfltkBetrachter::cb_ja_i(Fl_Button*, void*) {
  ueber->hide();
}
void ICCfltkBetrachter::cb_ja(Fl_Button* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_ja_i(o,v);
}

inline void ICCfltkBetrachter::cb_Gut_i(Fl_Button*, void*) {
  vcgt->hide();
}
void ICCfltkBetrachter::cb_Gut(Fl_Button* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_Gut_i(o,v);
}

inline void ICCfltkBetrachter::cb_Auffrischen_i(Fl_Button*, void*) {
  icc_examin->vcgtZeigen();
}
void ICCfltkBetrachter::cb_Auffrischen(Fl_Button* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_Auffrischen_i(o,v);
}

inline void ICCfltkBetrachter::cb_setze_i(Fl_Button*, void*) {
  icc_examin->moniSetzen();
}
void ICCfltkBetrachter::cb_setze(Fl_Button* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_setze_i(o,v);
}

inline void ICCfltkBetrachter::cb_Standard_i(Fl_Button*, void*) {
  icc_examin->standardGamma();
}
void ICCfltkBetrachter::cb_Standard(Fl_Button* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_Standard_i(o,v);
}

inline void ICCfltkBetrachter::cb_ffnen_i(Fl_Menu_*, void*) {
  icc_examin->oeffnen();
}
void ICCfltkBetrachter::cb_ffnen(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_ffnen_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_html_speichern_i(Fl_Menu_*, void*) {
  DBG_PROG_START

  icc_examin->berichtSpeichern ();

  DBG_PROG_ENDE;
}
void ICCfltkBetrachter::cb_menueintrag_html_speichern(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_html_speichern_i(o,v);
}

inline void ICCfltkBetrachter::cb_Beenden_i(Fl_Menu_*, void*) {
  quit();
}
void ICCfltkBetrachter::cb_Beenden(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_Beenden_i(o,v);
}

inline void ICCfltkBetrachter::cb_Voreinstellungen_i(Fl_Menu_*, void*) {
  voreinstellungen();
}
void ICCfltkBetrachter::cb_Voreinstellungen(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_Voreinstellungen_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_Voll_i(Fl_Menu_*, void*) {
  Fl_Window *w = (Fl_Window *)details;

  if (!fullscreen) {
    px = w->x();
    py = w->y();
    pw = w->w();
    ph = w->h();

    w->fullscreen();
    fullscreen = true;
  } else {
    w->fullscreen_off(px,py,pw,ph);
    fullscreen = false;
  };
}
void ICCfltkBetrachter::cb_menueintrag_Voll(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_Voll_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_inspekt_i(Fl_Menu_* o, void*) {
  Fl_Menu_* mw = (Fl_Menu_*)o;
  const Fl_Menu_Item* m = mw->mvalue();

  DBG_PROG_S (m->value())

  if (m->value())
  { bool export_html = false;
    inspekt_html->value(profile.profil()->report(export_html).c_str());
    inspekt_html->topline(tag_text->inspekt_topline);
  } else {
    tag_text->inspekt_topline = inspekt_html->topline();
  }
  icc_examin->neuzeichnen(inspekt_html);
}
void ICCfltkBetrachter::cb_menueintrag_inspekt(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_inspekt_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_3D_i(Fl_Menu_* o, void*) {
  Fl_Menu_* mw = (Fl_Menu_*)o;
  const Fl_Menu_Item* m = mw->mvalue();

  DBG_PROG_S (m->value())
  if (m->value()) {
    //icc_examin->histogram();
  }
  icc_examin->neuzeichnen(DD_histogram);
}
void ICCfltkBetrachter::cb_menueintrag_3D(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_3D_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_zeigcgats_i(Fl_Menu_* o, void*) {
  Fl_Menu_* mw = (Fl_Menu_*)o;
  const Fl_Menu_Item* m = mw->mvalue();

  DBG_PROG_S (m->value())

  icc_examin->zeigCGATS();
}
void ICCfltkBetrachter::cb_menueintrag_zeigcgats(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_zeigcgats_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_vcgt_i(Fl_Menu_*, void*) {
  vcgt->show();
icc_examin->vcgtZeigen();
}
void ICCfltkBetrachter::cb_menueintrag_vcgt(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_vcgt_i(o,v);
}

inline void ICCfltkBetrachter::cb_menueintrag_testkurven_i(Fl_Menu_*, void*) {
  vcgt->show();
icc_examin->testZeigen();
}
void ICCfltkBetrachter::cb_menueintrag_testkurven(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_menueintrag_testkurven_i(o,v);
}

inline void ICCfltkBetrachter::cb_ber_i(Fl_Menu_*, void*) {
  ueber->show();
ueber_html->value(getUeberHtml().c_str());
}
void ICCfltkBetrachter::cb_ber(Fl_Menu_* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->user_data()))->cb_ber_i(o,v);
}

Fl_Menu_Item ICCfltkBetrachter::menu_[] = {
 {"Daten", 0,  0, 0, 64, 0, 0, 14, 56},
 {"\326""ffnen", 0x4006f,  (Fl_Callback*)ICCfltkBetrachter::cb_ffnen, 0, 0, 0, 0, 14, 56},
 {"Bericht Speichern", 0,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_html_speichern, 0, 129, 0, 0, 14, 56},
 {"Beenden", 0x40071,  (Fl_Callback*)ICCfltkBetrachter::cb_Beenden, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0},
 {"Bearbeiten", 0,  0, 0, 64, 0, 0, 14, 56},
 {"Voreinstellungen", 0,  (Fl_Callback*)ICCfltkBetrachter::cb_Voreinstellungen, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0},
 {"Ansicht", 0,  0, 0, 192, 0, 0, 14, 56},
 {"Ganzer Bildschirm an/aus", 0x40076,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_Voll, 0, 128, 0, 0, 14, 56},
 {"Pr\374""fansicht", 0x40062,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_inspekt, 0, 3, 0, 0, 14, 56},
 {"3D Ansicht", 0x40068,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_3D, 0, 130, 0, 0, 14, 56},
 {"CGATS Ansicht", 0x40067,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_zeigcgats, 0, 1, 0, 0, 14, 56},
 {"Grafikkarten Gamma", 0x40074,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_vcgt, 0, 0, 0, 0, 14, 56},
 {"Test Kurven", 0x50074,  (Fl_Callback*)ICCfltkBetrachter::cb_menueintrag_testkurven, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0},
 {"Hilfe", 0,  0, 0, 64, 0, 0, 14, 56},
 {"\334""ber", 0,  (Fl_Callback*)ICCfltkBetrachter::cb_ber, 0, 0, 0, 0, 14, 56},
 {0,0,0,0,0,0,0,0,0},
 {0,0,0,0,0,0,0,0,0}
};
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_html_speichern = ICCfltkBetrachter::menu_ + 2;
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_Voll = ICCfltkBetrachter::menu_ + 9;
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_inspekt = ICCfltkBetrachter::menu_ + 10;
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_3D = ICCfltkBetrachter::menu_ + 11;
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_zeigcgats = ICCfltkBetrachter::menu_ + 12;
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_vcgt = ICCfltkBetrachter::menu_ + 13;
Fl_Menu_Item* ICCfltkBetrachter::menueintrag_testkurven = ICCfltkBetrachter::menu_ + 14;
Fl_Menu_Item* ICCfltkBetrachter::menu_hilfe = ICCfltkBetrachter::menu_ + 16;

inline void ICCfltkBetrachter::cb_tag_browser_i(TagBrowser* o, void*) {
  o->selectItem( o->value() );
}
void ICCfltkBetrachter::cb_tag_browser(TagBrowser* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->parent()->user_data()))->cb_tag_browser_i(o,v);
}

inline void ICCfltkBetrachter::cb_mft_choice_i(MftChoice* o, void*) {
  o->auswahlCb();
}
void ICCfltkBetrachter::cb_mft_choice(MftChoice* o, void* v) {
  ((ICCfltkBetrachter*)(o->parent()->parent()->parent()->parent()->parent()->user_data()))->cb_mft_choice_i(o,v);
}

Fl_Double_Window* ICCfltkBetrachter::init() {
  Fl_Double_Window* w;
  DBG_PROG_START
  fullscreen = false;
  setTitleUrl = true;

  #ifdef HAVE_FLU
    Flu_File_Chooser::favoritesTxt = _("Lesezeichen");
    Flu_File_Chooser::myComputerTxt = _("Heimverzeichnis");
    Flu_File_Chooser::myDocumentsTxt = _("Dokumente");
    Flu_File_Chooser::desktopTxt = _("Desktop");

    Flu_File_Chooser::filenameTxt = _("Dateiname");
    Flu_File_Chooser::okTxt = _("Ja");
    Flu_File_Chooser::cancelTxt = _("Abbrechen");
    Flu_File_Chooser::locationTxt = _("Ordner");
    Flu_File_Chooser::showHiddenTxt = _("zeige versteckte Dateien");
    Flu_File_Chooser::fileTypesTxt = _("Datei Typen");
    Flu_File_Chooser::directoryTxt = _("Verzeichnis");
    Flu_File_Chooser::allFilesTxt = _("Alle Dateien (*)");
    Flu_File_Chooser::defaultFolderNameTxt = _("Neues Verzeichnis");

    Flu_File_Chooser::backTTxt = _("vorheriges Verzeichnis");
    Flu_File_Chooser::forwardTTxt = _("n�chstes Verzeichnis");
    Flu_File_Chooser::upTTxt = _("n�chsth�heres Verzeichnis");
    Flu_File_Chooser::reloadTTxt = _("Auffrischen");
    Flu_File_Chooser::trashTTxt = _("L�schen");
    Flu_File_Chooser::newDirTTxt = _("Verzeichnis erstellen");
    Flu_File_Chooser::addFavoriteTTxt = _("zu Lesezeichen");
    Flu_File_Chooser::previewTTxt = _("Vorschau");
    Flu_File_Chooser::listTTxt = _("Standard Anzeige");
    Flu_File_Chooser::wideListTTxt = _("weite Anzeige");
    Flu_File_Chooser::detailTTxt = _("detailierte Informationen");

    Flu_File_Chooser::detailTxt[0] = _("Name");
    Flu_File_Chooser::detailTxt[3] = _("Typ");
    Flu_File_Chooser::detailTxt[1] = _("Gr�sse");
    Flu_File_Chooser::detailTxt[2] = _("Datum");
    Flu_File_Chooser::contextMenuTxt[0] = _("contextMenuTxt 0");
    Flu_File_Chooser::contextMenuTxt[1] = _("Umbenennen");
    Flu_File_Chooser::contextMenuTxt[2] = _("L�schen");
    Flu_File_Chooser::diskTypesTxt[0] = _("Typ1");
    Flu_File_Chooser::diskTypesTxt[1] = _("Typ2");
    Flu_File_Chooser::diskTypesTxt[2] = _("Typ3");
    Flu_File_Chooser::diskTypesTxt[3] = _("Typ4");
    Flu_File_Chooser::diskTypesTxt[4] = _("Typ5");
    Flu_File_Chooser::diskTypesTxt[5] = _("Typ6");

    Flu_File_Chooser::createFolderErrTxt = _("Ordner konnte nicht erstellt werden");
    Flu_File_Chooser::deleteFileErrTxt = _("konnte Datei nicht l�schen");
    Flu_File_Chooser::fileExistsErrTxt = _("Datei existiert");
    Flu_File_Chooser::renameErrTxt = _("kontte nicht umbenennen");

/*    Flu_File_Chooser:: = _("");
    Flu_File_Chooser:: = _("");
    Flu_File_Chooser:: = _("");
    Flu_File_Chooser:: = _("");
    Flu_File_Chooser:: = _("");*/

    const char* ptr = NULL;
    if (profile.size())
      ptr = profile.name().c_str();
    dateiwahl = new Flu_File_Chooser(ptr, _("ICC Farbprofile (*.ic*)"), Flu_File_Chooser::SINGLE, _("Welches ICC Profil?"));
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "icc", _("Profil �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "icm", _("Profil �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "it8", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "txt", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "IT8", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "Q60", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "LAB", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "CMYK", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "DLY", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
    dateiwahl->add_context_handler(Flu_File_Chooser::ENTRY_FILE, "nCIE", _("Messdatei �ffnen"), dateiwahl_cb, NULL);
  #else
    const char* ptr = NULL;
    if (profile.size())
      ptr = profile.name().c_str();
    dateiwahl = new Fl_File_Chooser(ptr, _("ICC Farbprofile (*.{I,i}{C,c}{M,m,C,c})"), Fl_File_Chooser::SINGLE, _("Welches ICC Profil?"));
    dateiwahl->callback(dateiwahl_cb);
    dateiwahl->preview_label = _("Vorschau");
  #endif
  { Fl_Double_Window* o = ueber = new Fl_Double_Window(366, 241, "\334""ber ICC examin");
    w = o;
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(0, 0, 365, 240);
      { Fl_Help_View* o = ueber_html = new Fl_Help_View(0, 0, 365, 205);
        Fl_Group::current()->resizable(o);
      }
      { Fl_Button* o = ja = new Fl_Button(130, 210, 110, 25, "Gut");
        o->callback((Fl_Callback*)cb_ja);
      }
      o->end();
    }
    o->hide();
    o->set_non_modal();
    o->end();
    o->resizable(o);
  }
  { Fl_Double_Window* o = vcgt = new Fl_Double_Window(370, 390, "Grafikkarten Gamma Tabellen");
    w = o;
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(0, 0, 370, 390);
      { TagDrawings* o = vcgt_viewer = new TagDrawings(0, 0, 370, 360);
        o->box(FL_NO_BOX);
        o->color(FL_BACKGROUND_COLOR);
        o->selection_color(FL_BACKGROUND_COLOR);
        o->labeltype(FL_NORMAL_LABEL);
        o->labelfont(0);
        o->labelsize(14);
        o->labelcolor(FL_BLACK);
        o->align(FL_ALIGN_CENTER);
        o->when(FL_WHEN_RELEASE);
        o->show();
      }
      { Fl_Button* o = new Fl_Button(290, 360, 75, 25, "Gut");
        o->callback((Fl_Callback*)cb_Gut);
      }
      { Fl_Button* o = new Fl_Button(185, 360, 105, 25, "Auffrischen");
        o->callback((Fl_Callback*)cb_Auffrischen);
        w->hotspot(o);
      }
      { Fl_Button* o = new Fl_Button(80, 360, 105, 25, "setze Gamma");
        o->callback((Fl_Callback*)cb_setze);
        w->hotspot(o);
      }
      { Fl_Button* o = new Fl_Button(5, 360, 75, 25, "Standard");
        o->callback((Fl_Callback*)cb_Standard);
        w->hotspot(o);
      }
      o->end();
    }
    o->hide();
    o->set_non_modal();
    o->end();
    o->resizable(o);
  }
  { Fl_Double_Window* o = details = new Fl_Double_Window(385, 520, "ICC Details");
    w = o;
    o->box(FL_NO_BOX);
    o->color((Fl_Color)53);
    o->user_data((void*)(this));
    { Fl_Group* o = new Fl_Group(0, 0, 385, 520);
      { Fl_Menu_Bar* o = new Fl_Menu_Bar(0, 0, 385, 25);
        o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        o->when(3);
        { Fl_Menu_Item* o = &menu_[13];
          o->hide();
        }
        o->menu(menu_);
      }
      { Fl_Tile* o = examin = new Fl_Tile(0, 25, 385, 470);
        { Fl_Help_View* o = inspekt_html = new Fl_Help_View(0, 25, 385, 470, "Inspect");
          o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
          o->hide();
        }
        { TagBrowser* o = tag_browser = new TagBrowser(0, 25, 385, 135, "Bitte w\344hlen Sie ein Profilmerkmal aus");
          o->box(FL_NO_BOX);
          o->color(FL_BACKGROUND_COLOR);
          o->selection_color(FL_SELECTION_COLOR);
          o->labeltype(FL_NORMAL_LABEL);
          o->labelfont(0);
          o->labelsize(14);
          o->labelcolor(FL_BLACK);
          o->textcolor(32);
          o->callback((Fl_Callback*)cb_tag_browser);
          o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
          o->when(FL_WHEN_RELEASE_ALWAYS);
          int lines = tag_browser->size();
          DBG_PROG_V( lines )
        }
        { Fl_Group* o = ansichtsgruppe = new Fl_Group(0, 160, 385, 335);
          { Fl_Group* o = tabellengruppe = new Fl_Group(0, 160, 385, 335);
            { MftChoice* o = mft_choice = new MftChoice(0, 160, 385, 25, "W\344hlen Sie ein Kettenmerkmal");
              o->box(FL_NO_BOX);
              o->down_box(FL_BORDER_BOX);
              o->color(FL_BACKGROUND_COLOR);
              o->selection_color(FL_SELECTION_COLOR);
              o->labeltype(FL_NORMAL_LABEL);
              o->labelfont(0);
              o->labelsize(14);
              o->labelcolor(FL_BLACK);
              o->callback((Fl_Callback*)cb_mft_choice);
              o->align(FL_ALIGN_LEFT);
              o->when(FL_WHEN_RELEASE);
              o->show();
            }
            { TagDrawings* o = mft_viewer = new TagDrawings(0, 185, 385, 310);
              o->box(FL_NO_BOX);
              o->color(FL_BACKGROUND_COLOR);
              o->selection_color(FL_BACKGROUND_COLOR);
              o->labeltype(FL_NORMAL_LABEL);
              o->labelfont(0);
              o->labelsize(14);
              o->labelcolor(FL_BLACK);
              o->align(FL_ALIGN_CENTER);
              o->when(FL_WHEN_RELEASE);
              o->show();
            }
            { TagTexts* o = mft_text = new TagTexts(0, 185, 385, 310, "Der Text");
              o->box(FL_NO_BOX);
              o->color((Fl_Color)48);
              o->selection_color(FL_SELECTION_COLOR);
              o->labeltype(FL_NORMAL_LABEL);
              o->labelfont(0);
              o->labelsize(14);
              o->labelcolor(FL_BLACK);
              o->textcolor(32);
              o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
              o->when(FL_WHEN_RELEASE_ALWAYS);
              o->show();
            }
            { GL_Ansicht* o = mft_gl = new GL_Ansicht(0, 185, 385, 310);
              o->box(FL_NO_BOX);
              o->color(FL_BACKGROUND_COLOR);
              o->selection_color(FL_BACKGROUND_COLOR);
              o->labeltype(FL_NORMAL_LABEL);
              o->labelfont(0);
              o->labelsize(14);
              o->labelcolor(FL_BLACK);
              o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
              o->when(FL_WHEN_RELEASE);
              o->hide();
            }
            o->show();
            o->end();
          }
          { TagDrawings* o = tag_viewer = new TagDrawings(0, 160, 385, 335);
            o->box(FL_NO_BOX);
            o->color(FL_BACKGROUND_COLOR);
            o->selection_color(FL_BACKGROUND_COLOR);
            o->labeltype(FL_NORMAL_LABEL);
            o->labelfont(0);
            o->labelsize(14);
            o->labelcolor(FL_BLACK);
            o->align(FL_ALIGN_CENTER);
            o->when(FL_WHEN_RELEASE);
            o->hide();
          }
          { TagTexts* o = tag_text = new TagTexts(0, 160, 385, 335, "Der Text");
            o->box(FL_NO_BOX);
            o->color(FL_BACKGROUND_COLOR);
            o->selection_color(FL_SELECTION_COLOR);
            o->labeltype(FL_NORMAL_LABEL);
            o->labelfont(0);
            o->labelsize(14);
            o->labelcolor(FL_BLACK);
            o->textcolor(32);
            o->align(FL_ALIGN_BOTTOM|FL_ALIGN_INSIDE);
            o->when(FL_WHEN_RELEASE_ALWAYS);
            o->show();
          }
          o->end();
        }
        { GL_Ansicht* o = DD_histogram = new GL_Ansicht(0, 25, 385, 470);
          o->box(FL_NO_BOX);
          o->color(FL_BACKGROUND_COLOR);
          o->selection_color(FL_BACKGROUND_COLOR);
          o->labeltype(FL_NORMAL_LABEL);
          o->labelfont(0);
          o->labelsize(14);
          o->labelcolor(FL_BLACK);
          o->align(FL_ALIGN_CENTER);
          o->when(FL_WHEN_RELEASE);
          o->hide();
        }
        o->end();
        Fl_Group::current()->resizable(o);
      }
      { Fl_Group* o = new Fl_Group(0, 495, 385, 25);
        { Fl_Box* o = box_stat = new Fl_Box(0, 495, 385, 25, "No wrl file loaded.");
          o->box(FL_THIN_DOWN_BOX);
          o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        }
        { Fl_Progress* o = load_progress = new Fl_Progress(0, 495, 385, 25, "Laden ..");
          o->hide();
          o->maximum(1.0);
          o->minimum(0.0);
        }
        o->end();
      }
      o->end();
    }
    o->end();
  }
  tag_text->inspekt_topline = 0;

  w->resizable(tag_text);
  w->show();
  Fl::scheme(NULL);
  Fl_File_Icon::load_system_icons();
  DBG_PROG_ENDE
  return w;
}

void ICCfltkBetrachter::run() {
  DBG_PROG_START
  DBG_PROG_ENDE;  Fl::run();
}

std::vector<std::string> ICCfltkBetrachter::open(std::vector<std::string> dateinamen) {
  DBG_PROG_START
  #include "icc_vrml.h"

  //Fl_File_Icon	*icon;	// New file icon
  DBG_PROG
  load_progress->show ();    load_progress->value (0.0);

    const char* ptr = NULL;
    if (dateinamen.size()) {
      ptr = dateinamen[0].c_str();
      dateiwahl->value(ptr);
      //DBG_PROG_S( dateinamen[0])
    }
    dateiwahl->show(); //filename=fl_file_chooser("W�hle ICC Profil?", "ICC Farbprofile (*.{I,i}{C,c}{M,m,C,c})", filenamen_alt[0].c_str());

    


    while (dateiwahl->visible())
      Fl::wait();

    DBG_NUM_V( dateiwahl->count() )
    if (dateiwahl->count() && dateiwahl->value()) {
      DBG_NUM_V( dateiwahl->value() )
      dateinamen.resize(dateiwahl->count());
      for (int i = 1; i <= dateiwahl->count(); i++)
        dateinamen[i-1] = dateiwahl->value(i);
    }
  DBG_PROG

  if (dateinamen.size() == 0) {
    load_progress->hide ();

    DBG_PROG_ENDE
    return dateinamen;
  }

  DBG_PROG_ENDE
  return dateinamen;
}

void ICCfltkBetrachter::quit(void) {
  DBG_PROG_START
  /*Fl::remove_timeout((void (*)(void *))timeIT, (void *)viewer);
  delete viewer;
  delete browser;
  delete canvas;*/
  details->hide();
  icc_examin->quit();
  DBG_PROG_ENDE
}

void ICCfltkBetrachter::zeig_mich_(void* widget) {
  DBG_PROG_START
  // zeigt das ausgew�hlte Fenster (widget)

  mft_viewer->hide();
  mft_text->hide();
  tag_viewer->hide(); DBG_PROG
  tag_viewer->clear_visible(); DBG_PROG
  tag_text->hide();
  inspekt_html->hide();

  // stop
  if (widget != mft_gl) {
    mft_gl->stop();
    mft_gl->hide();
  }
  if (widget != DD_histogram ) {
    DD_histogram->stop();
    DD_histogram->hide();
  }

  // start
  if( widget == inspekt_html )
  { DBG_PROG_S("inspekt_html behandeln")
    if( menueintrag_inspekt->value() )
      inspekt_html->show();
    else
      inspekt_html->hide();
  }

  if (widget != mft_gl &&
      widget != DD_histogram )
  { 
    ((Fl_Widget*)widget)->show(); DBG_PROG
    if( !menueintrag_inspekt->value() &&
        !menueintrag_3D->value() )
      ;//selectItem(icc_examin->tag_nr()+6);
  } else if (widget == mft_gl) {
    DBG_PROG_S( "mft GL Fenster belassen." )
    mft_gl->show();
  } else if (widget == DD_histogram) {
    DBG_PROG_S( "3D GL Fenster belassen." )
    DD_histogram->show();
  }
  DBG_PROG_ENDE
}

void ICCfltkBetrachter::measurement(bool has_measurement) {
  if (has_measurement) {
    DBG_PROG_S(menueintrag_inspekt->value())
    if (menueintrag_inspekt->value()) {
      inspekt_html->value(profile.profil()->report(false).c_str());
      if (inspekt_html->size() -75 < tag_text->inspekt_topline)
        inspekt_html->topline (inspekt_html->size() - 75);
      else
        inspekt_html->topline (tag_text->inspekt_topline);
    }
    menueintrag_inspekt->activate();
    menueintrag_html_speichern->activate();
    menueintrag_zeigcgats->activate();
  } else {
    menueintrag_inspekt->deactivate();
    menueintrag_html_speichern->deactivate();
    menueintrag_zeigcgats->deactivate();
  }
}

void dHaendler(void* o) {
  DBG_PROG_START
  Fl::remove_timeout( (void(*)(void*))dHaendler, 0 );

  if (!Fl::has_timeout( (void(*)(void*))dHaendler, 0 )
   && ((TagDrawings*)o)->active()
   && ((TagDrawings*)o)->visible_r()
   && ((TagDrawings*)o)->wiederholen)
  {
    ((TagDrawings*)o)->ruhigNeuzeichnen();

    #ifdef DEBUG
    DBG_PROG_V( ((TagDrawings*)o)->wiederholen )
    #endif
  }
  DBG_PROG_ENDE
}
