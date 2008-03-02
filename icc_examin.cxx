// generated by Fast Light User Interface Designer (fluid) version 1.0104

#include "icc_examin.h"
static char *statlabel;
 char *filename_alt;
 bool setTitleUrl = true;
 using namespace std;
 int px,py,pw,ph;
 int fullscreen;
static openvrml::browser *browser = 0;
static ViewerFLTK  *viewer = 0;
#include "icc_draw.h"
ICCprofile profile;

Fl_Double_Window *details=(Fl_Double_Window *)0;

Fl_Menu_Bar *Fl_lookat_MenuBar=(Fl_Menu_Bar *)0;

static void cb_Offnen(Fl_Menu_*, void*) {
  open();
}

static void cb_Beenden(Fl_Menu_*, void*) {
  quit();
}

static void cb_Voll(Fl_Menu_*, void*) {
  Fl_Window *w = (Fl_Window *)details;

  if (!fullscreen) {
    px = w->x();
    py = w->y();
    pw = w->w();
    ph = w->h();

    w->fullscreen();
    fullscreen = true;
  };
}

static void cb_normal_ansicht(Fl_Menu_*, void*) {
  Fl_Window *w = (Fl_Window *)details;

    w->fullscreen_off(px,py,pw,ph);
    fullscreen = false;
}

Fl_Menu_Item menu_Fl_lookat_MenuBar[] = {
 {"Daten", 0,  0, 0, 64, 0, 0, 14, 56},
 {"Offnen", 0x4006f,  (Fl_Callback*)cb_Offnen, 0, 0, 0, 0, 14, 56},
 {"Beenden", 0x40071,  (Fl_Callback*)cb_Beenden, 0, 0, 0, 0, 14, 56},
 {0},
 {"Ansicht", 0,  0, 0, 64, 0, 0, 14, 56},
 {"Ganzer Bildschirm", 0,  (Fl_Callback*)cb_Voll, 0, 0, 0, 0, 14, 56},
 {"normales Fenster", 0,  (Fl_Callback*)cb_normal_ansicht, 0, 0, 0, 0, 14, 56},
 {0},
 {0}
};

Fl_Box *stat=(Fl_Box *)0;

Fl_Progress *load_progress=(Fl_Progress *)0;

TagBrowser *tag_browser=(TagBrowser *)0;

TagDrawings *tag_viewer=(TagDrawings *)0;

vFLGLWidget *canvas=(vFLGLWidget *)0;

Fl_Output *tag_texts=(Fl_Output *)0;

int main(int argc, char **argv) {
  Fl_Double_Window* w;
  filename_alt = (char*)calloc (sizeof (char), 1024);
  statlabel = (char*)calloc (sizeof (char), 1024);
  fullscreen = false;
  { Fl_Double_Window* o = details = new Fl_Double_Window(385, 520, "ICC Details");
    w = o;
    o->box(FL_NO_BOX);
    o->color((Fl_Color)53);
    { Fl_Group* o = new Fl_Group(-5, 0, 395, 520);
      { Fl_Menu_Bar* o = Fl_lookat_MenuBar = new Fl_Menu_Bar(0, 0, 385, 25);
        o->color((Fl_Color)53);
        o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        o->when(3);
        o->menu(menu_Fl_lookat_MenuBar);
      }
      { Fl_Group* o = new Fl_Group(0, 495, 390, 25);
        { Fl_Box* o = stat = new Fl_Box(0, 495, 385, 25, "No wrl file loaded.");
          o->box(FL_THIN_DOWN_BOX);
          o->color((Fl_Color)53);
          o->align(FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
        }
        { Fl_Progress* o = load_progress = new Fl_Progress(0, 495, 385, 25, "Laden ..");
          o->color((Fl_Color)53);
          o->hide();
          o->maximum(1.0);
          o->minimum(0.0);
        }
        o->end();
      }
      { Fl_Tile* o = new Fl_Tile(0, 25, 385, 470);
        { TagBrowser* o = tag_browser = new TagBrowser(0, 25, 385, 135, "Bitte w\344hlen Sie ein Profilmerkmal aus");
          o->box(FL_NO_BOX);
          o->color((Fl_Color)53);
          o->selection_color(FL_SELECTION_COLOR);
          o->labeltype(FL_NORMAL_LABEL);
          o->labelfont(0);
          o->labelsize(14);
          o->labelcolor(FL_BLACK);
          o->align(FL_ALIGN_TOP|FL_ALIGN_INSIDE);
          o->when(FL_WHEN_RELEASE_ALWAYS);
          int lines = tag_browser->size();
          cout << lines << endl;
        }
        { Fl_Group* o = new Fl_Group(0, 160, 385, 335);
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
          { vFLGLWidget* o = canvas = new vFLGLWidget(0, 160, 385, 335, "OpenVRML");
            o->box(FL_NO_BOX);
            o->color(FL_BACKGROUND_COLOR);
            o->selection_color(FL_BACKGROUND_COLOR);
            o->labeltype(FL_NORMAL_LABEL);
            o->labelfont(0);
            o->labelsize(14);
            o->labelcolor(FL_BLACK);
            o->align(FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
            o->when(FL_WHEN_RELEASE);
            o->hide();
          }
          { Fl_Output* o = tag_texts = new Fl_Output(0, 160, 385, 335);
            o->type(12);
            //o->hide();
            o->show();
          }
          o->end();
        }
        o->end();
        Fl_Group::current()->resizable(o);
      }
      o->end();
      Fl_Group::current()->resizable(o);
    }
    browser = new openvrml::browser(cout, cerr);
    DBG
    if (argc>1) {
      std::vector<std::string> url;
      std::vector<std::string> param;

      if (browser && argc>1) {
        url.push_back (argv[1]);
        browser->load_url(url, param);
        sprintf (statlabel, "%s geladen", argv[1]);
        stat->label(statlabel);
        sprintf(filename_alt, argv[1]);
      } else {
        stat->label("Error loading file!");
      }
    } // if
    DBG
    browser->add_world_changed_callback( worldChangedCB );
    worldChangedCB( openvrml::browser::replace_world_id );
    DBG
    viewer = new ViewerFLTK( *browser, canvas );
    canvas->setViewerPtr( viewer );
    o->end();
  }
  w->show();
  //canvas->show();
  viewer->Hok=1;
  viewer->Hdraw=1;
  viewer->timerUpdate();
  viewer->handleRedraw();
  Fl::add_timeout(0.01, (void (*)(void *))timeIT, (void *)viewer);

  Fl::scheme(NULL);
  Fl_File_Icon::load_system_icons();
  w->show(argc, argv);
  return Fl::run();
}

const char* open(void) {
  #include "icc_vrml.h"

  char *filename;
  //Fl_File_Icon	*icon;	// New file icon
  DBG
  load_progress->show ();    load_progress->value (0.0);
  char vrmlDatei[] = "/tmp/tmp_vrml.wrl";

  filename=fl_file_chooser("W�hle ICC Profil?", "ICC Farbprofile (*.[I,i][C,c][M,m,C,c])", filename_alt);
  DBG printf (filename_alt); printf ("\n");
  if (!filename) {
    return "";
    load_progress->hide ();
  }

  // Laden
  profile.load (filename);
  // Register the ICC type ...
  //Fl_Shared_Image::add_handler(icc_check);
  //Fl_Shared_Image::add_handler(ps_check);

  std::vector<std::string> url;
  std::vector<std::string> param;

  if (browser && filename) { DBG

    create_vrml ( filename, "/usr/share/color/icc/sRGB.icm", &vrmlDatei[0]);

    load_progress->value (0.8);
    sprintf (filename_alt, "%s", filename);
    url.push_back (&vrmlDatei[0]);
    browser->load_url(url, param);
    sprintf (statlabel, "%s geladen", filename);
    cout << statlabel << endl;
    stat->label(statlabel);
    DBG
  } else {
    stat->label("Error loading file!");
  } DBG

  stat->hide();
  stat->show();
  load_progress->value (1.0);
  viewer->timerUpdate();
  viewer->handleRedraw();
  load_progress->value (0.0);
  load_progress->hide();
  DBG

  tag_browser->reopen ();

  return filename;
}

void quit(void) {
  Fl::remove_timeout((void (*)(void *))timeIT, (void *)viewer);
  delete viewer;
  delete browser;
  delete canvas;
  details->hide();
  exit(0);
}

void worldChangedCB( const openvrml::browser::cb_reason reason ) {
  switch (reason)
    {
        case openvrml::browser::destroy_world_id:
            delete browser;
            delete viewer;
            Fl::remove_timeout((void (*)(void *))timeIT, (void *)viewer);
            exit(0);
            break;
        case openvrml::browser::replace_world_id: DBG
            if (setTitleUrl) { DBG
                stat->label(statlabel);//browser->world_url().c_str());
            }
            //buildViewpointMenu();
            break;
            DBG
    }
}

void timeIT() {
  if (canvas->LeftViewer == 0) {
    viewer->timerUpdate();
    viewer->handleRedraw();
  }
  Fl::add_timeout(0.01, (void (*)(void *))timeIT, (void *)viewer);
}

char* icc_read_info(char* filename) {
  char systemBefehl[1024];
  char *textfile = "/tmp/icc_temp.txt";


  sprintf (systemBefehl, "iccdump \"%s\" > %s",
                                    filename, textfile);
  system (systemBefehl);

  return textfile;
}

Fl_Double_Window* makeKurvenWindow() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(100, 100);
    w = o;
    o->end();
  }
  return w;
}

TagDrawings::TagDrawings(int X,int Y,int W,int H) : Fl_Widget(X,Y,W,H), X(X), Y(Y), W(W), H(H) {
}

void TagDrawings::draw() {
  draw_cie_shoe(x(),y(),w(),h());
  DBG
}

TagBrowser::TagBrowser(int X,int Y,int W,int H,char* start_info) : Fl_Hold_Browser(X,Y,W,H,start_info), X(X), Y(Y), W(W), H(H) {
}

void TagBrowser::draw_noe() {
  //draw_cie_shoe(x(),y(),w(),h());
  DBG
}

void TagBrowser::reopen() {
  std::stringstream s;
  std::string text;
  #define add_s(stream) s << stream; text = s.str(); add (text.c_str()); s.str("");

  clear();
  add_s ("@fDateiname:")
  add_s ( "@b    " << profile.filename() )
  add_s ("")
  add_s ("@B26@tNr. Bezeichner  Typ         Gr��e Beschreibung")
  add_s ("@t" << profile.print_header());
  select (5); // Header voreinstellen

  text = profile.print_long_header(); DBG
  tag_texts->show(); DBG
  tag_texts->value("");
  tag_texts->insert (text.c_str()); DBG
  tag_viewer->hide(); DBG
  canvas->hide(); DBG
}
