#include "essential.h"
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

string folder_selector(string path) // Find all txt files in the folder
{
    string found_files_path = "temp_FoundFiles.txt";
    string command = "find " + path + " -type f -name '*.txt'> " + found_files_path;
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

void compare_hist(vector<vector<double>> data, string filter1, string filter2, string hist_type, string output_path) // Create Histograms for Certain Combinations
{
    vector<string> temp_source_type = splitter(filter1, ",");
    vector<string> temp_scintillator_type = splitter(filter2, ",");
    string head_source = temp_source_type[0];
    string tail_source = temp_source_type[1];
    string head_scintillator = temp_scintillator_type[0];
    string tail_scintillator = temp_scintillator_type[1];

    string histo_name = tail_source + "-" + tail_scintillator + "-" + hist_type;
    // double min_std = *min_element(data[1].begin(), data[1].end());
    double min_val = *min_element(data[0].begin(), data[0].end());
    double max_val = *max_element(data[0].begin(), data[0].end());
    int bin_num = 50;
    TCanvas *c_hist = new TCanvas("c1", "c1", 200, 10, 600, 400);
    c_hist->SetGrid();
    TH1 *histo = new TH1D(histo_name.c_str(), histo_name.c_str(), bin_num, min_val, max_val);
    for (int i = 0; i < int(data.size()); ++i)
    {
        histo->Fill(data[i][0]);
    }

    /*
    double std = histo->GetStdDev();
    delete histo;
    bin_num = abs((max_val - min_val) / std);
    TH1 *new_histo = new TH1D(histo_name.c_str(), histo_name.c_str(), bin_num, min_val, max_val);
    for (int i = 0; i < int(data.size()); ++i)
    {
        new_histo->Fill(data[i][0]);
    }
    new_histo->Draw();
    */

    histo->Draw();

    string histo_file_name = output_path + histo_name + ".pdf";
    c_hist->Print(histo_file_name.c_str());

    delete c_hist;
    delete histo;
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

void hadd_creator(string hadd_path, string input_path)
{
    //string hadd_path = "outputs/compare/" + hist_data_source + "/" + hist_data_source + "_compare_hist.root";
    string hadd_command = "hadd -f " + hadd_path + " `find " + input_path + " -type f -name '*.root'`";

    if (std::filesystem::exists(hadd_path))
    {
        remove(hadd_path.c_str());
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

                    compare_hist(hist_values, filter1, filter2, filter3, compare_root_path);
                }
                hist_root_file->Write();
                delete hist_root_file;
                cout << GREEN << "RESULT SAVED TO THE DIRECTORY: " << RESET << compare_root_path << endl;
            }
        }
    }

    // Merge Root Files:    
    string hadd_path = "outputs/compare/" + hist_data_source + "/" + hist_data_source + "_compare_hist.root";
    string hadd_input = "outputs/compare/" + hist_data_source + "/";
    hadd_creator(hadd_path, hadd_input);    

    delete hist_file;
}