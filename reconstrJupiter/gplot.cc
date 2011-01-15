// generates inputfile for gnuplot and optionally starts gnuplot with it

/*   This file is part of a software package written by 
     Rainer Heintzmann
     Institute of Applied Optics and Information Processing
     Albert Ueberle Strasse 3-5
     69120 Heidelberg
     Tel.: ++49 (0) 6221  549264
     Current Address : Max Planck Inst. for Biophysical Chemistry, Am Fassberg 11, 37077 Goettingen, Germany
     Tel.: ++49 (0) 551 201 1029, e-mail: rheintz@gwdg.de  or rainer@heintzmann.de
     No garantee, whatsoever is given about functionallaty and  savety.
     No warranty is taken for any kind of damage it may cause.
     No support for it is provided !

     THIS IS NOT FREE SOFTWARE. All rights are reserved, and the software is only
     given to a limited number of persons for evaluation purpose !
     Please do not modify and/or redistribute this file or any other files, libraries
     object files or executables of this software package !
*/

#include <iostream>
#include <stdlib.h>
#include "vec.h"
#include "parseargs.h"
#include "rawarray.h"

typedef float ArrayBType;

typedef TArray3d<ArrayBType>   TArray;  // Clipping is not needed

const int MAXTITLE=4;

TArray Measured,Coordinates,VecLen;

void usage(char * filename)
{
  cerr <<  "usage: " << filename << " [-k] [-i infile] [-title ...] [-xtitle ...] [-ytitle] [-xrange xrange] [-yr yrange] [-ps psfilename] [-c] [-o outputfile] [-p preamble]\n" << flush;
  cerr << "[-c] : color ps ? \n";
  cerr << "[-land] : landscape ps ? \n";
  cerr << "[-gp] : If given gnuplot is started automatically \n";
  exit(-1);
}

int main(int argc, char *argv[])
{  
static int MEASUREDSizeX=256,CSizeX=256;
static int MEASUREDSizeY=1,CSizeY=1;
static int MEASUREDSizeZ=1,CSizeZ=1;
int startgp=0,Elem,CElem,MaxColor,Nr,khoros=0,i,j,x,y,z,color=0,landscape=0,vecflag=0,PSFontSize=24,FitNr=0, nolegend=0;

double Scale=1.0,Offset=0.0;

string InFileName, OutFileName, PsFile,CorInFileName,VecLenFileName,
     Preamble, title,xtitle,ytitle,ztitle,Fit,FitF,FitS,Via,
     xrange, xfitrange, yrange, zrange,gnucomand,ftitle;
     
string dtitle[MAXTITLE],dext[MAXTITLE];
bool MakeEPS = false,quiet=false, FitErrors=false;

char ** parg= & argv[1];

printCall (argc,argv);

while (* parg)
  {
    if (readArg("-k",parg)) {khoros=1;continue;}
    if (readArg("-v",parg)) {vecflag=1;continue;}
    if (readArg("-gp",parg)) {startgp=1;continue;}
    if (readArg("-c",parg)) {color=1;continue;}
    if (readArg("-nolegend",parg)) {nolegend=1;continue;}
    if (readArg("-quiet",parg)) {quiet=true;continue;}
    if (readArg("-land",parg)) {landscape=1;continue;}
    if (readArg("-mx",& MEASUREDSizeX,parg)) continue;
    if (readArg("-my",& MEASUREDSizeY,parg)) continue;
    if (readArg("-p",Preamble,parg)) continue;
    if (readArg("-fn",& FitNr,parg)) continue;          // Nr of element to fit to
    if (readArg("-fit",Fit,parg)) continue;
    if (readArg("-fiterrors",parg)) {FitErrors=true;continue;}  // account for errors in fit
    if (readArg("-fitf",FitF,parg)) continue;
    if (readArg("-fits",FitS,parg)) continue;
    if (readArg("-via",Via,parg)) continue;
    if (readArg("-sc",& Scale,parg)) continue;
    if (readArg("-of",& Offset,parg)) continue;
    if (readArg("-title", title,parg)) continue;
    if (readArg("-dtitle0",dtitle[0],parg)) continue;
    if (readArg("-dext0",dext[0],parg)) continue;
    if (readArg("-dtitle1",dtitle[1],parg)) continue;
    if (readArg("-dext1",dext[1],parg)) continue;
    if (readArg("-dtitle2",dtitle[2],parg)) continue;
    if (readArg("-dext2", dext[2],parg)) continue;
    if (readArg("-dtitle3", dtitle[3],parg)) continue;
    if (readArg("-dext3", dext[3],parg)) continue;
    if (readArg("-ftitle",ftitle,parg)) continue;
    if (readArg("-xtitle",xtitle,parg)) continue;
    if (readArg("-xfitrange",xfitrange,parg)) continue;
    if (readArg("-xrange",xrange,parg)) continue;
    if (readArg("-ytitle",ytitle,parg)) continue;
    if (readArg("-yrange", yrange,parg)) continue;
    if (readArg("-ztitle", ztitle,parg)) continue;
    if (readArg("-zrange", zrange,parg)) continue;
    if (readArg("-i", InFileName, parg)) continue;        // 1D-2D hight-field or vector starting points
    if (readArg("-vl", VecLenFileName, parg)) continue;  // Lengt and directions of vectors
    if (readArg("-ci", CorInFileName, parg)) continue;
    if (readArg("-o", OutFileName, parg)) continue;
    if (readArg("-ps", PsFile, parg)) continue;
    if (readArg("-eps", parg)) {MakeEPS = true; continue;}
    if (readArg("-fontsize",& PSFontSize,parg)) continue;
 
    usage(argv[0]);
  }

  ofstream * ostrm= new ofstream(OutFileName.c_str());

  if (! ostrm)
    cerr << "Could not open file " << OutFileName << " for writing ! \n", exit(-1);

  (* ostrm) << "# automatical generated gnuplot file \n";
 if (Preamble[0])
    {
      (* ostrm) << Preamble << "\n";
    }


  if (xtitle[0])
    (* ostrm) << "set xlabel \"" << xtitle <<"\"\n";

  if (ytitle[0])
    (* ostrm) << "set ylabel \"" << ytitle <<"\"\n";

  if (ztitle[0])
    (* ostrm) << "set zlabel \"" << ztitle <<"\"\n";

  if (title[0])
    (* ostrm) << "set title \"" << title <<"\"\n";

  if (xrange[0])
    (* ostrm) << "set xrange " << xrange <<"\n";

  if (yrange[0])
    (* ostrm) << "set yrange " << yrange <<"\n";

  if (zrange[0])
    (* ostrm) << "set zrange " << zrange <<"\n";


  // if necessary, first do the fit and then print and display the data
  if (FitF != "")   // Fit a funktion to data
    {
      Elem=Measured.DLoad(khoros,InFileName.c_str(),"Float",& MEASUREDSizeX,& MEASUREDSizeY,& MEASUREDSizeZ,FitNr);
      if (CorInFileName != "")
      {
        if (FitNr < CElem)
          CElem=Coordinates.DLoad(khoros,CorInFileName.c_str(),"Float",& CSizeX,& CSizeY,& CSizeZ,FitNr);
          else
          CElem=Coordinates.DLoad(khoros,CorInFileName.c_str(),"Float",& CSizeX,& CSizeY,& CSizeZ,0);
        if (CSizeX < MEASUREDSizeX)
        {
          cerr << "ERROR!! Coordinate Size is smaller than Data size!\n";
          exit (-1);
        }
      }

      (* ostrm) << FitF << " = " << Fit<< "\n";
      (* ostrm) << FitS << "\n";
      if (Via!="")   // else jsut plot this
	{
	  (* ostrm) << "fit ";
  	  if (xfitrange[0])
    	  (* ostrm) << xfitrange << " ";
	  (* ostrm) << FitF;
	  if (MEASUREDSizeY == 1 || ! FitErrors)
	    (* ostrm) <<  " '-' using 1:2 ";
	  else if (MEASUREDSizeY == 2)
	    (* ostrm) <<  " '-' using 1:2:3 ";
	   else
	     (* ostrm) <<  " '-' using 1:2:3:(1) "; // matrix ";
	  (* ostrm) << " via " << Via << "\n";
	  
	  for (x=0;x<MEASUREDSizeX;x++)   // print the data for fitting
	    {
	      if (MEASUREDSizeY <= 2)
		{
		  if (CorInFileName=="")
		    (* ostrm) << x*Scale+Offset << "        ";
		  else
		    (* ostrm) << Coordinates.Value(x,0,0) << "        ";

		  (* ostrm) << Measured.Value(x,0,0) << flush;
		  if (MEASUREDSizeY == 2 && FitErrors)
		    (* ostrm) << "        " << Measured.Value(x,1,0) << flush;
		  (* ostrm) << "\n" << flush;
		}
	      else for (y=0;y<MEASUREDSizeY;y++)  // i < Elem
		{
		  (* ostrm) << x*Scale+Offset << "        " << y << "      ";
		  (* ostrm) << Measured.Value(x,y,0) << "\n" << flush;
		}
	      // (* ostrm) << "\n";
	    } 
	  (* ostrm) << "e\n";
	}
    }
  
  // Now plot the data

  Elem=1;
  Nr=0;
  for (i=0;i<Elem;i++)  // i < Elem
    for (z=0;z<MEASUREDSizeZ;z++)  // i < Elem
      {
	Nr++;
	Elem=Measured.DLoad(khoros,InFileName.c_str(),"Float",& MEASUREDSizeX,& MEASUREDSizeY,& MEASUREDSizeZ,i);
	if (VecLenFileName!="")
	  {
	    int SizeX=MEASUREDSizeX,SizeY=MEASUREDSizeY,SizeZ=MEASUREDSizeZ;
	    vecflag=1;
	    VecLen.DLoad(khoros,VecLenFileName.c_str(),"Float",& SizeX,& SizeY,& SizeZ,i);
	  }

	if (CorInFileName!="")
    {
	  if(i==0)
	    CElem=Coordinates.DLoad(khoros,CorInFileName.c_str(),"Float",& CSizeX,& CSizeY,& CSizeZ,i);
	  else
	    if (i >= CElem)
	      {
		cerr << "WARNING : To few elements in Coordinate-File ! Taking element # " << CElem-1 << " for x-coordinates !\n";
		CElem=Coordinates.DLoad(khoros,CorInFileName.c_str(),"Float",& CSizeX,& CSizeY,& CSizeZ,CElem-1);
	      }
	    else
	      {
		// cerr << "Error : Not capable of displaying more than one x-scale !\n",exit(-1);
		CElem=Coordinates.DLoad(khoros,CorInFileName.c_str(),"Float",& CSizeX,& CSizeY,& CSizeZ,i);
	      }
          
       if (CSizeX != MEASUREDSizeX)
         cerr << "Error : Data hast different size than coordinate file !\n",exit(-1);
       else if (CSizeY != MEASUREDSizeY && CSizeY != 1)
         cerr << "Error : Data hast different size than coordinate file !\n",exit(-1);
       else if (CSizeZ != MEASUREDSizeZ)
         cerr << "Error : Data hast different size than coordinate file !\n",exit(-1);
     }
     
   if (Nr==1)
      {
	  if (MEASUREDSizeY > 2)
	    (* ostrm) << "set hidden3d\n";

	  if (PsFile != "")
	    {
	      if (! MakeEPS)
		{
		  (* ostrm) << "set terminal postscript enhanced";
		  if (landscape)
		    (* ostrm) << " landscape ";
		}
	      else
		(* ostrm) << "set terminal postscript eps enhanced";
	      if (color)
		(* ostrm) << " color ";
	      (* ostrm) << PSFontSize << " \n";
	      
	      (* ostrm) << "set output '"<<PsFile<<"'\n";
	    }
	  

	  
	  if (vecflag)
	    (* ostrm) << "plot ";
	  else
	    if (MEASUREDSizeY <= 2)
	      (* ostrm) << "plot ";
	    else
	      (* ostrm) << "splot ";

	  MaxColor= Elem*MEASUREDSizeZ;
	  // if (MaxColor > 3) MaxColor =3;

	  for (j=0;j < MaxColor;j++)
	    {
	      if (vecflag)
		(* ostrm) <<  "'-' using 1:(($2)):3:(($4)) ";  // flip to point to image coordinates
	      else
		if (MEASUREDSizeY == 1)
		  (* ostrm) <<  "'-' using 1:2 ";
		else if (MEASUREDSizeY == 2)
		  (* ostrm) <<  "'-' using 1:2:3 ";
		else
		  (* ostrm) <<  "'-' matrix ";

	      if ((j < MAXTITLE) && dtitle[j][0])
		(* ostrm) << "title \"" << dtitle[j] << "\" ";
	      
	      if (vecflag)
		(* ostrm) <<  "with vector ";
	      else
		if ((j < MAXTITLE) && dext[j][0]) (* ostrm) << dext[j];
		else 
		  if (MEASUREDSizeY ==2)
		    (* ostrm) << "with error ";
		  else 
		    if (dext[0][0])
		       (* ostrm) << dext[0];
		    else
		      (* ostrm) << "with linespoints ";
	      
	      if (j+1 < MaxColor)
		(* ostrm) << " , " ;
	    }

	  if (FitF!="")
	    {
	      (* ostrm) << "," << FitF;
	      if (ftitle !="")
		(* ostrm) << " title \"" << ftitle << "\" ";
	    }

	  (* ostrm)<< "\n";
	}

      if (vecflag)
	{
	  if (Measured.GetSize(0) < 3) 
	    cerr << "Fatal error : starting-points vectorfile has to contain at least 3 values in x-direction (1st is intensity: ignored)!\n",exit(-1);
	  if (VecLen.GetSize(0) < 3)
	    cerr << "Fatal error : direction vectorfile has to contain at least 3 values in x-direction (1st is intensity: ignored) !\n",exit(-1);
	  for (y=0;y<MEASUREDSizeY;y++)  // i < Elem
	    {
	      if ((VecLen.GetSize(0) >= 3) && (VecLen.GetSize(1) > y)) 
		{
		  (* ostrm) << Measured.Value(1,y,z) << "     " << flush;
		  (* ostrm) << Measured.Value(2,y,z) << "     " << flush;
		  (* ostrm) << VecLen.Value(1,y,z) << "     " << flush;
		  (* ostrm) << VecLen.Value(2,y,z) << "\n" << flush;
		}
	      else
		{
		  (* ostrm) << 0 << "     " << flush;
		  (* ostrm) << 0 << "     " << flush;
		  (* ostrm) << Measured.Value(1,y,z) << "     " << flush;
		  (* ostrm) << Measured.Value(2,y,z) << "\n" << flush;
		} 
	    }
	}
      else
	for (x=0;x<MEASUREDSizeX;x++)   // print the data for displaying
	  {
	    if (MEASUREDSizeY <= 2)
	      if (CorInFileName=="")    // Problem with two x-scales !!
            (* ostrm) << x*Scale+Offset << "        ";
	      else
            (* ostrm) << Coordinates.Value(x,0,0) << "        ";
	    
	    for (y=0;y<MEASUREDSizeY;y++)  // i < Elem
	      {
		(* ostrm) << Measured.Value(x,y,z) << "     " << flush;
	      }
	    (* ostrm) << "\n";
	  }

      (* ostrm) << "e\n";
      
    }

  if (PsFile=="")
    {
      if (FitF != "")
      {
        if (!quiet)
          (* ostrm) << "pause 30\n" << flush ;  // isnt there a better solution ?? To be able to read the fit
        if (!quiet)
          gnucomand+="xterm -e gnuplot -persist "+OutFileName+"\n";
        else
          gnucomand+="xterm -e gnuplot "+OutFileName+"\n";
      }
      else
        if (!quiet)
          gnucomand +="gnuplot -persist "+OutFileName+"\n";
        else
          gnucomand +="gnuplot "+OutFileName+"\n";
    }
  else
    {
      (* ostrm) << "set terminal x11\n";
      // (* ostrm) << "# replot\n";  // This does not work
      (* ostrm) << "# GENERATED POSTSCRIPT\n" << flush ;  // isnt there a better solution ??
      (* ostrm) << "pause 1\n" << flush ;  
      (* ostrm) << "quit\n";
      // sprintf(gnucomand,"xterm -e gnuplot %s\n",OutFileName);
            gnucomand+="gnuplot "+OutFileName+"\n";
    }

  ostrm->close();

  int retval=0;
  
  if (startgp)
    retval=system(gnucomand.c_str());
  if (retval!=0) 
    cerr << "gnuplot command failed with return code:  " << retval << "\n",exit(-1);
  string ghostview("gv -scale 2 " + PsFile+" &\n");
  if (PsFile!="" && !quiet)
    retval=system(ghostview.c_str());
  if (retval!=0) 
    cerr << "ghostview command failed with return code:  " << retval << "\n",exit(-1);
  return retval;

}
