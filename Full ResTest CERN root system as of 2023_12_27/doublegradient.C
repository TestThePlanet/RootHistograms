#ifdef __CLING__
#pragma cling optimize(0)
#endif
#include <iostream>  
#include "TAxis.h"
#include <TStyle.h>
#include "TH2F.h"
#include "TH1F.h"
#include "TRandom1.h"
#include "TCanvas.h"
#include <TImage.h>
//#include "TFile.h"
#include "TTree.h"
#include <TF2.h>
#include "TLegend.h"
#include "TColor.h"
#include "CMSStyle.C"
#include "TMath.h"
#include <TPaveText.h>
//#include "cuts.h"
//#include "TVirtualFitter.h"

void doublegradient(){
	static const int nbins = 60;
	TRandom* r = new TRandom1();
	CMSStyle(); //RETURN //This kills the title somehow

	float* logbinning = new float[nbins+1];
	float* linbinning = new float[nbins+1];
	for(int i=0;i<nbins+1;i++){
        linbinning[i] = 0.1*i;
		logbinning[i] = (float) pow(10., double(linbinning[i]) );
	}
	TH1F* hist = new TH1F("h","Mask 1;Exposure Reduction Factor                              ;Likelihood",nbins,linbinning);
    TH1F* histarr[nbins];
    for(int i=0;i<nbins;i++){
        histarr[i] = new TH1F( ("h"+std::to_string(i)).c_str(),";m41 Score;Relative Probability", nbins,linbinning);
    }
    float ymax = 75.;
    SetRange(hist,0,ymax);
	PrettyFillColor(hist, kAzure + 5);
    TF2* grad = new TF2("grad","x/(4+x)",linbinning[0],linbinning[nbins],0,ymax);
    grad->SetNpx(300);
    //alternative functions: https://upload.wikimedia.org/wikipedia/commons/6/6f/Gjl-t%28x%29.svg

    //define the color gradient
    const Int_t Ncolors = 3;
    Double_t Red[Ncolors] = {0.957,1.0,1.0};
    Double_t Green[Ncolors] = {0.435,1.0,1.0};
    Double_t Blue[Ncolors] = {0.039,1.0,1.0};
    Double_t Stops[Ncolors] = {0.0,0.5,1.0};
    Int_t nb = 128;
    TColor::CreateGradientColorTable(Ncolors,Stops,Red,Green,Blue,nb);
    grad->SetContour(nb);
    //grad->SetLineWidth(1);


    

	//set the bin labels
	TAxis *x = hist->GetXaxis();
	for(int i=1;i<nbins+1;i++){
		char label[20];
		if(logbinning[i] < 100 && logbinning[i-1] < 100){
			sprintf(label, "%.1f - %.1f",logbinning[i-1], logbinning[i]);
        } else if(logbinning[i] >= 100 && logbinning[i-1] < 100){
			sprintf(label, "%.1f - %.0f",logbinning[i-1], logbinning[i]);
		} else {
			sprintf(label, "%.0f - %.0f",logbinning[i-1], logbinning[i]);
		}
		//cout<<i<<" "<<logbinning[i]<<" "<<label<<endl;
		x->SetBinLabel(	i, label);
		//x->ChangeLabel(	i, 45.);
	}
	
	//Fill it
	for(int i = 0;i<500;i++){
		hist->Fill(TMath::Log10(r->Landau(100.,30.)));
	}
    UnitNorm(hist);
    double mean = hist->GetMean();
    double stddev = hist->GetStdDev();
    cout<<"mean "<<mean<<" stdev "<<stddev<<endl;
    for(int i=0;i<nbins;i++){
        float bc = hist->GetBinCenter(i+1);
        float xxx = (bc - mean)/stddev;
        float gray = 1.0f/(1.0f + exp(-2.0*xxx));
        cout<<"i "<<i<<" bc "<<bc<<" x "<<xxx<<" gray "<<gray<<endl;
        //float gray = float(i)/float(nbins);
        histarr[i]->SetFillColor(TColor::GetColor( gray, gray, 1.0f));
        histarr[i]->SetLineColor(TColor::GetColor( gray, gray, 1.0f));
        histarr[i]->SetFillStyle(1001);
    }
	//could also do h->FillN
    for(int i=0;i<nbins;i++){
        histarr[i]->SetBinContent(i+1,hist->GetBinContent(i+1));
        //PrettyHist(histarr[i]
    }
//ways to do this: either fill with log10 and have custom bin labels, or have custom binning and fill with regular nubmers. 
//lets do the latter.
//and just set log x
	
	TCanvas * C = newTCanvas(const_cast<char*>("thehist"), const_cast<char*>("Random Mask-Like Data"),1660,989);
	////TLegend *leg = new TLegend(0.646985, 0.772727, 0.978643, 0.891608);
    ////PrettyLegend(leg);
	////leg->AddEntry(h,"Super nice histogram");
	PrettyHist(hist,kAzure + 5,3); //RETURN
	C->cd();
	gStyle->SetOptStat(0);
	hist->Draw();
    grad->Draw("colzsame");
	//hist->Draw("E1same");
	//hist->Draw("same");
    for(int i=0;i<nbins;i++){
        histarr[i]->Draw("same");
    }

    TPaveText *pt = new TPaveText(0.4432569,0.94,0.5567431,0.995,"blNDC");
    pt->SetBorderSize(0);
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(132);
    TText *pt_LaTex = pt->AddText(hist->GetTitle());
    pt->Draw();

    gPad->RedrawAxis();
	//leg->Draw("same");
	C->SaveAs("thehist2.png");
}
