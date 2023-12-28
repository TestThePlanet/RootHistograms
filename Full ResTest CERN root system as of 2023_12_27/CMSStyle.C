#ifdef __CLING__
#pragma cling optimize(0)
#endif
#include "TStyle.h"
#include "TLegend.h"
#include "TH1.h"
#include "TROOT.h"
#include <TCanvas.h>
#include "TLegend.h"

#include <TPaveText.h>
//##########################################################################
// Initialization code executed at the start of a ROOT session.
//
// File: $Id: CMSStyle.C,v 1.1 2010/08/20 00:20:34 iraklis Exp $
//##########################################################################

//#include <iostream>
//using namespace std;
 

void CMSStyle();
void PrettyLegend(TLegend* leg, float fontSize = 0.05);
void PrettyHist(TH1F* h, int color = 1, int width = 3, int linestyle = 0);

void PrettyMarker(TH1F* h, int color = 1, int markerstyle = 20, float markersize = 1.2);
void PrettyFonts(TH1F* h);
void PrettyFillColor(TH1F* h, int color);

TLegend* newTLegend();
void UnitNorm(TH1F* hist);
void MatchArea(TH1F* toScale, TH1F* toMatch);
void SameRange(TH1F* hist1, TH1F* hist2);
void SetRange(TH1F* hist, float min = 0.0f, float max = -1.15);

void DrawSame(TH1F* h0, TH1F* h1, char* ops=const_cast<char*>(""));
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, char* ops=const_cast<char*>(""));
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, TH1F* h3, char* ops=const_cast<char*>(""));
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, TH1F* h3,TH1F* h4, char* ops=const_cast<char*>(""));
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, TH1F* h3,TH1F* h4,TH1F* h5, char* ops=const_cast<char*>(""));
TCanvas * newTCanvas(char* rootname, char* title = const_cast<char*>(""), int x = 800, int y = 800);
TCanvas * newTCanvas(string rootname, string title = "", int x = 800, int y = 800);
/*TPaveText AddPrettyTitle(string theTitle);

TPaveText AddPrettyTitle(string theTitle){
     TPaveText *pt = new TPaveText(0.4432569,0.94,0.5567431,0.995,"blNDC");
     pt->SetBorderSize(0);
     pt->SetFillColor(0);
     pt->SetFillStyle(0);
     pt->SetTextFont(42);
     TText *pt_LaTex = pt->AddText(theTitle);
     return pt
}*/

void CMSStyle(){
  // use the 'plain' style for plots (white backgrounds, etc)
  //  cout << "...using style 'Plain'" << endl;
  /* This is killing the title somehow. Candidates:
   * gStyle->SetOptFit(1);
   * gStyle->SetOptTitle(0);
   * h__1->SetStats(0);
   *
   */

  gROOT->SetStyle("Plain");

  
  // Create the 'CMS' style for approved plots. Note that this style may need
  // some fine tuning in your macro depending on what you are plotting, e.g.
  //
  //  gStyle->SetMarkerSize(0.75);  // use smaller markers in a histogram with many bins
  //  gStyle->SetTitleOffset(0.65,"y");  // bring y axis label closer to narrow values
  
  TStyle *cmsStyle= new TStyle("CMS","CMS approved plots style"); //This is enough to kill the title.
  
  // use plain black on white colors
  cmsStyle->SetFrameBorderMode(0);
  cmsStyle->SetCanvasBorderMode(0);
  cmsStyle->SetPadBorderMode(0);
  cmsStyle->SetPadColor(0);
  cmsStyle->SetCanvasColor(0);
  cmsStyle->SetTitleColor(kBlack);
  cmsStyle->SetStatColor(0);
  cmsStyle->SetFrameFillColor(0); 

  // set the paper & margin sizes
  cmsStyle->SetPaperSize(20,26);
  cmsStyle->SetPadTopMargin(0.05);
  cmsStyle->SetPadRightMargin(0.08);//change.. orig was 0.05
  cmsStyle->SetPadBottomMargin(0.17);
  cmsStyle->SetPadLeftMargin(0.17);

  // use large Times-Roman fonts
  cmsStyle->SetTextFont(42);
  cmsStyle->SetTextSize(0.08);
  cmsStyle->SetLabelFont(42,"x");
  cmsStyle->SetLabelFont(42,"y");
  cmsStyle->SetLabelFont(42,"z");
  cmsStyle->SetLabelSize(0.05,"x");
  cmsStyle->SetTitleSize(0.06,"x");
  cmsStyle->SetLabelSize(0.05,"y");
  cmsStyle->SetTitleSize(0.06,"y");
  cmsStyle->SetLabelSize(0.05,"z");
  cmsStyle->SetTitleSize(0.06,"z");

  // use bold lines and markers
  cmsStyle->SetMarkerStyle(8);
  cmsStyle->SetHistLineWidth(1.);
  //cmsStyle->SetHistLineWidth(1.85);
  cmsStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes
  



  // do not display any of the standard histogram decorations
  cmsStyle->SetOptTitle(1);
  cmsStyle->SetOptStat(1);
  cmsStyle->SetOptFit(1);
  cmsStyle->SetOptTitle(0);
  cmsStyle->SetOptStat(0);

  // put tick marks on top and RHS of plots
  cmsStyle->SetPadTickX(1);
  cmsStyle->SetPadTickY(1);

//  cout << endl << "    For approved plots use: gROOT->SetStyle(\"CMS\");"
//       << endl << "  To add a CMS label use: CMSLabel();"
//       << endl << endl;

  // restore the plain style
  gROOT->SetStyle("Plain");
  gROOT->SetStyle("CMS");
  return ;
}


void PrettyLegend(TLegend* leg, float fontSize){
	//default fontSize = 0.05;
        leg->SetTextFont(42);
        leg->SetFillColor(0);
        leg->SetBorderSize(0);
        leg->SetTextSize(fontSize);
}

void PrettyHist(TH1F* h, int color, int width, int linestyle ){
//defaults: PrettyHist(TH1F* h, int color = 1, int width = 3, int linestyle = 0){
        h->SetLineColor(color); // formerly "color" and "kBlack"
        h->SetLineWidth(width);
        h->SetLineStyle(linestyle);
        h->SetStats(0);
        h->GetXaxis()->CenterTitle(true);
        h->GetYaxis()->CenterTitle(true);
        PrettyFonts(h);
}

	///Can I do this? 
	//Descriptive constants for line Styles. 
static const int sSolid = 1;
static const int sdashed = 2;
static const int sDashed = 7;
static const int sdotted = 3;
static const int sdotdash = 4;
static const int sdotDash = 5;
static const int sbigDash = 9;
static const int bigdotDash = 10;

void PrettyFonts(TH1F* h){
        TAxis * x = h->GetXaxis();
        TAxis * y = h->GetYaxis();
        x->SetTitleFont(42);
        y->SetTitleFont(42);
        x->SetTitleSize(0.06);
        x->SetTitleOffset(2.05); //make the Title a little further from the axis // originally // x->SetTitleOffset(1.71); 2.21 ok but goes off page
        y->SetTitleOffset(0.71); // .82
        y->SetTitleSize(0.06);
        x->SetLabelFont(42);
        y->SetLabelFont(42);
        x->SetLabelSize(0.04);
        y->SetLabelSize(0.06);
}

void PrettyFillColor(TH1F* h, int color){
        h->SetLineColor(color);
        h->SetFillColor(color);
        h->SetFillStyle(1001);
}

void PrettyMarker(TH1F* h, int color, int markerstyle, float markersize){
           h->SetMarkerColor(color);
           h->SetMarkerSize(markersize);
           h->SetMarkerStyle(markerstyle);
}
/*
 Marker Styles: 
 Marker number         Marker shape          Marker name
 1                    dot                  kDot
 2                    +                    kPlus
 3                    *                    kStar
 4                    o                    kCircle
 5                    x                    kMultiply
 6                    small scalable dot   kFullDotSmall
 7                    medium scalable dot  kFullDotMedium
 8                    large scalable dot   kFullDotLarge
 9 -->19              dot
 20                    full circle          kFullCircle
 21                    full square          kFullSquare
 22                    full triangle up     kFullTriangleUp
 23                    full triangle down   kFullTriangleDown
 24                    open circle          kOpenCircle
 25                    open square          kOpenSquare
 26                    open triangle up     kOpenTriangleUp
 27                    open diamond         kOpenDiamond
 28                    open cross           kOpenCross
 29                    open star            kOpenStar
 30                    full star            kFullStar
 http://physics.lunet.edu/docs/root/TAttMarker.html
 */


TLegend* newTLegend(){
		//makes a pretty TLegend with an unremarkable initial position. Use in place of "new TLegend(..." when you're lazy.
	TLegend* leg = 	new TLegend(0.75,0.60,.95,.80);
	PrettyLegend(leg);
	return leg;
}
void UnitNorm(TH1F* hist){ //makes hist have unit norm
	float norm = hist->Integral();
	hist->Scale(1.f/norm);
}
void MatchArea(TH1F* toScale, TH1F* toMatch){
		//makes the area of toScale the same as toMatch
	float normM = toMatch->Integral();
	float normS = toScale->Integral();
	toScale->Scale(normM/normS);
}
void SetRange(TH1F* hist, float min, float max){ 
		//set histogram range. max = -1.15 -> max is 115% highest bin.
	if (max<-1 && max > -1.2) max = 1.15*hist->GetMaximum();
	hist->SetMaximum(max);
	hist->SetMinimum(min);
}
void SameRange(TH1F* hist1, TH1F* hist2){
		//sets makes the range of both histograms accomodate either histogram. 
	float max = 1.15*TMath::Max(hist1->GetMaximum(),hist2->GetMaximum());
	float min = TMath::Min(hist1->GetMinimum(),hist2->GetMinimum());
	hist1->SetMaximum(max);
	hist1->SetMinimum(min);
	hist2->SetMaximum(max);
	hist2->SetMinimum(min);
}

void DrawSame(TH1F* h0, TH1F* h1, char* ops){
	string same = "same";
	h0->Draw(ops);
	h1->Draw((same+ops).c_str());
}
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, char* ops){
	string same = "same";
	h0->Draw(ops);
	h1->Draw((same+ops).c_str());
	h2->Draw((same+ops).c_str());
}

void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, TH1F* h3, char* ops){
	string same = "same";
	h0->Draw(ops);
	h1->Draw((same+ops).c_str());
	h2->Draw((same+ops).c_str());
	h3->Draw((same+ops).c_str());
}
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, TH1F* h3,TH1F* h4, char* ops){
	string same = "same";
	h0->Draw(ops);
	h1->Draw((same+ops).c_str());
	h2->Draw((same+ops).c_str());
	h3->Draw((same+ops).c_str());
	h4->Draw((same+ops).c_str());
}
void DrawSame(TH1F* h0, TH1F* h1, TH1F* h2, TH1F* h3,TH1F* h4,TH1F* h5, char* ops){
	string same = "same";
	h0->Draw(ops);
	h1->Draw((same+ops).c_str());
	h2->Draw((same+ops).c_str());
	h3->Draw((same+ops).c_str());
	h4->Draw((same+ops).c_str());
	h5->Draw((same+ops).c_str());
}



TCanvas * newTCanvas(char* rootname, char* title, int x, int y){
	TCanvas * canv =new TCanvas( rootname, title, x, y);
    canv->Range(-0.4507237,-11.42139,6.157133,74.97806);

    canv->SetFillColor(kWhite);
    canv->SetFrameFillColor(kWhite);
    canv->SetFrameFillStyle(0);
    canv->SetFrameLineColor(kWhite);
    canv->SetBorderMode(0);
    canv->SetBorderSize(2);
    canv->SetTickx(1);
    canv->SetTicky(1);
     canv->SetLeftMargin(0.0);
     canv->SetLeftMargin(0.10659831);
     //canv->SetLeftMargin(0.07659831);
   canv->SetRightMargin(0.0331725);
    canv->SetTopMargin(0.06438214);
    canv->SetBottomMargin(0.2321929);


    canv->SetFrameBorderMode(0);
    canv->SetFrameBorderMode(0);
	canv->cd();
	return canv;
}

TCanvas * newTCanvas(string rootname, string title, int x, int y){
    return newTCanvas(rootname.c_str(), title.c_str());
}




