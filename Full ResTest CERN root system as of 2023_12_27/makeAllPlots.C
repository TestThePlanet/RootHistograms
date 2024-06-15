#ifdef __CLING__
#pragma cling optimize(0)
#endif

/* 
#TOC
#TomlDeclare
#TomlGet
#StructHist
#Prototypes
#READFILE
*/ 

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

#define TOML_EXCEPTIONS 0
#define TOML_IMPLEMENTATION
#include "toml.hpp"

#define nbins (60)

/*
Getting to ilya's stupid fucking points system

* Column settings are under [TSV]
* TesterID is column Q (16)
	testerID_tsv_column_index
* Protocol is column T (Acode -1) (19)
* Frankly we should just import every column
* Test Date is column R (17)

*/


enum FeatureState { enable = true, disable = false };
enum SigmoidOption{S_abs,S_erf,S_tanh, S_gd, S_algeb, S_atan, S_absalgeb};
enum ColorScheme { trafficLight, trafficLightFaded, blueberry, LUT1, LUT2, grayGreen, blackWhite};
enum BkgColorScheme { White, OffWhite, Dark };
enum sizeCode{Lg=0,Sm,IQR,NOSIZE,SIZEMAX};
//IQR
enum Ymax_state{auto_fit_each_histogram=0, manual=1, global_full_auto=2, global_auto_with_manual_min_ymax=3};

struct ScoreWeights{
    float points_2xCrash25_2xOSHA_withGrimaceStored;
    float points_2xC25_1xOSHA_withGrimaceStored;
    float points_2xC25;
    float points_1xC25_2xOSHA_withGrimaceStored;
    float points_1xCrash25_1xOSHA_withGrimaceStored;
    float points_2xOSHA_withGrimaceStored;
    float points_1xCrash25;
    float points_1xOSHA_withGrimaceStored;
    float points_1xCrash25short;
};
struct Settings{    // #TomlDeclare

    bool load(std::string tomlfile);
    void debug(int thresh,std::string msg);

    //[SinglePlotMode]
    bool single_plot_mode_enabled;
    std::string which_one;

    //[Output]
    bool save_plots_enabled;
    bool save_with_HMFF_prefix;
    std::string low_contrib_count_prefix;
    bool X11_persistence;
    int output_print_level;

    //[Analysis]
    bool use_only_analysis_grade;
    bool use_sizes;

    //[TSV]
    std::string tsv_filename;
    std::string error_flag_file;
    bool skip_first_line_of_tsv_file;
    unsigned int maskname_tsv_column_index;
    unsigned int exer1_tsv_column_index;
    unsigned int headSize_tsv_column_index;
    int number_of_exercises;
    unsigned int analysis_grade_tsv_column_index;
    unsigned int testerID_tsv_column_index;
    unsigned int date_tsv_column_index;
    unsigned int maxJawCm_tsv_column_index;
    unsigned int protocol_tsv_column_index;
    unsigned int queryResult_tsv_column_index;

    //[Scoring]
    std::string score_file_name = "scores.txt";
    ScoreWeights sw;

    //[Histogram_Graphics]
    int sizePixelsX;
    int sizePixelsY;
    std::string x_axis_title;
    std::string y_axis_title;
    Ymax_state ymax_setting;
    float histogram_ymax;

    //[HistTitle]
    float HistTitle_X1;
    float HistTitle_Y1;
    float HistTitle_X2;
    float HistTitle_Y2;

    //[TimeStamp]
    float Contributers_X1;
    float Contributers_Y1;
    float Contributers_X2;
    float Contributers_Y2;
    std::string Contributers_prefixText;
    std::string Contributers_timeFormat;
    std::string Contributers_suffixText;
    int Contributers_textAlignment;

    //[Legend]
    float Legend_HMSideSwitchThresh;
    float Legend_fontSize;
    float Legend_lowX1;
    float Legend_Y1;
    float Legend_lowX2;
    float Legend_Y2;
    float Legend_highX1;
    float Legend_highX2;
    int Legend_markerSize;
    int Legend_histSize;
    std::string Legend_entryTextAll;
    std::string Legend_entryTextLg;
    std::string Legend_entryTextSm;
    std::string Legend_entryTextIQR;
    int Legend_colorAll;
    int Legend_colorLg;
    int Legend_colorSm;
    int Legend_colorIQR;
    int Legend_fillStyleLg;
    int Legend_fillStyleSm;
    int Legend_fillStyleIQR;
    std::string Legend_sizeHistDrawOption;

    //[Arrow]
    bool Arrow_useHarmMean;
    int Arrow_color;
    float Arrow_size;
    int Arrow_linewidth;	
    float histmax_factor;
    float Arrow_ymin;
    float Arrow_length;
    std::string Arrow_tipStyle;
    std::string ArrowText_label;
    float ArrowText_lowerNDC;
    float ArrowText_upperNDC;
    float ArrowText_leftOffset;
    float ArrowText_rightOffset;
    
    //[ColorScheme]
    BkgColorScheme bkgColorScheme;
    ColorScheme colorScheme;

    //[ColorScheme.LUT1]
    std::string LUT1_red_hex;
    std::string LUT1_yellow_hex;
    float LUT1_red_end;
    float LUT1_yellow_end;
    static const int lut1_len = 8; 
    int r[lut1_len] = { 110, 180, 220, 230, 235, 240, 242, 242}; 
    int g[lut1_len] = { 230, 242, 242, 242, 242, 242, 242, 242}; 
    int b[lut1_len] = { 110, 180, 220, 230, 235, 240, 242, 242}; 
    bool LUT1_uses_harmean;

    //[ColorScheme.LUT2]
    std::string LUT2_red_hex;
    std::string LUT2_yellow_hex;
    float LUT2_red_end;
    float LUT2_yellow_end;
    static const int lut2_len = 8; 
    int LUT2_r[lut2_len] = { 242, 242, 242, 242, 242, 242, 242, 242};
    int LUT2_g[lut2_len] = { 242, 242, 242, 242, 242, 242, 242, 242};
    int LUT2_b[lut2_len] = { 242, 242, 242, 242, 242, 242, 242, 242};
    bool LUT2_uses_harmean;
    
    //[ColorScheme.trafficLight]
    std::string red_hex;
    std::string yellow_hex;
    std::string green_hex;
    float red_end;
    float yellow_end;

    //[ColorScheme.grayGreen]
    std::string GG_red_hex;
    std::string GG_yellow_hex;
    std::string GG_green_hex;
    float GG_red_end;
    float GG_yellow_end;
    
    //[ColorScheme.blackWhite] 
    std::string BW_red_hex;
    std::string BW_yellow_hex;
    float BW_red_end;
    float BW_yellow_end;
    float BW_hue_green;
    float BW_value_green;
    float BW_percentile_hardness;
    float BW_percentile_corner;
    float BW_graymax;

    //[ColorScheme.trafficLightFaded] 
    std::string TLF_red_hex;
    std::string TLF_yellow_hex;
    float TLF_red_end;
    float TLF_yellow_end;
    float TLF_hue_green;
    float TLF_value_green;
    float TLF_percentile_hardness;
    float TLF_percentile_corner;
    float TLF_graymax;
    //SigmoidOption satur_func;
    
    //[ColorScheme.blueberry] 
    float blueness;
    float graymax;
    
    //[ColorScheme.darkMode]
    std::string red_hex_dark;
    std::string yellow_hex_dark;
    std::string green_hex_dark;
    float value_green_dark;
};

bool Settings::load(std::string tomlfile){
    bool ret = true;
    toml::parse_result cfg = toml::parse_file( tomlfile );
    if (!cfg) {
        std::cerr << "Failed to parse TOML file " <<tomlfile << cfg.error() << std::endl;
        ret = false;
        return ret;
    }

    // #TomlGet
    single_plot_mode_enabled=cfg.at_path("SinglePlotMode.single_plot_mode_enabled").value_or( false );  
    which_one 			    =cfg.at_path("SinglePlotMode.which_one").value_or( "3M Aura 9210+"sv ); 

    save_plots_enabled 		=cfg.at_path("Output.save_plots_enabled").value_or( true ); 
    save_with_HMFF_prefix 	=cfg.at_path("Output.save_with_HMFF_prefix").value_or( true ); 
    low_contrib_count_prefix =cfg.at_path("Output.low_contrib_count_prefix").value_or( "L_" ); 
    if(low_contrib_count_prefix.length() > 7) 
        low_contrib_count_prefix = low_contrib_count_prefix .substr(0,7);

    X11_persistence  		=cfg.at_path("Output.X11_persistence").value_or( true );  
    output_print_level      = cfg.at_path("Output.print_level").value_or( 0 );  


    use_only_analysis_grade	=cfg.at_path("Analysis.use_only_analysis_grade").value_or( true ); 
    use_sizes 			    =cfg.at_path("Analysis.use_sizes").value_or( true ); 

    tsv_filename 			        =cfg.at_path("TSV.tsv_filename").value_or( "Main.tsv"sv ); 
    skip_first_line_of_tsv_file 	=cfg.at_path("TSV.skip_first_line_of_tsv_file").value_or( true ); 
    maskname_tsv_column_index 		=cfg.at_path("TSV.maskname_tsv_column_index").value_or( 2 ); 
    exer1_tsv_column_index 			=cfg.at_path("TSV.exer1_tsv_column_index").value_or( 3 ); 
    headSize_tsv_column_index 		=cfg.at_path("TSV.headSize_tsv_column_index").value_or( 25 ); 
    number_of_exercises 			=cfg.at_path("TSV.number_of_exercises").value_or( 12 ); 
    analysis_grade_tsv_column_index	=cfg.at_path("TSV.analysis_grade_tsv_column_index").value_or( 20 ); 
    testerID_tsv_column_index 		=cfg.at_path("TSV.testerID_tsv_column_index").value_or( 16 );  
    protocol_tsv_column_index		=cfg.at_path("TSV.protocol_tsv_column_index").value_or( 19 );  

    
    date_tsv_column_index =cfg.at_path("TSV.date_tsv_column_index").value_or( 17 );  
    maxJawCm_tsv_column_index = cfg.at_path("TSV.maxJawCm_tsv_column_index").value_or(18);
    queryResult_tsv_column_index = cfg.at_path("TSV.queryResult_tsv_column_index").value_or(21);
    error_flag_file 			    =cfg.at_path("TSV.error_flag_file").value_or( "error.flag"sv ); 

    Legend_HMSideSwitchThresh  = cfg.at_path("Legend.HMSideSwitchThresh").value_or(5000.0);
    Legend_fontSize = cfg.at_path("Legend.fontSize").value_or(0.05);
    Legend_lowX1  = cfg.at_path("Legend.lowX1").value_or(0.672497);
    Legend_Y1  = cfg.at_path("Legend.Y1").value_or(0.6);
    Legend_lowX2  = cfg.at_path("Legend.lowX2").value_or(0.872738);
    Legend_Y2  = cfg.at_path("Legend.Y2").value_or(0.8);
    Legend_highX1  = cfg.at_path("Legend.highX1").value_or(0.132087);
    
    Legend_highX2  = cfg.at_path("Legend.highX2").value_or(0.332328);
   
    Legend_markerSize  = cfg.at_path("Legend.markerSize").value_or(20);
    Legend_histSize  = cfg.at_path("Legend.histSize").value_or(3);
    Legend_entryTextAll  = cfg.at_path("Legend.entryTextAll").value_or("All sizes");
    Legend_entryTextLg  = cfg.at_path("Legend.entryTextLg").value_or("Known Lg Heads");
    Legend_entryTextSm  = cfg.at_path("Legend.entryTextSm").value_or("Known Sm Heads");
    Legend_entryTextIQR  = cfg.at_path("Legend.entryTextIQR").value_or("Known IQR Heads");
    Legend_colorAll = cfg.at_path("Legend.colorAll").value_or(600);//600=kBlue
    Legend_colorLg = cfg.at_path("Legend.colorLg").value_or(432);//432=kCyan
    Legend_colorSm = cfg.at_path("Legend.colorSm").value_or(616);//616=kMagenta
    Legend_colorIQR = cfg.at_path("Legend.colorIQR").value_or(516);//616=kMagenta
    Legend_fillStyleLg = cfg.at_path("Legend.fillStyleLg").value_or(1001); // #1001 = solid, #4050
    Legend_fillStyleSm = cfg.at_path("Legend.fillStyleSm").value_or(0); //0 = hollow
    Legend_fillStyleIQR = cfg.at_path("Legend.fillStyleIQR").value_or(0); //0 = hollow
    Legend_sizeHistDrawOption  = cfg.at_path("Legend.sizeHistDrawOption").value_or("samehist");

    Arrow_useHarmMean=cfg.at_path("Arrow.useHarmMean ").value_or( true);
    Arrow_color 		=cfg.at_path("Arrow.color").value_or( 1 ); //1 = kBlack 
    Arrow_size 		=cfg.at_path("Arrow.size").value_or( 0.015 ); 
    Arrow_linewidth 	=cfg.at_path("Arrow.linewidth").value_or( 4 ); 
    histmax_factor 	=cfg.at_path("Arrow.histmax_factor").value_or( 1.052631 ); 
    Arrow_ymin 		=cfg.at_path("Arrow.ymin").value_or( 0.74 );  
    Arrow_length 	=cfg.at_path("Arrow.length").value_or( 0.146 );  
    Arrow_tipStyle 	=cfg.at_path("Arrow.tipStyle ").value_or( "|>"sv );

    ArrowText_label 		=cfg.at_path("ArrowText.label ").value_or( "HMpop"sv );
    ArrowText_lowerNDC 	=cfg.at_path("ArrowText.lowerNDC ").value_or( 0.865145);
    ArrowText_upperNDC 	=cfg.at_path("ArrowText.upperNDC ").value_or( 0.910788);
    ArrowText_leftOffset	=cfg.at_path("ArrowText.leftOffset ").value_or( -0.015);
    ArrowText_rightOffset=cfg.at_path("ArrowText.rightOffset ").value_or( 0.15);

    //[Scoring]
    score_file_name =cfg.at_path("Scoring.score_file").value_or( "scores.txt");
    sw.points_2xCrash25_2xOSHA_withGrimaceStored = cfg.at_path("Scoring.points_2xCrash25_2xOSHA_withGrimaceStored").value_or(50);
    sw.points_2xC25_1xOSHA_withGrimaceStored = cfg.at_path("Scoring.points_2xC25_1xOSHA_withGrimaceStored").value_or(25);
    sw.points_2xC25 = cfg.at_path("Scoring.points_2xC25").value_or(20);
    sw.points_1xC25_2xOSHA_withGrimaceStored = cfg.at_path("Scoring.points_1xC25_2xOSHA_withGrimaceStored").value_or(15);
    sw.points_1xCrash25_1xOSHA_withGrimaceStored = cfg.at_path("Scoring.points_1xCrash25_1xOSHA_withGrimaceStored").value_or(12);
    sw.points_2xOSHA_withGrimaceStored = cfg.at_path("Scoring.points_2xOSHA_withGrimaceStored").value_or(10);
    sw.points_1xCrash25 = cfg.at_path("Scoring.points_1xCrash25").value_or(5);
    sw.points_1xOSHA_withGrimaceStored = cfg.at_path("Scoring.points_1xOSHA_withGrimaceStored").value_or(2);
    sw.points_1xCrash25short = cfg.at_path("Scoring.points_1xCrash25short").value_or(0.6);

    sizePixelsX =cfg.at_path("Histogram_Graphics.sizePixelsX").value_or( 1660); 
    sizePixelsY =cfg.at_path("Histogram_Graphics.sizePixelsY").value_or( 989); 
    x_axis_title 	=cfg.at_path("Histogram_Graphics.x_axis_title").value_or( "Exposure Reduction Factor               "sv ); 
    y_axis_title 	=cfg.at_path("Histogram_Graphics.y_axis_title").value_or( "Event Count"sv ); 

    std::string ymax_setting_str = cfg.at_path("Histogram_Graphics.ymax_setting").value_or( "auto_fit_each_histogram" ); 
    //enum Ymax_state
    if(ymax_setting_str == "manual")
            ymax_setting=manual;
    if(ymax_setting_str == "global_full_auto")
            ymax_setting=global_full_auto;
    if(ymax_setting_str == "global_auto_with_manual_min_ymax")
            ymax_setting=global_auto_with_manual_min_ymax;
    if(ymax_setting_str == "auto_fit_each_histogram")
            ymax_setting=auto_fit_each_histogram;
    else ymax_setting=auto_fit_each_histogram;

    histogram_ymax 	=cfg.at_path("Histogram_Graphics.histogram_ymax").value_or( 80.0 );  


    HistTitle_X1 =cfg.at_path("HistTitle.X1").value_or( 0.16 );  
    HistTitle_Y1 =cfg.at_path("HistTitle.Y1").value_or( 0.94 );  
    HistTitle_X2 =cfg.at_path("HistTitle.X2").value_or( 0.75 );  
    HistTitle_Y2 =cfg.at_path("HistTitle.Y2").value_or( 0.995 );  

    Contributers_X1 =cfg.at_path("Contributers.X1").value_or(0.00120627);
    Contributers_Y1 =cfg.at_path("Contributers.Y1").value_or(0.965768);
    Contributers_X2 =cfg.at_path("Contributers.X2").value_or(0.179131);
    Contributers_Y2 =cfg.at_path("Contributers.Y2").value_or(0.997925);
    Contributers_prefixText =cfg.at_path("Contributers.prefixText").value_or("TIL distribution as of "sv);
    Contributers_timeFormat =cfg.at_path("Contributers.timeFormat").value_or("%Y-%m-%d %H:%M"sv );
    Contributers_suffixText =cfg.at_path("Contributers.suffixText").value_or("");
    Contributers_textAlignment =cfg.at_path("Contributers.textAlignment").value_or(12); //12 = Align to top left

    std::string colorScheme_str 	=cfg.at_path("ColorScheme.colorScheme").value_or( "" ); 
    //enum ColorScheme 
    if(colorScheme_str == "trafficLight") colorScheme=trafficLight;
    if(colorScheme_str == "trafficLightFaded") colorScheme=trafficLightFaded;
    if(colorScheme_str == "blueberry") colorScheme=blueberry;
    if(colorScheme_str == "LUT1") colorScheme=LUT1;
    if(colorScheme_str == "LUT2") colorScheme=LUT2;
    if(colorScheme_str == "blackWhite") colorScheme=blackWhite;
    if(colorScheme_str == "grayGreen") colorScheme=grayGreen;
    else colorScheme=LUT1;

    std::string bkgColorScheme_str 	=cfg.at_path("ColorScheme.bkgColorScheme").value_or( "" ); 
    
    //enum BkgColorScheme 
    if(bkgColorScheme_str == "White") bkgColorScheme = White; 
    if(bkgColorScheme_str == "OffWhite") bkgColorScheme = OffWhite; 
    if(bkgColorScheme_str == "Dark") bkgColorScheme = Dark; 
    else bkgColorScheme = White; 

    LUT1_red_hex 	=cfg.at_path("ColorScheme.LUT1.red_hex").value_or( "#FF3355" ); 
    LUT1_yellow_hex =cfg.at_path("ColorScheme.LUT1.yellow_hex").value_or( "#FFAA00"  );
    LUT1_red_end 	=cfg.at_path("ColorScheme.LUT1.red_end").value_or( 1.0 );  
    LUT1_yellow_end =cfg.at_path("ColorScheme.LUT1.yellow_end").value_or( 1.47712125472 );  
    //lut1_len 		=cfg.at_path("ColorScheme.LUT1.lut1_len").value_or( 8 ); 


    toml::array* r_TArr = cfg["ColorScheme"]["LUT1"]["r"].as_array();
    if(r_TArr and r_TArr->is_homogeneous(toml::node_type::integer) ){ 
        size_t i = 0;
        for (auto it = r_TArr->cbegin(); it != r_TArr->cend() and i<lut1_len; ++it) 
            r[i++] = it->value_or(-1);
        //if anything ends up -1, complain.
        bool neg1 = false;
        for(int j=0;j<lut1_len;j++) if( r[j] == -1 ) neg1 = true;

        if(neg1){
            std::cerr << "Warning: LUT1::r has invalid entries. Toml file: " <<tomlfile << std::endl;
            ret &= !neg1;
        }

        if(i<lut1_len){
            std::cerr << "Warning: LUT1::r is too short. Toml file: " <<tomlfile << std::endl;
            ret = false;
        }
    }

    toml::array* g_TArr = cfg["ColorScheme"]["LUT1"]["g"].as_array();
    if(g_TArr and g_TArr->is_homogeneous(toml::node_type::integer) ){ 
        size_t i = 0;
        for (auto it = g_TArr->cbegin(); it != g_TArr->cend() and i<lut1_len; ++it) 
            g[i++] = it->value_or(-1);
        //if anything ends up -1, complain.
        bool neg1 = false;
        for(int j=0;j<lut1_len;j++) if( g[j] == -1 ) neg1 = true;

        if(neg1){
            std::cerr << "Warning: LUT1::g has invalid entries. Toml file: " <<tomlfile << std::endl;
            ret &= !neg1;
        }

        if(i<lut1_len){
            std::cerr << "Warning: LUT1::g is too short. Toml file: " <<tomlfile << std::endl;
            ret = false;
        }
    }

    toml::array* b_TArr = cfg["ColorScheme"]["LUT1"]["b"].as_array();
    if(b_TArr and b_TArr->is_homogeneous(toml::node_type::integer) ){ 
        size_t i = 0;
        for (auto it = b_TArr->cbegin(); it != b_TArr->cend() and i<lut1_len; ++it) 
            b[i++] = it->value_or(-1);
        //if anything ends up -1, complain.
        bool neg1 = false;
        for(int j=0;j<lut1_len;j++) if( b[j] == -1 ) neg1 = true;

        if(neg1){
            std::cerr << "Warning: LUT1::b has invalid entries. Toml file: " <<tomlfile << std::endl;
            ret &= !neg1;
        }

        if(i<lut1_len){
            std::cerr << "Warning: LUT1::b is too short. Toml file: " <<tomlfile << std::endl;
            ret = false;
        }
    }

    LUT2_uses_harmean =cfg.at_path("ColorScheme.LUT2.uses_harmean").value_or( true );
    //LUT2

    LUT2_red_hex 	=cfg.at_path("ColorScheme.LUT2.red_hex").value_or( "#FF3355" ); 
    LUT2_yellow_hex =cfg.at_path("ColorScheme.LUT2.yellow_hex").value_or( "#FFAA00"  );
    LUT2_red_end 	=cfg.at_path("ColorScheme.LUT2.red_end").value_or( 1.0 );  
    LUT2_yellow_end =cfg.at_path("ColorScheme.LUT2.yellow_end").value_or( 1.47712125472 );  
    //lut2_len 		=cfg.at_path("ColorScheme.LUT2.lut2_len").value_or( 8 ); 


    toml::array* r_TArr2 = cfg["ColorScheme"]["LUT2"]["r"].as_array();
    if(r_TArr2 and r_TArr2->is_homogeneous(toml::node_type::integer) ){ 
        size_t i = 0;
        for (auto it = r_TArr2->cbegin(); it != r_TArr2->cend() and i<lut2_len; ++it) 
            LUT2_r[i++] = it->value_or(-1);
        //if anything ends up -1, complain.
        bool neg1 = false;
        for(int j=0;j<lut2_len;j++) if( LUT2_r[j] == -1 ) neg1 = true;

        if(neg1){
            std::cerr << "Warning: LUT2::r has invalid entries. Toml file: " <<tomlfile << std::endl;
            ret &= !neg1;
        }

        if(i<lut2_len){
            std::cerr << "Warning: LUT2::r is too short. Toml file: " <<tomlfile << std::endl;
            ret = false;
        }
    }

    toml::array* g_TArr2 = cfg["ColorScheme"]["LUT2"]["g"].as_array();
    if(g_TArr2 and g_TArr2->is_homogeneous(toml::node_type::integer) ){ 
        size_t i = 0;
        for (auto it = g_TArr2->cbegin(); it != g_TArr2->cend() and i<lut2_len; ++it) 
            LUT2_g[i++] = it->value_or(-1);
        //if anything ends up -1, complain.
        bool neg1 = false;
        for(int j=0;j<lut2_len;j++) if( LUT2_g[j] == -1 ) neg1 = true;

        if(neg1){
            std::cerr << "Warning: LUT2::g has invalid entries. Toml file: " <<tomlfile << std::endl;
            ret &= !neg1;
        }

        if(i<lut2_len){
            std::cerr << "Warning: LUT2::g is too short. Toml file: " <<tomlfile << std::endl;
            ret = false;
        }
    }

    toml::array* b_TArr2 = cfg["ColorScheme"]["LUT2"]["b"].as_array();
    if(b_TArr2 and b_TArr2->is_homogeneous(toml::node_type::integer) ){ 
        size_t i = 0;
        for (auto it = b_TArr2->cbegin(); it != b_TArr2->cend() and i<lut2_len; ++it) 
            LUT2_b[i++] = it->value_or(-1);
        //if anything ends up -1, complain.
        bool neg1 = false;
        for(int j=0;j<lut2_len;j++) if( LUT2_b[j] == -1 ) neg1 = true;

        if(neg1){
            std::cerr << "Warning: LUT2::b has invalid entries. Toml file: " <<tomlfile << std::endl;
            ret &= !neg1;
        }

        if(i<lut2_len){
            std::cerr << "Warning: LUT2::b is too short. Toml file: " <<tomlfile << std::endl;
            ret = false;
        }
    }

    LUT2_uses_harmean =cfg.at_path("ColorScheme.LUT2.uses_harmean").value_or( true );

    //trafficLight
    red_hex 		=cfg.at_path("ColorScheme.trafficLight.red_hex").value_or( "#FF3355" );
    yellow_hex 		=cfg.at_path("ColorScheme.trafficLight.yellow_hex").value_or( "#FFAA00" );
    green_hex 		=cfg.at_path("ColorScheme.trafficLight.green_hex").value_or( "#26E600" );
    red_end 		=cfg.at_path("ColorScheme.trafficLight.red_end").value_or( 1.0 );  
    yellow_end 		=cfg.at_path("ColorScheme.trafficLight.yellow_end").value_or( 1.47712125472 ); 

    GG_red_hex =cfg.at_path("ColorScheme.grayGreen.red_hex").value_or( "#FF3355" );
    GG_yellow_hex =cfg.at_path("ColorScheme.grayGreen.yellow_hex").value_or( "#FFAA00" );
    GG_green_hex =cfg.at_path("ColorScheme.grayGreen.green_hex").value_or( "#26E600" );
    GG_red_end =cfg.at_path("ColorScheme.grayGreen.red_end").value_or( 1.0 );  
    GG_yellow_end =cfg.at_path("ColorScheme.grayGreen.yellow_end").value_or( 1.47712125472 ); 

    TLF_red_hex 	=cfg.at_path("ColorScheme.trafficLightFaded.red_hex").value_or("#FF3355");
    TLF_yellow_hex 	=cfg.at_path("ColorScheme.trafficLightFaded.yellow_hex").value_or( "#FFAA00" );
    TLF_red_end 	=cfg.at_path("ColorScheme.trafficLightFaded.red_end").value_or( 1.0 );  
    TLF_yellow_end 	=cfg.at_path("ColorScheme.trafficLightFaded.yellow_end").value_or( 1.47712125472 );  
    TLF_hue_green 		=cfg.at_path("ColorScheme.trafficLightFaded.hue_green").value_or( 120.0 );  
    TLF_value_green 	=cfg.at_path("ColorScheme.trafficLightFaded.value_green").value_or( 0.274 );  
    TLF_percentile_hardness =cfg.at_path("ColorScheme.trafficLightFaded.percentile_hardness").value_or( 5.0 );  
    TLF_percentile_corner 	=cfg.at_path("ColorScheme.trafficLightFaded.percentile_corner").value_or( 0.5 ); 
    TLF_graymax 	=cfg.at_path("ColorScheme.trafficLightFaded.graymax").value_or( 0.945 ); 

    BW_red_hex 	=cfg.at_path("ColorScheme.trafficLightFaded.red_hex").value_or("#C3C3C3");
    BW_yellow_hex 	=cfg.at_path("ColorScheme.trafficLightFaded.yellow_hex").value_or( "#C3C3C3" );
    BW_red_end 	=cfg.at_path("ColorScheme.trafficLightFaded.red_end").value_or( 1.0 );  
    BW_yellow_end 	=cfg.at_path("ColorScheme.trafficLightFaded.yellow_end").value_or( 1.47712125472 );  

    BW_hue_green 		=cfg.at_path("ColorScheme.trafficLightFaded.hue_green").value_or( 120.0 );  
    BW_value_green 	=cfg.at_path("ColorScheme.trafficLightFaded.value_green").value_or( 0.82 );  
    BW_percentile_hardness =cfg.at_path("ColorScheme.trafficLightFaded.percentile_hardness").value_or( 5.0 );  
    BW_percentile_corner 	=cfg.at_path("ColorScheme.trafficLightFaded.percentile_corner").value_or( 0.5 ); 
    BW_graymax 	=cfg.at_path("ColorScheme.trafficLightFaded.graymax").value_or( 0.945 ); 


    /*std::string satur_func_str 	=cfg.at_path("ColorScheme.trafficLightFaded.satur_func").value_or( "" ); 
    if(satur_func_str == "S_abs") satur_func = S_abs;
    if(satur_func_str == "S_erf") satur_func = S_erf;
    if(satur_func_str == "S_gd") satur_func = S_gd;
    if(satur_func_str == "S_algeb") satur_func = S_algeb;
    if(satur_func_str == "S_atan") satur_func = S_atan;
    if(satur_func_str == "S_absalgeb") satur_func = S_absalgeb;
    if(satur_func_str == "S_tanh") satur_func = S_tanh;
    else satur_func = S_tanh;*/

    blueness 		=cfg.at_path("ColorScheme.blueberry.blueness").value_or( 1.0 ); 
    graymax 		=cfg.at_path("ColorScheme.blueberry.graymax").value_or( 0.945 );  

    red_hex_dark 	=cfg.at_path("ColorScheme.darkMode.red_hex_dark").value_or( "#FF3355" ); 
    yellow_hex_dark	=cfg.at_path("ColorScheme.darkMode.yellow_hex_dark").value_or( "#FFAA00" ); 
    green_hex_dark 	=cfg.at_path("ColorScheme.darkMode.green_hex_dark").value_or( "#26E600" ); 
    value_green_dark=cfg.at_path("ColorScheme.darkMode.value_green_dark").value_or( 0.682 );  

    return ret;
} //load toml

void Settings::debug(int thresh,std::string msg){
    if(output_print_level >= thresh)
        std::cout<<msg<<std::endl;
}
///////////////////////////////////////////////////////////////////
///////////////////////// End Settings /////////////////////////////
///////////////////////////////////////////////////////////////////

struct Hist{ // #StructHist
    bool is_sorted;
    bool has_cdf;
    float cdf[nbins+1];
    string title;
    TH1F* hist;
    TH1F* histBySize[SIZEMAX];
    std::vector<float> all_vals;
    std::set<std::string> unique_testers;

    Hist(string title, float* linbinning, const Settings& cfg);
    ~Hist(){ delete hist; }
    void Fill(float val,sizeCode s);
    void FillLog10(float val,sizeCode s);
    string GetTitle(){return title;}
    float Percentile2X(float percentile);
    float X2Percentile(float x);
    float Get_Median(){ return Percentile2X(0.5f); }
    float Get_HarmonicMean();
};
struct MaskUserCombo{
    float harMean;
    float _harMeanTally = 0.0f;
    float n = 0.0f;
    std::string holdDate = "";
    std::unordered_map<std::string, int> protocol_counts; //index with protocol
    std::unordered_map<std::string, int> protocol_counts_cpy; //a copy consumed by scoreCombo
    void Fill(float x){
        _harMeanTally += 1.0f/x;
        n += 1.0f;
        harMean = n/_harMeanTally;
    }
    void copyProtocolCounts(){ protocol_counts_cpy.insert(protocol_counts.begin(), protocol_counts.end()); }
    float scoreCombo(const ScoreWeights& sw); //produce a score for this combo.
    void increment_protocol(std::string protocol, bool hold);

};
float MaskUserCombo::scoreCombo(const ScoreWeights& sw){
    //Score a combination of user and mask
    static const std::string OG  = "OSHAwGrimaceStored";
    static const std::string C25 = "Crash2.5";
    static const std::string F25s= "FTtP2.5 short";
    static const std::string C25s = "Crash2.5short";
    //static const OnoG= "OSHAwNoGrimaceRecord";
    //static const CU = "CrashUM";
    //static const C40 = "Crash4.0";
    //static const CSA = "CSA";
    //static const CSAz= "CSA Z94.4-2018";
    //static const CUKa = "CrashUMkids adversarial";
    //static const xxx = "CUSTOM - lying in bed";
    //static const GT = "Ground Truth"; //hold
    //static const CJ = "Crown Jewel";//hold

    if(protocol_counts_cpy[C25] >= 2){
        protocol_counts_cpy[C25] = 0;
        //protocol_counts_cpy[C25] -= 2;
        if(protocol_counts_cpy[OG] >= 2){
            protocol_counts_cpy[OG] = 0;
            //protocol_counts_cpy[OG] -= 2;
            return sw.points_2xCrash25_2xOSHA_withGrimaceStored + scoreCombo(sw);
        } else if(protocol_counts_cpy[OG] == 1){
            protocol_counts_cpy[OG] = 0;
            //protocol_counts_cpy[OG] -= 1;
            return sw.points_2xC25_1xOSHA_withGrimaceStored + scoreCombo(sw);
        }
        else{
            return sw.points_2xC25+ scoreCombo(sw);
        }
    } else if(protocol_counts_cpy[C25] >= 1){
        protocol_counts_cpy[C25] = 0;
        //protocol_counts_cpy[C25] -= 1;
        if(protocol_counts_cpy[OG] >= 2){
            protocol_counts_cpy[OG] = 0;
            //protocol_counts_cpy[OG] -= 2;
            return sw.points_1xC25_2xOSHA_withGrimaceStored + scoreCombo(sw);
        } else if(protocol_counts_cpy[OG] == 1){
            protocol_counts_cpy[OG] = 0;
            //protocol_counts_cpy[OG] -= 1;
            return sw.points_1xCrash25_1xOSHA_withGrimaceStored + scoreCombo(sw);
        }
        else{
            return sw.points_1xCrash25 + scoreCombo(sw);
        }
    } else if(protocol_counts_cpy[OG] >= 2){
        protocol_counts_cpy[OG] = 0;
        //protocol_counts_cpy[OG] -= 2;
        return sw.points_2xOSHA_withGrimaceStored+ scoreCombo(sw);
    } else if(protocol_counts_cpy[OG] == 1){
        protocol_counts_cpy[OG] = 0;
        //protocol_counts_cpy[OG] -= 1;
        return sw.points_1xOSHA_withGrimaceStored + scoreCombo(sw);
    }
    else if(protocol_counts_cpy[C25s] >= 1){
        protocol_counts_cpy[C25s] = 0;
        //protocol_counts_cpy[C25s] -= 1;
        return sw.points_1xCrash25short + scoreCombo(sw);
    }
    else return 0.0f;
     //need to truncate out " extended*", followe by maybe a comma at the end., or ", Extended"
     //or if the name ends in " ##", exclude that. 
     //hold needs to be at the loop level since it is not tied to any particular protocol   .
}
struct User{
    float score = 0.0f;
    std::unordered_map<std::string, MaskUserCombo*> maskUserComboMap; //index with mask name
    float scoreUser(const ScoreWeights& sw);
    void include(std::string maskname, std::string protocol, bool hold);
    ~User(){ 
        for (const auto& pair : maskUserComboMap) 
            delete pair.second; 
    }
};
void User::include(std::string maskname, std::string protocol, bool hold){
    if ( std::unordered_map<std::string, MaskUserCombo*>::iterator it = maskUserComboMap.find(maskname);
            it != maskUserComboMap.end()) { //This mask has been seen already. Fill the existing 
        maskUserComboMap[maskname]->increment_protocol(protocol, hold);
    } else {//New mask, create a new combo
        MaskUserCombo* combo = new MaskUserCombo();
        combo->protocol_counts[protocol] = 1;
        maskUserComboMap[maskname] = combo;
    }
}
void MaskUserCombo::increment_protocol(std::string protocol, bool hold){
    if ( std::unordered_map<std::string, int>::iterator it = protocol_counts.find(protocol);
            it != protocol_counts.end()) { //This protocol has been seen already. Fill the existing 
        if( not hold) protocol_counts[protocol] += 1;
    } else {//New mask, create a new combo
        protocol_counts[protocol] = 1;
    }
}


float User::scoreUser(const ScoreWeights& sw){
    for (const auto& pair : maskUserComboMap) {
        pair.second->copyProtocolCounts();
        score += pair.second->scoreCombo(sw);
    }
    return score;
}

struct HoldStateMachine{
    bool is_continuation = false;
    int dateState= -1;
    std::string maskNameState  = "";
    std::string testerIDSstate = "";
    std::string protocolState  = "";
    void reset(){ 
        is_continuation = false;
        dateState = -1;
        maskNameState  = "";
        testerIDSstate = "";
        protocolState  = "";
    }
    void set(int dateJ2000, std::string maskname, std::string testerID, std::string protocol) {
        if(dateJ2000 < 0){ //blank or invalid date, reset.
            reset();
        } else{ 
            is_continuation = (dateState > 0 and 
                    dateState == dateJ2000 and 
                    maskNameState == maskname and 
                    testerIDSstate == testerID and
                    protocolState == protocol);
            dateState = dateJ2000;
            maskNameState = maskname;
            testerIDSstate = testerID;
            protocolState = protocol;
        }
    }

};


// #Prototypes
std::vector<std::string> parseTSVLine(const std::string& tsv_line);
float Str2float(std::string& token);
int   Str2int(std::string& token);
bool  Str2bool(std::string& token);
sizeCode Str2size(std::string s);
float dateTimeStr_to_J2000(std::string dateTimeStr);
int dateStr_to_J2000(std::string dateStr);
float* generateLinBinning();
float* generateLogBinning();
TF2* makeGrad(float ymax);
void SetBinLabels(Hist* hist);
void PlotAndSave(Hist* hist, TF2* grad, string fname_noext, const Settings& cfg);
bool isALlWhiteSpace(const std::string& str);
std::string getCurrentDateTime(const Settings& cfg);

double sigmoid(double x, SigmoidOption softness);

Hist::Hist(string title, float* linbinning, const Settings& cfg): is_sorted(false), has_cdf(false), title(title){
    hist = new TH1F(title.c_str(), 
            (title+";"+cfg.x_axis_title+";"+cfg.y_axis_title).c_str(), 
            nbins,linbinning);
    
    histBySize[Lg] = new TH1F((title+" Lg").c_str(), 
            (title+" Lg;"+cfg.x_axis_title+";"+cfg.y_axis_title).c_str(), 
            nbins,linbinning);
    histBySize[Sm] = new TH1F((title+" Sm").c_str(), 
            (title+" Sm;"+cfg.x_axis_title+";"+cfg.y_axis_title).c_str(), 
            nbins,linbinning);
    histBySize[IQR] = new TH1F((title+" IQR").c_str(), 
            (title+" IQR;"+cfg.x_axis_title+";"+cfg.y_axis_title).c_str(), 
            nbins,linbinning);
    histBySize[NOSIZE] = new TH1F((title+" no size").c_str(), 
            (title+" no size;"+cfg.x_axis_title+";"+cfg.y_axis_title).c_str(), 
            nbins,linbinning);
}
void Hist::Fill(float val,sizeCode s){
    //Fill for linear axis
    hist->Fill(val);
    histBySize[s]->Fill(val);
    all_vals.push_back(val);
}
void Hist::FillLog10(float val,sizeCode s){
    //Correctly handles fill for artificial log axis
    hist->Fill(TMath::Log10(val));
    histBySize[s]->Fill(TMath::Log10(val));
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

void makeAllPlots(std::string tomlfile = "config.toml"){ //main

    Settings cfg;
    if(not cfg.load(tomlfile)){
        std::cerr<<" Errors encountered in TOML config file "<<tomlfile<<std::endl;
        return;
    }

    cfg.debug(3,"start makeAllPlots");
    { //Check if there's an error flag, and if so, don't run.
        std::ifstream flag_file(cfg.error_flag_file);
        if (flag_file.good()){
            std::cout << "Error flag was raised, so the TSV file must be no good. Root exits without generating plots." << std::endl;
            return;
        }
    }


    switch(cfg.bkgColorScheme){
        case OffWhite:
            //BkgColor = 10; // 254, 254, 254
            //BkgColor = 19; // very light gray
            BkgColor = GetColorForced(248,248,248);//CFGTODO
            std::cout<<"Bkg color "<<BkgColor <<std::endl;//DEBUG
            FontColor = kBlack;
            break;
        case Dark:
            BkgColor = TColor::GetColor(0.188f, 0.22f, 0.255f);//dark gray #303841//CFGTODO
            FontColor = TColor::GetColor(0.847f, 0.871f, 0.914f);//#D8DEE9//CFGTODO
            break;
        default:
        case White:
            BkgColor = kWhite; //255, 255, 255
            FontColor = kBlack;
            break;
    }
                     
    if( cfg.single_plot_mode_enabled) std::cout<<"Single Plot Mode ENABLED, see single_plot_mode_enabled"<<std::endl;
    std::unordered_map<std::string, Hist*> hMap;
    std::unordered_map<std::string, User*> users;
	CMSStyle(); 
	float* linbinning = generateLinBinning();
    static const float grad_height = max(75.f,cfg.histogram_ymax+1.f);//the float argument (75) is the height that the gradient will go up to. anything pretty big is ok
    TF2* grad = makeGrad(grad_height); 

    std::ifstream inputFile(cfg.tsv_filename);
    if (!inputFile.is_open()) {
        std::cerr << "Error unable to opening file" << cfg.tsv_filename << std::endl;
        return;
    }

    cfg.debug(4,"start readfile");

    //      ____  _________    ____     ____________
    //     / __ \/ ____/   |  / __ \   / ____/  _/ /__
    //    / /_/ / __/ / /| | / / / /  / /_   / // / _ \
    //   / _, _/ /___/ ___ |/ /_/ /  / __/ _/ // /  __/
    //  /_/ |_/_____/_/  |_/_____/  /_/   /___/_/\___/
    //  #READFILE
    { //while namespace
    std::string tsv_line;
    int jline=0; //tsv line number counter.
    HoldStateMachine hold;
    //bool hold = false;
    //int holdState = -1; //-1 means continue, >0 represents a J2000 date
    int dateJ2000;
    size_t pos;
    bool is_analysis_grade;
    string dateStr;
    string maskname;
    string testerID;
    string protocol;
    while (std::getline(inputFile, tsv_line)) { //for every line
        jline++; 
        if(cfg.skip_first_line_of_tsv_file and jline == 1) continue;
        // Parse the line into tokens
        tsv_line.erase(std::remove(tsv_line.begin(), tsv_line.end(), ','), tsv_line.end());
        std::vector<std::string> tokens = parseTSVLine(tsv_line);

        //Guard against invalid lines
        if(tokens.size() <= cfg.exer1_tsv_column_index){
            std::cerr<<"ERROR nogo line "<<jline<<std::endl;
        } else if(cfg.use_sizes and tokens.size() <= cfg.headSize_tsv_column_index ){
            std::cerr<<"Warning Unable to access sizes due to line "<<jline<<std::endl;
        } else if(tokens.size() < cfg.exer1_tsv_column_index + cfg.number_of_exercises) {
            std::cerr<<"ERROR weird length line "<<jline<<std::endl;
        } else{ //line not obviously invalid
            if(jline % 100 == 0) std::cout<<"readln "<<jline<<std::endl;

            if(cfg.use_only_analysis_grade and tokens.size() < cfg.analysis_grade_tsv_column_index){
                std::cerr<<"Warning line too short for analysis grade "<<jline<<std::endl;
                continue;
            }
            sizeCode s = NOSIZE;
            if(cfg.use_sizes and tokens.size() > cfg.headSize_tsv_column_index ){
                s = Str2size(tokens[cfg.headSize_tsv_column_index]);
            }

            //Get all the data 
            is_analysis_grade = Str2bool(tokens[cfg.analysis_grade_tsv_column_index]);
            if(cfg.use_only_analysis_grade and not is_analysis_grade){
                //std::cout<<"line rejected for non-analysis grade tag "<<jline<<std::endl;
                continue;
            }
            dateStr = tokens[cfg.date_tsv_column_index]; //string, blank or YYYY_MM_DD
            dateJ2000 = dateStr_to_J2000(dateStr);
            testerID = tokens[cfg.testerID_tsv_column_index];
            protocol = tokens[cfg.protocol_tsv_column_index];
            //string queryResult = tokens[cfg.queryResult_tsv_column_index];
            //in the set of {"","FFP3","P2","N95","R95","KN95","ASTM F3502-21", "GB19083-2010", "KF94", "N100","water vapor","N99","FFP3, N95"...others
            //string maxJawCm = tokens[cfg.maxJawCm_tsv_column_index]; //blank or float
            maskname = tokens[cfg.maskname_tsv_column_index];


            //Clean the mask name
            std::replace(maskname.begin(), maskname.end(),'/','_'); //Guard names against /
            std::replace(maskname.begin(), maskname.end(),'\'','_'); //Guard names against '

            if( (pos = maskname.find(" extended")) != std::string::npos){
                maskname.erase(pos);
                hold.set(dateJ2000, maskname, testerID, protocol);
            } 
            else if( (pos = maskname.find(" Extended")) != std::string::npos){
                maskname.erase(pos);
                hold.set(dateJ2000, maskname, testerID, protocol);
            } 
            else if (maskname.size() >= 3 && 
                    maskname[maskname.size() - 3] == ' ' && 
                    isdigit(maskname[maskname.size() - 2]) && 
                    isdigit(maskname[maskname.size() - 1]) && 
                    maskname != "Breathe 99") {
                maskname.pop_back(); 
                maskname.pop_back(); 
                maskname.pop_back(); 
                hold.set(dateJ2000, maskname, testerID, protocol);
            } 
            else hold.reset(); 
            //end clean the mask name



            if ( std::unordered_map<std::string, Hist*>::iterator it = hMap.find(maskname);
                    it != hMap.end()) { //This mask has been seen already. Fill the existing histogram
                //int i=0;
                //for (;i<cfg.number_of_exercises;i++){
                for (int i=0;i<cfg.number_of_exercises;i++){
                    float x = Str2float(tokens[cfg.exer1_tsv_column_index + i]);
                    if(x<0.f) break;
                    else it->second->FillLog10(x,s);
                }
                //std::cout<<jline<<" "<<maskname<<" old fill #="<<i<<std::endl;
            } else { //New mask, create a new histogram
                Hist* newHistogram = new Hist(maskname,linbinning,cfg);

                //int i=0;
                //for (;i<cfg.number_of_exercises;i++){
                for (int i=0;i<cfg.number_of_exercises;i++){
                    float x = Str2float(tokens[cfg.exer1_tsv_column_index + i]);
                    if(x<0.f) break;
                    else newHistogram->FillLog10(x,s);
                }
                //std::cout<<jline<<" "<<maskname<<" new fill #="<<i<<std::endl;
                hMap[maskname] = newHistogram;
            } //end else 

            if(not isALlWhiteSpace(testerID))
                hMap[maskname]->unique_testers.insert(testerID);

            //Fill in users and user-mask combos
            if ( std::unordered_map<std::string, User*>::iterator uit = users.find(testerID);
                    uit != users.end()) { //This user has been seen already. Fill the existing 
                users[testerID]->include(maskname, protocol, hold.is_continuation);

                //mask fakes a digit counter, no new date, and new tester.
                //situation: not new user, but is new mask. 

                for (int i=0;i<cfg.number_of_exercises;i++){
                    float x = Str2float(tokens[cfg.exer1_tsv_column_index + i]);
                    if(x>=0.f) users[testerID]->maskUserComboMap[maskname]->Fill(x);
                    else break;
                }
            } else {//New user, create a new User
                User* user = new User();
                user->include(maskname, protocol, false);
                for (int i=0;i<cfg.number_of_exercises;i++){
                    float x = Str2float(tokens[cfg.exer1_tsv_column_index + i]);
                    if(x>=0.f) user->maskUserComboMap[maskname]->Fill(x);
                    else break;
                }
                users[testerID] = user;
            }

        } //end else ok line
    } //end while every tsv line

    }//end while namespace
    inputFile.close();

    cfg.debug(4,"end readfile");

    //Write Scores File.
    if ( std::ofstream score_file(cfg.score_file_name);
            score_file.is_open()) {

        std::vector<std::string> testerIDs;
        for (const auto& pair : users){
            testerIDs.push_back(pair.first);
            pair.second->scoreUser(cfg.sw);
        }

        std::sort(testerIDs.begin(), testerIDs.end(), 
                [&users](std::string a, std::string b) { return users[a]->score > users[b]->score; });

        for (const auto& testerID : testerIDs) {
            //std::cout << name << ": " << nameToStruct[name].score << std::endl;
            score_file << testerID << " " << users[testerID]->score << std::endl;
        }
        /*for (const auto& pair : users) {
            score_file << pair.first << " " << pair.second->scoreUser(cfg.sw) << std::endl;
        }*/
        score_file.close();
    } else {
        std::cout<<"Error! Unable to write score file due to a file IO problem"<<std::endl;
    }
    cfg.debug(4,"end scoring");

    if(cfg.ymax_setting >= global_full_auto){
        static const float ymax_margin = 1.10f;//CFGTODO
        //calculate the largest bin content of all histograms
        float global_max_bin = 0.f;
        for (const std::pair<const std::string, Hist*>& pair : hMap) {
            global_max_bin = max(global_max_bin, (float)((Hist*) pair.second)->hist->GetMaximum());
        }
        if(cfg.ymax_setting == global_full_auto or 
                (cfg.ymax_setting == global_auto_with_manual_min_ymax and global_max_bin > cfg.histogram_ymax*ymax_margin))
            cfg.histogram_ymax = global_max_bin*ymax_margin;
    }

    //Now make all plots and save them to file.
    bool something_was_found = false;
    for (const std::pair<const std::string, Hist*>& pair : hMap) {
        const std::string& mask = pair.first;
        Hist* histogram = pair.second;
        if( cfg.single_plot_mode_enabled and mask != cfg.which_one ) continue;
        something_was_found = true;

        if(cfg.save_plots_enabled){
            if(cfg.single_plot_mode_enabled){
                std::cout<<"Plot and save "<<mask<<" single_plot_mode_enabled = true so no other plot get generated."<<std::endl;
            } 

            PlotAndSave(histogram, grad, mask, cfg);
        }
    }
    if( cfg.single_plot_mode_enabled and not something_was_found ){
        std::cout<<"Warning! No plot found that matched name which_one = "<<cfg.which_one<<std::endl;
    }
    
    if(not (cfg.single_plot_mode_enabled and cfg.X11_persistence)){
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
    const UInt_t Ncolors = 3;//CFGTODO
    Double_t Red[Ncolors]   = {0.957,0.999,0.999};//CFGTODO
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


void PlotAndSave(Hist* hist, TF2* grad, string fname_noext, const Settings& cfg){
    //plot it and make it pretty
	PrettyFillColor(hist->hist, TColor::GetColor(242, 242, 242));  // ilya - formerly kAzure + 5//CFGTODO
    SetBinLabels(hist);
    float* linbinning = generateLinBinning();

    TH1F* histarr[nbins];
    for(int i=0;i<nbins;i++){
        histarr[i] = new TH1F( 
                (hist->GetTitle()+std::to_string(i)).c_str(),
                (";"+cfg.x_axis_title+","+cfg.y_axis_title).c_str(),
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
        if(cfg.colorScheme == trafficLightFaded){
            if(bc < cfg.TLF_red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(cfg.TLF_red_hex.c_str()));
            } else if(bc < cfg.TLF_yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.TLF_yellow_hex.c_str()));
            }
            else{ // >= TMath::Log10(30)
                //float xxx = (bc - mean)/stddev;
                //float xxx = (bc - hist->Get_Median())/stddev;
                //float xxx = (bc - TMath::Log10(hist->Get_HarmonicMean()))/stddev;
                float xxx = cfg.TLF_percentile_hardness*(hist->X2Percentile(bc) - cfg.TLF_percentile_corner );
                //float saturation = 1.0 - std::max(0, sigmoid(xxx, cfg.satur_func ));
                float saturation = 1.0f/(1.0f + exp(2.0*xxx));//CFGTODO
                float value = cfg.TLF_graymax - (cfg.TLF_graymax - cfg.TLF_value_green)*saturation; //CFGTODO graymax is in blueberry
                PrettyFillColor(histarr[i],GetColorHSV(cfg.TLF_hue_green, saturation, value) );
                //PrettyFillColor(histarr[i],GetColorHSV(cfg.TLF_hue_green, saturation, cfg.TLF_value_green) );
            }
        } else if( cfg.colorScheme == blackWhite){ 
            if(bc < cfg.BW_red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(cfg.BW_red_hex.c_str()));
            } else if(bc < cfg.BW_yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.BW_yellow_hex.c_str()));
            }
            else{ // >= TMath::Log10(30)
                float xxx = cfg.BW_percentile_hardness*(hist->X2Percentile(bc) - cfg.BW_percentile_corner);
                float value = cfg.BW_graymax - (cfg.BW_graymax - cfg.BW_value_green)/(1.0f + exp(2.0*xxx)); 
                PrettyFillColor(histarr[i],GetColorHSV(cfg.BW_hue_green, 0, value) );
            }
        } else if( cfg.colorScheme == trafficLight){
            if(bc < cfg.red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(cfg.red_hex.c_str()));
            } else if(bc < cfg.yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.yellow_hex.c_str()));
            }
            else{ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.green_hex.c_str()));
            }
        } else if( cfg.colorScheme == grayGreen){
            if(bc < cfg.GG_red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(cfg.GG_red_hex.c_str()));
            } else if(bc < cfg.GG_yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.GG_yellow_hex.c_str()));
            }
            else{ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.GG_green_hex.c_str()));
            }
        } else if( cfg.colorScheme == blueberry ){
                float xxx = (bc - hist->Get_Median())/stddev;
                float gray = 1.0f/(1.0f + exp(-2.0*xxx));//CFGTODO
                if(gray > cfg.graymax) gray = cfg.graymax;
                PrettyFillColor(histarr[i],TColor::GetColor( gray, gray, cfg.blueness) );
        } else if(cfg.colorScheme == LUT1){
            //float bc_middle = hist->Get_Median();
            float bc_middle; 
            if(cfg.LUT1_uses_harmean) bc_middle = TMath::Log10(hist->Get_HarmonicMean());
            else bc_middle = hist->Get_Median();
            i_middle = hist->hist->FindBin(bc_middle);
            //std::cout<<"bini="<<i+1<<" ["<<hist->hist->GetXaxis()->GetBinLowEdge(i+1)<<"-"<<
            //    hist->hist->GetXaxis()->GetBinUpEdge(i+1)<<
            //    "] LUT1: use harmean = "<<cfg.LUT1_uses_harmean<<" harmean/bc_middle: "<<bc_middle <<" i_middle "<<i_middle<<std::endl;//DEBUG
            if(bc < cfg.LUT1_red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(cfg.LUT1_red_hex.c_str()));
            } else if(bc < cfg.LUT1_yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.LUT1_yellow_hex.c_str()));
            } else { //Green lut
                int j = std::max(0,std::min(cfg.lut1_len-1, i+1-i_middle));
            //    std::cout<<"green j="<<j<<" di="<<i+1-i_middle<<" r "<<cfg.r[j]<<" b "<<cfg.b[j]<<std::endl;//DEBUG
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.r[j],cfg.g[j],cfg.b[j]));
            } 

        } else if(cfg.colorScheme == LUT2){ 
            float bc_middle; 
            if(cfg.LUT2_uses_harmean) bc_middle = TMath::Log10(hist->Get_HarmonicMean());
            else bc_middle = hist->Get_Median();
            i_middle = hist->hist->FindBin(bc_middle);
            if(bc < cfg.LUT2_red_end){ 
                PrettyFillColor(histarr[i],TColor::GetColor(cfg.LUT2_red_hex.c_str()));
            } else if(bc < cfg.LUT2_yellow_end){ 
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.LUT2_yellow_hex.c_str()));
            } else { //Green lut
                int j = std::max(0,std::min(cfg.lut2_len-1, i+1-i_middle));
                PrettyFillColor(histarr[i], TColor::GetColor(cfg.LUT2_r[j],cfg.LUT2_g[j],cfg.LUT2_b[j]));
            } 
        }

        //hue is on 0..360, mod 360
        /*hue = hue_red + (cfg.TLF_hue_green - hue_red)*sigmoid(hue_transition_hardness*bc,hue_func);//sigmoid(0.36*bc,S_tanh)
        if(bc < cfg.red_end){
            hue = hue_red;
        } else if(bc <  cfg.yellow_end ){
            hue = hue_yellow;
        }
        else{ 
            hue = cfg.TLF_hue_green;
        }

        saturation = std::min(1.0,
                satur_at_center + (satur_green - satur_at_center )*sigmoid(
                    satur_transition_hardness*(hist->X2Percentile(bc) - satur_center_percential),cfg.satur_func )
                    
                );

        float value_delta = 0.5*(value_red - cfg.TLF_value_green ); 
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
    TCanvas * canv =new TCanvas( newcanvname.c_str(), superfluousTitle.c_str(), cfg.sizePixelsX, cfg.sizePixelsY);
    PrettyCanvas(canv);
    canv->cd();

    //Arrow
	PrettyHist(hist->hist,BkgColor,0);

    if(cfg.ymax_setting > auto_fit_each_histogram) 
        SetRange(hist->hist,0,cfg.histogram_ymax);
	canv->cd();
	gStyle->SetOptStat(0);
	hist->hist->Draw();
    for(int i=0;i<nbins;i++){
        histarr[i]->Draw("same");
    }

    if(cfg.use_sizes){

        TLegend* leg;
        if(hist->Get_HarmonicMean() < cfg.Legend_HMSideSwitchThresh){ 
            leg =  new TLegend(
                    cfg.Legend_lowX1,
                    cfg.Legend_Y1,
                    cfg.Legend_lowX2,
                    cfg.Legend_Y2);
        } else {
            leg =  new TLegend(
                    cfg.Legend_highX1,
                    cfg.Legend_Y1,
                    cfg.Legend_highX2,
                    cfg.Legend_Y2);
        }
        PrettyLegend(leg,cfg.Legend_fontSize);

        //Format main histogram for legend 
        PrettyMarker(hist->hist,cfg.Legend_colorAll, cfg.Legend_markerSize,0);    
        leg->AddEntry(hist->hist,cfg.Legend_entryTextAll.c_str());

        //Large size histogram
        PrettyHist(  hist->histBySize[Lg],cfg.Legend_histSize, cfg.Legend_histSize,0);
        PrettyMarker(hist->histBySize[Lg],cfg.Legend_histSize, cfg.Legend_markerSize,0);
        /*TH1F* Aoutline    = hist->histBySize[Lg].Clone( (((string)hist->histBySize[Lg]) + "outline").c_str()  );
        TH1F* Alegendfill = hist->histBySize[Lg].Clone( (((string)hist->histBySize[Lg]) + "legendfill").c_str()  );
        PrettyHist(Aoutline, kBlack, cfg.Legend_histSize,0);
        PrettyHist(Alegendfill, kBlack, cfg.Legend_histSize,0);*/
        PrettyFillColor(hist->histBySize[Lg], cfg.Legend_colorLg);
        hist->histBySize[Lg]->SetFillStyle(cfg.Legend_fillStyleLg);//Experimental
        hist->histBySize[Lg]->Draw(cfg.Legend_sizeHistDrawOption.c_str());

        //Small size histogram
        PrettyHist(  hist->histBySize[Sm],cfg.Legend_colorSm, cfg.Legend_histSize,0);
        PrettyMarker(hist->histBySize[Sm],cfg.Legend_colorSm, cfg.Legend_markerSize,0);
        //PrettyFillColor(hist->histBySize[Sm], cfg.Legend_colorSm);//Experimental
        //hist->histBySize[Lg]->SetFillStyle(cfg.Legend_fillStyleSm);//Experimental
        hist->histBySize[Sm]->Draw(cfg.Legend_sizeHistDrawOption.c_str());

        //IQR size histograms
        PrettyHist(  hist->histBySize[IQR],cfg.Legend_colorIQR, cfg.Legend_histSize,0);
        PrettyMarker(hist->histBySize[IQR],cfg.Legend_colorIQR, cfg.Legend_markerSize,0);
        hist->histBySize[IQR]->SetFillStyle(cfg.Legend_fillStyleIQR);//Experimental
        hist->histBySize[IQR]->Draw(cfg.Legend_sizeHistDrawOption.c_str());

        //Draw the legend.
        leg->AddEntry(hist->histBySize[Lg],cfg.Legend_entryTextLg.c_str() );
        leg->AddEntry(hist->histBySize[IQR],cfg.Legend_entryTextSm.c_str() );
        leg->AddEntry(hist->histBySize[Sm],cfg.Legend_entryTextSm.c_str() );
        leg->Draw("same");
        gPad->RedrawAxis();
    }

    //CODE TO SHOW THE ARROW and "HM"
    Double_t arrowX = std::max(0.0,TMath::Log10(hist->Get_HarmonicMean()));
    Double_t ytop = hist->hist->GetMaximum()/0.95; //Top of the y-axis. TH1->GetYaxis()->GetXmax() incorrectly returns 1 //CFGTODO
    Double_t ymin = ytop*cfg.Arrow_ymin;
    //std::cout<<"ymax_init "<< ymax<<std::endl; //comes out 1 every time F*CK
    //Double_t ymax *= ytop*0.886;
    Double_t arrow_length = ytop*cfg.Arrow_length;
    TArrow* arrow = new TArrow(arrowX, ymin + arrow_length, arrowX, ymin, cfg.Arrow_size, cfg.Arrow_tipStyle.c_str());//x1,y1 and x2,y2 the Arrow_size in regular coordinates

    //HM
    Double_t textX = -0.0644+((arrowX+0.7435)/6.9749); //Should work when hist->hist->GetXaxis()->GetXmax() = 6 //CFGTODO
    Double_t text_ymin_ndc = 0.383817;// 0.865145; //CFGTODO

    Double_t text_dy_NDC = cfg.ArrowText_upperNDC-cfg.ArrowText_lowerNDC;		
    //TODO: chamge this to a simple setting, and make text_ymin_ndc to a param.
    TPaveText *arrpt = new TPaveText(textX + cfg.ArrowText_leftOffset 		,text_ymin_ndc,textX+cfg.ArrowText_rightOffset,text_ymin_ndc + text_dy_NDC,"NDC");
    if(cfg.Arrow_useHarmMean){
        arrow->SetLineWidth(cfg.Arrow_linewidth);
        arrow->SetFillColor(cfg.Arrow_color);
        arrow->SetLineColor(cfg.Arrow_color);
        arrow->Draw();

        PrettyPaveText(arrpt);
        TText *apt_LaTex = arrpt->AddText(cfg.ArrowText_label.c_str());
        arrpt->Draw();
    } 

    TPaveText *pt = new TPaveText(cfg.HistTitle_X1, cfg.HistTitle_Y1, cfg.HistTitle_X2, cfg.HistTitle_Y2,"NDC");

    PrettyPaveText(pt);
    TText *pt_LaTex = pt->AddText(hist->GetTitle().c_str());
    pt->Draw();

    //Display timestamp
    TPaveText *timestamp1= new TPaveText( 
            cfg.Contributers_X1,
            cfg.Contributers_Y1,
            cfg.Contributers_X2,
            cfg.Contributers_Y2,
            "NDC");
   // timestamp1->AddText("TIL score distribution as of ");
    timestamp1->AddText((cfg.Contributers_prefixText + getCurrentDateTime(cfg) + cfg.Contributers_suffixText).c_str());
    PrettyPaveText(timestamp1);
    timestamp1->SetTextAlign(cfg.Contributers_textAlignment);
    timestamp1->Draw();
    canv->Update();

    //Display #Contributers and Samples
    TPaveText *nc = new TPaveText( 0.750000, 0.936722, 0.994572, 0.997925,"NDC");//0.857057//CFGTODO
    PrettyPaveText(nc);
    nc->SetTextAlign(12); //CFGTODO
    nc->AddText("M41 Mode Exercise Samples:");//CFGTODO
    nc->AddText("Adversarial Contributor Count:");//CFGTODO
    nc->Draw();
    TPaveText *sc = new TPaveText( 0.936068, 0.936722, 0.974668, 0.997925,"NDC"); //CFGTODO
    PrettyPaveText(sc);
    sc->SetTextAlign(32); //CFGTODO
    sc->AddText(std::to_string( static_cast<int>(hist->hist->Integral())).c_str());
    sc->AddText(std::to_string(hist->unique_testers.size() ).c_str());
    sc->Draw();

    TPaveText *lowCountWarning = new TPaveText( 0.700000, 0.800000, 0.95, 0.999,"NDC"); //( 0.300000, 0.100000, 0.800000, 0.997925,"NDC");
                                                                                        //CFGTODO
    PrettyPaveText(lowCountWarning);
    lowCountWarning->SetTextAlign(12); //CFGTODO
    lowCountWarning->SetTextColor(kGray+1);//CFGTODO
    bool apply_lowTester_prefix = false;
    if (hist->unique_testers.size() < 4){//CFGTODO
        apply_lowTester_prefix = true;
        lowCountWarning->AddText("Very Low Adversarial Contributor Count");//CFGTODO
    } 
    lowCountWarning->Draw();

    gPad->RedrawAxis();
	//leg->Draw("same");
    string fname;
    if(cfg.save_with_HMFF_prefix){
        std::ostringstream prefix;
        if( apply_lowTester_prefix){
            size_t len_prefix = cfg.low_contrib_count_prefix.length();
            prefix << cfg.low_contrib_count_prefix << std::fixed << std::setw(7-len_prefix) << std::setfill('0') << static_cast<int>(hist->Get_HarmonicMean()*10);
        }else{
            prefix << std::fixed << std::setw(7) << std::setfill('0') << static_cast<int>(hist->Get_HarmonicMean()*10);
        }

        fname = "plots/"+prefix.str()+"_"+fname_noext + ".png";//CFGTODO
    } else{
        if( apply_lowTester_prefix)
            fname = "plots/"+cfg.low_contrib_count_prefix+fname_noext + ".png";
        else
            fname = "plots/"+fname_noext + ".png";
    }
	canv->SaveAs(fname.c_str());

    if(not (cfg.single_plot_mode_enabled and cfg.X11_persistence)){ //This memory leaks, but who cares.
        for(int i=0;i<nbins;i++){
            delete histarr[i];
        }
    }
} //PlotAndSave

/*********************************************************************************************************************/

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

bool isALlWhiteSpace(const std::string& str) {
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            return false;
        }
    }
    return true;
}

std::string getCurrentDateTime(const Settings& cfg) {
    // Get the current time point
    auto now = std::chrono::system_clock::now();

    // Convert the time point to a time_t object
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Convert the time_t object to a tm struct
    std::tm* tm_now = std::localtime(&now_time);

    // Create a string stream to build the formatted date-time string
    std::stringstream ss;

    // Set the format flags for the month to display 3-letter month name
    ss << std::put_time(tm_now, cfg.Contributers_timeFormat.c_str() );
    //ss << std::put_time(tm_now, "%Y-%m-%d %H:%M");//CFGTODO

    //ss << std::put_time(tm_now, "%Y-%b-%d %H:%M");

    // Return the formatted date-time string
    return ss.str();
}

sizeCode Str2size(std::string s){
    if(s == "A") return sizeCode::Lg;//CFGTODO
    else if(s == "B") return sizeCode::Sm;//CFGTODO
    else if(s == "IQR") return sizeCode::IQR;//CFGTODO
    else return sizeCode::NOSIZE;
}

float dateTimeStr_to_J2000(std::string dateTimeStr){
    /*
     * Empty dates yield -1.0
     * expect dateTimeStr in any of the following formats 
     * YYYY      //Defaults to Jan 1, 00:00
     * YYYY_MM 
     * YYYY_MM_DD 
     * YYYY_MM_DDTHH 
     * YYYY_MM_DDTHH:mm
     * YYYY_MM_DDTHH:mm:ss.ssss
     * 0123456789012345678 index
     * 1234567890123456789 len
     * _,T,:, etc are ignored and can be anything. Interpretation is positional only
     * JD = 367Y - INT(7(Y + INT((M + 9)/12))/4) - INT(3(INT((7*Y + (M - 9))/700) + 1)/4) + INT(275M/9) + D + 1721028.5 + TIME/24
     * where all ints can be ignored as long as the division is truncating.
     * J2000 = JD - 2451545.0
     */
    size_t len; 
    int Y,M = 1, D=1; //Year, month (default is January), day (default is 1st)
    float H = 0.f, m=0.f, s=0.f;//hour (0-23), min, sec
    static const float def = -1.f;
    if((len= dateTimeStr.size()) < 4) return def;
    try{
        Y = std::stoi(dateTimeStr.substr(0,4));
        if(len >= 7){  M = std::stoi(dateTimeStr.substr( 5,2));
        if(len >= 10){ D = std::stoi(dateTimeStr.substr( 8,2));
        if(len >= 13){ H = std::stof(dateTimeStr.substr(11,2));
        if(len >= 16){ m = std::stof(dateTimeStr.substr(14,2));
        if(len >= 19){ s = std::stof(dateTimeStr.substr(17)); }
        }}}}
    } 
    catch(const std::invalid_argument){ return def; }
    catch(const std::out_of_range){ return def; }

    return float(367*Y - 
            (7*(Y + ((M + 9)/12))/4) - 
            (3*(((7*Y + M - 9)/700) + 1)/4) + 
            (275*M/9) + D)
            -730516.5f + (H/24.f) + (m/1440.f) + (s/86400.f);
}//end dateTimeStr_to_J2000
int dateStr_to_J2000(std::string dateStr){
    /*
     * Due to integer restriction, this will produce a date 0.5 higher than the equivalent date from dateTimeStr.
     *
     * Empty dates yield -1
     * expect dateStr in any of the following formats 
     * YYYY      //Defaults to Jan 1, 00:00
     * YYYY_MM 
     * YYYY_MM_DD 
     * 0123456789 index
     * 1234567890 len
     * _,T,:, etc are ignored and can be anything. Interpretation is positional only
     * JD = 367Y - INT(7(Y + INT((M + 9)/12))/4) - INT(3(INT((7*Y + (M - 9))/700) + 1)/4) + INT(275M/9) + D + 1721028.5 + TIME/24
     * where all ints can be ignored as long as the division is truncating.
     * J2000 = JD - 2451545.0
     */
    size_t len; 
    int Y,M = 1, D=1; //Year, month (default is January), day (default is 1st)
    static const int def = -1;
    if((len= dateStr.size()) < 4) return def;
    try{
        Y = std::stoi(dateStr.substr(0,4));
        if(len >= 7){  M = std::stoi(dateStr.substr( 5,2));
        if(len >= 10){ D = std::stoi(dateStr.substr( 8,2)); }
        }
    } 
    catch(const std::invalid_argument){ return def; }
    catch(const std::out_of_range){ return def; }

    return 367*Y - 
            (7*(Y + ((M + 9)/12))/4) - 
            (3*(((7*Y + M - 9)/700) + 1)/4) + 
            (275*M/9) + D -730516; 
}//end dateStr_to_J2000
