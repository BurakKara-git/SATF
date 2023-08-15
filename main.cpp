#include "essential.h"
#include "functions.h"
using namespace std;
namespace fs = std::filesystem;

int main()
{
    interface(); // Load Interface Logo

    // Initialize Variables:
    vector<vector<double>> output;
    vector<string> results;
    vector<string> errors;
    int count = 0;
    int no_of_data;
    double ns;
    string found_name;
    string data_path;
    string ns_string;
    string data_format_path;
    string option;

    cout << "Type 'compare', 'hadd' or Press ENTER: ";
    getline(cin, option);

    if (option == "compare" || option == "c") // Compare an existing histogram result file
    {
        string option_path;

        while (true) // Ask For Histogram Path
        {
            cout << "Histogram Path (For Default Press Enter): ";
            getline(cin, option_path);

            if (option_path == "")
            {
                option_path = string(fs::current_path()) + "/outputs/data/data_hist_result.txt";
                if (std::filesystem::exists(option_path))
                {
                    cout << GREEN << "OPENED FILE: " + option_path << RESET << endl;
                    break;
                }
                else
                {
                    cout << RED << "FILE NOT FOUND: " + option_path << RESET << endl;
                    return 0;
                }
            }
            else
            {
                if (std::filesystem::exists(option_path))
                {
                    cout << GREEN << "OPENED FILE: " + option_path << RESET << endl;
                    break;
                }
                else
                {
                    cout << RED << "FILE NOT FOUND: " + option_path << RESET << endl;
                }
            }
        }

        full_compare(option_path);
        return 0;
    }

    else if (option == "hadd") // Merge Root Files
    {
        string option_hadd_input_path;
        string option_hadd_output_path;
        cout << "Input Path: ";
        getline(cin, option_hadd_input_path);
        option_hadd_output_path = option_hadd_input_path + ".root";
        hadd_creator(option_hadd_output_path, option_hadd_input_path);
        return 0;
    }

    else // Analyse
    {
        while (true) // Ask For Data Folder
        {
            cout << "Data Folder Path (For Default press ENTER): ";
            getline(cin, data_path);

            if (data_path == "")
            {
                data_path = "data";
            }

            if (std::filesystem::exists(data_path))
            {
                found_name = file_selector(data_path);
                break;
            }

            else
            {
                cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
            }
        }

        while (true) // Ask For Data Format
        {
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

            if (std::filesystem::exists(data_format_path))
            {
                break;
            }

            else
            {
                cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
            }
        }

        while (true) // Ask For Sampling Time
        {
            cout << "Sampling Time(ns) (For Default-2.5ns press ENTER):";
            getline(cin, ns_string);
            if (ns_string == "")
            {
                ns = 2.5e-9;
                break;
            }
            if (is_number(ns_string))
            {
                ns = 1e-9 * stod(ns_string.c_str());
                break;
            }
            else
            {
                cout << RED << "ERROR: INPUT IS NOT A NUMBER!" << RESET << endl;
            }
        }

        std::ifstream inputfile(found_name);
        std::string filename;
        no_of_data = line_counter(found_name);

        while (std::getline(inputfile, filename)) // Loop Through All Files
        {
            count += 1;

            cout << "\n" GREEN "Successfully opened the file: " << count << "/" << no_of_data << RESET << endl;
            cout << YELLOW << "File name: " << RESET << filename << endl;

            vector<string> temp_results_and_errors;
            string extension = splitter(filename, ".").back();

            if (extension == "h5")
            {
                /*
                int segment = 1024;
                output = h5_matrix(filename, segment);
                if (output.size() == 0)
                {
                    cout << RED << "ERROR - FILE DOES NOT CONTAIN DATA: " << RESET << filename << endl;
                    continue;
                }
                */
                
                temp_results_and_errors = analyser_h5(filename, ns);
                results.push_back(temp_results_and_errors[0]);
                errors.push_back(temp_results_and_errors[1]);
            }

            else if (extension == "txt")
            {
                ifstream *datafile = new ifstream;
                datafile->open(filename.c_str());
                if (datafile != nullptr)
                {

                    for (int i = 0; i < 24; ++i) // Skip 24 Lines, not containing data
                    {
                        string line;
                        getline(*datafile, line);
                    }
                    output = reader(*datafile);
                    temp_results_and_errors = analyser_matrix(output, filename, ns);
                    results.push_back(temp_results_and_errors[0]);
                    errors.push_back(temp_results_and_errors[1]);

                    if (output.size() == 0)
                    {
                        cout << RED << "ERROR - FILE DOES NOT CONTAIN DATA: " << RESET << filename << endl;
                        continue;
                    }
                }
                delete datafile;
            }

            else
            {
                cout << RED "ERROR: Could not open the file." RESET << endl;
            }
        }

        histogram_result_writer(data_path, data_format_path, results, errors); // Write Histogram Results
        remove("temp_FoundFiles.txt");
        return 0;
    }
}
