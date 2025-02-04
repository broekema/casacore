//# StIndArrAIO.cc: Read/write a table column of arrays array using AipsIO
//# Copyright (C) 1994,1995,1996,1997,1999,2001,2002
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#include <casacore/tables/DataMan/StIndArrAIO.h>
#include <casacore/tables/DataMan/StArrayFile.h>
#include <casacore/tables/DataMan/StIndArray.h>
#include <casacore/casa/Utilities/DataType.h>
#include <casacore/casa/IO/AipsIO.h>
#include <casacore/casa/Arrays/Array.h>
#include <casacore/casa/BasicSL/Complex.h>
#include <casacore/casa/BasicSL/String.h>
#include <casacore/tables/DataMan/DataManError.h>
#include <casacore/casa/stdio.h>                            //# for sprintf


namespace casacore { //# NAMESPACE CASACORE - BEGIN

//# Define a macro which gets the pointer for the given row
//# and casts it to the block.
#define STMANINDGETBLOCK(rownr) \
((StIndArray*)(getArrayPtr(rownr)))



StManColumnIndArrayAipsIO::StManColumnIndArrayAipsIO (StManAipsIO* smptr,
						      int dataType)
: StManColumnAipsIO (smptr, dataType, True),
  staioPtr_p    (smptr),
  seqnr_p       (smptr->uniqueNr()),
  shapeIsFixed_p(False),
  version_p     (2),
  iosfile_p     (0)
{}

//# Delete all objects created.
StManColumnIndArrayAipsIO::~StManColumnIndArrayAipsIO()
{
    rownr_t nr = stmanPtr_p->nrow();
    for (rownr_t i=0; i<nr; i++) {
	deleteArray (i);
    }
    if (version_p <= 1) {
        delete iosfile_p;
    }
}


//# Create the array file (for a new column).
//# Compose the file name from the mother file name extended with
//# the unique column sequence nr.
//# Create the given nr of rows in it.
void StManColumnIndArrayAipsIO::doCreate (rownr_t nrrow)
{
    //# Create the file.
    openFile (ByteIO::New);
    //# Add the nr of rows.
    addRow (nrrow, 0);
}

void StManColumnIndArrayAipsIO::openFile (ByteIO::OpenOption opt)
{
    //# For newer versions one file is maintained by the parent
    //# for all indirect columns.
    if (version_p > 1) {
        iosfile_p = staioPtr_p->openArrayFile (opt);
    } else {
        //# Open/create the file holding the arrays in the column.
        if (iosfile_p == 0) {
	  char strc[8];
	  sprintf (strc, "i%i", seqnr_p);
	  iosfile_p = new StManArrayFile (stmanPtr_p->fileName() + strc, opt);
	} else {
	  iosfile_p->resync();
	}
    }
}

void StManColumnIndArrayAipsIO::reopenRW()
{
    iosfile_p->reopenRW();
}


void StManColumnIndArrayAipsIO::setShapeColumn (const IPosition& shape)
{
    fixedShape_p   = shape;
    shapeIsFixed_p = True;
}


void StManColumnIndArrayAipsIO::addRow (rownr_t nrnew, rownr_t nrold)
{
    //# Extend data blocks if needed.
    StManColumnAipsIO::addRow (nrnew, nrold);
    //# Allocate the data arrays if fixed shape.
    if (shapeIsFixed_p) {
	for (; nrold<nrnew; nrold++) {
	    setShape (nrold, fixedShape_p);
	}
    }
}


void StManColumnIndArrayAipsIO::setShape (rownr_t rownr, const IPosition& shape)
{
    StIndArray* ptr = STMANINDGETBLOCK(rownr);
    if (ptr == 0) {
	ptr = new StIndArray (0);
    }
    //# Put the new shape (if changed).
    //# When changed, put the file offset.
    if (ptr->setShape (*iosfile_p, dtype(), shape)) {
	putArrayPtr (rownr, ptr);
    }
}

//# Get the shape for the array (if any) in the given row.
//# Read shape if not read yet.
StIndArray* StManColumnIndArrayAipsIO::getShape (rownr_t rownr)
{
    StIndArray* ptr = STMANINDGETBLOCK(rownr);
    if (ptr == 0) {
        throw (DataManInvOper ("ASM: no array in row " +
			       String::toString(rownr) +
			       " of " + stmanPtr_p->fileName()));
    }
    ptr->getShape (*iosfile_p);
    return ptr;
}

Bool StManColumnIndArrayAipsIO::isShapeDefined (rownr_t rownr)
    { return (STMANINDGETBLOCK(rownr) == 0  ?  False : True); }

uInt StManColumnIndArrayAipsIO::ndim (rownr_t rownr)
    { return getShape(rownr)->shape().nelements(); }

IPosition StManColumnIndArrayAipsIO::shape (rownr_t rownr)
    { return getShape(rownr)->shape(); }

Bool StManColumnIndArrayAipsIO::canChangeShape() const
    { return (shapeIsFixed_p  ?  False : True); }


void StManColumnIndArrayAipsIO::getArrayV (rownr_t rownr, ArrayBase& arr)
{
  StIndArray* sia = getShape (rownr);
  sia->getArrayV (*iosfile_p, arr, dtype());
}

void StManColumnIndArrayAipsIO::putArrayV (rownr_t rownr,
                                           const ArrayBase& arr)
{
  StIndArray* sia = getShape (rownr);
  sia->putArrayV (*iosfile_p, arr, dtype());
  stmanPtr_p->setHasPut();
}

void StManColumnIndArrayAipsIO::getSliceV (rownr_t rownr, const Slicer& ns,
                                           ArrayBase& arr)
{
  StIndArray* sia = getShape (rownr);
  sia->getSliceV (*iosfile_p, ns, arr, dtype());
}

void StManColumnIndArrayAipsIO::putSliceV (rownr_t rownr, const Slicer& ns,
                                           const ArrayBase& arr)
{
  StIndArray* sia = getShape (rownr);
  sia->putSliceV (*iosfile_p, ns, arr, dtype());
  stmanPtr_p->setHasPut();
}


  void StManColumnIndArrayAipsIO::remove (rownr_t rownr)
{
    deleteArray (rownr);
    StManColumnAipsIO::remove (rownr);
}


Bool StManColumnIndArrayAipsIO::ok() const
{
    return StManColumnAipsIO::ok();
}


void StManColumnIndArrayAipsIO::deleteArray (rownr_t rownr)
{
    delete STMANINDGETBLOCK(rownr);
}


//# Write all data into AipsIO.
void StManColumnIndArrayAipsIO::putFile (rownr_t nrval, AipsIO& ios)
{
    ios.putstart ("StManColumnIndArrayAipsIO", version_p);
    ios << dtype();                    // for backward compatibility
    ios << seqnr_p;
    StManColumnAipsIO::putFile (nrval, ios);
    ios.putend();
    iosfile_p->flush (False);
}

void StManColumnIndArrayAipsIO::putData (void* dp, uInt nrval, AipsIO& ios)
{
    StIndArray** dpa = (StIndArray**)dp;
    while (nrval--) {
	if (*dpa == 0) {
	    ios << (uInt)0;
	}else{
	    Int64 off = (*dpa)->fileOffset();
	    if (off <= 2u*1024u*1024u*1024u) {
	        ios << uInt(off);
	    }else{
	        ios << 2u*1024u*1024u*1024u + 1u;
		ios << off;
	    }
	}
	dpa++;
    }
}


//# Read all data from AipsIO.
void StManColumnIndArrayAipsIO::getFile (rownr_t nrval, AipsIO& ios)
{
    int dtype;
    version_p = ios.getstart ("StManColumnIndArrayAipsIO");
    ios >> dtype;                          // for backward compatibility
    ios >> seqnr_p;
    openFile (stmanPtr_p->fileOption());   // open the array file
    StManColumnAipsIO::getFile (nrval, ios);
    ios.getend();
}

void StManColumnIndArrayAipsIO::getData (void* dp, uInt inx, uInt nrval,
					 AipsIO& ios, uInt)
{
    Int64 offset;
    uInt off;
    StIndArray** dpa = (StIndArray**)dp;
    dpa += inx;
    while (nrval--) {
	ios >> off;
	if (off == 2u*1024u*1024u*1024u + 1u) {
	    ios >> offset;
	}else{
	    offset = off;
	}
	if (offset == 0) {
	    *dpa = 0;
	}else{
	    *dpa = new StIndArray (offset);
	}
	dpa++;
    }
}

} //# NAMESPACE CASACORE - END

