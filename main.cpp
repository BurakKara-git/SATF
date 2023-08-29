#include "essential.h"
#include "utilities.h"
#include "functions.h"

/**
 * Function to run compare function
 *
 * @param option_path
 * @param option_type
 */
void compare(std::string option_path, std::string option_type)
{
    if (option_type == "custom" || option_type == "c")
    {
        custom_compare(option_path);
    }
    else
    {
        standard_compare(option_path, "");
    }

    return;
}

/**
 * Function to run  analyser_h5 and analyse_matrix depending on the extensions
 *
 * @param data_path
 * @param data_format_path
 * @param found_names
 * @param ns
 * @return std::string
 */
std::string analyse(std::string data_path, std::string data_format_path, vector<std::string> found_names, double ns)
{
    // Initialize Variables:
    vector<vector<double>> output;
    vector<std::string> results;
    vector<std::string> errors;

    cout << BOLDORANGE << "______________________ANALYSE______________________" << RESET << endl;
    int n = found_names.size();
    for (int i = 0; i < n; i++)
    { // Loop All Files

        std::string filename = found_names[i];
        cout << "\n" GREEN "Successfully opened the file: " << i + 1 << "/" << n << RESET << endl;
        cout << YELLOW << "File name: " << RESET << filename << endl;

        vector<std::string> temp_results_and_errors;
        std::string extension = splitter(filename, ".").back();

        if (extension == "h5")
        {
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
                    std::string line;
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
        cout << BOLDORANGE << "_____________________________________________" << RESET << endl;
    }

    std::string result_path = histogram_result_writer(data_path, data_format_path, results, errors); // Write Histogram Results
    return result_path;
}

int main()
{
    interface(); // Load Interface Logo
    std::string option;

    cout << YELLOW << "Type 'analyse', 'compare', 'hadd', 'browser' or Press ENTER\n>" << RESET;
    getline(cin, option);

    if (option == "compare" || option == "c") // Compare an existing histogram result file
    {
        cout << BOLDORANGE << "______________________COMPARE______________________" << RESET << endl;
        std::string option_path;
        std::string option_type;
        cout << YELLOW << "'standard' or 'custom'\n> " << RESET;
        getline(cin, option_type);

        while (true) // Ask For Histogram Path
        {
            cout << YELLOW << "Histogram Path (For Default Press Enter):\n>" << RESET;
            getline(cin, option_path);

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
                return 0;
            }
        }
        compare(option_path, option_type);
        return 0;
    }

    else if (option == "hadd" || option == "h") // Merge Root Files
    {
        cout << BOLDORANGE << "______________________HADD______________________" << RESET << endl;
        std::string option_hadd_input_path;
        std::string option_hadd_output_path;
        cout << YELLOW << "Input Path\n>" << RESET;
        getline(cin, option_hadd_input_path);
        option_hadd_output_path = option_hadd_input_path + "result.root";
        hadd_creator(option_hadd_output_path, option_hadd_input_path);
        return 0;
    }

    else if (option == "browser" || option == "b") // Open ROOT Browser
    {
        cout << BOLDORANGE << "______________________BROWSER______________________" << RESET << endl;
        int systemErr = system(browser_h.c_str()); // Start Browser
        if (systemErr == -1)
        {
            cout << RED << "ERROR - COULD NOT MERGE ROOT FILES" << endl;
        }
    }

    else if (option == "analyse", option == "a") // Analyse
    {
        cout << BOLDORANGE << "______________________ANALYSE______________________" << RESET << endl;
        std::string data_path;
        std::string ns_string;
        std::string data_format_path;
        vector<std::string> found_names;
        double ns;

        while (true) // Ask For Data Folder
        {
            cout << YELLOW << "Data Folder Path (For Default press ENTER)\n>" << RESET;
            getline(cin, data_path);

            if (data_path == "")
            {
                data_path = data_path_h;
            }

            if (std::filesystem::exists(data_path))
            {
                found_names = file_selector(data_path);
                cout << GREEN << "  FOUND " << found_names.size() << " FILES." << RESET << endl;
                break;
            }

            else
            {
                cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
            }
        }

        while (true) // Ask For Data Format
        {
            cout << YELLOW << "Data Format Path (For Default press ENTER, For Custom '0')\n>" << RESET;
            getline(cin, data_format_path);

            if (data_format_path == "")
            {
                data_format_path = default_data_format_path_h;
            }

            if (data_format_path == "0")
            {
                data_format_path = custom_data_format_path_h;
            }

            if (std::filesystem::exists(data_format_path))
            {
                cout << GREEN << "  SUCCESSFULLY OPENED FORMAT FILE." << endl;
                break;
            }

            else
            {
                cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
            }
        }

        while (true) // Ask For Sampling Time
        {
            cout << YELLOW << "Sampling Time(ns) (For Default-2.5ns press ENTER)\n>" << RESET;
            getline(cin, ns_string);
            if (ns_string == "")
            {
                ns = sampling_time_h;
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
        analyse(data_path, data_format_path, found_names, ns);
        return 0;
    }

    else // Analyse + Compare
    {
        cout << BOLDORANGE << "______________________ANALYSE______________________" << RESET << endl;
        std::string data_path;
        std::string ns_string;
        std::string data_format_path;
        vector<std::string> found_names;
        double ns;

        while (true) // Ask For Data Folder
        {
            cout << YELLOW << "Data Folder Path (For Default press ENTER)\n>" << RESET;
            getline(cin, data_path);

            if (data_path == "")
            {
                data_path = data_path_h;
            }

            if (std::filesystem::exists(data_path))
            {
                found_names = file_selector(data_path);
                cout << GREEN << "  FOUND " << found_names.size() << " FILES." << RESET << endl;
                break;
            }

            else
            {
                cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
            }
        }

        while (true) // Ask For Data Format
        {
            cout << YELLOW << "Data Format Path (For Default press ENTER, For Custom '0')\n>" << RESET;
            getline(cin, data_format_path);

            if (data_format_path == "")
            {
                data_format_path = default_data_format_path_h;
            }

            if (data_format_path == "0")
            {
                data_format_path = custom_data_format_path_h;
            }

            if (std::filesystem::exists(data_format_path))
            {
                cout << GREEN << "  SUCCESSFULLY OPENED FORMAT FILE." << endl;
                break;
            }

            else
            {
                cout << RED << "ERROR: FILE DOES NOT EXIST!" << RESET << endl;
            }
        }

        while (true) // Ask For Sampling Time
        {
            cout << YELLOW << "Sampling Time(ns) (For Default-2.5ns press ENTER)\n>" << RESET;
            getline(cin, ns_string);
            if (ns_string == "")
            {
                ns = sampling_time_h;
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
        std::string hist_path = analyse(data_path, data_format_path, found_names, ns);

        cout << BOLDORANGE << "______________________COMPARE______________________" << RESET << endl;
        standard_compare(hist_path, "fast");
        return 0;
    }
}
