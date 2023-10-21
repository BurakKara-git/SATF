#include "essential.h"
#include "utilities.h"
#include "functions.h"

int main()
{
    interface(); // Load Interface Logo
    std::string option;

    cout << YELLOW << "Type 'read', 'analyse', 'compare', 'hadd', 'browser' or Press ENTER\n>" << RESET;
    getline(cin, option);

    if (option == "compare" || option == "c") // Compare an existing histogram result file
    {
        compare();        
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

    else if (option == "read" || option == "r"){
        reader();
    }

    else if (option == "analyse", option == "a") // Analyse
    {
        analyser_root();
        return 0;
    }

    else if (option == "t"){
        FileFormalism File_Obj = file_formaliser("/home/burak/SATF2/outputs/root/6Temmuz/NoSourceAfterBaBeforeCo_CR003_1024Seg_Th125mV_1.root");
        cout << File_Obj.source << endl;
        cout << File_Obj.date << endl;
    }

}
