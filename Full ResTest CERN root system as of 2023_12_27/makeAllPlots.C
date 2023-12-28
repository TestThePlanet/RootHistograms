#ifdef __CLING__
#pragma cling optimize(0)
#endif
#include "TAxis.h"
#include <TStyle.h>
#include "TH2F.h"
#include "TH1F.h"
//#include "TRandom1.h"
#include "TCanvas.h"
#include <TImage.h>
//#include "TTree.h"
#include <TF2.h>
#include "TLegend.h"
#include "TColor.h"
#include "CMSStyle.C"
#include "TMath.h"
#include <TPaveText.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "TH1F.h"

#define nbins (60)

std::vector<std::string> parseTSVLine(const std::string& tsv_line);
float Str2float(std::string& token);
int   Str2int(std::string& token);
bool  Str2bool(std::string& token);
float* generateLinBinning();
float* generateLogBinning();
TF2* makeGrad(float ymax);
void SetBinLabels(TH1F* hist);
void PlotAndSave(TH1F* hist, TF2* grad, string fname_noext);

/////////////////////////////////////////////////////////////////
void makeAllPlots(){ //main

    /////////////// INTAKE PARAMETERS ///////////////////////////
    static const std::string tsv_filename = "Main.tsv";
    static const long unsigned int maskname_index = 2;
    static const long unsigned int exer1_index = 3;
    static const int nexer = 12; //says that there are 12 exercises going from indicies [exer1_index..exer1_index + nexer)
    static const int analysis_grade_index = 20;
    static const bool use_only_analysis_grade = true;
    static const bool enable_save_plots = true;

    static const bool just_save_one  = false; //TRUE enables single plot mode 
    static const string which_one = "3M AFFM"; //the mask name of the one plot to make.
    /////////////////////////////////////////////////////////////
    if( just_save_one) std::cout<<"Single Plot Mode ENABLED, see just_save_one"<<std::endl;
    std::unordered_map<std::string, TH1F*> hMap;
	CMSStyle(); 
	float* linbinning = generateLinBinning();
    TF2* grad = makeGrad(75.f); //the float argument (75) is the height that the gradient will go up to. anything pretty big is ok.

    std::ifstream inputFile(tsv_filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error unable to opening file" << tsv_filename << std::endl;
        return;
    }

    std::string tsv_line;
    int jline=0; //tsv line number counter.
    while (std::getline(inputFile, tsv_line)) { //for every line
        jline++; 
        // Parse the line into tokens
        std::vector<std::string> tokens = parseTSVLine(tsv_line);

        if(tokens.size() < exer1_index)
            std::cout<<"ERROR nogo line "<<jline<<std::endl;
        else if(tokens.size() < exer1_index + nexer) //
            std::cout<<"ERROR weird length line "<<jline<<std::endl;
        else{ //line not obviously invalid
            if(jline % 100 == 0) std::cout<<"readln "<<jline<<std::endl;

            if(use_only_analysis_grade and tokens.size() < analysis_grade_index){
                std::cout<<"Warning line too short for analysis grade "<<jline<<std::endl;
                continue;
            }
            bool is_analysis_grade = Str2bool(tokens[analysis_grade_index]);
            if(use_only_analysis_grade and not is_analysis_grade){
                //std::cout<<"line rejected for non-analysis grade tag "<<jline<<std::endl;
                continue;
            }

            //if the mask on this line has been seen before, find its entry in map. 
            std::unordered_map<std::string, TH1F*>::iterator it = hMap.find(tokens[maskname_index]);

            if (it != hMap.end()) { //This mask has been seen already. Fill the existing histogram
                //int i=0;
                //for (;i<nexer;i++){
                for (int i=0;i<nexer;i++){
                    float x = Str2float(tokens[exer1_index + i]);
                    if(x<0.f) break;
                    else it->second->Fill(TMath::Log10(x));
                }
                //std::cout<<jline<<" "<<tokens[maskname_index]<<" old fill #="<<i<<std::endl;
            } else { //New mask, create a new histogram
                TH1F* newHistogram = new TH1F(tokens[maskname_index].c_str(), 
                        (tokens[maskname_index]+";Exposure Reduction Factor               ;Event Count").c_str(), 
                        nbins,linbinning);
                //int i=0;
                //for (;i<nexer;i++){
                for (int i=0;i<nexer;i++){
                    float x = Str2float(tokens[exer1_index + i]);
                    if(x<0.f) break;
                    else newHistogram->Fill(TMath::Log10(x));
                }
                //std::cout<<jline<<" "<<tokens[maskname_index]<<" new fill #="<<i<<std::endl;
                hMap[tokens[maskname_index]] = newHistogram;
            } //end else 
        } //end else ok line
    } //end while every tsv line
    inputFile.close();

    //Now make all plots and save them to file.
    bool something_was_found = false;
    for (const std::pair<const std::string, TH1F*>& pair : hMap) {
        const std::string& mask = pair.first;
        TH1F* histogram = pair.second;
        if( just_save_one and mask != which_one ) continue;
        something_was_found = true;

        if(enable_save_plots){
            if(just_save_one){
                std::cout<<"Plot and save "<<mask<<" just_save_one = true so no other plot get generated."<<std::endl;
            } else {
                std::cout<<"Plot and save "<<mask<<std::endl;
            }

            PlotAndSave(histogram, grad, mask);
        }
    }
    if( just_save_one and not something_was_found ){
        std::cout<<"Warning! No plot found that matched name which_one = "<<which_one<<std::endl;
    }
    
    for (auto& pair : hMap) {
        delete pair.second;
    }
} //end main

std::vector<std::string> parseTSVLine(const std::string& tsv_line) {
    // Function to parse a TSV line into a vector of tokens
    std::vector<std::string> tokens;
    std::istringstream iss(tsv_line);
    std::string token;
    while (std::getline(iss, token, '\t')) {
        tokens.push_back(token);
    }
    return tokens;
}

float Str2float(std::string& token){
    float floatValue;
    if (token == "") return -2.f;

    try {
        floatValue = std::stof(token);
    } catch (const std::invalid_argument& e) {
        floatValue = -1.f;
    }
    return floatValue;
}

int Str2int(std::string& token){
    int intValue;
    if (token == "") return -2;

    try {
        intValue = std::stoi(token);
    } catch (const std::invalid_argument& e) {
        intValue = -1;
    }
    return intValue;
}

bool Str2bool(std::string& token){
    bool boolValue;
    if (token == "true" || token == "True" || token == "TRUE" || token == "1") {
        boolValue = true;
    } else if (token == "false" || token == "False" || token == "FALSE" || token == "0") {
        boolValue = false;
    } else {
        boolValue = false;
    }
    return boolValue;
}

float* generateLinBinning() {
	//float* logbinning = new float[nbins+1];
	float* linbinning = new float[nbins+1];
	for(int i=0;i<nbins+1;i++){
        linbinning[i] = 0.1*i;
	}
    return linbinning;
}

float* generateLogBinning() {
    float* linbinning = generateLinBinning();
	float* logbinning = new float[nbins+1];
	for(int i=0;i<nbins+1;i++){
		//logbinning[i] = std::pow(10.f, 0.1f*i );
        logbinning[i] = std::pow(10.0f, static_cast<float>(linbinning[i]));
	}
    return logbinning;
}

TF2* makeGrad(float ymax){
    float* linbinning = generateLinBinning();
    TF2* grad = new TF2("grad","x/(4+x)",linbinning[0],linbinning[nbins],0,ymax);
    grad->SetNpx(300);
    //alternative functions: https://upload.wikimedia.org/wikipedia/commons/6/6f/Gjl-t%28x%29.svg

    //define the color gradient
    const UInt_t Ncolors = 3;
    Double_t Red[Ncolors]   = {0.957,0.999,0.999};
    Double_t Green[Ncolors] = {0.435,0.999,0.999};
    Double_t Blue[Ncolors]  = {0.039,0.999,0.999};
    Double_t Stops[Ncolors] = {0.000,0.500,1.000};
    UInt_t nb = 128;
    TColor::CreateGradientColorTable(Ncolors,Stops,Red,Green,Blue,nb,1.0f);
    grad->SetContour(nb);
    //grad->SetLineWidth(1);
    return grad;
}

void SetBinLabels(TH1F* hist){
    float* logbinning = generateLogBinning();
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
}


void PlotAndSave(TH1F* hist, TF2* grad, string fname_noext){
    static const float graymax = 0.95f; 
    //plot it and make it pretty
	PrettyFillColor(hist, kAzure + 5);
    //SetRange(hist,0,ymax);
    SetBinLabels(hist);
    float* linbinning = generateLinBinning();

    TH1F* histarr[nbins];
    for(int i=0;i<nbins;i++){
        histarr[i] = new TH1F( (((string)hist->GetTitle())+std::to_string(i)).c_str(),";m41 Score;Relative Probability", nbins,linbinning);
    }

 //   UnitNorm(hist);
    double mean = hist->GetMean();
    double stddev = hist->GetStdDev();
    //cout<<"    integral: "<<hist->Integral()<< " mean: "<<mean<<" stdev: "<<stddev<<endl;
    //Set Histogram Gradient Color Here
    for(int i=0;i<nbins;i++){ 
        float bc = hist->GetBinCenter(i+1);
        float xxx = (bc - mean)/stddev;
        float gray = 1.0f/(1.0f + exp(-2.0*xxx));

        if(gray > graymax) gray = graymax;
        //cout<<"i "<<i<<" bc "<<bc<<" x "<<xxx<<" gray "<<gray<<endl;

        PrettyFillColor(histarr[i],TColor::GetColor( gray, gray, 1.0f) );
    }

    for(int i=0;i<nbins;i++){
        histarr[i]->SetBinContent(i+1,hist->GetBinContent(i+1));
    }
    //ways to do this: either fill with log10 and have custom bin labels, or have custom binning and fill with regular nubmers. 
    //lets do the latter.
    //and just set log x
    string newcanvname = 	((string)hist->GetTitle())+"thecanvas";
    string superfluousTitle = "asdf";

	//TCanvas * C = newTCanvas(newcanvname.c_str(), superfluousTitle.c_str(),1660,989); //This line blows up.

    TCanvas * canv =new TCanvas( newcanvname.c_str(), superfluousTitle.c_str(),1660,989);
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
                                                                             //
	//TCanvas * C = newTCanvas(((string)hist->GetTitle())+"thecanvas", "Random Mask-Like Data",1660,989);
	////TLegend *leg = new TLegend(0.646985, 0.772727, 0.978643, 0.891608);
    ////PrettyLegend(leg);
	////leg->AddEntry(h,"Super nice histogram");
	PrettyHist(hist,kAzure + 5,3); //RETURN
	canv->cd();
	gStyle->SetOptStat(0);
	hist->Draw();
    //grad->Draw("colzsame");
	//hist->Draw("E1same");
	//hist->Draw("same");
    for(int i=0;i<nbins;i++){
        histarr[i]->Draw("same");
    }

    TPaveText *pt = new TPaveText(0.05,0.94,0.95,0.995,"blNDC");
    //TPaveText *pt = new TPaveText(0.4432569,0.94,0.5567431,0.995,"blNDC");
    pt->SetBorderSize(0);
    pt->SetFillColor(0);
    pt->SetFillStyle(0);
    pt->SetTextFont(42);
    TText *pt_LaTex = pt->AddText(hist->GetTitle());
    pt->Draw();

    gPad->RedrawAxis();
	//leg->Draw("same");
    string fname = "plots/"+fname_noext + ".png";
    //std::cout<<"    saving to "<<fname<<std::endl;
	canv->SaveAs(fname.c_str());

    for(int i=0;i<nbins;i++){
        delete histarr[i];
    }

}
