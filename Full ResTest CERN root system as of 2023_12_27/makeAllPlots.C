#ifdef __CLING__
#pragma cling optimize(0)
#endif
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <map>
#include <set>
#include <chrono>
#include <ctime>
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
#include <TArrow.h>
#include <TPad.h>
//#include "TTree.h"
//#include "TRandom1.h"
#include "CMSStyle.C"

#define nbins (60)

enum FeatureState { enable = true, disable = false };
enum SigmoidOption{S_abs,S_erf,S_tanh, S_gd, S_algeb, S_atan, S_absalgeb};
enum ColorScheme { trafficLight, trafficLightFaded, blueberry, LUT1 };
enum BkgColorScheme { White, OffWhite, Dark };
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
//     _____      __  __  _                 
//    / ___/___  / /_/ /_(_)___  ____ ______
//    \__ \/ _ \/ __/ __/ / __ \/ __ `/ ___/
//   ___/ /  __/ /_/ /_/ / / / / /_/ (__  ) 
//  /____/\___/\__/\__/_/_/ /_/\__, /____/  
//                            /____/        @settings
static const std::string tsv_filename = "Main.tsv";
static const std::string error_flag_file = "error.flag";
static const long unsigned int maskname_tsv_column_index = 2;
static const long unsigned int exer1_tsv_column_index = 3;
static const int number_of_exercises = 12; //says that there are 12 exercises going from indicies [exer1_tsv_column_index..exer1_tsv_column_index + number_of_exercises)
static const int analysis_grade_tsv_column_index = 20;
static const bool use_only_analysis_grade = true;
static const bool save_plots_enabled = enable;
static const bool save_with_HMFF_prefix = enable;
static const bool skip_first_line_of_tsv_file = true;

static const int testerID_tsv_column_index = 16;

static const bool single_plot_mode_enabled = disable; 
//static const bool single_plot_mode_enabled = enable;// ilya undo
static const string which_one = "AOK Tooling Softseal cup PN 20180021-L";
//static const string which_one = "3M AFFM";  //missalighend left
//static const string which_one = "3M Aura 9205+"; //mid
//static const string which_one = "Easimask FSM18";  //misalighend right 

//static const string which_one = "Vitacore CAN99 model 9500";
//static const string which_one = "3M 8862";
//static const string which_one = "CanadaMasq Q100 Medium CA-N95F-100PA";
//static const string which_one ="Drager1920ML_1950ML";
static const bool X11_persistence  = true; 

static const std::string x_axis_title = "Exposure Reduction Factor               ";
static const std::string y_axis_title = "Event Count";

enum Ymax_state{auto_fit_each_histogram=0, manual=1, global_full_auto=2, global_auto_with_manual_min_ymax=3};
static const Ymax_state ymax_setting = auto_fit_each_histogram;
static float histogram_ymax = 80.f;

//Arrow
static const bool UseHarmMeanArrow = true;
static const int ArrowColor = kBlack;

//Background Color Scheme Selection Control
static const BkgColorScheme bkgColorScheme = White;//OffWhite;

//Histogram Gradient Color Selection Control
static const ColorScheme colorScheme = LUT1;

//LUT1 Color Scheme Controls
//To set light green, set r[0],b[0], g[0] here:
static const char* red_hex_LUT1 = "#FF3355";
static const char* yellow_hex_LUT1 = "#FFAA00";
static const float red_end_LUT1 = TMath::Log10(10.);
static const float yellow_end_LUT1 = TMath::Log10(30.);
static const int lut1_len = 8;
static const int r[lut1_len] = { 110, 180, 220, 230, 235, 240, 242, 242}; //ilya
static const int g[lut1_len] = { 230, 242, 242, 242, 242, 242, 242, 242}; //ilya
static const int b[lut1_len] = { 110, 180, 220, 230, 235, 240, 242, 242}; //ilya
bool lut1_uses_harmean = true; //false = uses median.
//also uses red_hex and yellow_hex

//trafficLight Color Scheme Constants
static const char* red_hex = "#FF8A80"; // formerly "#FF3355"; // ilya
static const char* yellow_hex = "#FFCDAB";// formerly "#FFAA00"; // ilya
static const char* green_hex = "##AFFFAB";// formerly "#26E600"; // but doesn't work
static const float red_end = TMath::Log10(10.);
static const float yellow_end = TMath::Log10(30.);

//trafficLightFaded Color Scheme Constants 
static const char* red_hex_TLF = "#FF3355";
static const char* yellow_hex_TLF = "#FFAA00";
static const char* green_hex_TLF = "#26E600";
static const float red_end_TLF = TMath::Log10(10.);
static const float yellow_end_TLF = TMath::Log10(30.);
static const float hue_green = 120.f;//overrides green_hex
static const float value_green = 0.274;//0.682f;//overrides green_hex
static const float percentile_hardness = 5.0;
static const float percentile_corner = 0.5;
static const SigmoidOption satur_func = S_tanh;

//blueberry Color Scheme Constants 
static const float blueness = 1.0;
static const float graymax = 0.945;//0.961;//0.95f; 

//Dark Mode Colors
static const char* red_hex_dark = "#FF3355";
static const char* yellow_hex_dark = "#FFAA00";
static const char* green_hex_dark = "#26E600";
static const float value_green_dark = 0.682f;//overrides green_hex_dark


//Sigmoid scheme constants
/*static const float hue_red = -10.f;
static const float hue_yellow = 40.f;
static const float hue_green = 110.f;
//static const float hue_transition_hardness = 0.36f; 
//static const SigmoidOption hue_func = S_abs;

static const float satur_green = 0.15f;
static const float satur_at_center = 0.7f;
static const float satur_center_percential = 0.5f;
static const float satur_transition_hardness = 6.0f;


static const float value_red = 1.0f;
static const float value_green = 0.9f;
static const float value_transition_center = 1.17f;//1.17 = log10(25)
static const float value_transition_hardness = 8.0f;
static const SigmoidOption value_func = S_tanh;
*/
///////////////////////////////////////////////////////////////////
///////////////////////// End Settings /////////////////////////////
///////////////////////////////////////////////////////////////////

void readJsonFile(const std::string& filename, std::map<std::string, std::string>& jsonData) ;

struct Hist{
    bool is_sorted;
    bool has_cdf;
    float cdf[nbins+1];
    string title;
    TH1F* hist;
    std::vector<float> all_vals;
    std::set<std::string> unique_testers;

    Hist(string title, float* linbinning);
    ~Hist(){ delete hist; }
    void Fill(float val);
    void FillLog10(float val);
    string GetTitle(){return title;}
    float Percentile2X(float percentile);
    float X2Percentile(float x);
    float Get_Median(){ return Percentile2X(0.5f); }
    float Get_HarmonicMean();
};
std::vector<std::string> parseTSVLine(const std::string& tsv_line);
float Str2float(std::string& token);
int   Str2int(std::string& token);
bool  Str2bool(std::string& token);
float* generateLinBinning();
float* generateLogBinning();
TF2* makeGrad(float ymax);
void SetBinLabels(Hist* hist);
void PlotAndSave(Hist* hist, TF2* grad, string fname_noext);
bool isALlWhiteSpace(const std::string& str);
std::string getCurrentDateTime();

double sigmoid(double x, SigmoidOption softness);

Hist::Hist(string title, float* linbinning): is_sorted(false), has_cdf(false), title(title){
    hist = new TH1F(title.c_str(), 
            (title+";"+x_axis_title+";"+y_axis_title).c_str(), 
            nbins,linbinning);
}
void Hist::Fill(float val){
    //Fill for linear axis
    hist->Fill(val);
    all_vals.push_back(val);
}
void Hist::FillLog10(float val){
    //Correctly handles fill for artificial log axis
    hist->Fill(TMath::Log10(val));
    all_vals.push_back(val);
}
float Hist::X2Percentile(float x){
    if(not has_cdf){
        cdf[0] = 0.;
        for(int i=1;i<=nbins;i++){
            cdf[i] = cdf[i-1] + hist->GetBinContent(i);
        }
        has_cdf = true;
    }
    int ibin = std::min(nbins,std::max(1,hist->FindBin(x))); // presumed to be a root binnumber, so starts at 1 unless underflow..
    float low  = hist->GetXaxis()->GetBinLowEdge(ibin);
    float high = hist->GetXaxis()->GetBinUpEdge(ibin);
    float del = x - low;
    float Delta = high-low;
    return ((cdf[ibin-1]*del/Delta) + (cdf[ibin]*(Delta-del)/Delta))/cdf[nbins];
}
float Hist::Percentile2X(float percentile){
    //Returns the interpolated position in the distribution at the given percentile. 
    //percentile is between 0 and 1 inclusively.
    std::size_t n = all_vals.size();
    if(n <= 0) return 0.0f;
    else if(n == 1) return all_vals[0];
    if(not is_sorted){
        std::sort(all_vals.begin(), all_vals.end()); 
        is_sorted = true;
    }
    percentile = std::min(1.0f, std::max(0.0f, percentile ));
    const float approxN = percentile*n - 0.5f; 
    const float approxN_floor = std::floor(approxN );
    const float x = approxN - approxN_floor;
    const int ifloor = std::max(0,static_cast<int>(approxN_floor));
    const int iceil = std::min((int)n-1,static_cast<int>(std::ceil( approxN )));
    return x*all_vals[ifloor] + (1.0f-x)*all_vals[iceil];
}
float Hist::Get_HarmonicMean(){
    float harmonicSum = 0.0;
    for (float val : all_vals) {
        harmonicSum += 1.0f / val;
    }
    return ((float)all_vals.size()) / harmonicSum;
}

void makeAllPlots(){ //main

    { //Check if there's an error flag, and if so, don't run.
        std::ifstream flag_file(error_flag_file);
        if (flag_file.good()){
            std::cout << "Error flag was raised, so the TSV file must be no good. Root exits without generating plots." << std::endl;
            return;
        }
    }

    if(bkgColorScheme  == White){
        BkgColor = kWhite; //255, 255, 255
        FontColor = kBlack;
    } else if(bkgColorScheme  == OffWhite){ 
        //BkgColor = 10; // 254, 254, 254
        //BkgColor = 19; // very light gray
        BkgColor = GetColorForced(248,248,248);
        std::cout<<"Bkg color "<<BkgColor <<std::endl;//DEBUG
        FontColor = kBlack;
    } else if(bkgColorScheme  == Dark ){
        BkgColor = TColor::GetColor(0.188f, 0.22f, 0.255f);//dark gray #303841
        FontColor = TColor::GetColor(0.847f, 0.871f, 0.914f);//#D8DEE9
    }
                     
    if( single_plot_mode_enabled) std::cout<<"Single Plot Mode ENABLED, see single_plot_mode_enabled"<<std::endl;
    std::unordered_map<std::string, Hist*> hMap;
	CMSStyle(); 
	float* linbinning = generateLinBinning();
    static const float grad_height = max(75.f,histogram_ymax+1.f);//the float argument (75) is the height that the gradient will go up to. anything pretty big is ok
    TF2* grad = makeGrad(grad_height); 

    std::ifstream inputFile(tsv_filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error unable to opening file" << tsv_filename << std::endl;
        return;
    }

    //      ____  _________    ____     ____________
    //     / __ \/ ____/   |  / __ \   / ____/  _/ /__
    //    / /_/ / __/ / /| | / / / /  / /_   / // / _ \
    //   / _, _/ /___/ ___ |/ /_/ /  / __/ _/ // /  __/
    //  /_/ |_/_____/_/  |_/_____/  /_/   /___/_/\___/
    //  READFILE
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
            string maskname = tokens[maskname_tsv_column_index];
            std::replace(maskname.begin(), maskname.end(),'/','_'); //Guard names against /
            std::replace(maskname.begin(), maskname.end(),'\'','_'); //Guard names against '
            std::unordered_map<std::string, Hist*>::iterator it = hMap.find(maskname);

            if (it != hMap.end()) { //This mask has been seen already. Fill the existing histogram
                //int i=0;
                //for (;i<number_of_exercises;i++){
                for (int i=0;i<number_of_exercises;i++){
                    float x = Str2float(tokens[exer1_tsv_column_index + i]);
                    if(x<0.f) break;
                    else it->second->FillLog10(x);
                }
                //std::cout<<jline<<" "<<maskname<<" old fill #="<<i<<std::endl;
            } else { //New mask, create a new histogram
                Hist* newHistogram = new Hist(maskname,linbinning);

                //int i=0;
                //for (;i<number_of_exercises;i++){
                for (int i=0;i<number_of_exercises;i++){
                    float x = Str2float(tokens[exer1_tsv_column_index + i]);
                    if(x<0.f) break;
                    else newHistogram->FillLog10(x);
                }
                //std::cout<<jline<<" "<<maskname<<" new fill #="<<i<<std::endl;
                hMap[maskname] = newHistogram;
            } //end else 

            string testerID = tokens[testerID_tsv_column_index];
            if(not isALlWhiteSpace(testerID))
                hMap[maskname]->unique_testers.insert(testerID);

        } //end else ok line
    } //end while every tsv line
    inputFile.close();

    if(ymax_setting >= global_full_auto){
        static const float ymax_margin = 1.10f;
        //calculate the largest bin content of all histograms
        float global_max_bin = 0.f;
        for (const std::pair<const std::string, Hist*>& pair : hMap) {
            global_max_bin = max(global_max_bin, (float)((Hist*) pair.second)->hist->GetMaximum());
        }
        if(ymax_setting == global_full_auto or 
                (ymax_setting == global_auto_with_manual_min_ymax and global_max_bin > histogram_ymax*ymax_margin))
            histogram_ymax = global_max_bin*ymax_margin;
    }

    //Now make all plots and save them to file.
    bool something_was_found = false;
    for (const std::pair<const std::string, Hist*>& pair : hMap) {
        const std::string& mask = pair.first;
        Hist* histogram = pair.second;
        if( single_plot_mode_enabled and mask != which_one ) continue;
        something_was_found = true;

        if(save_plots_enabled){
            if(single_plot_mode_enabled){
                std::cout<<"Plot and save "<<mask<<" single_plot_mode_enabled = true so no other plot get generated."<<std::endl;
            } 
            /*else {
                std::cout<<"Plot and save "<<mask<<std::endl;
            }*/

            PlotAndSave(histogram, grad, mask);
        }
    }
    if( single_plot_mode_enabled and not something_was_found ){
        std::cout<<"Warning! No plot found that matched name which_one = "<<which_one<<std::endl;
    }
    
    if(not (single_plot_mode_enabled and X11_persistence)){
        for (auto& pair : hMap) {
            delete pair.second;
        }
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

void SetBinLabels(Hist* hist){
    float* logbinning = generateLogBinning();
	TAxis *x = hist->hist->GetXaxis();
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


void PlotAndSave(Hist* hist, TF2* grad, string fname_noext){
    //plot it and make it pretty
	PrettyFillColor(hist->hist, kAzure + 5);
    SetBinLabels(hist);
    float* linbinning = generateLinBinning();

    TH1F* histarr[nbins];
    for(int i=0;i<nbins;i++){
        histarr[i] = new TH1F( 
                (hist->GetTitle()+std::to_string(i)).c_str(),
                (";"+x_axis_title+","+y_axis_title).c_str(),
                nbins,linbinning);
    }

    std::cout<<"unique testers: "<<hist->unique_testers.size()<<" #samples "<<hist->hist->Integral()<<std::endl;

 //   UnitNorm(hist->hist);
    double mean = hist->hist->GetMean();
    double stddev = hist->hist->GetStdDev();
    //cout<<"    integral: "<<hist->hist->Integral()<< " mean: "<<mean<<" stdev: "<<stddev<<endl;

    //locate the bin center of the histogram peak
    float hist_peak = 0.f;
    float maxbinval = 0.f;
    for(int i=0;i<nbins;i++){ 
        float binval = hist->hist->GetBinContent(i+1);
        if(binval >= maxbinval){
            maxbinval = binval;
            hist_peak = hist->hist->GetBinCenter(i+1);
        }
    }

    //float hue, saturation, value;
    int i_middle;
    for(int i=0;i<nbins;i++){ 
        float bc = hist->hist->GetBinCenter(i+1);
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
        //Logic for Histogram Fill Color
        /*
           rgb(255,51,85) for 0..10]   TColor::GetColor(1.0f,0.2f, 0.3333f)           red "#FF3355"
                hsv = 350, 80, 100     TColor::GetColor(255,51,85)

           rgb(255,179,191) unused     TColor::GetColor(1.0f,0.70196f,0.7490196f)     light red "#FFB3BF"

           rgb(255,170,0) for (10..30] TColor::GetColor(1.0f,0.66667f,0.0f)           yellow "#FFAA00"
                hsv = 40, 100, 100     TColor::GetColor(255,170,0)

           rgb(255,212,128) unused     TColor::GetColor(1.0f,0.83137f,0.5019608f)     light yellow "#FFD480"

           rgb(38,230,0) for >30       TColor::GetColor(0.1490196f,0.9019608f,0.0f)   green "#26E600"
                hsv = 110, 100, 90     TColor::GetColor(38,230,0)

           rgb(149,255,128) unused     TColor::GetColor(0.5843137f,1.0f,0.5019608f)   light green "#95FF80"
           */
        if(colorScheme == trafficLightFaded){
            if(bc < red_end_TLF){ 
                PrettyFillColor(histarr[i],TColor::GetColor(red_hex_TLF));
            } else if(bc < yellow_end_TLF){ 
                PrettyFillColor(histarr[i], TColor::GetColor(yellow_hex_TLF));
            }
            else{ // >= TMath::Log10(30)
                //float xxx = (bc - mean)/stddev;
                //float xxx = (bc - hist->Get_Median())/stddev;
                //float xxx = (bc - TMath::Log10(hist->Get_HarmonicMean()))/stddev;
                float xxx = percentile_hardness*(hist->X2Percentile(bc) - percentile_corner );
                //float saturation = 1.0 - std::max(0, sigmoid(xxx, satur_func ));
                float saturation = 1.0f/(1.0f + exp(2.0*xxx));
                float value = graymax - (graymax - value_green)*saturation;
                PrettyFillColor(histarr[i],GetColorHSV(hue_green, saturation, value) );
                //PrettyFillColor(histarr[i],GetColorHSV(hue_green, saturation, value_green) );
            }
        } else if( colorScheme == trafficLight){
            if(bc < red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(red_hex));
            } else if(bc < yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(yellow_hex));
            }
            else{ 
                PrettyFillColor(histarr[i], TColor::GetColor(green_hex));
            }
        } else if( colorScheme == blueberry ){
                float xxx = (bc - hist->Get_Median())/stddev;
                float gray = 1.0f/(1.0f + exp(-2.0*xxx));
                if(gray > graymax) gray = graymax;
                PrettyFillColor(histarr[i],TColor::GetColor( gray, gray, blueness) );
        } else if(colorScheme == LUT1){
            //float bc_middle = hist->Get_Median();
            float bc_middle; 
            if(lut1_uses_harmean) bc_middle = TMath::Log10(hist->Get_HarmonicMean());
            else bc_middle = hist->Get_Median();
            i_middle = hist->hist->FindBin(bc_middle);
            //std::cout<<"bini="<<i+1<<" ["<<hist->hist->GetXaxis()->GetBinLowEdge(i+1)<<"-"<<
            //    hist->hist->GetXaxis()->GetBinUpEdge(i+1)<<
            //    "] LUT1: use harmean = "<<lut1_uses_harmean<<" harmean/bc_middle: "<<bc_middle <<" i_middle "<<i_middle<<std::endl;//DEBUG
            if(bc < red_end_LUT1){ 
                PrettyFillColor(histarr[i],TColor::GetColor(red_hex_LUT1));
            } else if(bc < yellow_end_LUT1){ 
                PrettyFillColor(histarr[i], TColor::GetColor(yellow_hex_LUT1));
            } else { //Green lut
                int j = std::max(0,std::min(lut1_len-1, i+1-i_middle));
            //    std::cout<<"green j="<<j<<" di="<<i+1-i_middle<<" r "<<r[j]<<" b "<<b[j]<<std::endl;//DEBUG
                PrettyFillColor(histarr[i], TColor::GetColor(r[j],g[j],b[j]));
            } 

        }

        //hue is on 0..360, mod 360
        /*hue = hue_red + (hue_green - hue_red)*sigmoid(hue_transition_hardness*bc,hue_func);//sigmoid(0.36*bc,S_tanh)
        if(bc < red_end){
            hue = hue_red;
        } else if(bc <  yellow_end ){
            hue = hue_yellow;
        }
        else{ 
            hue = hue_green;
        }

        saturation = std::min(1.0,
                satur_at_center + (satur_green - satur_at_center )*sigmoid(
                    satur_transition_hardness*(hist->X2Percentile(bc) - satur_center_percential),satur_func )
                    
                );

        float value_delta = 0.5*(value_red - value_green ); 
        value = (1.0 - value_delta ) - value_delta*sigmoid(value_transition_hardness*(bc - value_transition_center),value_func); //darkens green

        PrettyFillColor(histarr[i],GetColorHSV(hue, saturation, value) );
        */
    }//end for every bin.

    for(int i=0;i<nbins;i++){
        histarr[i]->SetBinContent(i+1,hist->hist->GetBinContent(i+1));
    }
    //ways to do this: either fill with log10 and have custom bin labels, or have custom binning and fill with regular nubmers. 
    //lets do the latter.
    //and just set log x

    string newcanvname = hist->GetTitle()+"thecanvas";
    string superfluousTitle = "asdf";
	//TCanvas * C = newTCanvas(newcanvname.c_str(), superfluousTitle.c_str(),1660,989); //This line blows up.
    TCanvas * canv =new TCanvas( newcanvname.c_str(), superfluousTitle.c_str(),1660,989);
    PrettyCanvas(canv);
    canv->cd();

    //Arrow
                                                                             //
	//TCanvas * C = newTCanvas(hist->GetTitle()+"thecanvas", "Random Mask-Like Data",1660,989);
	////TLegend *leg = new TLegend(0.646985, 0.772727, 0.978643, 0.891608);
    ////PrettyLegend(leg);
	////leg->AddEntry(h,"Super nice histogram");
	PrettyHist(hist->hist,BkgColor,0); //RETURN
                                   //
    if(ymax_setting > auto_fit_each_histogram) 
        SetRange(hist->hist,0,histogram_ymax);
	canv->cd();
	gStyle->SetOptStat(0);
	hist->hist->Draw();
    for(int i=0;i<nbins;i++){
        histarr[i]->Draw("same");
    }

    //CODE TO SHOW THE ARROW and "HM"
    Double_t arrowX = TMath::Log10(hist->Get_HarmonicMean());
    Double_t ytop = hist->hist->GetMaximum()/0.95; //Top of the y-axis. TH1->GetYaxis()->GetXmax() incorrectly returns 1
    Double_t ymin = ytop*0.06;
    //Double_t ymin = ytop*0.74;
    //std::cout<<"ymax_init "<< ymax<<std::endl; //comes out 1 every time F*CK
    //Double_t ymax *= ytop*0.886;
    Double_t arrow_length = ytop*0.146;
    TArrow* arrow = new TArrow(arrowX, ymin + arrow_length, arrowX, ymin, 0.015, "|>");//x1,y1 and x2,y2 the arrowsize in regular coordinates

    //HM
    Double_t textX = -0.0644+((arrowX+0.7435)/6.9749); //Should work when hist->hist->GetXaxis()->GetXmax() = 6
    Double_t text_ymin_ndc = 0.383817;// 0.865145;
    Double_t text_dy_NDC = 0.910788-0.865145;
    TPaveText *arrpt = new TPaveText(textX-0.015,text_ymin_ndc,textX+0.15,text_ymin_ndc + text_dy_NDC,"NDC");
    //std::cout<<"Tarrow x: "<<arrowX<<" ymin "<<ymin<<" ymax "<<ymax<<" histogram_ymax "<<histogram_ymax<< " textX "<<textX<<std::endl<<std::endl;
    if(UseHarmMeanArrow){
        arrow->SetLineWidth(4);
        arrow->SetFillColor(ArrowColor);
        arrow->SetLineColor(ArrowColor);
        arrow->Draw();

        PrettyPaveText(arrpt);
        TText *apt_LaTex = arrpt->AddText("HM");
        arrpt->Draw();
    } 

    TPaveText *pt = new TPaveText(0.16,0.94,0.75,0.995,"NDC");
    //TPaveText *pt = new TPaveText(0.16,0.94,0.84,0.995,"NDC");
    //TPaveText *pt = new TPaveText(0.05,0.94,0.95,0.995,"blNDC");
    PrettyPaveText(pt);
    TText *pt_LaTex = pt->AddText(hist->GetTitle().c_str());
    pt->Draw();

    //Display timestamp
    TPaveText *timestamp1= new TPaveText( 
            0.00120627, 0.965768, 0.179131, 0.997925,
            "NDC");
   // timestamp1->AddText("TIL score distribution as of ");
    timestamp1->AddText(("TIL distribution as of " + getCurrentDateTime()).c_str());
    PrettyPaveText(timestamp1);
    timestamp1->SetTextAlign(12); //Align to top left
    timestamp1->Draw();
    canv->Update();

    //Display #Contributers and Samples
    TPaveText *nc = new TPaveText( 0.750000, 0.936722, 0.994572, 0.997925,"NDC");//0.857057
    PrettyPaveText(nc);
    nc->SetTextAlign(12); 
    nc->AddText("M41 Mode Exercise Samples:");
    nc->AddText("Adversarial Contributor Count:");
    nc->Draw();
    TPaveText *sc = new TPaveText( 0.936068, 0.936722, 0.974668, 0.997925,"NDC"); 
    PrettyPaveText(sc);
    sc->SetTextAlign(32); 
    sc->AddText(std::to_string( static_cast<int>(hist->hist->Integral())).c_str());
    sc->AddText(std::to_string(hist->unique_testers.size() ).c_str());
    sc->Draw();





TPaveText *lowCountWarning = new TPaveText( 0.700000, 0.800000, 0.95, 0.999,"NDC"); //( 0.300000, 0.100000, 0.800000, 0.997925,"NDC");
    PrettyPaveText(lowCountWarning);
    lowCountWarning->SetTextAlign(12); 
    lowCountWarning->SetTextColor(kGray+1);
if (hist->unique_testers.size() < 4)
    lowCountWarning->AddText("Low Adversarial Contributor Count");
    lowCountWarning->Draw();








    gPad->RedrawAxis();
	//leg->Draw("same");
    string fname;
    if(save_with_HMFF_prefix){
        std::ostringstream prefix;
        prefix << std::fixed << std::setw(7) << std::setfill('0') << static_cast<int>(hist->Get_HarmonicMean()*10);
        fname = "plots/"+prefix.str()+"_"+fname_noext + ".png";
    } else{
        fname = "plots/"+fname_noext + ".png";
    }
	canv->SaveAs(fname.c_str());

    if(not (single_plot_mode_enabled and X11_persistence)){ //This memory leaks, but who cares.
        for(int i=0;i<nbins;i++){
            delete histarr[i];
        }
    }
}

double sigmoid(double x, SigmoidOption softness){
    //enum SigmoidOption{S_abs,S_erf,S_tanh, S_gd, S_algeb, S_atan, S_absalgeb};
    //should all be -1 at -inf, 0 at 0, and 1 at inf. Around 0 should have y=x
    //softness is in range of [0,6], with lower numbers having harder corners.
    switch(softness){
        case S_abs:
                return std::min(1.0,std::max(-1.0,x));
        case S_erf: 
            return TMath::Erf(x*0.8862269254527580);
        case S_tanh: 
             return TMath::TanH(x);
        case S_gd: 
               return 2.0* TMath::ATan(TMath::TanH(0.5*x));
        case S_algeb: 
                return x/sqrt(1.0+x*x);
        case S_atan: 
                return TMath::ATan(x);
        case S_absalgeb: 
        default:
                return x/(1+abs(x));
    }
}

void readJsonFile(const std::string& filename, std::map<std::string, std::string>& jsonData) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Process each line as needed, for simplicity, we assume key-value pairs separated by colon
        std::istringstream iss(line);
        std::string key, value;
        if (std::getline(iss, key, ':') && std::getline(iss, value)) {
            // Trim leading and trailing whitespaces
            key.erase(0, key.find_first_not_of(" \t\n\r\f\v"));
            key.erase(key.find_last_not_of(" \t\n\r\f\v") + 1);
            value.erase(0, value.find_first_not_of(" \t\n\r\f\v"));
            value.erase(value.find_last_not_of(" \t\n\r\f\v") + 1);

            // Add to the map
            jsonData[key] = value;
        }
    }
}

bool isALlWhiteSpace(const std::string& str) {
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

std::string getCurrentDateTime() {
    // Get the current time point
    auto now = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a tm struct
    std::tm* tm_now = std::localtime(&now_time);

    // Create a string stream to build the formatted date-time string
    std::stringstream ss;

    // Set the format flags for the month to display 3-letter month name
    ss << std::put_time(tm_now, "%Y-%m-%d %H:%M");
    //ss << std::put_time(tm_now, "%Y-%b-%d %H:%M");

    // Return the formatted date-time string
    return ss.str();
}
