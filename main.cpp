#include "essential.h"
#include "TF1.h"
using namespace std;
namespace fs = std::filesystem;

// This will prepare the data file for the program to read, if not, will return a nullptr
ifstream *open_data_file(string filename){
    ifstream *file = new ifstream;
    file->open(filename);
    if (file->is_open()) {
        return file; 
    }else{
        delete file; 
        return nullptr; 
    }
}

vector<vector<double> > reader(ifstream &thefile) {
    string line;
    vector<vector<string> > matrix;
    vector<vector<double> > empty;
    int n_rows = 0;
    int n_columns = 0;

    while (getline(thefile, line)) {
        istringstream ss(line);
        string element;
        vector<string> row;

        while (ss >> element) {
            row.push_back(element);
        }
        matrix.push_back(row);
        n_rows++;
        if (n_columns == 0) {
            n_columns = row.size();
        }
    }

    // Transpose the matrix:
    vector<vector<double> > transpose(n_columns, vector<double>(n_rows));
    for (int i = 0; i < n_columns; i++){
        for (int j = 0; j < n_rows; j++){
            transpose[i][j] = stof(matrix[j][i]); // convert string to float values
        }
    }

    cout << "\n" " " "# of rows      :  " <<  n_rows << "\n"
         <<      " " "# of columns   :  " <<  n_columns << "\n" << endl;

    return transpose;
}

void graph(vector<vector<double> > input , string output_path, string outputname) {

    int no_of_datasets = int(input.size());
    vector<double> peak_voltages(no_of_datasets);
    vector<double> peak_time;
    vector<double> integrals;
    vector<double> risetime;
    vector<double> falltime;

    int no_of_datas = input[0].size();
    vector<double> x_axis(no_of_datas); // time
    for (int i = 0; i < no_of_datas; i++) {
        x_axis[i] = i * 2.5e-9;  // 2.5 ns is the sampling time
    }

    for (int j = 0; j < no_of_datasets; j++) {        
        double x_max = no_of_datas * 2.5e-9;        
        vector<double> y_axis = input[j]; 
        auto it = min_element(y_axis.begin(), y_axis.end());
        peak_voltages[j] = *it; // find the peak y value
        int min_index = distance(y_axis.begin(), it); 
        peak_time.push_back(x_axis[min_index]);

        string canvasname = "c" + to_string(j);
        string canvas = "Graph_" + to_string(j+1);
        TCanvas *c1 = new TCanvas(canvasname.c_str(), canvas.c_str(), 200, 10, 600, 400);
        TGraph  *graph = new TGraph(no_of_datas, &x_axis[0], &y_axis[0]);
        c1->SetGrid();
        c1->Draw();

	// Implementation of Burak: 

        //Landau Fit:
        float min_value = *it;
        float scale = min_value;
        float mu = min_index * 2.5e-9; //Location Parameter
        float sigma = 1 * 2.5e-9; //Scale Parameter of Fit
        TF1 *fitFcn = new TF1("fitFcn","[0]*TMath::Landau(x,[1],[2])", 0, x_max);
        fitFcn->SetParameters(scale,mu,sigma);
        fitFcn->SetRange(0,x_max);
        graph->Fit("fitFcn","wRQ");

        //Calculate Rise-Fall Time:
        double min_value_fit = fitFcn->GetMinimum();
        double min_time_fit = fitFcn->GetParameter(1);
        double r1 = fitFcn->GetX(min_value_fit*0.2,0,min_time_fit);
        double r2 = fitFcn->GetX(min_value_fit*0.8,0,min_time_fit);
        double l1 = fitFcn->GetX(min_value_fit*0.8,min_time_fit,x_max);
        double l2 = fitFcn->GetX(min_value_fit*0.2,min_time_fit,x_max);
        risetime.push_back(r2-r1);
        falltime.push_back(l2-l1);

        //Graph Design:
        graph->SetTitle(Form("Voltage vs. Time  (No. %d) ", j + 1));
        graph->GetXaxis()->SetTitle("Time (s)");
        graph->GetYaxis()->SetTitle("Voltage (V)");
        graph->SetMarkerStyle(8);
        graph->SetMarkerColor(kBlue);
        graph->SetMarkerSize(0.7);
        graph->SetLineColor(kBlue);
        graph->SetLineWidth(3);
        graph->Draw("A*");
        gStyle->SetOptFit(1);

        //Save Graph as Root and PDF File:
        gErrorIgnoreLevel = kWarning;
        string root_path = output_path + "/root";
        string pdf_path = output_path + "/pdf";
        fs::create_directories(root_path.c_str());
        fs::create_directories(pdf_path.c_str());
        string output_pdf = output_path + "/pdf/graph_" + to_string(j+1) + ".pdf";
        string output_root = output_path + "/root/graph_" + to_string(j+1) + ".root";
        c1-> SaveAs(output_pdf.c_str());
        c1-> SaveAs(output_root.c_str());

        //Calculate Integral of the Fall:
        double epsrel = 1.0e-20;
        integrals.push_back(-1 * fitFcn->Integral(l1, x_max, epsrel));
        cout << j+1 << "/" << no_of_datasets << "\r";
        cout.flush();
    }

    //Print Results
    string output_txt = output_path + "result.txt";
    ofstream Out_txt(output_txt.c_str());

    Out_txt << "Graph,Fall-Integral,Peaks(V),Time(s),RiseTime(s),FallTime(s)";    
    for(int i = 0; i < no_of_datasets; i++){
    Out_txt << "\n" << integrals[i] << "," << peak_voltages[i] << "," << peak_time[i] << "," << risetime[i] << "," << falltime[i];
    }
    Out_txt.close();

    //Create Histograms
    string h_fall_title = outputname + "- Fall Time";
    string h_rise_title = outputname + "- Rise Time";
    string h_integral_title = outputname + " - Fall Integral";
    string h_peak_volt_title = outputname + " - Peak Voltage";
    string h_peak_time_title = outputname + " - Peak Time";

    TH1* h_fall = new TH1D("h_fall", h_fall_title.c_str(), 10, *min_element(falltime.begin(), falltime.end()), *max_element(falltime.begin(), falltime.end()));
    TH1* h_rise = new TH1D("h_rise", h_rise_title.c_str(), 10, *min_element(risetime.begin(), risetime.end()), *max_element(risetime.begin(), risetime.end()));
    TH1* h_integral = new TH1D("h_integral", h_integral_title.c_str(), 10, *min_element(integrals.begin(), integrals.end()), *max_element(integrals.begin(), integrals.end()));
    TH1* h_peak_volt = new TH1D("h_peak_volt", h_peak_volt_title.c_str(), 10, *min_element(peak_voltages.begin(), peak_voltages.end()), *max_element(peak_voltages.begin(), peak_voltages.end()));
    TH1* h_peak_time = new TH1D("h_peak_time", h_peak_time_title.c_str(), 10, *min_element(peak_time.begin(), peak_time.end()), *max_element(peak_time.begin(), peak_time.end()));

    for (int i=0; i<no_of_datasets; i++) h_fall->Fill(falltime[i]);
    for (int i=0; i<no_of_datasets; i++) h_rise->Fill(risetime[i]);
    for (int i=0; i<no_of_datasets; i++) h_integral->Fill(integrals[i]);
    for (int i=0; i<no_of_datasets; i++) h_peak_volt->Fill(peak_voltages[i]);
    for (int i=0; i<no_of_datasets; i++) h_peak_time->Fill(peak_time[i]);

    string histogram_path = output_path + "/histograms";
    fs::create_directories(histogram_path.c_str());
    string hist_name_fall = output_path + "/histograms/fall" + ".pdf";
    string hist_name_rise = output_path + "/histograms/rise" + ".pdf";
    string hist_name_integral = output_path + "/histograms/integral" + ".pdf";
    string hist_name_peak_volt = output_path + "/histograms/peak_volt" + ".pdf";
    string hist_name_peak_time = output_path + "/histograms/peak_time" + ".pdf";
    
    TCanvas *c1 = new TCanvas();
    h_fall->Draw();
    TCanvas *c2 = new TCanvas();
    h_rise->Draw();
    TCanvas *c3 = new TCanvas();
    h_integral->Draw();
    TCanvas *c4 = new TCanvas();
    h_peak_volt->Draw();
    TCanvas *c5 = new TCanvas();
    h_peak_time->Draw();

    c1->Print(hist_name_fall.c_str());
    c2->Print(hist_name_rise.c_str());
    c3->Print(hist_name_integral.c_str());
    c4->Print(hist_name_peak_volt.c_str());
    c5->Print(hist_name_peak_time.c_str());
}


int main(){
    ifstream *datafile;
    vector<vector<double>> output;
    string path_of_output_directory = string(fs::current_path()) + "/outputs_old/";
    fs::create_directory(path_of_output_directory);

    // Check if directory creation was successful
    if (!fs::exists(path_of_output_directory)) {
        cout << RED "ERROR: Failed to create the output directory." RESET << endl;
        return 0;
    }

    cout << BOLDORANGE << "\n"
        << "_____________________________________________"     << "\n"      
        << "                                             "     << "\n"    
        << "    _____      _______            ______     "     << "\n"
        << "   / ____|  /\\|__   __|          |  ____|   "     << "\n"
        << "  | (___   /  \\  | |     ______  | |__      "     << "\n"
        << "   \\___ \\ / /\\ \\ | |    |______| |  __|  "     << "\n"
        << "   ____) / ____ \\| |             | |        "     << "\n"
        << "  |_____/_/    \\_\\_|             |_|       "     << "\n"
        << "                                             "     << "\n"      
        << RESET << ORANGE 
        << "        SAT-Force Analysis Interface         "     << "\n"
        << RESET << BOLDORANGE
        << "_____________________________________________"     << "\n"      
        << RESET << endl;

    cout << YELLOW << "Enter the path of the .txt file:" << RESET << endl;

//For only one data file
    string filename;
    while (true) {
        cout << YELLOW "> " RESET << flush;
        getline(cin, filename);
        string extension = filename.substr(filename.length() - 4);
        if (extension == ".txt") {
            break;
        } else {
            cout << RED "ERROR: Please enter a .txt file." RESET << endl;
        }
    }

/*
    string found_name = string(fs::current_path()) + "/FoundFiles.txt";
    std::ifstream file(found_name);
    std::string filename; 

    while (std::getline(file, filename)){ //This method leads to memory allocation problems
*/

    datafile = open_data_file(filename);
    if (datafile != nullptr) {
        cout << "\n" GREEN "Successfully opened the file." RESET << endl;
        cout << YELLOW << "File name: " << RESET << filename << endl;

        // Skipping the first 24 lines of the oscilloscope data file since it is not what we need
        for (int i = 0; i < 24; ++i) {
            string line;
            getline(*datafile, line);
        }

        output = reader(*datafile);
        string outputname = filename.substr(0, filename.length() - 4);
        unsigned first = filename.find("data/");
        unsigned last = filename.find_last_of("/");
        string file = filename.substr (first,last-first);
        int index = outputname.find_last_of("/");
        outputname = outputname.substr(index + 1);
        string outputpath = string(fs::current_path()) + "/outputs_old/" + file + "/" +  outputname + "/";
        fs::create_directories(outputpath);
        graph(output, outputpath, outputname);
        cout << GREEN <<  "Output is saved to the directory: " << RESET << outputpath << endl;
    }

    
    
    else{
        cout << RED "ERROR: Could not open the file." RESET << endl;
    } 
    return 0;
}