// Author:    Kai-Uwe Behrmann
// Copyright: Kai-Uwe Behrmann <ku.b@gmx.de>
// Date:      04. 05. 2004

#if 1
  #ifndef DEBUG
   #define DEBUG
  #endif
  #define DEBUG_ICCHEADER
  #define DEBUG_ICCTAG
  #define DEBUG_ICCPROFILE
#endif

#include "icc_profile.h"
#include "icc_examin.h"
#include "icc_utils.h"

#define _(text) text
#define g_message printf

unsigned int
icValue (icUInt16Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  #define BYTES 2
  #define KORB  4
  unsigned char        *temp  = (unsigned char*) &val;
  static unsigned char  korb[KORB];
  for (int i = 0; i < KORB ; i++ )
    korb[i] = (int) 0;

  int klein = 0,
      gross = BYTES - 1;
  for (; klein < BYTES ; klein++ )
    korb[klein] = temp[gross--];

  unsigned int *erg = (unsigned int*) &korb[0];

  #ifdef DEBUG
  cout << *erg << " Gr��e nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; DBG
  #endif
  return (long)*erg;
#else
  return (long)val;
#endif
}

unsigned int
icValue (icUInt32Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned char        *temp = (unsigned char*) &val;

  static unsigned char  uint32[4];

  uint32[0] = temp[3];
  uint32[1] = temp[2];
  uint32[2] = temp[1];
  uint32[3] = temp[0];

  unsigned int *erg = (unsigned int*) &uint32[0];

  #ifdef DEBUG
  cout << *erg << " Gr��e nach Wandlung " << (int)temp[0] << " "
       << (int)temp[1] << " " << (int)temp[2] << " " <<(int)temp[3]
       << " "; DBG
  #endif

  return (int) *erg;
#else
  #ifdef DEBUG
  cout << "BIG_ENDIAN" << " "; DBG
  #endif
  return (int)val;
#endif
}

unsigned long
icValue (icUInt64Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  unsigned char        *temp  = (unsigned char*) &val;

  static unsigned char  uint64[8];
  int little = 0,
      big    = 8;

  for (; little < 8 ; little++ ) {
    uint64[little] = temp[big--];
  }

  unsigned long *erg = (unsigned long*) &uint64[0];

  #ifdef DEBUG
  cout << *erg << " Gr��e nach Wandlung " << (int)temp[0] << " "
       << (int)temp[1] << " " << (int)temp[2] << " " <<(int)temp[3]
       << " "; DBG
  #endif
  return (long)*erg;
#else
  return (long)val;
#endif
}

signed int
icValue (icInt32Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  #define BYTES 4
  #define KORB  4
  unsigned char        *temp  = (unsigned char*) &val;
  //unsigned char         maske = 0xf0;
  static unsigned char  korb[KORB];
  for (int i = 0; i < KORB ; i++ )
    korb[i] = (int) 0;
  #ifdef DEBUG_
  cout << " Gr��e nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; DBG
  #endif

  int klein = 0,
      gross = BYTES - 1;
  for (; klein < BYTES ; klein++ )
      korb[klein] = temp[gross--];

  signed int *erg = (signed int*) &korb[0];
  #ifdef DEBUG_
  cout << (double)*erg/65536.0 << " Gr��e nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; printf ("0x%X ", *erg); DBG
  #endif
  #ifdef DEBUG_
  cout << (double)*erg/65536.0 << " Gr��e nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; DBG
  #endif
  /*if (temp[0] & maske) {
    cout << " negativer Wert " << (signed int)korb[4] << " maske " << (int)maske << " "; DBG
    korb[4] = korb[4] | maske;
    #ifdef DEBUG
    cout << " negativer Wert " << (signed int)korb[4]; DBG
    #endif
  }*/

  #ifdef DEBUG
  cout << *erg << " Gr��e nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; DBG
  #endif
  return (signed int)*erg;
#else
  return (signed int)val;
#endif
}

signed int
icValue (icInt16Number val)
{
#if BYTE_ORDER == LITTLE_ENDIAN
  #define BYTES 2
  #define KORB  4
  unsigned char        *temp  = (unsigned char*) &val;
  static unsigned char  korb[KORB];
  for (int i = 0; i < KORB ; i++ )
    korb[i] = (int) 0;

  int klein = 0,
      gross = BYTES - 1;
  for (; klein < BYTES ; klein++ )
      korb[klein] = temp[gross--];

  signed int *erg = (signed int*) &korb[0];
  #ifdef DEBUG
  cout << *erg << " Gr��e nach Wandlung " << (int)korb[0] << " "
       << (int)korb[1] << " " << (int)korb[2] << " " <<(int)korb[3]
       << " "; DBG
  #endif
  return (signed int)*erg;
#else
  return (signed int)val;
#endif
}

double
icValueSF (icS15Fixed16Number val)
{
  return icValue(val) / 65536.0;
}

icColorSpaceSignature
icValue (icColorSpaceSignature val)
{
  icUInt32Number i = val;
  return (icColorSpaceSignature) icValue (i);
}

#define icValue_to_icUInt32Number(typ) \
typ \
icValue (typ val) \
{ \
  icUInt32Number i = val; \
  return (typ) icValue (i); \
}

icValue_to_icUInt32Number(icPlatformSignature)
icValue_to_icUInt32Number(icProfileClassSignature)
icValue_to_icUInt32Number(icTagSignature)
icValue_to_icUInt32Number(icTagTypeSignature)


/**
  *  @brief ICCheader Funktionen
  */ 

ICCheader::ICCheader()
{
  valid = false;
}

void
ICCheader::load (void *data)
{
  DBG
  memcpy ((void*)&header, data,/* (void*)&header,*/ sizeof (icHeader));
  #ifdef DEBUG_ICCHEADER
  cout << sizeof (icHeader) << " genommen" << " "; DBG
  #endif
  if (header.size > 0) {
    valid = true;
  #ifdef DEBUG_ICCHEADER
    cout << size() << " "; DBG
  #endif
  } else {
    valid = false;
  }
  DBG
}

std::string
ICCheader::renderingIntent (void)
{
  std::stringstream s;

  switch (header.renderingIntent) 
    {
    case 0:
      s << _("Fotographisch");
      break;
    case 1:
      s << _("relativ Farbmetrisch");
      break;
    case 2:
      s << _("Ges�ttigt");
      break;
    case 3:
      s << _("absolut Farbmetrisch");
      break;
    }
  return s.str();
}

std::string
ICCheader::attributes (void)
{
  std::stringstream s;
  char* f = (char*) &(header.attributes);
  unsigned char         maske1 = 0xf0;
  unsigned char         maske2 = 0x08;
  unsigned char         maske3 = 0x04;
  unsigned char         maske4 = 0x02;

  if (f[0] & maske1)
    s << _("Durchsicht, ");
  else
    s << _("Aufsicht, ");

  if (f[0] & maske2)  
    s << _("matt, ");
  else
    s << _("gl�nzend, ");

  if (f[0] & maske3)  
    s << _("negativ, ");
  else
    s << _("positiv, ");

  if (f[0] & maske4)  
    s << _("schwarz/weiss");
  else
    s << _("farbig");


  #ifdef DEBUG
  cout << f[0] << " " << (long)header.attributes; DBG
  char* ptr = (char*) &(header.attributes);
  for (int i = 0; i < 8 ; i++)
    cout << (int)ptr[i] << " ";
  DBG
  #endif
  return s.str();
}

std::string
ICCheader::flags (void)
{
  std::stringstream s;
  char* f = (char*) &(header.flags);
  unsigned char         maske1 = 0xf0;
  unsigned char         maske2 = 0x08;

  if (f[0] & maske1)
    s << _("Farbprofil ist eingebettet und ");
  else
    s << _("Farbprofil ist nicht eingebettet und ");

  if (f[0] & maske2)  
    s << _("kann nicht unabh�ngig vom Bild verwendet werden.");
  else
    s << _("kann unabh�ngig vom Bild verwendet werden.");

  #ifdef DEBUG
  cout << f[0] << " " << (long)header.flags; DBG
  char* ptr = (char*) &(header.flags);
  for (int i = 0; i < 8 ; i++)
    cout << (int)ptr[i] << " ";
  DBG
  #endif
  return s.str();
}

std::string
ICCheader::versionName (void)
{
  std::stringstream s;
  char* v = (char*)&(header.version);
  
  s << (int)v[0] << "." << (int)v[1]/16 << "." << (int)v[1]%16;

  return s.str();
}

std::string
ICCheader::print_long()
{
  #ifdef DEBUG_ICCHEADER
  cout << sizeof (icSignature) << " " << sizeof (icUInt32Number)<< endl;
  #endif
  std::string cm = cmmName();
  std::string mg = magicName();
  std::string ma = manufacturerName();
  std::string mo = modelName();
  std::string cr = creatorName();
  std::stringstream s; DBG
  s << "kein Dateikopf gefunden"; DBG
  
  //cout << "char icSignature ist " << sizeof (icSignature) << endl;
  if (valid) { DBG
    s.str("");
    s << "ICC Dateikopf:\n"<< endl \
      <<  "    " << _("Gr��e") << ":       " <<
                       size() << " " << _("bytes") << endl \
      <<  "    " << _("CMM") << ":         " << cm << endl \
      <<  "    " << _("Version") << ":     " << versionName() << endl \
      <<  "    " << _("Klasse") << ":      " <<
                       getDeviceClassName(deviceClass()) << endl \
      <<  "    " << _("Farbraum") << ":    " <<
                       getColorSpaceName(colorSpace()) << endl \
      <<  "    " << _("PCS") << ":         " <<
                       getColorSpaceName(pcs()) << endl \
      <<  "    " << _("Datum") << ":       " <<
                       icValue(header.date.day)     << "/" <<
                       icValue(header.date.month)   << "/" <<
                       icValue(header.date.year)    << " " <<
                       icValue(header.date.hours)   << ":" <<
                       icValue(header.date.minutes) << " " << _("Uhr") << " " <<
                       icValue(header.date.seconds) << " " << _("Sekunden") << endl \
      <<  "    " << _("Magic") << ":       " << mg << endl \
      <<  "    " << _("Plattform") << ":   " << platform() << endl \
      <<  "    " << _("Flags") << ":       " << flags() << endl \
      <<  "    " << _("Hersteller") << ":  " << ma << endl \
      <<  "    " << _("Model") << ":       " << mo << endl \
      <<  "    " << _("Attribute") << ":   " <<
                       attributes() << endl \
      <<  "    " << _("�bertragung") << ": " <<
                       renderingIntent() << endl \
      <<  "    " << _("Beleuchtung") << ": X=" <<
                       icValueSF(header.illuminant.X) << ", Y=" << \
                       icValueSF(header.illuminant.Y) << ", Z=" << \
                       icValueSF(header.illuminant.Z) << endl \
      <<  "    " << _("erzeugt von") << ": " << cr << endl ;
  } DBG
  return s.str();
}

std::string
ICCheader::print()
{
  string s = _("Dateikopf ung�ltig");
  if (valid)
    s = _("    Kopf        icHeader      128 Dateikopf");
  return s;
}


std::string
ICCheader::getColorSpaceName (icColorSpaceSignature color)
{
  std::string text;

  switch (color) {
    case icSigXYZData: text =_("XYZ"); break;
    case icSigLabData: text =_("Lab"); break;
    case icSigLuvData: text =_("Luv"); break;
    case icSigYCbCrData: text =_("YCbCr"); break;
    case icSigYxyData: text =_("Yxy"); break;
    case icSigRgbData: text =_("Rgb"); break;
    case icSigGrayData: text =_("Gray"); break;
    case icSigHsvData: text =_("Hsv"); break;
    case icSigHlsData: text =_("Hls"); break;
    case icSigCmykData: text =_("Cmyk"); break;
    case icSigCmyData: text =_("Cmy"); break;
    case icSig2colorData: text =_("2color"); break;
    case icSig3colorData: text =_("3color"); break;
    case icSig4colorData: text =_("4color"); break;
    case icSig5colorData: text =_("5color"); break;
    case icSig6colorData: text =_("6color"); break;
    case icSig7colorData: text =_("7color"); break;
    case icSig8colorData: text =_("8color"); break;
    case icSig9colorData: text =_("9color"); break;
    case icSig10colorData: text =_("10color"); break;
    case icSig11colorData: text =_("11color"); break;
    case icSig12colorData: text =_("12color"); break;
    case icSig13colorData: text =_("13color"); break;
    case icSig14colorData: text =_("14color"); break;
    case icSig15colorData: text =_("15color"); break;
    default: text =_(""); break;
  }
  return text;
}

std::string
ICCheader::getDeviceClassName (icProfileClassSignature deviceClass)
{
  std::string text;

  switch (deviceClass)
  {
    case icSigInputClass: text =_("Eingabe"); break;
    case icSigDisplayClass: text =_("Monitor"); break;
    case icSigOutputClass: text =_("Ausgabe"); break;
    case icSigLinkClass: text =_("Verkn�pfung"); break;
    case icSigAbstractClass: text =_("Abstrakter Farbraum"); break;
    case icSigColorSpaceClass: text =_("Farbraum"); break;
    case icSigNamedColorClass: text =_("Schmuckfarben"); break;
    default: text =_(""); break;
  }
  return text;
}

std::string
ICCheader::getPlatformName (icPlatformSignature platform)
{
  std::string text;

  switch (platform)
  {
    case icSigMacintosh: text =_("Macintosh"); break;
    case icSigMicrosoft: text =_("Microsoft"); break;
    case icSigSolaris: text =_("Solaris"); break;
    case icSigSGI: text =_("SGI"); break;
    case icSigTaligent: text =_("Taligent"); break;
    default: text =_(""); break;
  }
  return text;
}

/*int
ICCheader::getLcmsGetColorSpace ( cmsHPROFILE   hProfile)
{
  int lcmsColorSpace;

  #if (LCMS_VERSION >= 113)
    lcmsColorSpace = _cmsLCMScolorSpace ( cmsGetColorSpace (hProfile) );
  #else
  switch (cmsGetColorSpace (hProfile))
  {
    case icSigXYZData: lcmsColorSpace = PT_XYZ; break;
    case icSigLabData: lcmsColorSpace = PT_Lab; break;
    case icSigLuvData: lcmsColorSpace = PT_YUV; break;
    case icSigLuvKData: lcmsColorSpace = PT_YUVK; break;
    case icSigYCbCrData: lcmsColorSpace = PT_YCbCr; break;
    case icSigYxyData: lcmsColorSpace = PT_Yxy; break;
    case icSigRgbData: lcmsColorSpace = PT_RGB; break;
    case icSigGrayData: lcmsColorSpace = PT_GRAY; break;
    case icSigHsvData: lcmsColorSpace = PT_HSV; break;
    case icSigHlsData: lcmsColorSpace = PT_HLS; break;
    case icSigCmykData: lcmsColorSpace = PT_CMYK; break;
    case icSigCmyData: lcmsColorSpace = PT_CMY; break;
    case icSig2colorData: lcmsColorSpace = PT_ANY; break;
    case icSig3colorData: lcmsColorSpace = PT_ANY; break;
    case icSig4colorData: lcmsColorSpace = PT_ANY; break;
    case icSig5colorData: lcmsColorSpace = PT_ANY; break;
    case icSig6colorData: lcmsColorSpace = PT_ANY; break;
    case icSig7colorData: lcmsColorSpace = PT_ANY; break;
    case icSig8colorData: lcmsColorSpace = PT_ANY; break;
    case icSig9colorData: lcmsColorSpace = PT_ANY; break;
    case icSig10colorData: lcmsColorSpace = PT_ANY; break;
    case icSig11colorData: lcmsColorSpace = PT_ANY; break;
    case icSig12colorData: lcmsColorSpace = PT_ANY; break;
    case icSig13colorData: lcmsColorSpace = PT_ANY; break;
    case icSig14colorData: lcmsColorSpace = PT_ANY; break;
    case icSig15colorData: lcmsColorSpace = PT_ANY; break;
    case icSigHexachromeData: lcmsColorSpace = PT_HiFi; break; // speculation
    default: lcmsColorSpace = PT_ANY; break;
  }
  #endif
  return lcmsColorSpace;
}*/


/**
  *  @brief ICCtag Funktionen
  */

ICCtag::ICCtag ()
{
  _sig = icMaxEnumTag;
  _data = 0; DBG
}

ICCtag::ICCtag                      (icTag* tag, char* data)
{
  ICCtag::load (tag, data); DBG
}

ICCtag::~ICCtag ()
{
  _sig = icMaxEnumTag;
  _size = 0;
  if (!_data) free (_data);
  DBG
}

void
ICCtag::load                        ( icTag *tag, char* data )
{
  _sig    = icValue(tag->sig);
  _size   = icValue(tag->size);

  if (!_data) free (_data);
  _data = (char*) calloc ( _size , sizeof (char) );
  memcpy ( _data , data , _size );
  DBG

  #ifdef DEBUG_ICCTAG
  char* text = _data;
  cout << _sig << "=" << tag->sig << " offset " << icValue(tag->offset) << " size " << _size << " n�chster tag " << _size + icValue(tag->offset) << " " << text << " "; DBG
  #endif
}

std::string
ICCtag::getText                     (void)
{ DBG
  std::string text;

  // zur Sicherheit nochmal Kopieren
  //char*       ptr = (char*) calloc (_tag.size, sizeof (char));
  //memcpy (ptr, &_data[8], _tag.size - 8);

  text = &_data[8];//ptr;
  #ifdef DEBUG_ICCTAG
  cout << &_data[8] << "|" << "|" << text << " "; DBG
  #endif
  //free (ptr);
  return text;
}

std::vector<double>
ICCtag::getCIExy                                  (void)
{
  std::vector<double> punkte;
  icXYZType *daten = (icXYZType*) &_data[0];

  punkte.push_back( icValueSF( (daten->data.data[0].X) ) );
  punkte.push_back( icValueSF( (daten->data.data[0].Y) ) );
  punkte.push_back( icValueSF( (daten->data.data[0].Z) ) );

  return punkte;
}

std::vector<double>
ICCtag::getCurve                                  (void)
{
  std::vector<double> punkte;
  return punkte;
}

std::string
ICCtag::getDescription              ( void )
{
  std::string text = "Beschreibung";

  switch (_sig) {
    case icSigAToB0Tag: text = _("Farbtabelle, Ger�t an Kontaktfarbraum, Anpassung 0"); break;
    case icSigAToB1Tag: text = _("Farbtabelle, Ger�t an Kontaktfarbraum, Anpassung 1"); break;
    case icSigAToB2Tag: text = _("Farbtabelle, Ger�t an Kontaktfarbraum, Anpassung 2"); break;
    case icSigBlueColorantTag: text = _("blaue Grundfarbe"); break;
    case icSigBlueTRCTag: text = _("blaue Farbwiedergabekurve"); break;
    case icSigBToA0Tag: text = _("Farbtabelle, Kontaktfarbraum an Ger�t, Anpassung 0"); break;
    case icSigBToA1Tag: text = _("Farbtabelle, Kontaktfarbraum an Ger�t, Anpassung 1"); break;
    case icSigBToA2Tag: text = _("Farbtabelle, Kontaktfarbraum an Ger�t, Anpassung 2"); break;
    case icSigCalibrationDateTimeTag: text = _("Kalibrationsdatum"); break;
    case icSigCharTargetTag: text = _("targ"); break;
    case icSigCopyrightTag: text = _("Kopierrecht"); break;
    case icSigCrdInfoTag: text = _("crdi"); break;
    case icSigDeviceMfgDescTag: text = _("Herstellerbeschreibung"); break;
    case icSigDeviceModelDescTag: text = _("Ger�tebeschreibung"); break;
    case icSigGamutTag: text = _("Farbumfang"); break;
    case icSigGrayTRCTag: text = _("schwarze Wiedergabekurve"); break;
    case icSigGreenColorantTag: text = _("gr�ne Grundfarbe"); break;
    case icSigGreenTRCTag: text = _("gr�ne Farbwiedergabekurve"); break;
    case icSigLuminanceTag: text = _("lumi"); break;
    case icSigMeasurementTag: text = _("meas"); break;
    case icSigMediaBlackPointTag: text = _("Medienschwarzpunkt"); break;
    case icSigMediaWhitePointTag: text = _("Medienwei�punkt"); break;
    case icSigNamedColorTag: text = _("'ncol"); break;
    case icSigNamedColor2Tag: text = _("ncl2"); break;
    case icSigPreview0Tag: text = _("pre0"); break;
    case icSigPreview1Tag: text = _("pre1"); break;
    case icSigPreview2Tag: text = _("pre2"); break;
    case icSigProfileDescriptionTag: text = _("Profilbeschreibung"); break;
    case icSigProfileSequenceDescTag: text = _("Beschreibung der Profilverkn�pfung"); break;
    case icSigPs2CRD0Tag: text = _("psd0"); break;
    case icSigPs2CRD1Tag: text = _("psd1"); break;
    case icSigPs2CRD2Tag: text = _("psd2"); break;
    case icSigPs2CRD3Tag: text = _("psd3"); break;
    case icSigPs2CSATag: text = _("ps2s"); break;
    case icSigPs2RenderingIntentTag: text = _("ps2i"); break;
    case icSigRedColorantTag: text = _("rote Grundfarbe"); break;
    case icSigRedTRCTag: text = _("rote Farbwiedergabekurve"); break;
    case icSigScreeningDescTag: text = _("scrd"); break;
    case icSigScreeningTag: text = _("scrn"); break;
    case icSigTechnologyTag: text = _("tech"); break;
    case icSigUcrBgTag: text = _("bfd"); break;
    case icSigViewingCondDescTag: text = _("Beschreibung der Betrachtungbedingungen"); break;
    case icSigViewingConditionsTag: text = _("Betrachtungsbedingungen"); break;
    case 1147500100: text = _("Farbmessfl�chen"); break;
    case 1128875332: text = _("Farbmessergebnisse"); break;
    case 1349350514: text = _("Profilierungsparameter"); break;
    case 1986226036: text = _("Apple Monitor-Grafikkartentabelle"); break;
    case 1667785060: text = _("Farbanpassungsmatrix"); break;
    case 1667789421: text = _("Prim�rfarben"); break;
    case 1668051567: text = _("Schmuckfarbordnung"); break;
    case 1668051572: text = _("Schmuckfarbnamen"); break;
    case 0: text = _("----"); break;
    default: text = _("???"); break;
  }
  return text;
}

std::string
ICCtag::getSigTagName               ( icTagSignature  sig )
{
  std::string text;

  switch (sig) {
    case icSigAToB0Tag: text = _("A2B0"); break;
    case icSigAToB1Tag: text = _("A2B1"); break;
    case icSigAToB2Tag: text = _("A2B2"); break;
    case icSigBlueColorantTag: text = _("bXYZ"); break;
    case icSigBlueTRCTag: text = _("bTRC"); break;
    case icSigBToA0Tag: text = _("B2A0"); break;
    case icSigBToA1Tag: text = _("B2A1"); break;
    case icSigBToA2Tag: text = _("B2A2"); break;
    case icSigCalibrationDateTimeTag: text = _("calt"); break;
    case icSigCharTargetTag: text = _("targ"); break;
    case icSigCopyrightTag: text = _("cprt"); break;
    case icSigCrdInfoTag: text = _("crdi"); break;
    case icSigDeviceMfgDescTag: text = _("dmnd"); break;
    case icSigDeviceModelDescTag: text = _("dmdd"); break;
    case icSigGamutTag: text = _("gamt"); break;
    case icSigGrayTRCTag: text = _("kTRC"); break;
    case icSigGreenColorantTag: text = _("gXYZ"); break;
    case icSigGreenTRCTag: text = _("gTRC"); break;
    case icSigLuminanceTag: text = _("lumi"); break;
    case icSigMeasurementTag: text = _("meas"); break;
    case icSigMediaBlackPointTag: text = _("bkpt"); break;
    case icSigMediaWhitePointTag: text = _("wtpt"); break;
    case icSigNamedColorTag: text = _("'ncol"); break;
    case icSigNamedColor2Tag: text = _("ncl2"); break;
    case icSigPreview0Tag: text = _("pre0"); break;
    case icSigPreview1Tag: text = _("pre1"); break;
    case icSigPreview2Tag: text = _("pre2"); break;
    case icSigProfileDescriptionTag: text = _("desc"); break;
    case icSigProfileSequenceDescTag: text = _("pseq"); break;
    case icSigPs2CRD0Tag: text = _("psd0"); break;
    case icSigPs2CRD1Tag: text = _("psd1"); break;
    case icSigPs2CRD2Tag: text = _("psd2"); break;
    case icSigPs2CRD3Tag: text = _("psd3"); break;
    case icSigPs2CSATag: text = _("ps2s"); break;
    case icSigPs2RenderingIntentTag: text = _("ps2i"); break;
    case icSigRedColorantTag: text = _("rXYZ"); break;
    case icSigRedTRCTag: text = _("rTRC"); break;
    case icSigScreeningDescTag: text = _("scrd"); break;
    case icSigScreeningTag: text = _("scrn"); break;
    case icSigTechnologyTag: text = _("tech"); break;
    case icSigUcrBgTag: text = _("bfd"); break;
    case icSigViewingCondDescTag: text = _("vued"); break;
    case icSigViewingConditionsTag: text = _("view"); break;
    case 1147500100: text = _("DevD"); break;
    case 1128875332: text = _("CIED"); break;
    case 1349350514: text = _("Pmtr"); break;
    case 1986226036: text = _("vcgt"); break;
    case 1667785060: text = _("chad"); break;
    case 1667789421: text = _("chrm"); break;
    case 1668051567: text = _("clro"); break;
    case 1668051572: text = _("clrt"); break;
    //case : text = _(""); break;
    case 0: text = _("----"); break;
    default: text = _("???"); break;
  }
  #ifdef DEBUG_ICCTAG_
  char c[5] = "clrt";
  long* l = (long*) &c[0];
  cout << *l << ": " << (long)"clrt" << " "; DBG
  #endif
  return text;
}

std::string
ICCtag::getSigTypeName               ( icTagTypeSignature  sig )
{
  const char* name;
  std::string string;

  switch (sig) {
    case icSigCurveType: name = cp_char (_("curv")); break;
    case icSigDataType: name = cp_char (_("data")); break;
    case icSigDateTimeType: name = cp_char (_("dtim")); break;
    case icSigLut16Type: name = cp_char (_("mft2")); break;
    case icSigLut8Type: name = cp_char (_("mft1")); break;
    case icSigMeasurementType: name = cp_char (_("meas")); break;
    case icSigNamedColorType: name = cp_char (_("ncol")); break;
    case icSigProfileSequenceDescType: name = cp_char (_("pseq")); break;
    case icSigS15Fixed16ArrayType: name = cp_char (_("sf32")); break;
    case icSigScreeningType: name = cp_char (_("scrn")); break;
    case icSigSignatureType: name = cp_char (_("sig")); break;
    case icSigTextType: name = cp_char (_("text")); break;
    case icSigTextDescriptionType: name = cp_char (_("desc")); break;
    case icSigU16Fixed16ArrayType: name = cp_char (_("uf32")); break;
    case icSigUcrBgType: name = cp_char (_("bfd")); break;
    case icSigUInt16ArrayType: name = cp_char (_("ui16")); break;
    case icSigUInt32ArrayType: name = cp_char (_("ui32")); break;
    case icSigUInt64ArrayType: name = cp_char (_("ui64")); break;
    case icSigUInt8ArrayType: name = cp_char (_("ui08")); break;
    case icSigViewingConditionsType: name = cp_char (_("view")); break;
    case icSigXYZType: name = cp_char (_("XYZ")); break;
    //case icSigXYZArrayType: name = cp_char (_("XYZ")); break;
    case icSigNamedColor2Type: name = cp_char (_("ncl2")); break;
    case icSigCrdInfoType: name = cp_char (_("crdi")); break;
    case 1986226036: name = cp_char (_("vcgt")); break;
    default: name = cp_char (_("???")); break;
  }
  string = name;
  return string;
}

/*
void
ICCtag::printLut            (   LPLUT           Lut,
                                int             sig)
{
  unsigned int      i;
  int               channels=0;

  printf ("Channels In/Out = %d/%d Nr: %d/%d\n",Lut->InputChan, Lut->OutputChan,
                                        Lut->InputEntries, Lut->OutputEntries);

  switch (sig)
  {
  case icSigAToB0Tag:
  case icSigAToB1Tag:
  case icSigAToB2Tag:
    channels = Lut ->InputChan;
    break;
  case icSigBToA0Tag:
  case icSigBToA1Tag:
  case icSigBToA2Tag:
    channels = Lut ->OutputChan;
    break;
  case icSigGamutTag:
    channels = 1;
    break;
  }
  channels = Lut->OutputChan;

  for (i=0; i < Lut ->InputEntries; i++)  {
    if (channels > 0)
      printf ("C=%d ", Lut ->L2[0][i]);
    if (channels > 1)
      printf ("M=%d ", Lut ->L2[1][i]);
    if (channels > 2)
      printf ("Y=%d ", Lut ->L2[2][i]);
    if (channels > 3)
      printf ("K=%d ", Lut ->L2[3][i]);
  }
  printf ("\n");
}*/


/**
  *  @brief ICCprofile Funktionen
  */

ICCprofile::ICCprofile (void)
{
  //header = new ICCheader();
  _data = NULL;
  _size = 0;
  
}

ICCprofile::ICCprofile (const char *filename)
  : _filename (filename)
{
  _data = NULL;
  _size = 0;

  // delegieren
  _filename = filename;
  try {
    fload ();
  }
    catch (Ausnahme & a) {	// f�ngt alles von Ausnahme Abstammende
        printf ("Ausnahme aufgetreten: %s\n", a.what());
        a.report();
        _filename = "";
    }
    catch (std::exception & e) { // f�ngt alles von exception Abstammende
        printf ("Std-Ausnahme aufgetreten: %s\n", e.what());
        _filename = "";
    }
    catch (...) {		// f�ngt alles �briggebliebene
        printf ("Huch, unbekannte Ausnahme\n");
        _filename = "";
    }
}

ICCprofile::~ICCprofile (void)
{
  ;
}

void
ICCprofile::load (std::string filename)
{
  // delegieren
  _filename = filename;
  try {
    fload ();
  }
    catch (Ausnahme & a) {	// f�ngt alles von Ausnahme Abstammende
        printf ("Ausnahme aufgetreten: %s\n", a.what());
        a.report();
        _filename = "";
    }
    catch (std::exception & e) { // f�ngt alles von exception Abstammende
        printf ("Std-Ausnahme aufgetreten: %s\n", e.what());
        _filename = "";
    }
    catch (...) {		// f�ngt alles �briggebliebene
        printf ("Huch, unbekannte Ausnahme\n");
        _filename = "";
    }
}

void
ICCprofile::load (char* filename)
{
  // delegieren
  _filename = filename;
  try {
    fload ();
  }
    catch (Ausnahme & a) {	// f�ngt alles von Ausnahme Abstammende
        printf ("Ausnahme aufgetreten: %s\n", a.what());
        a.report();
        _filename = "";
    }
    catch (std::exception & e) { // f�ngt alles von exception Abstammende
        printf ("Std-Ausnahme aufgetreten: %s\n", e.what());
        _filename = "";
    }
    catch (...) {		// f�ngt alles �briggebliebene
        printf ("Huch, unbekannte Ausnahme\n");
        _filename = "";
    }
}

void
ICCprofile::fload ()
{ // ICC Profil laden
  //FilePtr fp ( _filename.c_str(), "rb");
  string file;
  ifstream f ( _filename.c_str(), ios::binary | ios::ate );

  DBG
  if (_filename == "")
    throw ausn_file_io ("kein Dateiname angegeben");
  DBG
  if (!f) {
    throw ausn_file_io ("keine lesbare Datei gefunden");
    _filename = "";
  }

  if (_data!=NULL) { free (_data); DBG }
  _size = (unsigned int)f.tellg();         f.seekg(0);
  _data = (icProfile*) calloc ( _size, sizeof (char)); DBG
  
  f.read ((char*)_data, _size); DBG
  #ifdef DEBUG_ICCPROFILE
  cout << _size << "|" << f.tellg() << endl;
  #endif
  f.close(); DBG

  DBG
  //Kopf
  header.load ((void*)_data); DBG

  //Profilabschnitte
  tags.clear(); DBG
  #ifdef DEBUG_ICCPROFILE
  cout << "TagCount: " << getTagCount() << " / " << tags.size() << " ";DBG
  #endif
  // TagTabelle bei 132 abholen
  icTag *tagList = (icTag*)&((char*)_data)[132];
  //(icTag*) calloc ( getTagCount() , sizeof (icTag));
  //memcpy (tagList , &((char*)_data)[132], sizeof (icTag) * getTagCount());
  for (int i = 0 ; i < getTagCount() ; i++) {
    ICCtag tag;//( tagList[i] , &((char*)_data)[ tagList[i].offset ] );
    DBG
    tag.load( &tagList[i] ,
              &((char*)_data)[ icValue(tagList[i].offset) ]); DBG
  #ifdef DEBUG_ICCPROFILE
    cout << " sig: " << tag.getTagName() << " "; DBG
  #endif
    tags.push_back(tag); DBG
  }
  #ifdef DEBUG_ICCPROFILE
  cout << "tags: " << tags.size() << " "; DBG
  #endif
  DBG
}

std::vector<std::string>
ICCprofile::printTagInfo         (int item)
{
  std::vector<std::string> liste;

  liste.push_back( tags.at(item).getTagName() );
  liste.push_back( tags.at(item).getTypName() );

  DBG
  return liste;
}

std::vector<std::string>
ICCprofile::printTags            ()
{
  DBG
  std::vector<std::string> StringList;
  std::string text;
  std::stringstream s;

  if (!tags.size()) { DBG
    return StringList;
  } DBG

  int count = 0;
  for (std::vector<ICCtag>::iterator it = tags.begin(); it != tags.end(); it++){
    s.str("");
    s << count; count++;           StringList.push_back(s.str()); s.str("");
    s.str((*it).getTagName());     StringList.push_back(s.str()); s.str("");
    s.str((*it).getTypName());     StringList.push_back(s.str()); s.str("");
    s << (*it).getSize();          StringList.push_back(s.str()); s.str("");
    s.str((*it).getDescription()); StringList.push_back(s.str()); s.str("");
  #ifdef DEBUG_ICCPROFILE
    cout << (*it).getTagName() << " "; DBG
  #endif
  }

  return StringList;
}

std::string
ICCprofile::getTagText                                  (int item)
{
  // Pr�fen
  std::string leer;
  if (tags[item].getTypName() != "text")
    return leer;

  return tags.at(item).getText();
}

std::vector<double>
ICCprofile::getTagCIExy                                  (int item)
{
  // Pr�fen
  std::vector<double> leer;
  if (tags[item].getTypName() != "XYZ")
    return leer;

  return tags.at(item).getCIExy();
}

std::vector<double>
ICCprofile::getTagCurve                                  (int item)
{
  // Pr�fen
  std::vector<double> leer;
  if (tags[item].getTypName() != "curv")
    return leer;

  return tags.at(item).getCurve();
}

char*
ICCprofile::getProfileInfo                   ( )
{
  static char text[128];
  static char profile_info[2048];
  icXYZNumber WhitePt;
  int       first = false,
            min_len = 24,
            len, i, pos=17;

    /* formatting */
/*    if (cmsIsTag(hProfile, icSigCopyrightTag)) {
        cmsReadICCText(hProfile, icSigCopyrightTag, &text[0]);
        len = strlen (text) + 16;
        if (len > min_len)
            min_len = len + 1;
    }
    profile_info[0] = '\000';

    if (cmsIsTag(hProfile, icSigProfileDescriptionTag)) {
        sprintf (text,_("Description:     "));
        cmsReadICCText(hProfile, icSigProfileDescriptionTag, &text[pos]);
        sprintf (profile_info,"%s%s",profile_info,text);
        if (!first) { // formatting for tooltips 
            len = min_len - strlen (profile_info);

            for (i=0; i < len * 2.2; i++) {
                sprintf (&profile_info[strlen (&profile_info[0])] ," ");
                //sprintf (profile_info,"%s ",profile_info);
            }
        }
        sprintf (profile_info,"%s\n",profile_info);
    }
    if (cmsIsTag(hProfile, icSigDeviceMfgDescTag)) {
        sprintf (text,_("Manufacturer:    "));
        cmsReadICCText(hProfile, icSigDeviceMfgDescTag, &text[pos]);
        sprintf (profile_info,"%s%s",profile_info,text);
    }
    if (cmsIsTag(hProfile, icSigDeviceModelDescTag)) {
        sprintf (text,_("Model:           "));
        cmsReadICCText(hProfile, icSigDeviceModelDescTag, &text[pos]);
        sprintf (profile_info,"%s%s",profile_info,text);
    }
    if (cmsIsTag(hProfile, icSigCopyrightTag)) {
        sprintf (text,_("Copyright:       "));
        cmsReadICCText(hProfile, icSigCopyrightTag, &text[pos]);
        sprintf (profile_info,"%s%s",profile_info,text);
    }

    sprintf (profile_info,"%s\n",profile_info);

    cmsTakeMediaWhitePoint (&WhitePt, hProfile);
    _cmsIdentifyWhitePoint (text, &WhitePt);
    sprintf (profile_info, "%s%s\n", profile_info, text);
*/
    sprintf (text,_("Device Class:   "));
    sprintf (profile_info,"%s%s %s\n",profile_info,text,
                              header.getDeviceClassName(header.deviceClass()).c_str());
    sprintf (text,_("Color Space:    "));
    sprintf (profile_info,"%s%s %s\n",profile_info,text,
                              header.getColorSpaceName(header.colorSpace()).c_str());
    sprintf (text,_("PCS Space:      "));
    sprintf (profile_info,"%s%s %s",profile_info,text,
                              header.getColorSpaceName(header.pcs()).c_str());

 
  return profile_info;
}

int
ICCprofile::checkProfileDevice (char* type, icProfileClassSignature deviceClass)
{
  int check = true;

  if ((strcmp(type, _("Work Space"))) == 0) {
      // test for device class
      switch (deviceClass)
        {
        case icSigInputClass:
        case icSigDisplayClass:
        case icSigOutputClass:
        case icSigLinkClass:
        case icSigAbstractClass:
        case icSigColorSpaceClass:
        case icSigNamedColorClass:
          // should be good
          break;
        default:
          g_message ("%s \"%s %s: %s",_("Your"), type,
                     _("Profile\" is designed for an other device"),
                     header.getDeviceClassName(deviceClass).c_str());
          check = false;
          break;
        }
  } else if ((strcmp(type, _("Separation"))) == 0) {
      switch (deviceClass)
        {
//        case icSigInputClass:
//        case icSigDisplayClass:
        case icSigOutputClass:
        case icSigLinkClass:
        case icSigAbstractClass:
        case icSigColorSpaceClass:
        case icSigNamedColorClass:
          // should be good
          break;
        default:
          g_message ("%s - %s - %s \"%s %s",_("Device class"),
                     header.getDeviceClassName(deviceClass).c_str(),
                     _("is not valid for an"),
                     type,
                     _("Profile\"."));
          check = false;
          break;
        if (icSigCmykData   != header.colorSpace())
          check = false;
        }
  } else if ((strcmp(type, _("Linearisation"))) == 0) {
      switch (deviceClass)
        {
//        case icSigInputClass:
//        case icSigDisplayClass:
//        case icSigOutputClass:
        case icSigLinkClass:
//        case icSigAbstractClass:
//        case icSigColorSpaceClass:
//        case icSigNamedColorClass:
          // should be good
          break;
        default:
          g_message ("%s - %s - %s \"%s %s",_("Device class"),
                     header.getDeviceClassName(deviceClass).c_str(),
                     _("is not valid for an"),
                     type,
                     _("Profile\"."));
          check = false;
          break;
        if (icSigCmykData   != header.colorSpace())
          g_message ("%s - %s - %s \"%s %s",_("Color space"),
                     header.getColorSpaceName(header.colorSpace()).c_str(),
                     _("is not valid for an"),
                     type,
                     _("Profile at the moment\"."));
          check = false;
        }
  }
  return check;
}

void
ICCprofile::saveProfileToFile (char* filename, char *profile, int size)
{
  FILE *fp=NULL;
  int   pt = 0;

  if ((fp=fopen(filename, "w")) != NULL) {
    do {
      fputc ( profile[pt++] , fp);
    } while (--size);
  }

  fclose (fp);
}


/**
  *  allgemeine Funktionen
  */

void
lcms_error (int ErrorCode, const char* ErrorText)
{
   g_message ("LCMS error:%d %s", ErrorCode, ErrorText);
}



const char* cp_char (char* text)
{
  static char string[128];

  sprintf(string, text);

  return &string[0];
}

const char* cp_nchar (char* text, int n)
{
  static char string[1024];

  for (int i = 0; i < 1024 ; i++)
    string[i] = '\000';

  if (n < 1024)
    snprintf(&string[0], n, text);
  else
    return NULL;

  #ifdef DEBUG
  cout << n << " Buchstaben kopieren " <<  (int)text << " " << string <<" "; DBG
  #endif

  return &string[0];
}


