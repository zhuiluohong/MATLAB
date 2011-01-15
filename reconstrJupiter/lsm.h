		// -*- C++ -*- 
#ifndef lsm_h
#define lsm_h

/*   This file is part of a software package written by 
     Rainer Heintzmann
     MPI bpC
     Am Fassberg 11
     37077 Goettingen
     Tel.: ++49 (0) 551 201 1029
     No garantee, whatsoever is given about functionallaty and  savety.
     No warranty is taken for any kind of damage it may cause.
     No support for it is provided !

     THIS IS NOT FREE SOFTWARE. All rights are reserved, and the software is only
     given to a limited number of persons for evaluation purpose !
     Please do not modify and/or redistribute this file or any other files, libraries
     object files or executables of this software package without permission!
*/

#include <tiffio.h>
#include <list>
#include <math.h>
#ifndef M_PI
#define M_PI 3.14159265358979
#endif

template <class vclass>
void VREAD(ifstream * stream,vclass & Var) {
  stream->read((char *) & (Var),sizeof(Var));
  /*cout << "read " << ": " << Var << ", " << sizeof(Var) << "\n"<<flush;*/
  }

template <class vclass>
void VWRITE(ofstream * stream,vclass Var) {
  stream->write((char *) & (Var),sizeof(Var));
  /*cout << "written: " << ": " << Var << ", " << sizeof(Var) << "\n"<<flush;*/
  }

//#define VREAD(stream,Var) {stream->read((char *) & (Var),sizeof(Var)); cout << "read "##Var << ": " << Var << "\n";}

const int MAXELEM = 400;
const int MAXDIR = 200;

const uint32 TIFFTAG_CZLSMINFO = 34412,  TIF_BLACKISZERO=1, SUBBLOCK_DESCRIPTION=0x10000002, SUBBLOCK_NOTES=0x10000003, SUBBLOCK_RECORDING=0x10000000, SUBBLOCK_END=0xFFFFFFFF;

const unsigned short TIF_IMAGEWIDTH=256,
    TIF_IMAGELENGHT=257,
    TIF_BITSPERSAMPLE=258,
    TIF_COMPRESSION=259,
    PHOTOMETRIC_INTERPRETATION=262,
    TIF_STRIPOFFSETS=273,
    TIF_SAMPLESPERPIXEL=277,
    TIF_STRIPBYTECOUNTS=279,
    TIF_COLORMAP=320,
    TIF_CZLSMINFO=34412;

const uint32 DE_FLOATRECTANGLE=18,DE_FLOATELLYPSE=19,DE_FLOATPOLYLINE=20,
                    DE_FLOATCIRCLE=24,DE_FLOATCIRCLE3POINTS=29,DE_CLOSEDBEZIER=22;

/*class CPoint {
  public:
  double x;double y;
  CPoint(double x1,double y1) : x(x1),y(y1) {
    //cout<<"new Point : " <<x << ", " << y <<"\n";
  }
  }; */

class Rectangle {
  double sx;double sy;double ex;double ey;
  public:
  Rectangle(double sx1,double sy1,double ex1,double ey1) : sx(sx1),sy(sy1),ex(ex1),ey(ey1) {
        //cout<<"new Rectangle: " <<sx << ", " << sy <<" ... " <<ex << ", " << ey <<"\n";
    if (ex < sx) swap(ex,sx);
    if (ey < sy) swap(ey,sy);
  }
  bool IsInside(int x, int y)
  {
    if (sx <= x && x <= ex && sy <= y && y <= ey) return true;
    else return false;
  }
  };
class Circle {
  double cx;double cy;double px;double py;
  public:
  Circle(double cx1,double cy1,double px1,double py1) : cx(cx1),cy(cy1),px(px1),py(py1) {
  //cout<<"new Circle: "<< cx << ", " << cy <<" and " <<px << ", " << py <<"\n";
  }
  bool IsInside(int x, int y)
  {
    double dx = x - cx;
    double dy = y - cy;
    double r2 = (px - cx) * (px - cx) + (py - cy) * (py - cy);
    if (dx*dx+dy*dy < r2) return true;
    else return false;
  }
  };
  
class Ellypse {
  double x1;double y1;double x2;double y2;
  double x3;double y3;double x4;double y4;
  double cx,cy,dx1,dy1,X2,dx2,dy2,Y2;
  public:
  Ellypse(double px1,double py1,double px2,double py2,double px3,double py3,double px4,double py4) :
         x1(px1),y1(py1),x2(px2),y2(py2),x3(px3),y3(py3),x4(px4),y4(py4) {
  //cout<<"new Ellypse: (" <<x1 << ", " << y1 <<") ("<< x2 << ", " << y2 <<"), (" <<
  // x3 << ", " << y3 <<") ("<< x4 << ", " << y4 <<")\n";
    cx = (x1 +x3)/2.0;
    cy = (y1 +y3)/2.0;
    dx1 = x1 - cx;
    dy1 = y1 - cy;
    X2 = dx1*dx1+dy1*dy1;
    dx2 = x2 - cx;
    dy2 = y2 - cy;
    Y2 = dx2*dx2+dy2*dy2;
   }

  bool IsInside(int x, int y)
  {
   double XRel = (dx1 * (x-cx) + dy1*(y-cy))/X2;
   double YRel = (dx2 * (x-cx) + dy2*(y-cy))/Y2;
   // cout << "   " << x << ", " << y << ", " << XRel << ", " << YRel << "\n";
   if (XRel*XRel + YRel*YRel < 1.0) return true;
   else return false;
  }

  };

class PolyLine {
  list<double> PointListX;
  list<double> PointListY;
  public:
  PolyLine () {
      //cout<<"new PolyLine:\n";
  }
  void AddPoint(double x,double y) {
    PointListX.push_back(x);
    PointListY.push_back(y);
    // cout << "Added Point: ("<<x<<", "<<y<<")\n";
    }
  bool IsInside(int x, int y)
  {
    double SumOfAngles=0.0, w1,wold,wdiff;
    // cout << "Im here \n" << flush;
    
    list<double>::iterator posX=PointListX.begin(),poldX = PointListX.end();
    list<double>::iterator posY=PointListY.begin(),poldY = PointListY.end();
    poldX--;poldY--;
    // int i=0;
    wold = atan2((*poldY) - y,(*poldX) - x);
   for (;posX != PointListX.end();posX++,posY++)

   {  // sums up the angles to each point. If close to zero, the point is outside else inside
   
     w1 = atan2((*posY) - y,(*posX) - x);
     wdiff = w1 - wold;
     if (wdiff > M_PI)
       wdiff -= 2.0*M_PI;
     if (wdiff < -M_PI)
       wdiff += 2.0*M_PI;
     SumOfAngles += wdiff;
     wold = w1;
   }
   // if (i++ == 5)
   if (fabs(SumOfAngles) > M_PI /2)
     return true;
   else
     return false;
   
  }
  };

class ROIs {
  public:                    
  list<Rectangle*> RectList;
  list<Circle*> CircList;
  list<Ellypse*> EllyList;
  list<PolyLine*> PolyList;

  int GetROICnt() {
     return EllyList.size()+CircList.size()+RectList.size()+PolyList.size();
   }

  bool IsInAnyROI(int x,int y, int ROINr) {  // Tests whether a pixel is inside any of the ROIs
     // First test for ellypses
     int actROI=0;
     for (list<Ellypse*>::iterator pos=EllyList.begin();pos != EllyList.end();pos++,actROI++)
       if ((actROI == ROINr) && (* pos) -> IsInside(x,y)) return true;
     // Test for circles
     for (list<Circle*>::iterator pos=CircList.begin();pos != CircList.end();pos++,actROI++)
       if ((actROI == ROINr) && (* pos) -> IsInside(x,y)) return true;
     // Test for rectangles
     for (list<Rectangle*>::iterator pos=RectList.begin();pos != RectList.end();pos++,actROI++)
       if ((actROI == ROINr) && (* pos) -> IsInside(x,y)) return true;
     // Test for polylines (not the Bezier ones!)
     for (list<PolyLine*>::iterator pos=PolyList.begin();pos != PolyList.end();pos++,actROI++)
       if ((actROI == ROINr) && (* pos) -> IsInside(x,y)) return true;

     return false;
   }


  };
  
ROIs BleachROIs, DataROIs;
    
class LSMParser {
  public:
    ifstream * from;
    TIFF* tif;
    uint32 w, h;
    // uint32 SizeX,SizeY,SizeZ,SizeE,SizeT;
    size_t npixels;

    unsigned short sdummy,magic1,magic2;
    uint32 offset,actstrip;
    uint32 timesoffset, offsetscaninfo;

    // Directory Tag:
    unsigned short dummy,compress;
    unsigned short numtags,dtag[MAXDIR],dtype[MAXDIR];
    uint32 dlng[MAXDIR],dval[MAXDIR];
    uint32 width,height,dataoffsets[MAXELEM],datasizes[MAXELEM],bits[MAXELEM],
                  infooffset,tableoffset,tablesize,sizesoffset,sizessize,bitssize,bitsoffset,
                  offsetBleachROI, offsetDataROI, offsetMeanROI, offsetVectorROI, sizeBleachROI, colormap;
    bool noDisable;

    // Infor Structure
    uint32 magic;
    int32 isize,SizeX,SizeY,SizeZ,SizeE,SizeT,DatType,ThumX,ThumY,datsize;
    double VoxX,VoxY,VoxZ;

    // Data Buffer
    char * data;
    
  void ReadTiff(const char * filename, unsigned char * buf, int TypeSize)
  {
     tif = TIFFOpen(filename, "r");
     if (tif) {
       TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
       TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
       // TIFFGetField(tif, TIFFTAG_CZLSMINFO, &offset);
       npixels = w * h;
       SizeX=w;
       SizeY=h;
       // cout << "Image Sizes: " << SizeX << ", " << SizeY << "\n";
       // TIFFPrintDirectory(tif,stdout,1);
       // cout << "Requested TypeSize is " << TypeSize << ", DatType is " << DatType << "\n" << flush;
       if (TypeSize == 2 && DatType == 1)  // This means 8-bit data, 2 means 12 bit data
         {  // first read into 8-bit and then convert to 16-bit
         unsigned char * bbuf = (unsigned char *) malloc(npixels * sizeof(unsigned char));
         if (! bbuf)
           {
               cerr << "ERROR in lsm.h : unable to allocate buffer!\n";
               exit(-1);
           }
         ReadTiffSlice(bbuf, 1);  // actually reads in the data
         for (unsigned long i=0;i< npixels;i++)
           (((unsigned short *) buf))[i] = (unsigned short) bbuf[i];
         if (bbuf)
           free(bbuf);
         }
       else
         ReadTiffSlice(buf, TypeSize);  // actually reads in the data
       TIFFClose(tif);
       }
       else
       {
         cerr << "ERROR: Unable to open tiff file\n";
         exit(-1);
       }
  }

  void ReadTiffSlice(unsigned char * buf, int TypeSize) {
      // cout << "Reading Segment Nr. " << actstrip  << "\n"<<flush;
  tstrip_t strip;
  uint32* bc;
  uint32 bytesread=0;
  TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &bc);

  for (strip = 0; strip < TIFFNumberOfStrips(tif); strip++)
    {
      if (bytesread +  bc[strip] >   npixels*TypeSize)
        {
          cerr << "Fatal Error reading LSM File: Tiff strips are more than allocated area!\n";
          exit(-1);
        }
    if (TIFFReadEncodedStrip(tif, strip, ((char *) buf) + bytesread, bc[strip]) > 0)   // ,npixels*TypeSize
           {
             bytesread +=  bc[strip];
             // cout << "Read Strip # " << strip << "\n";
           }
           else
           {
             cerr << "ERROR: No more data found\n";
             exit(-1);
           }
    }
  }
  
  LSMParser(ifstream * fr, bool NoDisable=false) : from(fr), actstrip(0), timesoffset(0), offsetscaninfo(0), offsetBleachROI(0), offsetDataROI(0), offsetMeanROI(0), offsetVectorROI(0), noDisable(NoDisable), data(0)
  {  }

  void ParseHeader() {
    VREAD(from,magic1);
    VREAD(from,magic2);
    VREAD(from,offset);

    ReadDir();
  }

  void SliceToBuffer(int element) {  // loads in the data (which might be compressed) and saves it temporarily
    if (data) free(data);
    data = (char *) malloc(datasizes[element]);
  	from->seekg(dataoffsets[element],ios::beg);
    from->read(data,datasizes[element]);
    }
    
 void WriteBufferToTif(const char * filename,int element) {
    ofstream * to = new ofstream(filename);
    uint32 Bits = 8 * DatType;
    if (! to)
      { cerr << "Error writing tiff-file; could not create file\n"; exit (-1);}

    VWRITE(to,magic1);
    VWRITE(to,magic2);
    VWRITE(to,8);  // first directory follows directly

    unsigned short numnewtags=numtags;

    for (int i=0;i < numtags;i++)
    {
      switch (dtag[i]) {
      case TIF_CZLSMINFO:
        numnewtags--; // cout << "INFOTAG REMOVED\n\n" << flush;
        break;
      case TIF_COLORMAP:
        numnewtags--; // cout << "COLORTAG REMOVED\n\n" << flush;
        break;
      case PHOTOMETRIC_INTERPRETATION:
        numnewtags--; // cout << "PHOTOTAG REMOVED\n\n" << flush;
        break;
      default: break;
      }
    }

    uint32 datanewoffset = 8 + 2 + 12* numnewtags + 4;
    size_t length = datasizes[element];
    // cout << "Length element " << element << " written: " << length << "\n";
    
    VWRITE(to,numnewtags);    

    for (int i=0;i < numtags;i++)
    {
      switch (dtag[i]) {
      case TIF_CZLSMINFO:
      case TIF_COLORMAP:
      case PHOTOMETRIC_INTERPRETATION:
        break;
      case TIF_STRIPBYTECOUNTS:
        VWRITE(to,dtag[i]);
        VWRITE(to,dtype[i]);
        VWRITE(to,(uint32)1);   // direct given bytecount here
        VWRITE(to,(uint32)length);
        break;
      case TIF_STRIPOFFSETS:
        VWRITE(to,dtag[i]);
        VWRITE(to,dtype[i]);
        VWRITE(to,(uint32)1);   // directly given offset here
        VWRITE(to,(uint32)datanewoffset);
        break;
      /*case PHOTOMETRIC_INTERPRETATION:
        VWRITE(to,dtag[i]);
        VWRITE(to,dtype[i]);
        VWRITE(to,(uint32) 1);   // directly given offset here
        VWRITE(to,(uint32) TIF_BLACKISZERO);      // no colormap
        */
      case TIF_BITSPERSAMPLE:
        VWRITE(to,dtag[i]);
        VWRITE(to,dtype[i]);
        VWRITE(to,(uint32)1);   // just one channel !
        VWRITE(to,Bits);   // corrected value of two
        break;
      case TIF_SAMPLESPERPIXEL:
        VWRITE(to,dtag[i]);
        VWRITE(to,dtype[i]);
        VWRITE(to,(uint32)1);   // directly given offset here
        VWRITE(to,(uint32)1);   // corrected value of two
        break;
      default:
        VWRITE(to,dtag[i]);
        VWRITE(to,dtype[i]);
        VWRITE(to,dlng[i]);
        VWRITE(to,dval[i]);
      } 
    }
    VWRITE(to,(uint32) 0);  // No more directory entries
    to->write(data,length);
    if (to->bad())
      { cerr << "Error writing tiff-file, no space on disk \n"; exit (-1);}
    to->close();
  }

  void ParseInfo() {
    // cout << "Parsing Info\n"<<flush;
    timesoffset = 0;
  	from->seekg(infooffset,ios::beg);
    VREAD(from,magic);
    VREAD(from,isize);
    VREAD(from,SizeX);
    VREAD(from,SizeY);
    VREAD(from,SizeZ);
    VREAD(from,SizeE);
    VREAD(from,SizeT);
    VREAD(from,DatType);
    VREAD(from,ThumX);
    VREAD(from,ThumY);
    VREAD(from,VoxX);
    VREAD(from,VoxY);
    VREAD(from,VoxZ);
    cout << "VOXELSIZES: " << VoxX << ", " << VoxY << ", " << VoxZ << "\n";

    uint32 dummy;
    for (int i=0;i<8;i++)
      VREAD(from,dummy);
    VREAD(from,offsetVectorROI);

    for (int i=0;i<3;i++)
      VREAD(from,dummy);
      
    double DeltaT;
    VREAD(from,DeltaT);
    VREAD(from,dummy);
    VREAD(from,offsetscaninfo);
    VREAD(from,dummy);

    VREAD(from,timesoffset);
    VREAD(from,dummy);
    VREAD(from,offsetDataROI);
    VREAD(from,offsetBleachROI);
    for (int i=0;i<9;i++)
      VREAD(from,dummy);
    VREAD(from,offsetMeanROI);

    if (offsetVectorROI != 0)
      {
      cout << "\nFound Vector ROI\n" << flush;
      ParseROIs(DataROIs, offsetVectorROI);
      }
    else
      cout << "\nNo Vector ROI could be found\n" << flush;

    if (offsetBleachROI != 0)
      {
      cout << "\nFound Bleach ROI\n" << flush;
      ParseROIs(BleachROIs, offsetBleachROI);
      }
    else
      cout << "\nNo Bleach ROI could be found\n" << flush;

    if (offsetDataROI != 0)
      {
      cout << "\nFound Data ROI\n" << flush;
      ParseROIs(DataROIs, offsetDataROI);
      }
    else
      cout << "\nNo Data ROI could be found\n" << flush;

    if (offsetMeanROI != 0)
      {
      cout << "\nFound Mean of ROIs ROI\n" << flush;
      ParseROIs(DataROIs, offsetMeanROI);
      }
    else
      cout << "\nNo Mean of ROIs ROI could be found\n" << flush;
    
    datsize = SizeX*SizeY*SizeZ*SizeT*DatType;
    cout << "Sizes: " << SizeX << ", " << SizeY << ", " << SizeZ << ", Times: " << SizeT << ", Elements: " << SizeE << "\n" << flush;
    }

void ParseROIs(ROIs & ROI, uint32 offsetROI)
    {
      int32 numElems=0, drawType=0, ldummy, drawSize=0,elemSize=0;
      uint32 numKnots;
      unsigned short sdummy, disabled;
      unsigned char cdummy;
      double v[8],ddummy;
      bool firstTimeBezier=true;
  	from->seekg(offsetROI,ios::beg);
    //cout << "Parsing Bleach-ROIs\n" << flush;
    VREAD(from,numElems);
    cout << "Number of Elements:" << numElems << flush;
    VREAD(from,drawSize);
    //cout << "size:" << drawSize << "\n" << flush;
    for (int i=0;i<21+16;i++)
      VREAD(from,ldummy);       // then we are at "Measure Flag
    for (int i=0;i<10;i++)
    {
      VREAD(from,cdummy);
      //cout << "flag:" << (int) cdummy << "\n" << flush;
      }
    for (int i=0;i<7;i++)
    {
      VREAD(from,ldummy);
      //cout << "found " << ldummy << "\n" << flush;
      }
     // here the drawing element should start
    VREAD(from,sdummy);  // What is wrong here??

    for (int elem=0; elem < numElems; elem ++)
    {
      VREAD(from,drawType);
      cout << "\n Drawing element nr. " << elem << " Type:" << drawType << "\n" << flush;
      VREAD(from,elemSize);
      //cout << "Size:" << elemSize << "\n" << flush;
      VREAD(from,ldummy);
      VREAD(from,ldummy);
      VREAD(from,ddummy);
      //cout << "found float" << ddummy << "\n" << flush;
      VREAD(from,ddummy);
      //cout << "found float" << ddummy << "\n" << flush;
      for (int i=0;i<17;i++)
      {
        VREAD(from,ldummy);
        //cout << "found " << ldummy << "\n" << flush;
      }
      //cout << "fontname: \n" << flush;
      for (int i=0;i<32;i++)
      {
        VREAD(from,sdummy);
        /*if (sdummy > 0)
          cout << (char)sdummy;*/
      }
      // cout << "\n\n" << flush;
      
      VREAD(from,disabled);
      // if (disabled != 0)
        cout << "disabled " << disabled << "\n" << flush;
      if (noDisable)
        disabled = 0;  // ignore the disable feature

      for (int i=0;i<9;i++)
      {
        VREAD(from,ldummy);
        // cout << "found " << ldummy << "\n" << flush;
      }
     // here the drawing element data should start

      switch (drawType) {

      case DE_FLOATRECTANGLE:
      for (int i=0;i<2;i++)         // What the heck is this ???
      {
        VREAD(from,cdummy);
        //cout << "then " << (int) cdummy << "\n" << flush;
      }
      VREAD(from,numKnots);
      //cout << "Number of knots: " << numKnots<< "\n";
      for (int i=0;i<4;i++)
        VREAD(from,v[i]);
      if (disabled == 0)
        ROI.RectList.push_back(new Rectangle(v[0]+0.0001,v[1]+0.0001,v[2]+0.0001,v[3]+0.0001));
      // cout << "Rect ROI coordinates: " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3] << "\n";
      break;
      case DE_FLOATELLYPSE:
      for (int i=0;i<18;i++)         // What the heck is this ???
      {
        VREAD(from,cdummy);
        //cout << "then " << (int) cdummy << "\n" << flush;
      }
      VREAD(from,numKnots);
      //cout << "Number of knots: " << numKnots<< "\n";
      for (int i=0;i<8;i++)
        VREAD(from,v[i]);
      if (disabled == 0)
        ROI.EllyList.push_back(new Ellypse(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]));
      break;
      case DE_FLOATCIRCLE:
      for (int i=0;i<2;i++)         // What the heck is this ???
      {
        VREAD(from,cdummy);
        cout << "then " << (int) cdummy << "\n" << flush;
      }
      VREAD(from,numKnots);
      // cout << "Cirlce: Number of knots: " << numKnots<< "\n";
      for (int i=0;i<4;i++)
        VREAD(from,v[i]);
      // cout << "coordinates: " << v[0] << ", " << v[1] << ", " << v[2] << ", " << v[3]<< "\n";
      if (disabled == 0)
        ROI.CircList.push_back(new Circle(v[0],v[1],v[2],v[3]));
      break;
      case DE_CLOSEDBEZIER:
	if (firstTimeBezier)
	  {
	  cout << "WARNING: Bezier-spline ROI was used! Unable to draw. Will approximate it by a polyline!\n";
          firstTimeBezier=false;
          }
      case DE_FLOATPOLYLINE:
        for (int i=0;i<2;i++)         // What the heck is this ???
          {
            VREAD(from,cdummy);
            //cout << "then " << (int) cdummy << "\n" << flush;
          }
        VREAD(from,numKnots);
        //cout << "Number of knots: " << numKnots<< "\n";
        {
        PolyLine * myPLine = new PolyLine();
        for (int i=0;i<(int) numKnots;i++)
          {
          VREAD(from,v[0]);
          VREAD(from,v[1]);
          myPLine->AddPoint(v[0],v[1]);
          }
        if (disabled == 0)
          ROI.PolyList.push_back(myPLine);
        }
        break;
      default:
        for (int i=0;i<(int) drawSize;i++)
          VREAD(from,cdummy);
        break;
      }
    }  // end of for-loop elem
    // cout << "Parsed Bleach-ROIs\n" << flush;
  }

  bool IsInBleachROI(int x,int y, int ROINr) {  // Tests whether a pixel is inside any of the ROIs
  return BleachROIs.IsInAnyROI(x,y,ROINr);
  }

  bool IsInDataROI(int x,int y, int ROINr) {  // Tests whether a pixel is inside any of the ROIs
  return DataROIs.IsInAnyROI(x,y,ROINr);
  }

  int GetBleachROICnt() {  // Tests whether a pixel is inside any of the ROIs
  return BleachROIs.GetROICnt();
  }

  int GetDataROICnt() {  // Tests whether a pixel is inside any of the ROIs
  return DataROIs.GetROICnt();
  }
    
void ReadSubblock()
{
    uint32 Entry=SUBBLOCK_END,Type,VarSize=0;
    unsigned char dummy;
    VREAD(from,Entry);
    VREAD(from,Type);
    VREAD(from,VarSize);
    // cout << "Parsing subblock " << hex << Entry << dec << " size " << VarSize << "\n";
    if (Entry ==  SUBBLOCK_DESCRIPTION)
      cout << "\n--------------------- Description ---------------------\n"<<flush;
    if (Entry ==  SUBBLOCK_NOTES)
      cout << "\n--------------------- Notes       ---------------------\n"<<flush;
    for (int i=0;i<(int) VarSize;i++)
    {
       VREAD(from,dummy);
       if (dummy == '\r') dummy = '\n';
       if (Entry ==  SUBBLOCK_DESCRIPTION || Entry ==  SUBBLOCK_NOTES)
         cout << dummy << flush;
    }
    if (Entry ==  SUBBLOCK_DESCRIPTION)
      cout << "\n--------------------- End Description -----------------\n"<<flush;
    if (Entry ==  SUBBLOCK_NOTES)
      cout << "\n--------------------- End Notes       -----------------\n"<<flush;
    if (Entry != SUBBLOCK_END)
      ReadSubblock();
    return;
}

void  PrintNotes()
{
    //cerr << "PrintNotes @" << offsetscaninfo << "\n" << flush;
    if (offsetscaninfo != 0)
    {
      if (! from->seekg(offsetscaninfo,ios::beg))
        cerr << "Error seeking offsetscaninfo\n"<<flush;
      ReadSubblock();
    }
    else
    {
      cerr << "ERROR ! Did not find offsetscaningfo!";
    }
}

void  GetTimeStamps(double * savehere, int timepoints, int StartFrame, int step, int SizeE)
{
     // cout << "getting timestamps @" << timesoffset << "\n" << flush;
     if (timesoffset > 0)
      {
        from->seekg(timesoffset,ios::beg);
        uint32 NumBytes,NumChannels;
        VREAD(from,NumBytes);
        VREAD(from,NumChannels);
        double StartTime,MyTime;
        VREAD(from,StartTime);
        MyTime = StartTime;
        int t,j=0;

        for (t=0;t < StartFrame;t++)
            VREAD(from,MyTime); // Ignored
        savehere[j++] = MyTime - StartTime;
        cout << "TimeStamp " << StartFrame << " is " << MyTime - StartTime << "\n" << flush;

        for (;t < (int) NumChannels;t++)
          {
            for (int i=1;i < step;i++)
            {
              VREAD(from,MyTime); // Ignored
              t++;
            }
            VREAD(from,MyTime);
            cout << "(" << t << ": " << MyTime - StartTime << "); ";
            if (j == timepoints)
              {
                cout << flush;
                cerr << "WARNING: More Timepoints than space permits. Not saving timestamps !\n"<< flush;
              }
            if (j < timepoints)
              savehere[j] = MyTime - StartTime;
            j++;
            if (j % 20 == 0) cout << "\n" << flush;
          }
         cout << "\n" << flush;
      }
      else
      {
            cout << "WARNING: No Timestamps present! " << flush;
      }
}

  void ScanTable(uint32 myoffset, uint32 mysize, uint32 * tabletofill) {
    // cout << "Scanning Table @ " << myoffset << "\n"<<flush;
    if (mysize == 1)
    {
      // cout << "Size is 1 !! using offset as value : " << myoffset << "\n"<<flush;
      tabletofill[0] = myoffset;
      return;
      }
      
  	from->seekg(myoffset,ios::beg);
    for (int i=0;i < (int) mysize;i++)
    {
      VREAD(from,tabletofill[i]);
      // cout << "Val : "<<tabletofill[i] << ", " << flush;
    }
    // cout << "Table scanned\n"<<flush;
    }

  bool ReadDir() {
    tableoffset=0;
    tablesize=0;
    sizesoffset=0;
    sizessize=0;
    infooffset=0;
    bitsoffset=0;
    colormap=0;
    compress=0;
    //cout << "Read Dir @ " << offset << "\n"<<flush;
    if (offset == 0) return false;
  	from->seekg(offset,ios::beg);
    VREAD(from,numtags);
    //cout << "numtags " << numtags << "\n"<<flush;
    for (int i=0;i < numtags;i++)
    {
     // cout << "New Tag\n"<<flush;
      VREAD(from,dtag[i]);
      VREAD(from,dtype[i]);
      VREAD(from,dlng[i]);
      /* if (dtag[i] == TIF_COMPRESSION)
      {
      VREAD(from,compress);
      VREAD(from,dummy);
        }
      else*/
      VREAD(from,dval[i]);
      switch(dtag[i]) {
        case TIF_IMAGEWIDTH:
        width = dval[i];
        break;
        /* case TIF_IMAGELENGHT:
        height = dval;
        break; */
        case TIF_BITSPERSAMPLE:
        bitssize = dlng[i];
        bitsoffset = dval[i];
        // cout << "BITS : " << bitssize << ", " << bitsoffset << "\n";
        break;
        case TIF_STRIPOFFSETS:
        tablesize = dlng[i];
        tableoffset = dval[i];
        break;
        case TIF_STRIPBYTECOUNTS:
        sizessize = dlng[i];
        sizesoffset = dval[i];
        break;
        case TIF_CZLSMINFO:
        infooffset = dval[i];
        case TIF_COLORMAP:
        colormap = dval[i];
        default:
        break;
      }
    }
    VREAD(from,offset);  // for next directory

    if (infooffset != 0) ParseInfo();
    if (tableoffset != 0) ScanTable(tableoffset, tablesize, dataoffsets);
    if (sizesoffset != 0) ScanTable(sizesoffset, sizessize, datasizes);
    if (bitsoffset != 0) ScanTable(bitsoffset, bitssize, bits);
    if (compress != 1)
      {
      // cerr << "WARNING! File is compressed!\n"<<flush;
      // exit (-1);
      }
    actstrip ++;
    if (numtags > 0 && width != (uint32) SizeX)
      {
        // cout << "Found Thumpnail image, which will be ignored\n";
        return ReadDir();  // this is a thumpnail, which should be ignored
      }
    return true;  
  }
    
};




#endif