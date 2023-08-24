#include "essential.h"
#include <bits/stdc++.h>
#include <H5Cpp.h>
#include <algorithm>
using namespace H5;
using namespace std;
namespace fs = std::filesystem;

vector<vector<double>> reader(ifstream &thefile) // Generate The Matrix - Hazal's Function
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

vector<string> splitter(string name, string DELIMITER) // Split string, Return string vector
{
    vector<string> name_split;
    char *token = strtok(name.data(), DELIMITER.c_str());
    while (token != NULL)
    {
        name_split.push_back(token);
        token = strtok(NULL, DELIMITER.c_str());
    }
    delete token;
    return name_split;
}

string file_selector(string path) // Find all txt files in the folder
{
    string found_files_path = "temp_FoundFiles.txt";
    string command = "find " + path + " -type f -iname \\*.txt -o -type f -iname \\*.h5 > " + found_files_path;
    int systemErr = system(command.c_str());
    if (systemErr == -1)
    {
        cout << RED << "ERROR - COULD NOT FOUND TXT FILES IN THE PATH" << endl;
    }
    return found_files_path;
}

int line_counter(string path) // Count number of lines
{
    unsigned int lines = 0;

    std::ifstream inputfile(path);
    std::string filename;
    while (std::getline(inputfile, filename))
    {
        lines += 1;
    }
    return lines;
}

bool is_number(const std::string &s) // Check if string is a number
{
    char *end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

void interface() // Cool - Hazal
{
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
}

int find_position(vector<string> vec_string, string search_string) // Find First Occurence of a String in a Vector
{
    int position = -1;

    for (int i = 0; i < int(vec_string.size()); ++i)
    {
        if (vec_string[i] == search_string)
        {
            return i;
        }
    }
    return position;
}

double summation_vec(vector<double> input, int first)
{
    int n = input.size();
    double sum = 0;
    for (int i = first; i < n; ++i)
    {
        sum += input[i];
    }
    return sum;
}

vector<vector<string>> hist_reader(ifstream &thefile) // Read Output Histogram Data
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

void compare_hist(vector<vector<double>> data, vector<string> filters, string hist_type, string output_path, string bin_division) // Create Histograms for Certain Combinations
{
    string histo_name = hist_type;

    for (int i = 0; i < int(filters.size()); i++)
    {
        string filtername = splitter(filters[i], ":")[1];
        histo_name.append("_" + filtername);
    }
    std::replace(histo_name.begin(), histo_name.end(), '/', '-');

    vector<double> temp_data;
    vector<double> temp_std;
    for (int i = 0; i < int(data.size()); ++i)
    {
        temp_data.push_back(abs(data[i][0]));
        temp_std.push_back(abs(data[i][1]));
    }

    double min_val = *min_element(temp_data.begin(), temp_data.end());
    double max_val = *max_element(temp_data.begin(), temp_data.end());

    int bin_num = 100;
    TCanvas *c_hist = new TCanvas("c1", "c1", 200, 10, 600, 400);
    TH1 *histo = new TH1D(histo_name.c_str(), histo_name.c_str(), bin_num, min_val, max_val);
    for (int i = 0; i < int(temp_data.size()); ++i)
    {
        histo->Fill(temp_data[i]);
    }

    double std = histo->GetStdDev();
    delete histo;

    // bin_num = int((max_val - min_val) / std);
    bin_num = int(temp_data.size() / stoi(bin_division));

    TH1 *new_histo = new TH1D(histo_name.c_str(), histo_name.c_str(), bin_num, min_val - 2 * std, max_val + 2 * std);
    for (int i = 0; i < int(temp_data.size()); ++i)
    {
        new_histo->Fill(temp_data[i]);
    }

    if (hist_type == "IntegralMean")
    {
        new_histo->SetXTitle("Weber(Vs)");
    }
    else if (hist_type == "PeakVoltMean")
    {
        new_histo->SetXTitle("Voltage(V)");
    }
    else
    {
        new_histo->SetXTitle("Time(s)");
    }

    new_histo->Draw();

    string histo_file_name = output_path + histo_name + ".pdf";
    c_hist->Print(histo_file_name.c_str());

    delete c_hist;
}

vector<int> filter(vector<vector<string>> data, string filter) // Filter for a Given Type and Value
{
    vector<int> positions;
    string filter_type = splitter(filter, ":")[0];
    string filter_value = splitter(filter, ":")[1];
    int filter_type_position = 0;

    for (int i = 0; i < int(data[0].size()); ++i)
    {
        if (data[0][i] == filter_type)
        {
            filter_type_position = i;
        }
    }

    if (filter_type_position >= int(data[1].size()))
    {
        cout << RED << "ERROR - FIRST ROW'S SIZE IS LARGER THAN DATA ROWS" << RESET << endl;
    }

    for (int i = 1; i < int(data.size()); ++i)
    {
        if (data[i][filter_type_position] == filter_value && stoi(data[i][0]) >= 1000)
        {
            positions.push_back(i);
        }
    }

    return positions;
}

string concatenate_vec(string head_string, vector<string> vector_line, string first, string last, string DELIMITER)
{
    int i_first;
    int i_last;
    if (first == "")
    {
        i_first = 0;
    }
    else
    {
        i_first = find_position(vector_line, first) + 1;
    }

    if (last == "")
    {
        i_last = int(vector_line.size());
    }
    else
    {
        i_last = find_position(vector_line, last);
    }

    string new_line = head_string;
    for (int i = i_first; i < i_last; ++i)
    {
        new_line.append(vector_line[i] + DELIMITER);
    }
    return new_line;
}

void hadd_creator(string hadd_path, string input_path) // Generate and run hadd command
{
    string hadd_command = "hadd -f " + hadd_path + " `find " + input_path + " -type f -name '*.root'`";

    if (std::filesystem::exists(hadd_path))
    {
        string option_delete;
        cout << RED << "ERROR: ROOT FILE EXISTS" << RESET << endl;
        cout << "   DELETE and CONTINUE? "
             << "y/n ";
        getline(cin, option_delete);
        if (option_delete == "y" || option_delete == "Y")
        {
            remove(hadd_path.c_str());
        }

        else
        {
            return;
        }
    }

    int systemErr = system(hadd_command.c_str()); // Merge all ROOT Files
    if (systemErr == -1)
    {
        cout << RED << "ERROR - COULD NOT MERGE ROOT FILES" << endl;
    }
}

vector<int> filter_intersector(vector<int> first, vector<int> second) // Intersection of Two Vectors
{
    int n1 = first.size();
    int n2 = second.size();
    vector<int> v(n1 + n2);
    vector<int>::iterator it;
    sort(first.begin(), first.end());
    sort(second.begin(), second.end());

    it = set_intersection(first.begin(), first.end(), second.begin(), second.end(), v.begin());
    v.resize(it - v.begin());
    return v;
}

vector<string> compare_available_options(vector<vector<string>> data, int column, vector<int> positions)
{
    vector<string> v;
    int n;

    if (positions.size() == 0)
    {
        n = int(data.size());
        for (int i = 1; i < n; i++)
        {
            v.push_back(data[i][column]);
        }
    }

    else
    {
        n = int(positions.size());
        for (int i = 0; i < n; i++)
        {
            int row = positions[i];
            v.push_back(data[row][column]);
        }
    }

    sort(v.begin(), v.end());
    vector<string>::iterator it;
    it = unique(v.begin(), v.end());
    v.resize(distance(v.begin(), it));

    return v;
}

void custom_compare(string hist_path) // filter + compare_hist for Custom Combinations
{
    gErrorIgnoreLevel = kFatal; // Verbose Mode

    // Find Date of the Source File:
    vector<string> temp_hist_data_source = splitter(hist_path, "/");
    temp_hist_data_source = splitter(temp_hist_data_source.back(), "_");
    string hist_data_source = temp_hist_data_source[0];

    // Read the Histogram Result File:
    vector<vector<string>> hist_output;
    ifstream *hist_file = new ifstream;
    hist_file->open(hist_path.c_str());
    hist_output = hist_reader(*hist_file);

    // Initialize:
    vector<string> filters;
    string option_division;
    vector<int> filtered_positions;
    int type_size = hist_output[0].size();

    // Ask For Filter Values:
    int count = 0;
    for (int i = 11; i < type_size - 1; i++)
    {
        string option_type;
        vector<string> availables = compare_available_options(hist_output, i, filtered_positions);

        cout << YELLOW << "Options: " << RESET;
        for (int j = 0; j < int(availables.size()); j++)
        {
            cout << availables[j] << ", ";
        }
        cout << "\n";

        cout << YELLOW << hist_output[0][i] << ": " << RESET;
        getline(cin, option_type);

        if (option_type == "")
        {
            continue;
        }

        if (option_type == "c")
        {
            break;
        }

        if (count == 0)
        {
            count += 1;
            filters.push_back(hist_output[0][i] + ":" + option_type);
            filtered_positions = filter(hist_output, filters.back());
        }

        else
        {

            filters.push_back(hist_output[0][i] + ":" + option_type);
            vector<int> temp_filtered_positions = filter(hist_output, filters.back());
            filtered_positions = filter_intersector(filtered_positions, temp_filtered_positions);
        }

        if (filtered_positions.size() == 0)
        {
            cout << ORANGE << "No Combinations For: " << RESET;
            for (int i = 0; i < int(filters.size()); i++)
            {
                cout << filters[i] << ",";
            }
            cout << "\n";
            break;
        }

        else
        {
            cout << GREEN << filtered_positions.size() << " Entries Found" << RESET << endl;
            cout << "   To Compare Press 'c'" << endl;
        }
    }

    if (filtered_positions.size() == 0)
    {
        return;
    }

    else
    {
        cout << GREEN << filtered_positions.size() << " Entries for Combination: " << RESET;
        for (int i = 0; i < int(filters.size()); i++)
        {
            cout << filters[i] << ",";
        }
        cout << "\n";

        cout << "Divide Entries By: ";
        getline(cin, option_division);

        // Create Directories and Initialize Root File:
        string name_filter = "filtered";
        for (int i = 0; i < int(filters.size()); i++)
        {
            string filtername = splitter(filters[i], ":")[1];
            name_filter.append("_" + filtername);
        }
        std::replace(name_filter.begin(), name_filter.end(), '/', '-');
        string compare_root_path = string(fs::current_path()) + "/outputs/compare/" + hist_data_source + "/" + name_filter + "/";
        string compare_root_name = compare_root_path + "compare_hist.root";
        fs::create_directories(compare_root_path.c_str());
        TFile *hist_root_file = new TFile(compare_root_name.c_str(), "RECREATE");

        for (int t = 1; t < 6; ++t) // Generate 5 Histograms
        {
            vector<vector<double>> hist_values;
            for (int k = 0; k < int(filtered_positions.size()); ++k)
            {
                int row = filtered_positions[k];
                vector<double> temp_hist_values;
                double value = stof(hist_output[row][t]);
                double error = stof(hist_output[row][t + 5]);
                temp_hist_values.push_back(value);
                temp_hist_values.push_back(error);
                hist_values.push_back(temp_hist_values);
            }
            string hist_type = hist_output[0][t];
            compare_hist(hist_values, filters, hist_type, compare_root_path, option_division);
        }
        hist_root_file->Write();
        delete hist_root_file;
        cout << GREEN << "RESULT SAVED TO THE DIRECTORY: " << RESET << compare_root_path << endl;
    }

    delete hist_file;
}

void standard_compare(string hist_path) // filter + compare_hist for Scintillator and Source Combinations
{
    gErrorIgnoreLevel = kFatal; // Verbose Mode

    // Find Date of the Source File:
    vector<string> temp_hist_data_source = splitter(hist_path, "/");
    temp_hist_data_source = splitter(temp_hist_data_source.back(), "_");
    string hist_data_source = temp_hist_data_source[0];

    // Read the Histogram Result File:
    vector<vector<string>> hist_output;
    ifstream *hist_file = new ifstream;
    hist_file->open(hist_path.c_str());
    hist_output = hist_reader(*hist_file);

    // Generate Result For All Sources, Scintillators and Thresholds:
    int position_source = find_position(hist_output[0], "Source");
    int position_scintillator = find_position(hist_output[0], "Scintillator");
    int position_th = find_position(hist_output[0], "Threshold");

    vector<string> option_source = compare_available_options(hist_output, position_source, {});
    vector<string> option_scintillator = compare_available_options(hist_output, position_scintillator, {});
    vector<string> option_th = compare_available_options(hist_output, position_th, {});

    // Print All Scintillator and Source Options:
    cout << YELLOW << "Sources: " << RESET;
    for (int i = 0; i < int(option_source.size()); i++)
    {
        cout << option_source[i] << ", ";
    }

    cout << YELLOW << "\nScintillators: " << RESET;
    for (int i = 0; i < int(option_scintillator.size()); i++)
    {
        cout << option_scintillator[i] << ", ";
    }

    cout << YELLOW << "\nThresholds: " << RESET;
    for (int i = 0; i < int(option_th.size()); i++)
    {
        cout << option_th[i] << ", ";
    }
    cout << "\n";

    // Ask for Bin Division:
    string option_division;
    cout << YELLOW << "Divide Entries By: " << RESET;
    getline(cin, option_division);

    for (int isource = 0; isource < int(option_source.size()); ++isource)
    {
        for (int iscintillator = 0; iscintillator < int(option_scintillator.size()); ++iscintillator)
        {
            for (int ith = 0; ith < int(option_th.size()); ++ith)
            {
                // Find Source-Scintillator Combination Positions:
                string filter_source = "Source:" + option_source[isource];
                string filter_scintillator = "Scintillator:" + option_scintillator[iscintillator];
                string filter_th = "Threshold:" + option_th[ith];

                vector<int> source_position = filter(hist_output, filter_source);
                vector<int> source_scintillator = filter(hist_output, filter_scintillator);
                vector<int> source_th = filter(hist_output, filter_th);
                vector<int> positions = filter_intersector(source_position, source_scintillator);
                positions = filter_intersector(positions, source_th);

                if (positions.size() == 0)
                {
                    cout << RED << "ERROR - NO COMBINATIONS FOR: " << option_source[isource] << " - " << option_scintillator[iscintillator] << " - " << option_th[ith] << RESET << endl;
                }

                else
                {
                    // Create Directories and Initialize Root File:
                    string compare_root_path = string(fs::current_path()) + "/outputs/compare/" + hist_data_source + "/" + option_source[isource] + "_" + option_scintillator[iscintillator] + "_" + option_th[ith] + "/";
                    string compare_root_name = compare_root_path + "compare_hist.root";
                    fs::create_directories(compare_root_path.c_str());
                    TFile *hist_root_file = new TFile(compare_root_name.c_str(), "RECREATE");

                    for (int t = 1; t < 6; ++t) // Generate 5 Histograms
                    {
                        vector<vector<double>> hist_values;
                        for (int k = 0; k < int(positions.size()); ++k)
                        {
                            int row = positions[k];
                            vector<double> temp_hist_values;
                            double value = stof(hist_output[row][t]);
                            double error = stof(hist_output[row][t + 5]);
                            temp_hist_values.push_back(value);
                            temp_hist_values.push_back(error);
                            hist_values.push_back(temp_hist_values);
                        }
                        string hist_type = hist_output[0][t];
                        vector<string> filters = {filter_scintillator, filter_source};
                        compare_hist(hist_values, filters, hist_type, compare_root_path, option_division);
                    }
                    hist_root_file->Write();
                    delete hist_root_file;
                    cout << GREEN << "RESULT SAVED TO THE DIRECTORY: " << RESET << compare_root_path << endl;
                }
            }
        }
    }
    delete hist_file;
}

void histogram_result_writer(string data_path, string data_format_path, vector<string> results, vector<string> errors) // Write Histogram Results
{
    string output_hist_path = string(fs::current_path()) + "/outputs/data/";
    string outputname = splitter(data_path, "/").back();
    fs::create_directories(output_hist_path);

    string output_hist = output_hist_path + outputname + "_hist_result.txt";
    ofstream Out_hist(output_hist.c_str());

    string data_format;
    ifstream data_format_reader(data_format_path);
    string temp_data_format;
    temp_data_format = "Entries,FallMean,RiseMean,IntegralMean,PeakVoltMean,PeakTimeMean,FallStd,RiseStd,IntegralStd,PeakVoltStd,PeakTimeStd,";

    Out_hist << temp_data_format;

    while (getline(data_format_reader, data_format)) // Write Data Format
    {
        Out_hist << data_format;
    }

    for (int k = 0; k < int(results.size()); ++k) // Write Results
    {
        Out_hist << "\n"
                 << results[k];
    }

    cout << GREEN << "Histogram Result Saved to The Directory: " << RESET << output_hist << endl;
    Out_hist.close();
    data_format_reader.close();

    if (errors.size() > 0) // Write Errors as TXT
    {
        string output_errors = output_hist_path + outputname + "_errors.txt";
        ofstream Out_errors(output_errors.c_str());

        for (int k = 0; k < int(errors.size()); ++k)
        {
            Out_errors << errors[k] << "\n";
        }

        cout << RED << "Errors Saved to the Directory: " << RESET << output_errors << endl;
        Out_errors.close();
    }
}

vector<string> analyser_matrix(vector<vector<double>> input, string filename, double ns) // Analysis for Matrix
{
    // Create Directories:
    string extension = splitter(filename, ".").back();
    string input_path = filename.substr(0, filename.length() - extension.size() - 1); // Remove .txt
    string outputname = splitter(input_path, "/").back();
    vector<string> vec_input_path = splitter(input_path, "/");
    string head = string(fs::current_path()) + "/outputs/";
    string outputpath = concatenate_vec(head, vec_input_path, "data", "", "/");
    string date = concatenate_vec("", vec_input_path, "data", outputname, "/");
    date = date.substr(0, date.size() - 1);
    fs::create_directories(outputpath);

    // Initialize:
    vector<string> results_and_errors;
    string temp_results;
    string temp_errors;
    string rootname = outputpath + "result.root";
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
        x_axis[i] = i * ns; // Sampling time
    }

    for (int j = 0; j < no_of_datasets; j++)
    {
        gErrorIgnoreLevel = kFatal; // Verbose Mode
        double x_max = no_of_datas * ns;
        vector<double> y_axis = input[j];
        double summation = summation_vec(y_axis, 0);

        TGraph *graph = new TGraph(no_of_datas, &x_axis[0], &y_axis[0]);
        TF1 *fitFcn = new TF1("fitFcn", "[0]*TMath::Landau(x,[1],[2])", 0, x_max);
        if (summation > 0)
        {
            auto it = max_element(y_axis.begin(), y_axis.end());
            peak_voltages[j] = *it; // Find the peak y value
            int max_index = distance(y_axis.begin(), it);
            peak_time.push_back(x_axis[max_index]);

            // Landau Fit:
            float max_value = *it;
            float scale = max_value;
            float mu = max_index * ns; // Location Parameter
            float sigma = 1 * ns;      // Scale Parameter of Fit
            fitFcn->SetParameters(scale, mu, sigma);
            fitFcn->SetRange(0, x_max);
            graph->Fit("fitFcn", "wRQ");

            // Calculate Rise-Fall Time:
            double max_value_fit = fitFcn->GetMaximum();
            double max_time_fit = fitFcn->GetParameter(1);
            double rise_low = fitFcn->GetX(max_value_fit * 0.2, 0, max_time_fit);
            double rise_high = fitFcn->GetX(max_value_fit * 0.8, 0, max_time_fit);
            double fall_high = fitFcn->GetX(max_value_fit * 0.8, max_time_fit, x_max);
            double fall_low = fitFcn->GetX(max_value_fit * 0.2, max_time_fit, x_max);
            risetime.push_back(rise_high - rise_low);
            falltime.push_back(fall_low - fall_high);

            // Calculate Integral of the Fall:
            integrals.push_back(fitFcn->Integral(fall_high, x_max)); // double epsrel = 1.0e-20
        }

        else
        {
            auto it = min_element(y_axis.begin(), y_axis.end());
            peak_voltages[j] = *it; // Find the peak y value
            int min_index = distance(y_axis.begin(), it);
            peak_time.push_back(x_axis[min_index]);

            // Landau Fit:
            float min_value = *it;
            float scale = min_value;
            float mu = min_index * ns; // Location Parameter
            float sigma = 1 * ns;      // Scale Parameter of Fit
            fitFcn->SetParameters(scale, mu, sigma);
            fitFcn->SetRange(0, x_max);
            graph->Fit("fitFcn", "wRQ");

            // Calculate Rise-Fall Time:
            double min_value_fit = fitFcn->GetMinimum();
            double min_time_fit = fitFcn->GetParameter(1);
            double rise_low = fitFcn->GetX(min_value_fit * 0.2, 0, min_time_fit);
            double rise_high = fitFcn->GetX(min_value_fit * 0.8, 0, min_time_fit);
            double fall_high = fitFcn->GetX(min_value_fit * 0.8, min_time_fit, x_max);
            double fall_low = fitFcn->GetX(min_value_fit * 0.2, min_time_fit, x_max);
            risetime.push_back(rise_high - rise_low);
            falltime.push_back(fall_low - fall_high);

            // Calculate Integral of the Fall:
            integrals.push_back(fitFcn->Integral(fall_high, x_max)); // double epsrel = 1.0e-20
        }

        if (isnan(risetime.back()) || isnan(falltime.back()) || isnan(integrals.back())) // Write Corrupted Data
        {
            cout << RED << "ERROR - NAN VALUE: " << RESET << outputpath << ", Segment: " << j + 1 << endl;

            // Graph Design
            TCanvas *c1 = new TCanvas("c1", "c1", 200, 10, 600, 400);
            c1->SetGrid();
            c1->Draw();
            string error_title = date + " " + outputname + " - Voltage vs. Time Segment: " + to_string(j + 1);
            graph->SetTitle(error_title.c_str());
            graph->GetXaxis()->SetTitle("Time (s)");
            graph->GetYaxis()->SetTitle("Voltage (V)");
            graph->SetMarkerStyle(8);
            graph->SetMarkerColor(kBlue);
            graph->SetMarkerSize(0.7);
            graph->SetLineColor(kBlue);
            graph->SetLineWidth(3);
            graph->Draw("A*");
            gStyle->SetOptFit(1);

            // Save Graph as Root and PDF File:
            string root_path = outputpath + "/errors/root";
            string pdf_path = outputpath + "/errors/pdf";
            fs::create_directories(root_path.c_str());
            fs::create_directories(pdf_path.c_str());
            string output_root = root_path + "/graph_" + to_string(j + 1) + ".root";
            string output_pdf = pdf_path + "/graph_" + to_string(j + 1) + ".pdf";
            c1->SaveAs(output_pdf.c_str());
            c1->SaveAs(output_root.c_str());
            delete c1;
        }

        cout << j + 1 << "/" << no_of_datasets << "\r";
        cout.flush();
        delete fitFcn;
        delete graph;
        gROOT->Reset();
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

    h_fall->SetXTitle("Time(s)");
    h_rise->SetXTitle("Time(s)");
    h_integral->SetXTitle("Weber(Vs)");
    h_peak_volt->SetXTitle("Voltage(V)");
    h_peak_time->SetXTitle("Time(s)");

    string error_numbers;
    for (int i = 0; i < no_of_datasets; i++)
    {
        if (isnan(risetime[i]) || isnan(falltime[i]) || isnan(integrals[i])) // Pushback Corrupted Segments
        {
            error_numbers.append(to_string(i + 1) + ",");
        }

        else // Fill Histograms
        {
            h_fall->Fill(falltime[i]);
            h_rise->Fill(risetime[i]);
            h_integral->Fill(integrals[i]);
            h_peak_volt->Fill(peak_voltages[i]);
            h_peak_time->Fill(peak_time[i]);
        }
    }

    temp_errors = outputpath + "," + error_numbers;

    // Print Results as CSV Format:
    hist_result.push_back(h_fall->GetEntries());
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

    name_split = splitter(outputname, "_"); // Split name

    // Merge Histogram Data as a String:
    string result;
    for (int k = 0; k < int(hist_result.size()); ++k)
    {
        result = result + hist_result[k] + ",";
    }
    result.append(date);
    for (int k = 0; k < int(name_split.size()); ++k)
    {
        result.append("," + name_split[k]);
    }
    temp_results = result;

    // Write Histograms and Error Plots as a Root File:
    rootfile->Write();

    // Print Histogram Results as Txt File:
    string output_txt = outputpath + "result.txt";
    ofstream Out_txt(output_txt.c_str());

    Out_txt << "Fall-Integral,Peaks(V),Time(s),RiseTime(s),FallTime(s)";
    for (int i = 0; i < no_of_datasets; i++)
    {
        Out_txt << "\n"
                << integrals[i] << "," << peak_voltages[i] << "," << peak_time[i] << "," << risetime[i] << "," << falltime[i];
    }
    Out_txt.close();

    // Free Memory:
    delete h_fall;
    delete h_rise;
    delete h_integral;
    delete h_peak_volt;
    delete h_peak_time;
    rootfile->Close();

    results_and_errors.push_back(temp_results);
    results_and_errors.push_back(temp_errors);

    cout << GREEN << "Output is saved to the directory: " << RESET << outputpath << endl;
    return results_and_errors;
}

vector<string> analyser_h5(string filename, double ns) // Analysis for H5 Files
{
    // Create Directories:
    string extension = splitter(filename, ".").back();
    string input_path = filename.substr(0, filename.length() - extension.size() - 1); // Remove .txt
    string outputname = splitter(input_path, "/").back();
    vector<string> vec_input_path = splitter(input_path, "/");
    string head = string(fs::current_path()) + "/outputs/";
    string outputpath = concatenate_vec(head, vec_input_path, "data", "", "/");
    string date = concatenate_vec("", vec_input_path, "data", outputname, "/");
    date = date.substr(0, date.size() - 1);

    fs::create_directories(outputpath);

    // Dataset Path:
    string ds_name_head = "/Waveforms/Channel 1/Channel 1 Seg";
    string ds_name_tail = "Data";

    // Open H5 File:
    H5File fp(filename.c_str(), H5F_ACC_RDONLY);

    // Get the Number of Segments:
    Group count_group = fp.openGroup("/Waveforms/Channel 1");
    hsize_t size = count_group.getNumObjs();
    int no_of_datasets = int(size);

    // Initialize:
    vector<string> results_and_errors;
    string temp_results;
    string temp_errors;
    string rootname = outputpath + "result.root";
    vector<double> peak_time;
    vector<double> integrals;
    vector<double> risetime;
    vector<double> falltime;
    vector<double> hist_result;
    vector<string> name_split;
    vector<double> peak_voltages(no_of_datasets);
    TFile *rootfile = new TFile(rootname.c_str(), "RECREATE");

    for (int i = 0; i < no_of_datasets; ++i)
    {
        // Open Dataset:
        string datasetPath = ds_name_head + to_string(i + 1) + ds_name_tail;
        DataSet dset = fp.openDataSet(datasetPath.c_str());

        // Define the Memory Dataspace:
        DataSpace dspace = dset.getSpace();
        hsize_t dims[2];
        dspace.getSimpleExtentDims(dims, NULL);
        hsize_t dimsm[1];
        dimsm[0] = dims[0];
        DataSpace memspace(1, dimsm);

        // Create a Vector with Same Size:
        vector<double> y_axis;
        y_axis.resize(dims[0]);
        dset.read(y_axis.data(), PredType::NATIVE_DOUBLE, memspace, dspace);

        // Check if Peak is at Negative or Positive:
        double summation = summation_vec(y_axis, 0);

        // Create X-Axis:
        int no_of_datas = y_axis.size();
        vector<double> x_axis(no_of_datas); // time
        for (int i = 0; i < no_of_datas; i++)
        {
            x_axis[i] = i * ns; // Sampling time
        }

        // Initialize Graph and Fit Function:
        double x_max = no_of_datas * ns;
        TGraph *graph = new TGraph(no_of_datas, &x_axis[0], &y_axis[0]);
        TF1 *fitFcn = new TF1("fitFcn", "[0]*TMath::Landau(x,[1],[2])", 0, x_max);

        if (summation > 0) // Peak at Positive
        {
            // Find Peak Value:
            auto it = max_element(y_axis.begin(), y_axis.end());
            peak_voltages[i] = *it;
            int max_index = distance(y_axis.begin(), it);
            peak_time.push_back(x_axis[max_index]);

            // Landau Fit:
            float max_value = *it;
            float scale = max_value;
            float mu = max_index * ns; // Location Parameter
            float sigma = 1 * ns;      // Scale Parameter of Fit
            fitFcn->SetParameters(scale, mu, sigma);
            fitFcn->SetRange(0, x_max);
            graph->Fit("fitFcn", "wRQ");

            // Calculate Rise-Fall Time:
            double max_value_fit = fitFcn->GetMaximum();
            double max_time_fit = fitFcn->GetParameter(1);
            double rise_low = fitFcn->GetX(max_value_fit * 0.2, 0, max_time_fit);
            double rise_high = fitFcn->GetX(max_value_fit * 0.8, 0, max_time_fit);
            double fall_high = fitFcn->GetX(max_value_fit * 0.8, max_time_fit, x_max);
            double fall_low = fitFcn->GetX(max_value_fit * 0.2, max_time_fit, x_max);
            risetime.push_back(rise_high - rise_low);
            falltime.push_back(fall_low - fall_high);

            // Calculate Integral of the Fall:
            integrals.push_back(fitFcn->Integral(fall_high, x_max)); // double epsrel = 1.0e-20
        }

        else // Peak at Negative
        {
            // Find Peak Value:
            auto it = min_element(y_axis.begin(), y_axis.end());
            peak_voltages[i] = *it;
            int min_index = distance(y_axis.begin(), it);
            peak_time.push_back(x_axis[min_index]);

            // Landau Fit:
            float min_value = *it;
            float scale = min_value;
            float mu = min_index * ns; // Location Parameter
            float sigma = 1 * ns;      // Scale Parameter of Fit
            fitFcn->SetParameters(scale, mu, sigma);
            fitFcn->SetRange(0, x_max);
            graph->Fit("fitFcn", "wRQ");

            // Calculate Rise-Fall Time:
            double min_value_fit = fitFcn->GetMinimum();
            double min_time_fit = fitFcn->GetParameter(1);
            double rise_low = fitFcn->GetX(min_value_fit * 0.2, 0, min_time_fit);
            double rise_high = fitFcn->GetX(min_value_fit * 0.8, 0, min_time_fit);
            double fall_high = fitFcn->GetX(min_value_fit * 0.8, min_time_fit, x_max);
            double fall_low = fitFcn->GetX(min_value_fit * 0.2, min_time_fit, x_max);
            risetime.push_back(rise_high - rise_low);
            falltime.push_back(fall_low - fall_high);

            // Calculate Integral of the Fall:
            integrals.push_back(fitFcn->Integral(fall_high, x_max)); // double epsrel = 1.0e-20
        }

        if (isnan(risetime.back()) || isnan(falltime.back()) || isnan(integrals.back())) // Write Corrupted Data
        {
            cout << RED << "ERROR - NAN VALUE: " << RESET << outputpath << ", Segment: " << i + 1 << endl;

            // Graph Design
            TCanvas *c1 = new TCanvas("c1", "c1", 200, 10, 600, 400);
            c1->SetGrid();
            c1->Draw();
            string error_title = date + " " + outputname + " - Voltage vs. Time Segment: " + to_string(i + 1);
            graph->SetTitle(error_title.c_str());
            graph->GetXaxis()->SetTitle("Time (s)");
            graph->GetYaxis()->SetTitle("Voltage (V)");
            graph->SetMarkerStyle(8);
            graph->SetMarkerColor(kBlue);
            graph->SetMarkerSize(0.7);
            graph->SetLineColor(kBlue);
            graph->SetLineWidth(3);
            graph->Draw("A*");
            gStyle->SetOptFit(1);

            // Save Graph as Root and PDF File:
            string root_path = outputpath + "/errors/root";
            string pdf_path = outputpath + "/errors/pdf";
            fs::create_directories(root_path.c_str());
            fs::create_directories(pdf_path.c_str());
            string output_root = root_path + "/graph_" + to_string(i + 1) + ".root";
            string output_pdf = pdf_path + "/graph_" + to_string(i + 1) + ".pdf";
            c1->SaveAs(output_pdf.c_str());
            c1->SaveAs(output_root.c_str());
            delete c1;
        }

        cout << i + 1 << "/" << no_of_datasets << "\r";
        cout.flush();
        delete fitFcn;
        delete graph;
        gROOT->Reset();
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

    h_fall->SetXTitle("Time(s)");
    h_rise->SetXTitle("Time(s)");
    h_integral->SetXTitle("Weber(Vs)");
    h_peak_volt->SetXTitle("Voltage(V)");
    h_peak_time->SetXTitle("Time(s)");

    string error_numbers;
    for (int i = 0; i < no_of_datasets; i++)
    {
        if (isnan(risetime[i]) || isnan(falltime[i]) || isnan(integrals[i])) // Pushback Corrupted Segments
        {
            error_numbers.append(to_string(i + 1) + ",");
        }

        else // Fill Histograms
        {
            h_fall->Fill(falltime[i]);
            h_rise->Fill(risetime[i]);
            h_integral->Fill(integrals[i]);
            h_peak_volt->Fill(peak_voltages[i]);
            h_peak_time->Fill(peak_time[i]);
        }
    }

    temp_errors = outputpath + "," + error_numbers;

    // Print Results as CSV Format:
    hist_result.push_back(h_fall->GetEntries());
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

    name_split = splitter(outputname, "_"); // Split name

    // Merge Histogram Data as a String:
    string result;
    for (int k = 0; k < int(hist_result.size()); ++k)
    {
        result = result + hist_result[k] + ",";
    }
    result.append(date);
    for (int k = 0; k < int(name_split.size()); ++k)
    {
        result.append("," + name_split[k]);
    }
    temp_results = result;

    // Write Histograms and Error Plots as a Root File:
    rootfile->Write();

    // Print Histogram Results as Txt File:
    string output_txt = outputpath + "result.txt";
    ofstream Out_txt(output_txt.c_str());

    Out_txt << "Fall-Integral,Peaks(V),Time(s),RiseTime(s),FallTime(s)";
    for (int i = 0; i < no_of_datasets; i++)
    {
        Out_txt << "\n"
                << integrals[i] << "," << peak_voltages[i] << "," << peak_time[i] << "," << risetime[i] << "," << falltime[i];
    }
    Out_txt.close();

    // Free Memory:
    delete h_fall;
    delete h_rise;
    delete h_integral;
    delete h_peak_volt;
    delete h_peak_time;
    rootfile->Close();
    fp.close();

    // Pushback Results and Errors
    results_and_errors.push_back(temp_results);
    results_and_errors.push_back(temp_errors);
    cout << GREEN << "Output is saved to the directory: " << RESET << outputpath << endl;
    return results_and_errors;
}
