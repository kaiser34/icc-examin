/*
 * ICC Examin ist eine ICC Profil Betrachter
 * 
 * Copyright (C) 2004-2011  Kai-Uwe Behrmann 
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
 * internal types and formulas
 * 
 */

// Date:      September 2004

#ifndef ICC_FORMELN_H
#define ICC_FORMELN_H


#include "icc_utils.h"
#include <string>
#include <iostream>
#include <sstream>
#include <map>
#include <fstream>



#define X_D50 0.9642
#define Y_D50 1.0000
#define Z_D50 0.8249

// internal types
typedef struct {
    double X;
    double Y;
    double Z;
} XYZ_s; //!< 0...1

typedef struct {
    double L;
    double a;
    double b;
} Lab_s; //!< 0...1

typedef struct {
    double R;
    double G;
    double B;
} RGB_s; //!< 0...1

typedef struct {
    double C;
    double M;
    double Y;
    double K;
} CMYK_s; //!< 0...1

// type conversions
void FarbeZuDouble (double* d_xyz, XYZ_s xyz);
void FarbeZuDouble (double* d_rgb, RGB_s rgb);
void FarbeZuDouble (double* d_lab, Lab_s lab);
void FarbeZuDouble (double* d_cmyk, CMYK_s cmyk);
void FarbeZuDouble (XYZ_s *xyz, double* d_xyz);
void FarbeZuDouble (RGB_s *rgb, double* d_rgb);
void FarbeZuDouble (Lab_s *lab, double* d_lab);
void FarbeZuDouble (CMYK_s *cmyk, double* d_cmyk);

// colour functions
// colour difference
double        dE2000 (const Lab_s & Lab1, const Lab_s & Lab2,
                      const double kL, const double kC, const double kH);
double        dE     ( const Lab_s Lab1, const Lab_s Lab2 );
double        dE     ( const double* Lab1, const Lab_s Lab2 );
double        dE     ( const double* Lab1, const double* Lab2 );

// standard colour transformations
void          XYZtoLab (const XYZ_s & xyz, Lab_s & lab);
void          XYZtoLab (const double* xyz, double* lab, int n);
void          LabtoXYZ (const Lab_s &lab, XYZ_s &XYZ);

// normalising  CIELab L 0...100 a/b -127...+127 // lab 0...1
void          CIELabToLab (const double* cielab, double* lab, int n);
void          CIELabToLab (const double* cielab, Lab_s & lab);
void          LabToCIELab (const double* lab, double* cielab, int n);
void          LabToCIELab (const Lab_s & lab, double* cielab);

#endif //ICC_FORMELN_H
