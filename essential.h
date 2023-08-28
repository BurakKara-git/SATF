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

// CONSTANTS

#ifndef consth
#define consth

const std::string hist_path_h = std::string(std::filesystem::current_path()) + "/outputs/data/data_hist_result.txt";
const std::string data_path_h = std::string(std::filesystem::current_path()) + "/data/";
const std::string default_data_format_path_h = std::string(std::filesystem::current_path()) + "/DefaultFormat.txt";
const std::string custom_data_format_path_h = std::string(std::filesystem::current_path()) + "/CustomFormat.txt";
const std::string head_data_format_h = "Entries,FallMean,RiseMean,IntegralMean,PeakVoltMean,PeakTimeMean,FallStd,RiseStd,IntegralStd,PeakVoltStd,PeakTimeStd,";
const double sampling_time_h = 2.5e-9;
const int filter_size_h = 1000;
#endif