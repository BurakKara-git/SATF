// COLORS
#ifndef COLORS
#define COLORS

#define RESET "\033[0m"
#define WHITE "\033[1;29m"
#define RED "\033[31m"
#define BOLDORANGE "\033[1;38;5;202m"
#define ORANGE "\033[38;5;202m"
#define BLUE "\033[34m"
#define YELLOW "\033[33m"
#define BOLDYELLOW "\033[1;33m"
#define GREEN "\033[32m"

#endif

// BASIC LIBRARIES
#ifndef basich
#define basich

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unistd.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <signal.h>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <filesystem>

#endif

// ROOT HEADERS
#ifndef rooth
#define rooth

#include "TF1.h"
#include "TChain.h"
#include "TError.h"
#include "TDirectory.h"
#include "TFile.h"
#include "TTree.h"
#include <TMath.h>
#include "TString.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "TApplication.h"
#include "TBrowser.h"
#include "TROOT.h"
#include "TH1F.h"
#include "TFrame.h"
#include "TSystem.h"
#include "TStyle.h"

#endif

#ifndef h5header
#define h5header

#include <H5Cpp.h>

#endif

// CONSTANTS
#ifndef consth
#define consth

const std::string hist_path_h = std::string(std::filesystem::current_path()) + "/outputs/results/root_hist_result.csv"; // Location of Default Histogram
const std::string output_hist_path_h = std::string(std::filesystem::current_path()) + "/outputs/results/";              // Location of Histogram Results
const std::string data_path_h = std::string(std::filesystem::current_path()) + "/data/";
const std::string root_path_h = std::string(std::filesystem::current_path()) + "/outputs/root/"; // Location of Data    
const std::string default_data_format_h = "Date,Source,Scintillator,Segment,Amp,Threshold,SiPM,PMT,MSps,Sample,Trial";
const std::string browser_h = "root -l --web=server:8899 $ROOTSYS/tutorials/v7/browser.cxx";                // Browser Command
const std::string compare_root_path_h = std::string(std::filesystem::current_path()) + "/outputs/compare/"; // Location of Compare Results (functions - custom_compare, standard_compare)
const std::vector<std::string> extensions_h = {
    ".txt",
    ".h5",
}; // Default Extensions (main)

#endif