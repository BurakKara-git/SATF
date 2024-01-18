#ifndef functionsh
#define functionsh

#include "essential.h"
#include "utilities.h"
using namespace H5;
using namespace std;
namespace fs = std::filesystem;

/**
 * Reads histogram data from an input file and returns a matrix of strings.
 *
 * This function reads histogram data from the provided input file, where each line represents
 * a row of comma-separated values. It splits each line into separate string elements using the
 * ',' delimiter and stores these elements in a vector. The resulting vector of vectors represents
 * the histogram data in tabular form.
 *
 * @author Burak
 * @param thefile An input file stream containing histogram data in comma-separated format.
 * @return A matrix of strings where each row represents histogram data.
 */
vector<vector<string>> hist_reader(ifstream &thefile)
{
    string line;
    vector<vector<string>> matrix;

    while (getline(thefile, line))
    {
        vector<string> rows;
        rows = splitter(line, ",");
        matrix.push_back(rows);
    }

    return matrix;
}

/**
 * Generate histogram with labels on the x-axis
 *
 * @author Burak
 * @param names
 * @param values
 * @param hist_name
 * @return TCanvas*
 */
TCanvas *hist_label(vector<string> names, vector<double> values, vector<double> values_stds, string hist_name)
{
    // Initialize:
    int nx = values.size();
    TCanvas *c1 = new TCanvas("c1", "c1", 10, 10, 900, 500);
    TH1D *h = new TH1D(hist_name.c_str(), hist_name.c_str(), nx, 0, nx);

    c1->SetGrid();
    c1->SetBottomMargin(0.2);
    h->SetStats(0);
    h->SetFillColor(30);
    h->SetMinimum(0);

    // Fill Histogram:
    for (int i = 0; i < nx; i++)
    {
        vector<string> new_name_vec = splitter(names[i], "_");
        string new_name = concatenate_vec("", new_name_vec, "", "", " ");
        h->Fill(new_name.c_str(), values[i]);
    }

    // Draw Histogram:
    h->Draw("HIST");

    // Draw Standard Deviation Lines:
    for (int i = 0; i < nx; i++)
    {
        vector<vector<double>> drawSet = {
            {i + 0.5, i + 0.5, values[i] - values_stds[i], values[i] + values_stds[i]},    // Vertical
            {i + 0.25, i + 0.75, values[i] - values_stds[i], values[i] - values_stds[i]},  // Low Horizontal
            {i + 0.25, i + 0.75, values[i] + values_stds[i], values[i] + values_stds[i]}}; // High Horizontal

        for (size_t j = 0; j < drawSet.size(); j++)
        {
            TLine *std_line = new TLine();
            std_line->SetX1(drawSet[j][0]);
            std_line->SetX2(drawSet[j][1]);
            std_line->SetY1(drawSet[j][2]);
            std_line->SetY2(drawSet[j][3]);
            std_line->Draw("SAME");
        }
    }

    return c1;
}

/**
 * Generate histogram name depending on its filters and type
 *
 * @author Burak
 * @param filters
 * @param hist_type
 * @return string
 */
string histogram_namer(vector<string> filters, string hist_type)
{
    string histo_name = hist_type;

    for (int i = 0; i < int(filters.size()); i++)
    {
        string filtername = splitter(filters[i], ":")[1];
        histo_name.append("_" + filtername);
    }
    std::replace(histo_name.begin(), histo_name.end(), '/', '-');

    return histo_name;
}

/**
 * Create histograms for certain combinations of data and filters.
 *
 * This function creates histograms based on the provided data and filters. It generates a histogram
 * for each combination of data and filter, where each histogram represents a specific histogram type.
 * The histograms are stored in PDF files with names determined by the histogram type and filter names.
 * The function supports specifying the number of bins for the histograms through bin division.
 *
 * @author Burak
 * @param data A matrix of data values where each row represents a data point and its associated standard deviation.
 * @param filters A vector of filter strings that provide additional information for histogram naming.
 * @param hist_type A string indicating the type of histogram to be created.
 * @param output_path The path where the generated histograms will be saved as PDF files.
 * @param bin_division The division factor used to determine the number of bins in the histograms.
 * @return vector<double>.
 */
vector<double> compare_hist(vector<vector<double>> data, vector<string> filters, string hist_type, string output_path, string bin_division)
{
    string histo_name = histogram_namer(filters, hist_type);
    vector<double> temp_data;
    vector<double> temp_std;
    for (int i = 0; i < int(data.size()); ++i)
    {
        temp_data.push_back(abs(data[i][0]));
    }
    int n = temp_data.size();

    double min_val = *min_element(temp_data.begin(), temp_data.end());
    double max_val = *max_element(temp_data.begin(), temp_data.end());

    int bin_num = 100;
    TCanvas *c_hist = new TCanvas("c1", "c1", 200, 10, 600, 400);
    TH1 *histo = new TH1D(histo_name.c_str(), histo_name.c_str(), bin_num, min_val, max_val);
    for (int i = 0; i < n; ++i)
    {
        histo->Fill(temp_data[i]);
    }

    double std = histo->GetStdDev();
    double mean = histo->GetMean();
    vector<double> values = {n * 1.0, mean, std};
    delete histo;

    // bin_num = int((max_val - min_val) / std);
    bin_num = int(n / stoi(bin_division));

    TH1 *new_histo = new TH1D(histo_name.c_str(), histo_name.c_str(), bin_num, min_val - 2 * std, max_val + 2 * std);
    for (int i = 0; i < n; ++i)
    {
        new_histo->Fill(temp_data[i]);
    }

    new_histo->Draw();

    string histo_file_name = output_path + histo_name + ".pdf";
    c_hist->Print(histo_file_name.c_str());
    delete c_hist;

    return values;
}

/**
 * Perform standard comparisons of histograms for various combinations.
 *
 * This function generates standard comparisons of histograms by considering different combinations.
 * It reads histogram data from a provided file,
 * prompts the user for types and bin division, and generates histograms for selected combinations.
 *
 * @author Burak
 * @param hist_path The path to the histogram result file.
 */
void standard_compare(string hist_path, string operation)
{
    gErrorIgnoreLevel = kFatal; // Verbose Mode
    vector<vector<double>> values;
    vector<string> names;
    string path_name;
    vector<string> all_paths;

    // Find Date of the Source File:
    vector<string> temp_hist_data_source = splitter(hist_path, "/");
    temp_hist_data_source = splitter(temp_hist_data_source.back(), "_");
    string hist_data_source = temp_hist_data_source[0];

    // Read the Histogram Result File:
    vector<vector<string>> hist_output;
    ifstream *hist_file = new ifstream;
    hist_file->open(hist_path.c_str());
    hist_output = hist_reader(*hist_file);

    // Ask Types:
    string option_types;
    vector<string> types;
    string available_types = concatenate_vec("", hist_output[0], "Integral", "Trial", ",");
    cout << BLUE << "Available Types: " << RESET << available_types << endl;
    cout << YELLOW << "Select Types(type1,type2,...)\n>" << RESET;

    if (operation == "fast")
    {
        option_types == "";
    }

    else
    {
        getline(cin, option_types);
    }

    if (option_types == "")
    {
        option_types = concatenate_vec("", hist_output[0], "Integral", "Trial", ",");
    }

    types = splitter(option_types, ",");

    // Find Type Values:
    vector<vector<string>> values_types;
    for (size_t i = 0; i < types.size(); i++)
    {
        int position = find_position(hist_output[0], types[i]);
        vector<string> values_type = compare_available_options(hist_output, position, {});
        string option_values;
        string values_msg = concatenate_vec("", values_type, "", "", ",");
        cout << BLUE << types[i] << " Values (Press ENTER to Select All): " << RESET << values_msg << YELLOW << "\n>" << RESET;

        if (operation == "fast")
        {
            option_values = "";
        }
        else
        {
            getline(cin, option_values);
        }

        if (option_values != "")
        {
            std::replace(option_values.begin(), option_values.end(), ',', '-');
            path_name.append(option_values + "_");
            values_type = splitter(option_values, "-");
        }
        else
        {
            path_name.append(types[i] + "-all_");
        }
        values_types.push_back(values_type);

        string type_msg = concatenate_vec("", values_type, "", "", ",");
        cout << BLUE << types[i] << ": " << RESET << type_msg << endl;
    }

    // Ask Bin Division:
    string option_division;
    cout << YELLOW << "Divide Entries By \n>" << RESET;

    if (operation == "fast")
    {
        option_division = "";
    }

    else
    {
        getline(cin, option_division);
    }

    if (option_division == "" || option_division == "0")
    {
        option_division = "1";
    }

    // Find All Combinations of Type Values:
    vector<string> all_combinations;
    combination(values_types, 0, "", all_combinations, ",");

    // Analyse Combinations
    for (size_t icomb = 0; icomb < all_combinations.size(); icomb++)
    {
        vector<string> combination_str = splitter(all_combinations[icomb], ",");
        vector<string> type_filters;
        vector<int> positions;
        for (size_t itype = 0; itype < types.size(); itype++)
        {
            string type_filter = types[itype] + ":" + combination_str[itype];
            vector<int> points = filter(hist_output, type_filter);
            if (itype == 0)
            {
                positions = points;
            }
            else
            {
                positions = filter_intersector(positions, points);
            }

            type_filters.push_back(type_filter);
        }

        if (positions.size() == 0)
        {
            string error_msg = concatenate_vec("", type_filters, "", "", ",");
            if (print_errors_h)
            {
                cout << RED << "ERROR - NO COMBINATIONS FOR: " << RESET << error_msg << endl;
            }
            continue;
        }

        else
        {
            // Create Directories and Initialize Root File:
            string compare_root_path = compare_root_path_h + hist_data_source + "/" + path_name + "/events/";

            for (size_t ifilter = 0; ifilter < type_filters.size(); ifilter++)
            {
                string path_tail = splitter(type_filters[ifilter], ":").back();
                std::replace(path_tail.begin(), path_tail.end(), '/', '-');
                compare_root_path.append(path_tail + "_");
            }
            compare_root_path.append("/");

            string compare_root_name = compare_root_path + "compare_hist.root";
            fs::create_directories(compare_root_path.c_str());
            TFile *hist_root_file = new TFile(compare_root_name.c_str(), "RECREATE");

            for (int t = 0; t < 5; ++t) // Generate 5 Histograms
            {
                vector<vector<double>> hist_values;
                for (int k = 0; k < int(positions.size()); ++k)
                {
                    int row = positions[k];
                    vector<double> temp_hist_values;
                    double value = stof(hist_output[row][t]);
                    temp_hist_values.push_back(value);
                    hist_values.push_back(temp_hist_values);
                }
                string hist_type = hist_output[0][t];
                vector<string> filters = type_filters;

                // Push Back Results:
                vector<double> value;
                string name;
                value = compare_hist(hist_values, filters, hist_type, compare_root_path, option_division);
                name = histogram_namer(filters, "");
                values.push_back(value);
                names.push_back(name);
            }
            hist_root_file->Write();
            delete hist_root_file;
            cout << GREEN << "RESULT SAVED TO THE DIRECTORY: " << RESET << compare_root_path << endl;
            all_paths.push_back(compare_root_path + "compare_hist.root");
        }
    }

    // Initialize:
    int n = (values.size() + 1) / 5;
    vector<vector<double>> value_groups = {{}, {}, {}, {}, {}};
    vector<vector<double>> value_groups_std = {{}, {}, {}, {}, {}};
    vector<vector<string>> name_groups = {{}, {}, {}, {}, {}};
    vector<string> pdf_names = {"PeakVoltage", "PeakTime", "RiseTime", "FallTime", "Integral"};
    string hist_result_path = "outputs/compare/" + hist_data_source + "/" + path_name + "/";
    // string hist_result_path = concatenate_vec("outputs/compare/", types, "", "", "_");
    string root_result_path = hist_result_path + "result.root";
    all_paths.push_back(root_result_path);
    TFile *hist_result = new TFile(root_result_path.c_str(), "RECREATE");

    // Group Values of Their Types:
    for (int i = 0; i < n; i++)
    {
        value_groups[0].push_back(values[i * 5 + 0][1]);
        value_groups[1].push_back(values[i * 5 + 1][1]);
        value_groups[2].push_back(values[i * 5 + 2][1]);
        value_groups[3].push_back(values[i * 5 + 3][1]);
        value_groups[4].push_back(values[i * 5 + 4][1]);

        value_groups_std[0].push_back(values[i * 5 + 0][2]);
        value_groups_std[1].push_back(values[i * 5 + 1][2]);
        value_groups_std[2].push_back(values[i * 5 + 2][2]);
        value_groups_std[3].push_back(values[i * 5 + 3][2]);
        value_groups_std[4].push_back(values[i * 5 + 4][2]);

        name_groups[0].push_back(names[i * 5 + 0]);
        name_groups[1].push_back(names[i * 5 + 1]);
        name_groups[2].push_back(names[i * 5 + 2]);
        name_groups[3].push_back(names[i * 5 + 3]);
        name_groups[4].push_back(names[i * 5 + 4]);
    }

    // Create Histograms for Each Group:
    for (int i = 0; i < 5; i++)
    {
        string pdf_name = hist_result_path + pdf_names[i] + ".pdf";
        string pdf_name_std = hist_result_path + pdf_names[i] + "_std.pdf";
        hist_label(name_groups[i], value_groups[i], value_groups_std[i], pdf_names[i])->Print(pdf_name.c_str());
        string current_path = fs::current_path();
        cout << GREEN << "COMPARE RESULT (PDF) SAVED TO THE DIRECTORY: " << RESET << pdf_name << endl;
    }
    cout << GREEN << "COMPARE RESULT (ROOT) SAVED TO THE DIRECTORY: " << RESET << root_result_path << endl;

    hist_result->Write();
    delete hist_result;

    // Merge All Root Files
    string hadd_path = "outputs/compare/" + hist_data_source + "/" + path_name + ".root";
    string hadd_input = "outputs/compare/" + hist_data_source + "/" + path_name;
    hadd_creator(hadd_path, hadd_input);
}

void compare()
{
    cout << BOLDORANGE << "______________________COMPARE______________________" << RESET << endl;
    std::string option_path;
    std::string option_type;

    // Ask Histogram Path:
    while (true)
    {
        cout << YELLOW << "Histogram Path (For Default Press Enter):\n>" << RESET;
        getline(cin, option_path);

        // Default Path:
        if (option_path == "")
        {
            option_path = hist_path_h;
        }

        if (std::filesystem::exists(option_path))
        {
            cout << GREEN << "OPENED FILE: " + option_path << RESET << endl;
            break;
        }

        else
        {
            cout << RED << "FILE NOT FOUND: " + option_path << RESET << endl;
            return;
        }
    }

    standard_compare(option_path, "");
}

void writer(FileFormalism File_Obj, vector<vector<double>> data)
{
    string root_path = string(fs::current_path()) + "/outputs/root/" + File_Obj.date + "/";
    string root_name = root_path + File_Obj.name + ".root";

    // Create new ROOT File:
    fs::create_directories(root_path.c_str());
    // Initialize Rootfile and Tree:
    TFile *rootFile = TFile::Open(root_name.c_str(), "RECREATE");
    TTree *tree = new TTree("tree", "Signal Tree");
    tree->SetEntries(data.size());

    // Loop Data Points
    for (size_t i = 0; i < data.size(); i++)
    {
        // Initialize Segments:
        string segment = "seg" + to_string(i + 1);
        double signal;
        TBranch *branch = tree->Branch(segment.c_str(), &signal, (segment + "/D").c_str());

        // Fill Segments:
        for (size_t j = 0; j < data[i].size(); j++)
        {
            signal = data[i][j];
            branch->Fill();
        }
    }

    // Write and Close Files:
    rootFile->Write();
    rootFile->Close();
}

void reader()
{
    cout << BOLDORANGE << "______________________READ______________________" << RESET << endl;
    std::string data_path;
    vector<std::string> found_names;

    // Ask Data Folder:
    while (true)
    {
        cout << YELLOW << "Data Folder Path (For Default press ENTER)\n>" << RESET;
        getline(cin, data_path);

        if (data_path == "")
        {
            data_path = data_path_h;
        }

        if (std::filesystem::exists(data_path))
        {
            for (size_t i = 0; i < extensions_h.size(); i++)
            {
                vector<std::string> temp_found_names;
                temp_found_names = file_selector(data_path, extensions_h[i]);
                found_names.insert(found_names.end(), temp_found_names.begin(), temp_found_names.end());
            }

            cout << GREEN << "  FOUND " << found_names.size() << " FILES." << RESET << endl;
            break;
        }

        else
        {
            cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
        }
    }

    // Loop Files:
    for (size_t i = 0; i < found_names.size(); i++)
    {
        FileFormalism File_Obj = file_formaliser(found_names[i]);
        string root_path = string(fs::current_path()) + "/outputs/root/" + File_Obj.date + "/";
        string root_name = root_path + File_Obj.name + ".root";
        // If Exists, Pass:
        if (std::filesystem::exists(root_name))
        {
            cout << GREEN << "ALREADY READ: " << RESET << root_name << endl;
        }

        else
        {
            // Read txt Files:
            if (splitter(found_names[i], ".").back() == "txt")
            {
                vector<vector<double>> output;
                ifstream *datafile = new ifstream;
                datafile->open(found_names[i].c_str());
                for (int i = 0; i < 24; ++i) // Skip 24 Lines, not containing data
                {
                    std::string line;
                    getline(*datafile, line);
                }
                output = txt_reader(*datafile);

                // Write the ROOT file:
                writer(File_Obj, output);
                delete datafile;
            }

            // Read h5 Files:
            else
            {
                vector<vector<double>> output;
                output = h5_reader(found_names[i]);

                // Write the ROOT file:
                writer(File_Obj, output);
            }

            cout << GREEN << "FILE SAVED TO: " << RESET << root_name << endl;
        }
    }
}

vector<double> analyser(vector<double> data, double ns, int segment, vector<TF1*> fitFcns, TGraph *graph, FileFormalism File_obj)
{
    // Verbose Mode:
    gErrorIgnoreLevel = kFatal;

    // Peak Voltage, Peak Time, Rise Time, Fall Time, Integral:
    vector<double> result;

    // Get Data Size:
    int no_of_datas = data.size();

    // Find Max x-axis Value:
    // double x_max = no_of_datas * ns;

    // Initialize x-axis and Fill Graph:
    vector<double> x_axis;
    for (size_t t = 0; t < data.size(); t++)
    {
        x_axis.push_back(t * ns);
        graph->SetPoint(t, t * ns, data[t]);
        // graph->SetPointError(t, ns/2, 0.05);
    }

    // Sum y-axis to Check the Peak Position:
    double summation = summation_vec(data, 0);
    double x_max = no_of_datas * ns;

    // Initialize Graph and Fit Function:
    TF1 *fitFcn1 = fitFcns[0];
    TF1 *fitFcn2 = fitFcns[1];
    TF1 *fitFcn3 = fitFcns[2];
    fitFcn1 ->SetLineColor(kRed);
    fitFcn2 ->SetLineColor(kBlue);
    fitFcn3 ->SetLineColor(kGreen);

    // Peak at Positive:
    if (summation > 0)
    {
        // Find Peak Voltage:
        auto it = max_element(data.begin(), data.end());
        result.push_back(*it); // Find the peak y value
        int max_index = distance(data.begin(), it);
        result.push_back(x_axis[max_index]);

        // Set Fit Range
        double x_range_min = findPos(data, data[max_index - 20], 0, max_index) * ns;
        double x_range_max = findPos(data, data[max_index + 20], max_index, no_of_datas) * ns;
        fitFcn1->SetRange(0, x_max);
        fitFcn2->SetRange(0, x_max);
        fitFcn3->SetRange(0, x_max);        

        // Fit:
        float max_value = *it;
        float scale = max_value;   // y-Scale Parameter
        float mu = max_index * ns; // Location Parameter
        float sigma = 1 * ns;      // x-Scale Parameter

        fitFcn1->SetParameters(scale, mu, sigma, abs(scale));
        fitFcn2->SetParameters(scale, mu, sigma);
        fitFcn3->SetParameters(scale, mu, sigma);      
        graph->Fit("fitFcn1", "wRQ");
        //graph->Fit("fitFcn2", "wR+Q");
        //graph->Fit("fitFcn3", "wR+Q");        

        // Calculate Rise-Fall Time:
        double max_value_fit = fitFcn1->GetMaximum();
        double max_time_fit = fitFcn1->GetMaximumX(x_range_min, x_range_max);
        double rise_low = fitFcn1->GetX(max_value_fit * 0.2, x_range_min, max_time_fit);
        double rise_high = fitFcn1->GetX(max_value_fit * 0.8, x_range_min, max_time_fit);
        double fall_high = fitFcn1->GetX(max_value_fit * 0.8, max_time_fit, x_range_max);
        double fall_low = fitFcn1->GetX(max_value_fit * 0.2, max_time_fit, x_range_max);
        result.push_back(rise_high - rise_low); // Rise Time
        result.push_back(fall_low - fall_high); // Fall Time

        // Calculate Integral:
        result.push_back(fitFcn1->Integral(fall_high, x_max));
    }

    else
    {
        // Find Peak Voltage:
        auto it = min_element(data.begin(), data.end());
        result.push_back(*it);
        int min_index = distance(data.begin(), it);
        result.push_back(x_axis[min_index]);

        // Set Fit Range
        double x_range_min = findPos(data, data[min_index - 20], 0, min_index) * ns;
        double x_range_max = findPos(data, data[min_index + 20], min_index, no_of_datas) * ns;
        fitFcn1->SetRange(0, x_max);
        fitFcn2->SetRange(0, x_max);
        fitFcn3->SetRange(0, x_max);

        // Landau Fit:
        float min_value = *it;
        float scale = min_value;   // y-Scale Parameter
        float mu = min_index * ns; // Location Parameter
        float sigma = 1 * ns;      // x-Scale Parameter

        fitFcn1->SetParameters(scale, mu, sigma, abs(scale));
        fitFcn2->SetParameters(scale, mu, sigma);
        fitFcn3->SetParameters(scale, mu, sigma);
        graph->Fit("fitFcn1", "wRQ");
        //graph->Fit("fitFcn2", "wR+Q");
        //graph->Fit("fitFcn3", "wR+Q");

        // Calculate Rise-Fall Time:
        double min_value_fit = fitFcn1->GetMinimum(x_range_min, x_range_max);
        double min_time_fit = fitFcn1->GetMinimumX(x_range_min, x_range_max);
        double rise_low = fitFcn1->GetX(min_value_fit * 0.2, x_range_min, min_time_fit);
        double rise_high = fitFcn1->GetX(min_value_fit * 0.8, x_range_min, min_time_fit);
        double fall_high = fitFcn1->GetX(min_value_fit * 0.8, min_time_fit, x_range_max);
        double fall_low = fitFcn1->GetX(min_value_fit * 0.2, min_time_fit, x_range_max);
        result.push_back(rise_high - rise_low); // Rise Time
        result.push_back(fall_low - fall_high); // Fall Time

        // Calculate Integral:
        double integral = fitFcn1->Integral(fall_high, x_max);
        result.push_back(integral); // double epsrel = 1.0e-20
    }

    if (print_results_h)
    {
        // Graph Design
        auto c1 = new TCanvas("c1", "c1", 200, 10, 600, 400);
        c1->Draw();
        c1->SetGrid();
        string error_title = string(File_obj.path) + " - Voltage vs. Time Segment: ";
        graph->SetTitle(error_title.c_str());
        graph->GetXaxis()->SetTitle("Time (s)");
        graph->GetYaxis()->SetTitle("Voltage (V)");
        graph->SetMarkerStyle(8);
        graph->SetMarkerColor(kBlack);
        graph->SetMarkerSize(0.7);
        graph->SetLineColor(kBlack);
        graph->SetLineWidth(3);
        graph->Draw("A*");

        auto legend = new TLegend(0.1, 0.7, 0.48, 0.9);
        legend->AddEntry(fitFcn1, "Gaussian with Exponential Decay", "L");        
        legend->AddEntry(fitFcn2, "Landau Function", "L");        
        legend->AddEntry(fitFcn3, "Approximate Landau Function", "L");

        legend->Draw("same");
        gStyle->SetOptStat(1);

        // Save Graph as Root and PDF File:
        string pdf_path = output_plot_path_h + File_obj.date;
        fs::create_directories(pdf_path.c_str());
        string output_pdf = pdf_path + "/graph_" + File_obj.name + "_seg" + to_string(segment + 1) + ".root";
        c1->SaveAs(output_pdf.c_str());
        delete c1;
    }
    return result;
}

void analyser_root()
{
    cout << BOLDORANGE << "______________________ROOT ANALYSER______________________" << RESET << endl;
    std::string data_path;
    vector<std::string> found_names;
    TF1 *fitFcn1 = new TF1("fitFcn1", fit_function1_h.c_str()); // Gaussian
    TF1 *fitFcn2 = new TF1("fitFcn2", fit_function2_h.c_str()); // Landau
    TF1 *fitFcn3 = new TF1("fitFcn3", fit_function3_h.c_str()); // Approximate Landau
    vector<TF1 *> fitFcns = {fitFcn1, fitFcn2, fitFcn3};    
    TGraph *graph = new TGraph();

    // Ask Data Folder:
    while (true)
    {
        cout << YELLOW << "Data Folder Path (For Default press ENTER)\n>" << RESET;
        getline(cin, data_path);

        if (data_path == "")
        {
            data_path = root_path_h;
        }

        if (std::filesystem::exists(data_path))
        {
            for (size_t i = 0; i < extensions_h.size(); i++)
            {
                found_names = file_selector(data_path, ".root");
            }

            cout << GREEN << "  FOUND " << found_names.size() << " FILES." << RESET << endl;
            break;
        }

        else
        {
            cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
        }
    }

    // Create Directories:
    string outputname = splitter(data_path, "/").back();
    fs::create_directories(output_hist_path_h);
    string output_hist = output_hist_path_h + outputname + "_results.csv";
    string errors = output_hist_path_h + outputname + "_errors.csv";

    // Get Number of Files:
    int found_names_size = found_names.size();

    // Initialize ofstream:
    std::ofstream out(output_hist.c_str());
    out << "PeakVoltage,PeakTime,RiseTime,FallTime,Integral," + default_data_format_h + ",SegNo"
                                                                                        "\n";

    std::ofstream error(errors.c_str());
    error << "PeakVoltage,PeakTime,RiseTime,FallTime,Integral," + default_data_format_h + ",SegNo"
                                                                                          "\n";
    // Loop All Files:
    for (int file_num = 0; file_num < found_names_size; file_num++)
    {
        cout << GREEN << "ANALYSING: " << file_num + 1 << "/" << found_names_size << RESET << "|" << found_names[file_num] << endl;

        FileFormalism File_Obj = file_formaliser(found_names[file_num]);

        // Open Root File:
        TFile f(found_names[file_num].c_str());
        TTree *tree = f.Get<TTree>("tree");
        int n = tree->GetEntries();

        // Loop Branches:
        for (int i = 0; i < n; i++)
        {
            // Open Branch:
            string segment = "seg" + to_string(i + 1);
            TBranch *branch = tree->GetBranch(segment.c_str());
            double signal;
            branch->SetAddress(&signal);

            // Initialize Vector to Hold the Data:
            vector<double> data;
            int no_of_datas = branch->GetEntries();

            // Fill the Vector:
            for (int i = 0; i < no_of_datas; i++)
            {
                branch->GetEntry(i);
                data.push_back(signal);
            }

            // Analyse Data:
            vector<double> result = analyser(data, 2.5e-9, i, fitFcns, graph, File_Obj);

            // Check nan and 0 values:
            bool passed = true;
            for (size_t result_i = 0; result_i < result.size(); result_i++)
            {
                if (isnan(result[result_i]) || result[result_i] == 0)
                {
                    passed = false;
                    if (print_errors_h)
                    {
                        cout << RED << "ERROR - NAN VALUE AT: " << RESET << found_names[file_num] << "Segment: " << i + 1 << endl;
                    }

                    break;
                }
            }

            if (result[2] <= 0 || result[3] <= 0)
            {
                passed = false;
                if (print_errors_h)
                {
                    cout << RED << "ERROR - NEGATIVE TIME: " << RESET << found_names[file_num] << "Segment: " << i + 1 << endl;
                }
            }

            if (passed)
            {
                // Write Results:
                for (size_t result_i = 0; result_i < result.size(); result_i++)
                {
                    out << result[result_i] << ",";
                }

                // Write File Specs:
                string date = File_Obj.date;
                std::replace(date.begin(), date.end(), '/', '-');
                out << date << "," << File_Obj.source << "," << File_Obj.scintillator << ","
                    << File_Obj.segment << "," << File_Obj.amp << "," << File_Obj.th << ","
                    << File_Obj.SiPM << "," << File_Obj.PMT << "," << File_Obj.MSps << ","
                    << File_Obj.sample << "," << File_Obj.trial << "," << i + 1 << "\n";
            }

            else
            {
                // Write Errors:
                for (size_t result_i = 0; result_i < result.size(); result_i++)
                {
                    error << result[result_i] << ",";
                }

                // Write File Specs:
                string date = File_Obj.date;
                std::replace(date.begin(), date.end(), '/', '-');
                error << date << "," << File_Obj.source << "," << File_Obj.scintillator << ","
                      << File_Obj.segment << "," << File_Obj.amp << "," << File_Obj.th << ","
                      << File_Obj.SiPM << "," << File_Obj.PMT << "," << File_Obj.MSps << ","
                      << File_Obj.sample << "," << File_Obj.trial << "," << i + 1 << "\n";
            }
        }
        delete tree;
    }
}

#endif