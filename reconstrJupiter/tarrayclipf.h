#ifndef tarray_clip_h		// -*- C++ -*- 
#define tarray_clip_h

/*   This file is part of a software package written by 
     Rainer Heintzmann
     Institute of Applied Optics and Information Processing
     Albert Ueberle Strasse 3-5
     69120 Heidelberg
     Tel.: ++49 (0) 6221  549264
     No garantee, whatsoever is given about functionallaty and  savety.
     No warranty is taken for any kind of damage it may cause.
     No support for it is provided !

     THIS IS NOT FREE SOFTWARE. All rights are reserved, and the software is only
     given to a limited number of persons for evaluation purpose !
     Please do not modify and/or redistribute this file or any other files, libraries
     object files or executables of this software package !
*/


#include "rawarray.h"
#include "clipperator.h"

#define Dyn3dArray(Type,Clipperator)  TArrayClip3d <Type,Clipperator >
#define Dyn2dArray(Type,Clipperator)  TArrayClip2d <Type,Clipperator >
#define Dyn1dArray(Type,Clipperator)  TArrayClip1d <Type,Clipperator >

// Not supported any longer :
// #define Stat3dArray(Type,Clipperator,X,Y,Z) TArrayClip3d <StaticArray3d< Type,X,Y,Z>,Clipperator >
// #define Stat2dArray(Type,Clipperator,X,Y) TArrayClip2d <StaticArray2d< Type,X,Y>,Clipperator >
// #define Stat1dArray(Type,Clipperator,X) TArrayClip1d <StaticArray1d< Type,X>,Clipperator >

#if 0 // do rangecheck ?
#define RANGECHECK(pointer) if (pointer < TheArray.StartPos() || pointer > TheArray.EndPos()) cerr << "Array out of bound !!\n" << flush, (* (int *) 0) = 1;
#else
#define RANGECHECK(pointer) // Do nothing
#endif


/** Array class with clipping for projections.
    Giving a quarder described by two vectors a fast clipping  at this quarder inside the array can be calculated and prepared.
    The clipping fuctionality can vary depending on the template argument Clipperator.*/

    
/// this class can deal with Clipperators and perform various clippings
template<class TheArrayType, class ClipperatorType> class TArrayClip3d : public TArrayIter3d<TheArrayType> {

public:
  typedef typename TArrayIter3d<TheArrayType>::ArrayBaseType ArrayBaseType; 
private:
  /// pointer to the actual position in the array
  ArrayBaseType * pVal;
  
void init(void)
{ }

public:
  /// knows all about clipping in arrays
  ClipperatorType Clipperator;

protected:

/** This fuction intiates a clipping, calculates width, height and depth of the nearly quarder-shaped ("Spat")
    clipping region. As long as the direction of clipping or sizes do not change it need NOT be called again.
    Pixel must be given in Array Coordinates. (0,0,..) is first. Direction MUST be a unitvector. Center is of no intrest here ! 
    The cliprange points to one corner of the clipped area. Here only the width depth and height of
    the maximal clipping range are calculated. */
void InitArrayClippingFirst3D(CalcFloatType DirectionX,CalcFloatType DirectionY,CalcFloatType DirectionZ,
			      CalcFloatType Length,CalcFloatType Width, CalcFloatType Depth)
{
  Clipperator.SizeX = TArrayIter3d<TheArrayType>::GetSize(0);
  Clipperator.SizeY = TArrayIter3d<TheArrayType>::GetSize(1);
  Clipperator.SizeZ = TArrayIter3d<TheArrayType>::GetSize(2);
  Clipperator.InitClippingFirst3D(DirectionX,DirectionY,DirectionZ,Length,Width,Depth);
}

/** Pixel must be given in Array Coordinates (0,0,..) is first.
 Direction and ClipRange are of no intrest here ! They are assumed to be the same as in InitArrayClippingFirst! 
 The given Center coordinates are the Center of the clipprange in arraycoordinates */
ArrayBaseType * InitArrayClippingSecond3D(CalcFloatType CenterX,CalcFloatType CenterY,CalcFloatType CenterZ)
{
  if (Clipperator.InitClippingSecond3D(CenterX,CenterY,CenterZ)) // sets Positions
    {
      pVal=TArrayIter3d<TheArrayType>::TheArray.GetPValue(Clipperator.PositionX,Clipperator.PositionY,Clipperator.PositionZ);
      TArrayIter3d<TheArrayType>::TheArray.checkpointer(pVal);
      return pVal;
    }
  else
    return 0;
}

public:


TArrayClip3d() : TArrayIter3d<TheArrayType>()
{
  init();
}

TArrayClip3d(int SizeX,int SizeY,int SizeZ) : TArrayIter3d<TheArrayType>(SizeX,SizeY,SizeZ)
{
  init();
}

TArrayClip3d(ArrayBaseType * ExistingArray,int SizeX,int SizeY,int SizeZ) : TArrayIter3d<TheArrayType>(ExistingArray,SizeX,SizeY,SizeZ)
{
  init();
}

void InitArrayClippingFirst(Vector * Direction,CalcFloatType Length,CalcFloatType Width, CalcFloatType Depth)
{
  InitArrayClippingFirst3D(Direction->comp(0),Direction->comp(1),Direction->comp(2),Length,Width,Depth);
}

ArrayBaseType * InitArrayClippingSecond(Vector * Center)
{
  return InitArrayClippingSecond3D(Center->comp(0),Center->comp(1),Center->comp(2));
}

/** returns the next voxel inside the clipping range.
    Everything is assumend to be in the first octant now. */

ArrayBaseType * GiveNextClipped(void)     
{
  IndexType Add=Clipperator.GiveNextClipped();
  if (Add)
    pVal +=  Add;
  else
    return 0;
  TArrayIter3d<TheArrayType>::TheArray.checkpointer(pVal);
  return pVal;
}

};






// ++++++++++++++++++++++++++++++++++++++++++++++ Other Dimension Classes +++++++++++++++++++++++++++++++++++++++++++++++

/// implementation in 2d. Calls everywhere more general 3d-functions with z-coordinate 0
template<class TheArrayType,class ClipperatorType> class TArrayClip2d : public TArrayClip3d<TheArrayType,ClipperatorType> {
public:
  typedef typename TArrayClip3d<TheArrayType,ClipperatorType>::ArrayBaseType ArrayBaseType; 

public:
TArrayClip2d() : TArrayClip3d<TheArrayType,ClipperatorType>() {}
TArrayClip2d(int SizeX,int SizeY) : TArrayClip3d<TheArrayType,ClipperatorType>(SizeX,SizeY,1) {}

void Resize(int SizeX,int SizeY)
{
  TArrayClip3d<TheArrayType,ClipperatorType>::TheArray.Resize(SizeX,SizeY,1);
}

ArrayBaseType Value(int x,int y)
{ 
  return TArrayClip3d<TheArrayType,ClipperatorType>::Value(x,y,0);
}

ArrayBaseType Value(const unsigned char Selector,const CalcFloatType x,const CalcFloatType y)
{
  return TArrayClip3d<TheArrayType,ClipperatorType>::Value(Selector,x,y,0.0);
}

ArrayBaseType Value(const unsigned char Selector,Vector * pPos1, Vector * pPos2) 
  {
    return TArrayClip2d<TheArrayType,ClipperatorType>::Value(Selector,pPos1->comp(0)-pPos2->comp(0),pPos1->comp(1)-pPos2->comp(1));
  }

ArrayBaseType Value(const unsigned char Selector,Vector * pPos1, Vector * pPos2,CalcFloatType fac) 
  {
    return TArrayClip2d<TheArrayType,ClipperatorType>::Value(Selector,fac*(pPos1->comp(0)-pPos2->comp(0)),fac*(pPos1->comp(1)-pPos2->comp(1)));
  }

ArrayBaseType Value(const unsigned char Selector,Vector * pPos1) 
  {
    return TArrayClip2d<TheArrayType,ClipperatorType>::Value(Selector,pPos1->comp(0),pPos1->comp(1));
  }

void SetValue(int x,int y,ArrayBaseType v) {TArrayClip3d<TheArrayType,ClipperatorType>::SetValue(x,y,0,v);}


void InitArrayClippingFirst(Vector * Direction,CalcFloatType Length,CalcFloatType Width, CalcFloatType Depth)
{
  InitArrayClippingFirst3D(Direction->comp(0),Direction->comp(1),0,Length,Width,Depth);
}

ArrayBaseType * InitArrayClippingSecond(Vector * Center) // Direction and ClipRange are of no intrest here ! Assumed to be the same !
{
  return InitArrayClippingSecond3D(Center->comp(0),Center->comp(1),0);
}


};


/// implementation in 1d. Calls everywhere more general 2d-functions with z- and y-coordinate 0
template<class TheArrayType,class ClipperatorType> class TArrayClip1d : public TArrayClip2d<TheArrayType,ClipperatorType> {
public:

  typedef typename TArrayClip2d<TheArrayType,ClipperatorType>::ArrayBaseType ArrayBaseType; 

public:

TArrayClip1d() : TArrayClip2d<TheArrayType,ClipperatorType>() {}
TArrayClip1d(int SizeX) : TArrayClip2d<TheArrayType,ClipperatorType>(SizeX,1) {}

void Resize(int SizeX)
{
  this.TheArray.Resize(SizeX,1,1);
}

ArrayBaseType Value(int x)
{
  return TArrayClip3d<TheArrayType,ClipperatorType>::Value(x,0,0);
}


ArrayBaseType Value(const unsigned char Selector,CalcFloatType x)
{
  return TArrayClip3d<TheArrayType,ClipperatorType>::Value(Selector,x,0.0,0.0);
}

ArrayBaseType Value(const unsigned char Selector,Vector * pPos1, Vector * pPos2) 
  {
    return TArrayClip1d<TheArrayType,ClipperatorType>::Value(Selector,pPos1->comp(0)-pPos2->comp(0));
  }

ArrayBaseType Value(const unsigned char Selector,Vector * pPos1, Vector * pPos2, CalcFloatType fac) 
  {
    return TArrayClip1d<TheArrayType,ClipperatorType>::Value(Selector,fac*(pPos1->comp(0)-pPos2->comp(0)));
  }

ArrayBaseType Value(const unsigned char Selector,Vector * pPos1) 
  {
    return TArrayClip1d<TheArrayType,ClipperatorType>::Value(Selector,pPos1->comp(0));
  }

void SetValue(int x,ArrayBaseType v) {TArrayClip3d<TheArrayType,ClipperatorType>::SetValue(x,0,0,v);}


void InitArrayClippingFirst(Vector * Direction,CalcFloatType Length,CalcFloatType Width,CalcFloatType Depth)
{
  InitArrayClippingFirst3D(Direction->comp(0),0,0,Length,Width,Depth);
}

ArrayBaseType * InitArrayClippingSecond(Vector * Center) // Direction and ClipRange are of no intrest here ! Assumed to be the same !
{
  return InitArrayClippingSecond3D(Center->comp(0),0,0);
}

};

#endif

