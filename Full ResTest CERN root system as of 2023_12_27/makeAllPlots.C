#ifdef __CLING__
#pragma cling optimize(0)
#endif
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include "TMath.h"
#include "TCanvas.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TAxis.h"
#include <TStyle.h>
#include <TImage.h>
#include <TF2.h>
#include "TLegend.h"
#include "TColor.h"
#include <TPaveText.h>
//#include "TTree.h"
//#include "TRandom1.h"
#include "CMSStyle.C"

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

enum FeatureState { enable = true, disable = false };
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//     _____      __  __  _                 
//    / ___/___  / /_/ /_(_)___  ____ ______
//    \__ \/ _ \/ __/ __/ / __ \/ __ `/ ___/
//   ___/ /  __/ /_/ /_/ / / / / /_/ (__  ) 
//  /____/\___/\__/\__/_/_/ /_/\__, /____/  
//                            /____/        @settings
static const std::string tsv_filename = "Main.tsv";
static const long unsigned int maskname_tsv_column_index = 2;
static const long unsigned int exer1_tsv_column_index = 3;
static const int number_of_exercises = 12; //says that there are 12 exercises going from indicies [exer1_tsv_column_index..exer1_tsv_column_index + number_of_exercises)
static const int analysis_grade_tsv_column_index = 20;
static const bool use_only_analysis_grade = true;
static const bool save_plots_enabled = enable;
static const bool skip_first_line_of_tsv_file = true;

static const bool single_plot_mode_enabled  = disable; 
static const string which_one = "3M AFFM"; //the mask name of the one plot to make.

static const std::string x_axis_title = "Exposure Reduction Factor               ";
static const std::string y_axis_title = "Event Count";

enum Ymax_state{auto_fit_each_histogram=0, manual=1, global_full_auto=2, global_auto_with_manual_min_ymax=3};
static const Ymax_state ymax_setting = auto_fit_each_histogram;
static float histogram_ymax = 80.f;
///////////////////////////////////////////////////////////////////
///////////////////////// End Settings /////////////////////////////
///////////////////////////////////////////////////////////////////

void makeAllPlots(){ //main

    if( single_plot_mode_enabled) std::cout<<"Single Plot Mode ENABLED, see single_plot_mode_enabled"<<std::endl;
    std::unordered_map<std::string, TH1F*> hMap;
	CMSStyle(); 
	float* linbinning = generateLinBinning();
    static const float grad_height = max(75.f,histogram_ymax+1.f);//the float argument (75) is the height that the gradient will go up to. anything pretty big is ok
    TF2* grad = makeGrad(grad_height); 

    std::ifstream inputFile(tsv_filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error unable to opening file" << tsv_filename << std::endl;
        return;
    }

    std::string tsv_line;
    int jline=0; //tsv line number counter.
    while (std::getline(inputFile, tsv_line)) { //for every line
        jline++; 
        if(skip_first_line_of_tsv_file and jline == 1) continue;
        // Parse the line into tokens
        std::vector<std::string> tokens = parseTSVLine(tsv_line);

        if(tokens.size() < exer1_tsv_column_index)
            std::cout<<"ERROR nogo line "<<jline<<std::endl;
        else if(tokens.size() < exer1_tsv_column_index + number_of_exercises) //
            std::cout<<"ERROR weird length line "<<jline<<std::endl;
        else{ //line not obviously invalid
            if(jline % 100 == 0) std::cout<<"readln "<<jline<<std::endl;

            if(use_only_analysis_grade and tokens.size() < analysis_grade_tsv_column_index){
                std::cout<<"Warning line too short for analysis grade "<<jline<<std::endl;
                continue;
            }
            bool is_analysis_grade = Str2bool(tokens[analysis_grade_tsv_column_index]);
            if(use_only_analysis_grade and not is_analysis_grade){
                //std::cout<<"line rejected for non-analysis grade tag "<<jline<<std::endl;
                continue;
            }

            //if the mask on this line has been seen before, find its entry in map. 
            std::unordered_map<std::string, TH1F*>::iterator it = hMap.find(tokens[maskname_tsv_column_index]);

            if (it != hMap.end()) { //This mask has been seen already. Fill the existing histogram
                //int i=0;
                //for (;i<number_of_exercises;i++){
                for (int i=0;i<number_of_exercises;i++){
                    float x = Str2float(tokens[exer1_tsv_column_index + i]);
                    if(x<0.f) break;
                    else it->second->Fill(TMath::Log10(x));
                }
                //std::cout<<jline<<" "<<tokens[maskname_tsv_column_index]<<" old fill #="<<i<<std::endl;
            } else { //New mask, create a new histogram
                TH1F* newHistogram = new TH1F(tokens[maskname_tsv_column_index].c_str(), 
                        (tokens[maskname_tsv_column_index]+";"+x_axis_title+";"+y_axis_title).c_str(), 
                        nbins,linbinning);

                //int i=0;
                //for (;i<number_of_exercises;i++){
                for (int i=0;i<number_of_exercises;i++){
                    float x = Str2float(tokens[exer1_tsv_column_index + i]);
                    if(x<0.f) break;
                    else newHistogram->Fill(TMath::Log10(x));
                }
                //std::cout<<jline<<" "<<tokens[maskname_tsv_column_index]<<" new fill #="<<i<<std::endl;
                hMap[tokens[maskname_tsv_column_index]] = newHistogram;
            } //end else 
        } //end else ok line
    } //end while every tsv line
    inputFile.close();

    if(ymax_setting >= global_full_auto){
        static const float ymax_margin = 1.10f;
        //calculate the largest bin content of all histograms
        float global_max_bin = 0.f;
        for (const std::pair<const std::string, TH1F*>& pair : hMap) {
            global_max_bin = max(global_max_bin, (float)((TH1F*) pair.second)->GetMaximum());
        }
        if(ymax_setting == global_full_auto or 
                (ymax_setting == global_auto_with_manual_min_ymax and global_max_bin > histogram_ymax*ymax_margin))
            histogram_ymax = global_max_bin*ymax_margin;
    }

    //Now make all plots and save them to file.
    bool something_was_found = false;
    for (const std::pair<const std::string, TH1F*>& pair : hMap) {
        const std::string& mask = pair.first;
        TH1F* histogram = pair.second;
        if( single_plot_mode_enabled and mask != which_one ) continue;
        something_was_found = true;

        if(save_plots_enabled){
            if(single_plot_mode_enabled){
                std::cout<<"Plot and save "<<mask<<" single_plot_mode_enabled = true so no other plot get generated."<<std::endl;
            } else {
                std::cout<<"Plot and save "<<mask<<std::endl;
            }

            PlotAndSave(histogram, grad, mask);
        }
    }
    if( single_plot_mode_enabled and not something_was_found ){
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
//      __  ___      __
//     /  |/  /___ _/ /_____
//    / /|_/ / __ `/ //_/ _ \
//   / /  / / /_/ / ,< /  __/
//  /_/  /_/\__,_/_/|_|\___/
//      ____             __                                    __
//     / __ )____ ______/ /______ __________  __  ______  ____/ /
//    / __  / __ `/ ___/ //_/ __ `/ ___/ __ \/ / / / __ \/ __  /
//   / /_/ / /_/ / /__/ ,< / /_/ / /  / /_/ / /_/ / / / / /_/ /
//  /_____/\__,_/\___/_/|_|\__, /_/   \____/\__,_/_/ /_/\__,_/
//                        /____/
//     ______               ___            __
//    / ____/________ _____/ (_)__  ____  / /_
//   / / __/ ___/ __ `/ __  / / _ \/ __ \/ __/
//  / /_/ / /  / /_/ / /_/ / /  __/ / / / /_
//  \____/_/   \__,_/\__,_/_/\___/_/ /_/\__/
//
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
    SetBinLabels(hist);
    float* linbinning = generateLinBinning();

    TH1F* histarr[nbins];
    for(int i=0;i<nbins;i++){
        histarr[i] = new TH1F( 
                (((string)hist->GetTitle())+std::to_string(i)).c_str(),
                (";"+x_axis_title+","+y_axis_title).c_str(),
                nbins,linbinning);
    }

 //   UnitNorm(hist);
    double mean = hist->GetMean();
    double stddev = hist->GetStdDev();
    //cout<<"    integral: "<<hist->Integral()<< " mean: "<<mean<<" stdev: "<<stddev<<endl;

    //locate the bin center of the histogram peak
    float hist_peak = 0.f;
    float maxbinval = 0.f;
    for(int i=0;i<nbins;i++){ 
        float binval = hist->GetBinContent(i+1);
        if(binval >= maxbinval){
            maxbinval = binval;
            hist_peak = hist->GetBinCenter(i+1);
        }
    }


    for(int i=0;i<nbins;i++){ 
        float bc = hist->GetBinCenter(i+1);
//      __  ___      __
//     / / / (_)____/ /_____  ____ __________ _____ ___
//    / /_/ / / ___/ __/ __ \/ __ `/ ___/ __ `/ __ `__ \
//   / __  / (__  ) /_/ /_/ / /_/ / /  / /_/ / / / / / /
//  /_/ /_/_/____/\__/\____/\__, /_/   \__,_/_/ /_/ /_/
//     ______      __      /____/     __  _
//    / ____/___  / /___  _________ _/ /_(_)___  ____
//   / /   / __ \/ / __ \/ ___/ __ `/ __/ / __ \/ __ \
//  / /___/ /_/ / / /_/ / /  / /_/ / /_/ / /_/ / / / /
//  \____/\____/_/\____/_/   \__,_/\__/_/\____/_/ /_/
//

        /*float xxx = (bc - mean)/stddev;
        float gray = 1.0f/(1.0f + exp(-2.0*xxx));
        if(gray > graymax) gray = graymax;
        //cout<<"i "<<i<<" bc "<<bc<<" x "<<xxx<<" gray "<<gray<<endl;
        PrettyFillColor(histarr[i],TColor::GetColor( gray, gray, 1.0f) ); */

        //Logic for Histogram Fill Color
        /*
           rgb(255,51,85) for 0..10]   TColor::GetColor(1.0f,0.2f, 0.3333f)           red
           rgb(255,179,191) unused     TColor::GetColor(1.0f,0.70196f,0.7490196f)     light red
           rgb(255,170,0) for (10..30] TColor::GetColor(1.0f,0.66667f,0.0f)           yellow
           rgb(255,212,128) unused     TColor::GetColor(1.0f,0.83137f,0.5019608f)     light yellow
           rgb(38,230,0) for >30       TColor::GetColor(0.1490196f,0.9019608f,0.0f)   green
           rgb(149,255,128) unused     TColor::GetColor(0.5843137f,1.0f,0.5019608f)   light green
           */
        if(bc < 1.0f){ //<1og10(10)
            PrettyFillColor(histarr[i],TColor::GetColor(1.0f,0.2f,0.3333333f) );
        } else if(bc < 1.477121255f ){ //< log10(30)
            PrettyFillColor(histarr[i], TColor::GetColor(1.0f,0.66667f,0.0f));
        }
        else{ // >= log10(30)
            if(bc < hist_peak)
                PrettyFillColor(histarr[i],TColor::GetColor( 0.1490196f,0.9019608f,0.0f) );
            else
                PrettyFillColor(histarr[i],TColor::GetColor( 0.7f,0.7f,0.7f) );

        }
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
                                   //
    if(ymax_setting > auto_fit_each_histogram) 
        SetRange(hist,0,histogram_ymax);
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
