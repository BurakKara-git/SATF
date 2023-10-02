#ifndef functionsh
#define functionsh

#include "essential.h"
#include "utilities.h"
#include <H5Cpp.h>
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
 * Check if the event is already analysed. If it is, return existing result.
 *
 * @author Burak
 * @param filename
 * @param hist_path
 * @param error_path
 * @return vector<string>
 */
vector<string> check_if_analysed(string filename, string hist_path, string error_path)
{
    if (!fs::exists(hist_path) || re_analyse)
    {
        return {};
    }

    else if (!fs::exists(error_path))
    {
        return {};
    }

    else
    {
        // Find Name and Date:
        string extension = splitter(filename, ".").back();
        string input_path = filename.substr(0, filename.length() - extension.size() - 1); // Remove .txt
        string outputname = splitter(input_path, "/").back();
        vector<string> vec_input_path = splitter(input_path, "/");
        string date = concatenate_vec("", vec_input_path, "data", outputname, "/");
        date = date.substr(0, date.size() - 1);

        vector<string> name_split = splitter(outputname, "_"); // Split name

        // Merge Histogram Data as a String:
        string result = date;
        for (int k = 0; k < int(name_split.size()); ++k)
        {
            result.append("," + name_split[k]);
        }

        ifstream reader(hist_path);
        ifstream in(error_path.c_str());
        string line;
        string error;

        int count = 0;
        while (getline(reader, line))
        {
            count += 1;
            int pos = line.find(result);
            if (pos != -1)
            {
                for (int i = 0; i < count; ++i)
                {
                    getline(in, error);
                }

                getline(in, error);

                return {line, error};
            }
        }
        return {};
    }
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
TCanvas *hist_label(vector<string> names, vector<double> values, string hist_name, int margin)
{
    int nx = values.size();
    TCanvas *c1 = new TCanvas("c1", "c1", 10, 10, 900, 500);
    c1->SetGrid();
    c1->SetBottomMargin(0.2);
    TH1D *h = new TH1D(hist_name.c_str(), hist_name.c_str(), nx, 0, nx);
    h->SetStats(0);
    h->SetFillColor(30);
    for (int i = 0; i < nx; i++)
    {
        vector<string> new_name_vec = splitter(names[i], "_");
        string new_name = concatenate_vec("", new_name_vec, "", "", " ");
        h->Fill(new_name.c_str(), values[i]);
    }
    h->Draw("HIST");
    return c1;
}

/**
 * Write histogram results and errors to output files.
 *
 * This function takes the paths to histogram data, data format, and the calculated results along with
 * associated errors. It generates output files to store histogram results and errors. The function also
 * creates directories for the output files based on the current working directory.
 *
 * @author Burak
 * @param data_path The path to the input histogram data.
 * @param data_format_path The path to the data format file.
 * @param results A vector containing calculated histogram results as strings.
 * @param errors A vector containing error messages as strings.
 */
string histogram_result_writer(string data_path, string data_format_path, vector<string> results, vector<string> errors)
{
    string output_hist_path = output_hist_path_h;
    string outputname = splitter(data_path, "/").back();
    fs::create_directories(output_hist_path);

    string output_hist = output_hist_path + outputname + "_hist_result.txt";
    ofstream Out_hist(output_hist.c_str());

    string data_format;
    ifstream data_format_reader(data_format_path);
    string temp_data_format;
    temp_data_format = head_data_format_h;

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
    return output_hist;
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
        temp_std.push_back(abs(data[i][1]));
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

    return values;
}

/**
 * Perform custom comparisons of histograms using user-defined filters.
 *
 * This function guides the user through the process of custom comparisons of histograms by applying
 * user-specified filters. It reads histogram data from a provided file, prompts the user to select
 * filters for comparison, and generates histograms for selected combinations.
 *
 * @author Burak
 * @param hist_path The path to the histogram result file.
 */
void custom_compare(string hist_path)
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

        string availables_msg = concatenate_vec("", availables, "", "", ", ");
        cout << BLUE << hist_output[0][i] << " Options: " << RESET << availables_msg << YELLOW << "\n>" << RESET;
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
            cout << GREEN << filtered_positions.size() << " Entries Found: " << RESET;
            cout << "To Compare Press 'c'" << endl;
        }
    }

    if (filtered_positions.size() == 0)
    {
        return;
    }

    else
    {
        string filters_msg = concatenate_vec("", filters, "", "", ",");
        cout << GREEN << filtered_positions.size() << " Entries for Combination: " << RESET << filters_msg << endl;

        // Ask Divide Option:
        cout << YELLOW << "Divide Entries By\n>" << RESET;
        getline(cin, option_division);

        // Create Directories:
        string name_filter = histogram_namer(filters, "filtered");
        string compare_root_path = compare_root_path_h + hist_data_source + "/" + name_filter + "/";
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
    string available_types = concatenate_vec("", hist_output[0], "PeakTimeStd", "Trial", ",");
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
        option_types = concatenate_vec("", hist_output[0], "PeakTimeStd", "Trial", ",");
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
            values_type = splitter(option_values, ",");
        }
        else
        {
            path_name.append(types[i] + "-all_");
        }
        values_types.push_back(values_type);

        string type_msg = concatenate_vec("", values_type, "", "", ",");
        cout << BLUE << types[i] << ": " << RESET << type_msg << endl;
    }

    // Ask for Bin Division:
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
            cout << RED << "ERROR - NO COMBINATIONS FOR: " << RESET << error_msg << endl;
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
    vector<string> pdf_names = {"FallTime", "RiseTime", "Integral", "PeakVolt", "PeakTime"};
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
        hist_label(name_groups[i], value_groups[i], pdf_names[i], int(types.size()))->Print(pdf_name.c_str());
        hist_label(name_groups[i], value_groups_std[i], pdf_names[i] + "_std", int(types.size()))->Print(pdf_name_std.c_str());
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

/**
 * Analyze matrix data and generate histogram results.
 *
 * This function takes a 2D matrix of input data, a filename, and a sampling interval in seconds.
 * It performs analysis on the matrix data, calculates peak voltages, rise and fall times, and integrates
 * the falling part of the data. The results are stored in histograms and saved as ROOT files. Error handling
 * is performed for NaN values and corrupted data segments.
 *
 * @author Burak
 * @param input A 2D vector containing the input data matrix.
 * @param filename The filename of the input data.
 * @param ns The sampling interval in seconds.
 * @return A vector of strings containing calculated histogram results and error messages.
 */
vector<string> analyser_matrix(vector<vector<double>> input, string filename, double ns)
{
    // Create Directories:
    fs::path path{filename.c_str()};
    path = fs::relative(path, fs::current_path());
    string head = output_analyse_path_h;
    string outputpath = head + string(path) + "/";
    outputpath = outputpath.substr(0, outputpath.size() - 5); // Remove .txt/
    fs::create_directories(outputpath);

    // Find Name and Date:
    string extension = splitter(filename, ".").back();
    string input_path = filename.substr(0, filename.length() - extension.size() - 1); // Remove .txt
    string outputname = splitter(input_path, "/").back();
    vector<string> vec_input_path = splitter(input_path, "/");
    string date = concatenate_vec("", vec_input_path, "data", outputname, "/");
    date = date.substr(0, date.size() - 1);

    // Initialize:
    vector<string> results_and_errors;
    string rootname = outputpath + "/result.root";
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

        if (((isnan(risetime.back()) || isnan(falltime.back()) || isnan(integrals.back())) && print_corrupted) || print_all) // Write Corrupted Data
        {
            if (isnan(risetime.back()) || isnan(falltime.back()) || isnan(integrals.back()))
            {
                cout << RED << "ERROR - NAN VALUE: " << RESET << outputpath << ", Segment: " << j + 1 << endl;
            }

            else
            {
                cout << "PRINTED: " << outputpath << ", Segment: " << j + 1 << endl;
            }

            // Graph Design
            TCanvas *c1 = new TCanvas("c1", "c1", 200, 10, 600, 400);
            c1->SetGrid();
            c1->Draw();
            string error_title = string(path) + " - Voltage vs. Time Segment: " + to_string(j + 1);
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

        cout << "Analysing... " << j + 1 << "/" << no_of_datasets << "\r";
        cout.flush();
        delete fitFcn;
        delete graph;
        gROOT->Reset();
    }

    // Create Histograms
    string path_str = string(path);
    std::replace(path_str.begin(), path_str.end(), '/', '-');
    string h_fall_title = path_str + " - Fall Time";
    string h_rise_title = path_str + " - Rise Time";
    string h_integral_title = path_str + " - Fall Integral";
    string h_peak_volt_title = path_str + " - Peak Voltage";
    string h_peak_time_title = path_str + " - Peak Time";

    string fall_histname = "h_fall_" + path_str;
    string rise_histname = "h_rise_" + path_str;
    string integral_histname = "h_integral_" + path_str;
    string peak_volt_histname = "h_peak_volt_" + path_str;
    string peak_time_histname = "h_peak_time_" + path_str;

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

    string errors = outputpath;
    for (int i = 0; i < no_of_datasets; i++)
    {
        if (isnan(risetime[i]) || isnan(falltime[i]) || isnan(integrals[i])) // Pushback Corrupted Segments
        {
            errors.append("," + to_string(i + 1));
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

    // Write Histograms and Error Plots as a Root File:
    rootfile->Write();

    // Print Histogram Results as Txt File:
    string output_txt = outputpath + "/result.txt";
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

    cout << GREEN << "Output is saved to the directory: " << RESET << outputpath << endl;

    results_and_errors = {result, errors};
    return results_and_errors;
}

/**
 * Analyze H5 file data and generate histogram results.
 *
 * This function takes an H5 file, reads dataset segments, and performs analysis on the data,
 * calculating peak voltages, rise and fall times, and integrating the falling part of the data.
 * The results are stored in histograms and saved as ROOT files. Error handling is performed for
 * NaN values and corrupted data segments.
 *
 * @author Burak
 * @param filename The filename of the H5 file.
 * @param ns The sampling interval in seconds.
 * @return A vector of strings containing calculated histogram results and error messages.
 */
vector<string> analyser_h5(string filename, double ns)
{
    // Create Directories:
    fs::path path{filename.c_str()};
    path = fs::relative(path, fs::current_path());
    string head = output_analyse_path_h;
    string outputpath = head + string(path) + "/";
    outputpath = outputpath.substr(0, outputpath.size() - 5); // Remove .txt/
    fs::create_directories(outputpath);

    // Find Name and Date:
    string extension = splitter(filename, ".").back();
    string input_path = filename.substr(0, filename.length() - extension.size() - 1); // Remove .txt
    string outputname = splitter(input_path, "/").back();
    vector<string> vec_input_path = splitter(input_path, "/");
    string date = concatenate_vec("", vec_input_path, "data", outputname, "/");
    date = date.substr(0, date.size() - 1);

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
    string rootname = outputpath + "/result.root";
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
        gErrorIgnoreLevel = kFatal; // Verbose Mode

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

        if (((isnan(risetime.back()) || isnan(falltime.back()) || isnan(integrals.back())) && print_corrupted) || print_all) // Write Corrupted Data
        {
            if (isnan(risetime.back()) || isnan(falltime.back()) || isnan(integrals.back()))
            {
                cout << RED << "ERROR - NAN VALUE: " << RESET << outputpath << ", Segment: " << i + 1 << endl;
            }

            else
            {
                cout << "PRINTED: " << outputpath << ", Segment: " << i + 1 << endl;
            }

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

        cout << "Analysing... " << i + 1 << "/" << no_of_datasets << "\r";
        cout.flush();
        delete fitFcn;
        delete graph;
        gROOT->Reset();
    }

    // Create Histograms
    string path_str = string(path);
    std::replace(path_str.begin(), path_str.end(), '/', '-');
    string h_fall_title = path_str + " - Fall Time";
    string h_rise_title = path_str + " - Rise Time";
    string h_integral_title = path_str + " - Fall Integral";
    string h_peak_volt_title = path_str + " - Peak Voltage";
    string h_peak_time_title = path_str + " - Peak Time";

    string fall_histname = "h_fall_" + path_str;
    string rise_histname = "h_rise_" + path_str;
    string integral_histname = "h_integral_" + path_str;
    string peak_volt_histname = "h_peak_volt_" + path_str;
    string peak_time_histname = "h_peak_time_" + path_str;

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

    string errors = outputpath;
    for (int i = 0; i < no_of_datasets; i++)
    {
        if (isnan(risetime[i]) || isnan(falltime[i]) || isnan(integrals[i])) // Pushback Corrupted Segments
        {
            errors.append("," + to_string(i + 1));
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

    // Write Histograms and Error Plots as a Root File:
    rootfile->Write();

    // Print Histogram Results as Txt File:
    string output_txt = outputpath + "/result.txt";
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
    results_and_errors.push_back(result);
    results_and_errors.push_back(errors);
    cout << GREEN << "Output is saved to the directory: " << RESET << outputpath << endl;
    return results_and_errors;
}

#endif