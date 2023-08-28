#ifndef utilitiesh
#define utilitiesh

#include "essential.h"
using namespace std;

/**
 * Displays a customized interface logo on the console.
 *
 * This function prints a visually appealing logo with the title "SAT-Force Analysis Interface"
 * on the console. The logo is generated using a combination of colored and plain text,
 * creating a distinct appearance.
 *
 * @author Hazal
 *
 */
void interface()
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

/**
 * Splits a given string into multiple substrings based on a specified delimiter.
 *
 * This function takes a string and a delimiter as input and splits the input
 * string into multiple substrings using the delimiter as a separator. The
 * substrings are stored in a vector of strings and returned.
 *
 * @author Burak
 * @param name The input string to be split.
 * @param DELIMITER The delimiter used to separate the substrings.
 * @return A vector of strings containing the substrings after splitting.
 */
vector<string> splitter(string name, string DELIMITER)
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

/**
 * Finds all TXT and H5 files in a specified directory.
 *
 * This function searches the specified directory for files with the extensions
 * ".txt" and ".h5".
 *
 * @author Burak
 * @param path The path of the directory in which to search for files.
 * @return vector<string>  The list of found files with absolute path.
 */
vector<string> file_selector(string path)
{
    vector<string> files;

    for (auto &p : filesystem::recursive_directory_iterator(path))
    {
        if (p.path().extension() == ".txt" || p.path().extension() == ".h5")
            files.push_back(filesystem::absolute(p).string());
    }
    return files;
}

/**
 * Counts the number of lines in a text file.
 *
 * This function reads the specified text file line by line and increments a
 * counter for each line read. It returns the total number of lines in the file.
 *
 * @author Burak
 * @param path The path of the text file for which to count the lines.
 * @return The total number of lines in the specified text file.
 */
int line_counter(string path)
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

/**
 * Checks if a given string represents a valid number.
 *
 * This function attempts to convert the given string to a double using the
 * strtod function. If the conversion is successful and the entire string has
 * been processed, and the resulting value is not infinity, the function returns true.
 * Otherwise, it returns false.
 *
 * @author Burak
 * @param s The string to be checked for numeric validity.
 * @return True if the string represents a valid number, false otherwise.
 */
bool is_number(const std::string &s)
{
    char *end = nullptr;
    double val = strtod(s.c_str(), &end);
    return end != s.c_str() && *end == '\0' && val != HUGE_VAL;
}

/**
 * Finds the position of the first occurrence of a specified string in a vector of strings.
 *
 * This function searches for the first occurrence of the provided search string in the given
 * vector of strings and returns the position (index) where the string is found. If the string
 * is not found, the function returns -1.
 *
 * @author Burak
 * @param vec_string A vector of strings in which the search will be performed.
 * @param search_string The string to search for within the vector.
 * @return The position (index) of the first occurrence of the search string, or -1 if not found.
 */
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

/**
 * Computes the summation of elements in a vector of doubles starting from a specified index.
 *
 * This function calculates the sum of elements in the provided vector of doubles, starting from
 * the specified index 'first' and summing up to the end of the vector. The result is the sum of
 * the elements within the specified range.
 *
 * @author Burak
 * @param input A vector of double values for which the summation will be performed.
 * @param first The index from which to start the summation (inclusive).
 * @return The summation of elements in the vector starting from the specified index.
 */
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

/**
 * Find available options for comparison in a given column of data.
 *
 * This function extracts and returns a vector of available options for comparison from a specified
 * column of data. It can optionally filter the options based on a set of positions or consider
 * the entire data set if an empty position vector is provided.
 *
 * @author Burak
 * @param data A 2D vector containing the data table.
 * @param column The index of the column from which to extract options.
 * @param positions A vector of positions to filter data rows (optional).
 * @return A vector containing the unique available options for comparison.
 */
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

/**
 * Create a ROOT file by merging multiple input ROOT files using 'hadd' command.
 *
 * This function generates and runs the 'hadd' command to merge multiple input ROOT files
 * into a single output ROOT file at the specified path.
 *
 * @author Burak
 * @param hadd_path The path where the merged ROOT file will be saved.
 * @param input_path The path containing the input ROOT files to be merged.
 */
void hadd_creator(string hadd_path, string input_path)
{
    string hadd_command = "hadd -f " + hadd_path + " `find " + input_path + " -type f -name '*.root'`";

    if (std::filesystem::exists(hadd_path))
    {
        string option_delete;
        cout << RED << "ERROR: ROOT FILE EXISTS" << RESET << endl;
        cout << YELLOW <<  "DELETE and CONTINUE? " << "y/n \n>" << RESET;
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

/**
 * Find the intersection of two integer vectors.
 *
 * This function computes the intersection of two integer vectors and returns a new vector
 * containing the common elements between the two input vectors.
 *
 * @author Burak
 * @param first The first integer vector.
 * @param second The second integer vector.
 * @return A vector containing the common elements of the input vectors.
 */
vector<int> filter_intersector(vector<int> first, vector<int> second)
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

/**
 * Reads data from an input file stream and generates a matrix of double values.
 *
 * This function takes an input file stream as an argument and reads data line by line
 * from the file. Each line is treated as a row of elements, which are space-separated.
 * The function then generates a matrix of double values from the read data. The matrix
 * is represented as a vector of vectors of double values. Transpose of the matrix is returned.
 *
 * @author Hazal
 * @param thefile An input file stream containing data to be read.
 * @return A transpose matrix of double values, represented as a vector of vectors.
 */
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

/**
 * Filter data based on a given filter type and value.
 *
 * This function filters a matrix of data based on a specified filter type and value.
 * It searches for the provided filter type in the header row of the data matrix,
 * then filters the data rows based on the matching filter value and entry size.
 * The function returns a vector containing the positions of the filtered data rows.
 *
 * @author Burak
 * @param data A matrix of data values where each row represents a data point and its associated attributes.
 * @param filter A string containing the filter type and value in the format "type:value".
 * @return A vector of integers representing the positions of the filtered data rows.
 */
vector<int> filter(vector<vector<string>> data, string filter)
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
        if (data[i][filter_type_position] == filter_value && stoi(data[i][0]) >= filter_size_h)
        {
            positions.push_back(i);
        }
    }

    return positions;
}

/**
 * Concatenate selected elements of a vector into a single string.
 *
 * This function concatenates selected elements of a given vector of strings,
 * starting from the 'first' element and ending before the 'last' element.
 * The concatenated string is formed by joining the selected elements using the provided delimiter.
 *
 * @author Burak
 * @param head_string The initial string that forms the beginning of the concatenated string.
 * @param vector_line The vector of strings containing the elements to be concatenated.
 * @param first The first element to include in the concatenation (exclusive), or an empty string to start from the beginning.
 * @param last The last element to include in the concatenation (exclusive), or an empty string to include all elements.
 * @param DELIMITER The delimiter used to join the concatenated elements.
 * @return A single string formed by concatenating the selected elements with the specified delimiter.
 */
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

/**
 * Takes vector<vector<string>> as an input and returns all combinations of vector<string>s
 * 
 * 
 * @author Burak
 * @param vectors 
 * @param index 
 * @param str_combination 
 * @param result 
 * @param DELIMITER 
 */
void combination(const vector<vector<string>> &vectors, size_t index, string str_combination, vector<string> &result, string DELIMITER)
{
    if (index >= vectors.size())
    {
        result.push_back(str_combination);
        return;
    }

    for (size_t i = 0; i < vectors[index].size(); i++){
        combination(vectors, index + 1, str_combination + vectors[index][i] + DELIMITER, result, DELIMITER);
    }
        
}

#endif