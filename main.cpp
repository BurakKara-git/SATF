#include "essential.h"
#include "TF1.h"
#include "TChain.h"
using namespace std;
namespace fs = std::filesystem;
vector<string> results;

vector<string> splitter(string name)
{
    vector<string> name_split;
    char *token = strtok(name.data(), "_");
    while (token != NULL)
    {
        name_split.push_back(token);
        token = strtok(NULL, "_");
    }
    delete token;
    return name_split;
}

vector<vector<double>> reader(ifstream &thefile)
{
    string line;
    vector<vector<string>> matrix;
    vector<vector<double>> empty;
    int n_rows = 0;
    int n_columns = 0;

    while (getline(thefile, line))
    {
        istringstream ss(line);
        string element;
        vector<string> row;

        while (ss >> element)
        {
            row.push_back(element);
        }
        matrix.push_back(row);
        n_rows++;
        if (n_columns == 0)
        {
            n_columns = row.size();
        }
    }

    // Transpose the matrix:
    vector<vector<double>> transpose(n_columns, vector<double>(n_rows));
    for (int i = 0; i < n_columns; i++)
    {
        for (int j = 0; j < n_rows; j++)
        {
            transpose[i][j] = stof(matrix[j][i]); // convert string to float values
        }
    }

    cout << "\n"
            " "
            "# of rows      :  "
         << n_rows << "\n"
         << " "
            "# of columns   :  "
         << n_columns << "\n"
         << endl;

    return transpose;
}

void graph_maker(vector<vector<double>> input, string output_path, string outputname, double ns, string date)
{
    string rootname = output_path + "result.root";
    TFile *rootfile = new TFile(rootname.c_str(), "RECREATE");

    int no_of_datasets = int(input.size());
    vector<double> peak_voltages(no_of_datasets);
    vector<double> peak_time;
    vector<double> integrals;
    vector<double> risetime;
    vector<double> falltime;
    vector<double> hist_result;
    vector<string> name_split;

    int no_of_datas = input[0].size();
    vector<double> x_axis(no_of_datas); // time
    for (int i = 0; i < no_of_datas; i++)
    {
        x_axis[i] = i * ns; // sampling time
    }

    for (int j = 0; j < no_of_datasets; j++)
    {
        double x_max = no_of_datas * ns;
        vector<double> y_axis = input[j];
        auto it = min_element(y_axis.begin(), y_axis.end());
        peak_voltages[j] = *it; // find the peak y value
        int min_index = distance(y_axis.begin(), it);
        peak_time.push_back(x_axis[min_index]);
        TGraph *graph = new TGraph(no_of_datas, &x_axis[0], &y_axis[0]);

        // Implementation of Burak:
        // Landau Fit:
        float min_value = *it;
        float scale = min_value;
        float mu = min_index * ns; // Location Parameter
        float sigma = 1 * ns;      // Scale Parameter of Fit
        TF1 *fitFcn = new TF1("fitFcn", "[0]*TMath::Landau(x,[1],[2])", 0, x_max);
        fitFcn->SetParameters(scale, mu, sigma);
        fitFcn->SetRange(0, x_max);
        graph->Fit("fitFcn", "wRQ");

        // Calculate Rise-Fall Time:
        double min_value_fit = fitFcn->GetMinimum();
        double min_time_fit = fitFcn->GetParameter(1);
        double r1 = fitFcn->GetX(min_value_fit * 0.2, 0, min_time_fit);
        double r2 = fitFcn->GetX(min_value_fit * 0.8, 0, min_time_fit);
        double l1 = fitFcn->GetX(min_value_fit * 0.8, min_time_fit, x_max);
        double l2 = fitFcn->GetX(min_value_fit * 0.2, min_time_fit, x_max);
        risetime.push_back(r2 - r1);
        falltime.push_back(l2 - l1);

        // Calculate Integral of the Fall:
        double epsrel = 1.0e-20;
        integrals.push_back(-1 * fitFcn->Integral(l1, x_max, epsrel));
        cout << j + 1 << "/" << no_of_datasets << "\r";
        cout.flush();
        delete fitFcn;
        delete graph;
    }

    // Create Histograms
    string h_fall_title = date + "_" + outputname + " - Fall Time";
    string h_rise_title = date + "_" + outputname + " - Rise Time";
    string h_integral_title = date + "_" + outputname + " - Fall Integral";
    string h_peak_volt_title = date + "_" + outputname + " - Peak Voltage";
    string h_peak_time_title = date + "_" + outputname + " - Peak Time";

    string fall_histname = "h_fall_" + date + "_" + outputname;
    string rise_histname = "h_rise_" + date + "_" + outputname;
    string integral_histname = "h_integral_" + date + "_" + outputname;
    string peak_volt_histname = "h_peak_volt_" + date + "_" + outputname;
    string peak_time_histname = "h_peak_time_" + date + "_" + outputname;

    TH1 *h_fall = new TH1D(fall_histname.c_str(), h_fall_title.c_str(), 10, *min_element(falltime.begin(), falltime.end()), *max_element(falltime.begin(), falltime.end()));
    TH1 *h_rise = new TH1D(rise_histname.c_str(), h_rise_title.c_str(), 10, *min_element(risetime.begin(), risetime.end()), *max_element(risetime.begin(), risetime.end()));
    TH1 *h_integral = new TH1D(integral_histname.c_str(), h_integral_title.c_str(), 10, *min_element(integrals.begin(), integrals.end()), *max_element(integrals.begin(), integrals.end()));
    TH1 *h_peak_volt = new TH1D(peak_volt_histname.c_str(), h_peak_volt_title.c_str(), 10, *min_element(peak_voltages.begin(), peak_voltages.end()), *max_element(peak_voltages.begin(), peak_voltages.end()));
    TH1 *h_peak_time = new TH1D(peak_time_histname.c_str(), h_peak_time_title.c_str(), 10, *min_element(peak_time.begin(), peak_time.end()), *max_element(peak_time.begin(), peak_time.end()));

    for (int i = 0; i < no_of_datasets; i++)
        h_fall->Fill(falltime[i]);
    for (int i = 0; i < no_of_datasets; i++)
        h_rise->Fill(risetime[i]);
    for (int i = 0; i < no_of_datasets; i++)
        h_integral->Fill(integrals[i]);
    for (int i = 0; i < no_of_datasets; i++)
        h_peak_volt->Fill(peak_voltages[i]);
    for (int i = 0; i < no_of_datasets; i++)
        h_peak_time->Fill(peak_time[i]);

    // Print Results as CSV Format
    hist_result.push_back(h_fall->GetMean());
    hist_result.push_back(h_rise->GetMean());
    hist_result.push_back(h_integral->GetMean());
    hist_result.push_back(h_peak_volt->GetMean());
    hist_result.push_back(h_peak_time->GetMean());
    hist_result.push_back(h_fall->GetStdDev());
    hist_result.push_back(h_rise->GetStdDev());
    hist_result.push_back(h_integral->GetStdDev());
    hist_result.push_back(h_peak_volt->GetStdDev());
    hist_result.push_back(h_peak_time->GetStdDev());

    name_split = splitter(outputname); // Split name

    string result;
    for (int k = 0; k < hist_result.size(); ++k)
    {
        result = result + hist_result[k] + ",";
    }
    result.append(date);
    for (int k = 0; k < name_split.size(); ++k)
    {
        result.append("," + name_split[k]);
    }

    results.push_back(result); // Merge histogram data as a string

    // Write Histograms as a Root File
    rootfile->Write();

    // Print Histogram Results as Txt File
    string output_txt = output_path + "result.txt";
    ofstream Out_txt(output_txt.c_str());

    Out_txt << "Fall-Integral,Peaks(V),Time(s),RiseTime(s),FallTime(s)";
    for (int i = 0; i < no_of_datasets; i++)
    {
        Out_txt << "\n"
                << integrals[i] << "," << peak_voltages[i] << "," << peak_time[i] << "," << risetime[i] << "," << falltime[i];
    }
    Out_txt.close();

    // Free memory
    delete h_fall;
    delete h_rise;
    delete h_integral;
    delete h_peak_volt;
    delete h_peak_time;
    rootfile->Close();
}

string folder_selector(string path) // Find all txt files in the folder
{
    string found_files_path = "temp_FoundFiles.txt";
    string command = "find " + path + " -type f -name '*.txt'> " + found_files_path;
    system(command.c_str());
    return found_files_path;
}

int main()
{
    vector<vector<double>> output;
    int count = 0;

    cout << BOLDORANGE << "\n"
         << "_____________________________________________"
         << "\n"
         << "                                             "
         << "\n"
         << "    _____      _______            ______     "
         << "\n"
         << "   / ____|  /\\|__   __|          |  ____|   "
         << "\n"
         << "  | (___   /  \\  | |     ______  | |__      "
         << "\n"
         << "   \\___ \\ / /\\ \\ | |    |______| |  __|  "
         << "\n"
         << "   ____) / ____ \\| |             | |        "
         << "\n"
         << "  |_____/_/    \\_\\_|             |_|       "
         << "\n"
         << "                                             "
         << "\n"
         << RESET << ORANGE
         << "        SAT-Force Analysis Interface         "
         << "\n"
         << RESET << BOLDORANGE
         << "_____________________________________________"
         << "\n"
         << RESET << endl;

    // Ask For Data Folder, Data Format and Sampling Time
    string found_name;
    string data_path;
    string ns_string;
    string data_format_path;

    cout << "Data Folder Path (For Default press ENTER): ";
    getline(cin, data_path);

    if (data_path == "")
    {
        data_path = "data";
    }

    found_name = folder_selector(data_path);

    cout << "Data Format Path (For Default press ENTER, For Custom '0'): ";
    getline(cin, data_format_path);

    if (data_format_path == "")
    {
        data_format_path = "DefaultFormat.txt";
    }

    if (data_format_path == "0")
    {
        data_format_path = "CustomFormat.txt";
    }

    cout << "Sampling Time(ns) (For Default-2.5ns press ENTER):";
    getline(cin, ns_string);

    double ns;
    if (ns_string == "")
    {
        ns = 2.5e-9;
    }

    else
    {
        ns = 1e-9 * stod(ns_string.c_str());
    }

    // Loop Through All Files
    std::ifstream inputfile(found_name);
    std::string filename;

    while (std::getline(inputfile, filename))
    {
        count += 1;

        ifstream *datafile = new ifstream;
        datafile->open(filename.c_str());
        if (datafile != nullptr)
        {
            cout << "\n" GREEN "Successfully opened the file." RESET << endl;
            cout << YELLOW << "File name: " << RESET << filename << endl;

            // Skipping the first 24 lines of the oscilloscope data file since it is not what we need
            for (int i = 0; i < 24; ++i)
            {
                string line;
                getline(*datafile, line);
            }

            output = reader(*datafile);
            string outputname = filename.substr(0, filename.length() - 4);
            unsigned first = filename.find("data/");
            unsigned last = filename.find_last_of("/");
            string newfile = filename.substr(first, last - first);
            string date = newfile.substr(5);
            int index = outputname.find_last_of("/");
            outputname = outputname.substr(index + 1);
            string outputpath = string(fs::current_path()) + "/outputs/" + newfile + "/" + outputname + "/";
            fs::create_directories(outputpath);
            graph_maker(output, outputpath, outputname, ns, date);
            cout << GREEN << "Output is saved to the directory: " << RESET << outputpath << endl;
        }

        else
        {
            cout << RED "ERROR: Could not open the file." RESET << endl;
        }
        delete datafile;
    }

    // Write Histogram Results
    string output_hist = string(fs::current_path()) + "/outputs/" + data_path + "_hist_result.txt";
    ofstream Out_hist(output_hist.c_str());

    string data_format;
    ifstream data_format_reader(data_format_path);

    Out_hist << "Fall-Mean,Rise-Mean,Integral-Mean,Peak_Volt-Mean,Peak_Time-Mean,Fall-Std,Rise-Std,Integral-Std,Peak_Volt-Std,Peak_Time-Std,";

    while (getline(data_format_reader, data_format))
    {
        Out_hist << data_format;
    }

    for (int k = 0; k < results.size(); ++k)
    {
        Out_hist << "\n"
                 << results[k];
    }
    Out_hist.close();
    data_format_reader.close();

    string root_path = string(fs::current_path()) + "/outputs/" + data_path;
    string hadd_command = "hadd -f outputs/" + data_path + ".root `find " + root_path + " -type f -name '*.root'`";
    system(hadd_command.c_str());

    cout << GREEN << "Histogram Result Saved to The Directory: " << RESET << output_hist << endl;
    cout << GREEN << "Root Result Saved to The Directory: " << RESET << root_path + ".root" << endl;

    remove("temp_FoundFiles.txt");
    gROOT->Reset();
    return 0;
}