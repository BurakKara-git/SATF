#include "essential.h"
#include <H5Cpp.h>
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

int find_position(vector<string> vec_string, string search_string)
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

vector<double> h5_vector(string path, string set_path)
{
    string ifn = path;
    string datasetPath = set_path;

    // Open HDF5 file handle, read only
    H5File fp(ifn.c_str(), H5F_ACC_RDONLY);

    // access the required dataset by path name
    DataSet dset = fp.openDataSet(datasetPath.c_str());

    // get the dataspace
    DataSpace dspace = dset.getSpace();

    // get the dataset type class
    H5T_class_t type_class = dset.getTypeClass();
    // According to HDFView, this is a 32-bit floating-point

    // get the size of the dataset
    hsize_t rank;
    hsize_t dims[2];
    rank = dspace.getSimpleExtentDims(dims, NULL); // rank = 1
    //cout << "Datasize: " << dims[0] << endl;       // this is the correct number of values

    // Define the memory dataspace
    hsize_t dimsm[1];
    dimsm[0] = dims[0];
    DataSpace memspace(1, dimsm);

    // create a vector the same size as the dataset
    vector<double> data;
    data.resize(dims[0]);
    //cout << "Vectsize: " << data.size() << endl;
    vector<double> new_vector;

    double data_out[65341];
    for (int i = 0; i < 65341; i++)
    {
        data_out[i] = 0;
    }
    // pass pointer to the array (or vector) to read function, along with the data type and space.
    dset.read(data_out, PredType::NATIVE_DOUBLE, memspace, dspace);    // FAILS
    dset.read(data_out, PredType::NATIVE_DOUBLE, dspace);              // FAILS
    dset.read(data.data(), PredType::NATIVE_DOUBLE, memspace, dspace); // FAILS

    // close the HDF5 file

    for (int i = 0; i < int(data.size()); i++)
    {
        new_vector.push_back(data_out[i]);
    }
    fp.close();

    return new_vector;
}

vector<vector<double>> h5_matrix(string data_path, int segment){
    string temp_head = "/Waveforms/Channel 1/Channel 1 Seg";
    string temp_tail = "Data";
    vector<vector<double>> matrix_h5;
    for(int i = 1; i < segment+1; ++i){
        string dataset_path = temp_head + to_string(i) + temp_tail;        
        vector<double> vec_h5 = h5_vector(data_path, dataset_path);
        matrix_h5.push_back(vec_h5);
    }
    return matrix_h5;
}

vector<vector<string>> hist_reader(ifstream &thefile) // Read Output Histogram Data
{
    string line;
    int n_rows = 0;
    int n_columns = 0;
    vector<vector<string>> matrix;

    while (getline(thefile, line))
    {
        n_rows += 1;
        vector<string> rows;
        rows = splitter(line, ",");
        n_columns = int(rows.size());
        matrix.push_back(rows);
    }

    vector<vector<string>> transpose(n_columns, vector<string>(n_rows));
    for (int i = 0; i < n_columns; i++)
    {
        for (int j = 0; j < n_rows; j++)
        {
            transpose[i][j] = matrix[j][i];
        }
    }

    return transpose;
}

void compare_hist(vector<vector<double>> data, string filter1, string filter2, string hist_type, string output_path, string bin_division) // Create Histograms for Certain Combinations
{
    vector<string> temp_source_type = splitter(filter1, ",");
    vector<string> temp_scintillator_type = splitter(filter2, ",");
    string head_source = temp_source_type[0];
    string tail_source = temp_source_type[1];
    string head_scintillator = temp_scintillator_type[0];
    string tail_scintillator = temp_scintillator_type[1];
    string histo_name = tail_source + "_" + tail_scintillator + "-" + hist_type;
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

vector<int> filter(vector<vector<string>> data, string source_type, string scintillator_type) // Filter Combinations
{
    vector<string> temp_source_type = splitter(source_type, ",");
    vector<string> temp_scintillator_type = splitter(scintillator_type, ",");
    string head_source = temp_source_type[0];
    string tail_source = temp_source_type[1];
    string head_scintillator = temp_scintillator_type[0];
    string tail_scintillator = temp_scintillator_type[1];

    int source_filter_position = 0;
    int scintillator_filter_position = 0;
    vector<int> data_positions;

    for (int i = 0; i < int(data.size()); ++i)
    {
        if (data[i][0] == head_source)
        {
            source_filter_position = i;
        }

        if (data[i][0] == head_scintillator)
        {
            scintillator_filter_position = i;
        }
    }

    for (int i = 1; i < int(data[source_filter_position].size()); ++i)
    {
        if (data[source_filter_position][i] == tail_source && data[scintillator_filter_position][i] == tail_scintillator)
        {
            data_positions.push_back(i);
        }
    }
    return data_positions;
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

void hadd_creator(string hadd_path, string input_path)
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

void full_compare(string hist_path) // filter + compare_hist
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

    // Generate Result For All Sources and Scintillators:
    vector<string> option_source = {"Ba133", "Cs137", "Co57"};
    vector<string> option_scintillator = {"EJ276", "CR001", "CR002", "CR003"};

    string option_division;
    cout << "Divide Entries By: ";
    getline(cin, option_division);

    for (int isource = 0; isource < int(option_source.size()); ++isource)
    {
        for (int iscintillator = 0; iscintillator < int(option_scintillator.size()); ++iscintillator)
        {
            string filter1 = "Source," + option_source[isource];
            string filter2 = "Scintillator," + option_scintillator[iscintillator];
            vector<int> positions = filter(hist_output, filter1, filter2); // Find Positions of Valid Combinations

            if (positions.size() == 0)
            {
                cout << RED << "ERROR - NO COMBINATIONS FOR: " << option_source[isource] << " - " << option_scintillator[iscintillator] << RESET << endl;
            }

            else
            {
                // Create Directories and Initialize Root File:
                string compare_root_path = string(fs::current_path()) + "/outputs/compare/" + hist_data_source + "/" + option_source[isource] + "_" + option_scintillator[iscintillator] + "/";
                string compare_root_name = compare_root_path + "compare_hist.root";
                fs::create_directories(compare_root_path.c_str());
                TFile *hist_root_file = new TFile(compare_root_name.c_str(), "RECREATE");

                for (int t = 1; t < 6; ++t) // Generate 5 Histograms
                {
                    vector<vector<double>> hist_values;
                    for (int k = 0; k < int(positions.size()); ++k)
                    {
                        int column = positions[k];
                        vector<double> temp_hist_values;
                        double value = stof(hist_output[t][column]);
                        double error = stof(hist_output[t + 5][column]);
                        temp_hist_values.push_back(value);
                        temp_hist_values.push_back(error);
                        hist_values.push_back(temp_hist_values);
                    }
                    string filter3 = hist_output[t][0];

                    compare_hist(hist_values, filter1, filter2, filter3, compare_root_path, option_division);
                }
                hist_root_file->Write();
                delete hist_root_file;
                cout << GREEN << "RESULT SAVED TO THE DIRECTORY: " << RESET << compare_root_path << endl;
            }
        }
    }
    delete hist_file;
}

void histogram_result_writer(string data_path, string data_format_path, vector<string> results, vector<string> errors)
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

vector<string> analyser(vector<vector<double>> input, string filename, double ns) // Analysis
{
    // Create Directories:
    string extension = splitter(filename,".").back();
    string input_path = filename.substr(0, filename.length() - extension.size() - 1); // Remove .txt
    string outputname = splitter(input_path, "/").back();
    vector<string> vec_input_path = splitter(input_path, "/");
    string head = string(fs::current_path()) + "/outputs/";
    string outputpath = concatenate_vec(head, vec_input_path, "data", "", "/");
    string date = concatenate_vec("", vec_input_path, "data", outputname, "/");
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
        auto it = min_element(y_axis.begin(), y_axis.end());
        peak_voltages[j] = *it; // Find the peak y value
        int min_index = distance(y_axis.begin(), it);
        peak_time.push_back(x_axis[min_index]);
        TGraph *graph = new TGraph(no_of_datas, &x_axis[0], &y_axis[0]);

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
