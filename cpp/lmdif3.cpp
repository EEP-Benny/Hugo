/*
 * ICE - C++ - Library for image processing
 *
 * Copyright (C) 2002 FSU Jena, Digital Image Processing Group
 * Contact: ice@pandora.inf.uni-jena.de
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
  lmdif3.cpp
  Levenberg-Marquard-Algorithm
  Implementierung unter Verwendung eines Funktors und
  von Verweisen auf die zu optimierenden Parameter
*/

#include <math.h>
#include <vector>

using namespace std;

#include "lmdif.h"

namespace ice
{
  static bool running = false; // avoid nested calls

  static vector<double*> *refvecx;
  static LMFunctor const* functor;
  static vector<double> funcresult;

#define FNAME "LMDif"

  static int lmfunction5(int m, int n, double x[], double fva[])
  {
    int i;
	// get parameters from double[] to Vector
    for (int i = 0; i < n; i++)
      {
        (*refvecx)[i] = &x[i];
      }

	(*functor)(funcresult);

    // return distances as double[]
    for (i = 0; i < m; i++)
	  fva[i] = funcresult[i];

    return 0;
  }

  int LMDif(const vector<double*>& refvecp,
            const LMFunctor& fcn, int maxiter, int& inumber,
            double epsStepLength)
  {
    int i;

    int onr = refvecp.size();
    int funcdim = fcn.funcdim();

    // ergebnisvektor für Fehlerfunktion anlegen
    funcresult.resize(funcdim);

    refvecx = &refvecp;

    // zu optimierende Werte für LMDif-Kernroutine bereitstellen
    double* x = new double[onr];

    for (i = 0; i < onr; i++)
      x[i] = *refvecp[i];

    // Funktor merken für Fehlerfunktion
    functor = &fcn;

    // Variablen für LMDif-Aufruf
    int info;

    lmdifx(lmfunction5, funcdim, onr, x,
           DBL_EPSILON, DBL_EPSILON, DBL_EPSILON, maxiter, epsStepLength,
           100.0, 0, &info, &inumber, funcdim);
for (i = 0; i < onr; i++)
      *refvecp[i] = x[i];

    delete funcresult;
    delete [] x;

    running = false;
    return info;
  }

  int LMDif(const vector<double*>& v,
            const LMFunctor& fcn, int funcdim)
  {
    int inumber = 0;
    return LMDif(v, fcn, funcdim, inumber);
  }
#undef FNAME
}