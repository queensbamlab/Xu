#include <iostream>
#include <fstream>

#include "headers/csv.h"
#include "headers/queryexp.hpp"

// ShowHelpMenu is a utility function to display information about the command line tool.
void showHelpMenu() {
	std::cout << "\nShowing Help Menu for Xu:\n" << std::endl;
	std::cout << "Xu is a multipurpose command line tool developed by Morgan A. Gallant for training word vectorization models, as well as automatically completing query expansion tasks.\n" << std::endl;
	std::cout << "Commands:" << std::endl;
	std::cout << "./xu queryexp <model file> <word>" << std::endl;
	std::cout << "./xu queryexp <model file> <input csv file> <output csv file> <automated: true/false>" << std::endl;
	std::cout << "./xu model <input file> <output file>\n" << std::endl;
	std::cout << "NOTE: Output files will not be automatically created. Please create them beforehand.\n" << std::endl;
}

void handle_csv_input_output(Word2Vec<std::string> &model, const std::string input_file, const std::string output_file, const bool automated) {
	io::CSVReader<1> in(input_file);
	in.read_header(io::ignore_extra_column, "Query");
	std::string query;

	std::ofstream o_file;
	o_file.open(output_file);

	std::vector<std::string> queries;

	while(in.read_row(query)) queries.push_back(query);

	if(!automated) {
		for(auto i = 0; i < queries.size(); i++) {
			try {
				auto query_exp = handle_phrase(model, queries.at(i), automated);
				auto index = 0;

				std::cout << "\nPhrase to Expand: " << queries.at(i) << ". Suggestions: \n" << std::endl;
				for(auto i = 0; i < query_exp.size(); i++) {
					std::cout << i+1 << ": " << query_exp.at(i) << "\n" << std::endl;
				}
				std::cout << "Which query expansion would you like to use? ";
				std::cin >> index;
				index--;
				
				if(query_exp.size() == 0) {
					o_file << "\"" << queries.at(i) << "\",\"UNABLE TO GENERATE QUERY EXPANSION.\"\n";
					continue;
				}

				o_file << "\"" << queries.at(i) << "\",\"" << query_exp.at(index) << "\"\n";
			} catch (const char* msg) {
				std::cout << "Could not expand phrase: " << queries.at(i) << std::endl;
				std::cout << "Moving On..." << std::endl;
			}
		}
	} else {
		#pragma omp parallel for
		for(auto i = 0; i < queries.size(); i++) {
			try {
				auto query_exp = handle_phrase(model, queries.at(i), automated);
				auto index = 0;

				std::cout << "Finished Phrase: " << queries.at(i) << std::endl;

				if(query_exp.size() == 0) {
					o_file << "\"" << queries.at(i) << "\",\"UNABLE TO GENERATE QUERY EXPANSION.\"\n";
					continue;
				}

				o_file << "\"" << queries.at(i) << "\",\"" << query_exp.at(index) << "\"\n";

				} catch (const char* msg) {
					std::cout << "Could not expand phrase: " << queries.at(i) << std::endl;
					std::cout << "Moving On..." << std::endl;
			}
		}
	}
	o_file.close();
}

// Main function of the program. Parses arguments and delegates tasks.
int main(int argc, char** argv) {
	// Initialize a variable to hold the model.
	Word2Vec<std::string> model(200);

	auto has_handled = false;

	// Handling ./xu queryexp <model file> <word>
	if(argc == 4 && strcmp(argv[1], "queryexp") == 0) {
		const std::string model_file = argv[2];
		const std::string word = argv[3];

		std::cout << "Loading model file '" << model_file << "'..." << std::endl;
		model.load(model_file);

		std::cout << "Query expansion for '" << word << "':" << std::endl;
		std::cout << generate_query_exp(model, word, 3, 10000) << std::endl;

		has_handled = true;
	}

	// Handling ./xu queryexp <model file> <input csv file> <output csv file> <automated: true/false>
	if(argc == 6 && strcmp(argv[1], "queryexp") == 0 && (strcmp(argv[5], "true") == 0 || strcmp(argv[5], "false") == 0)) {
		std::string model_file = argv[2];
		std::string input_file = argv[3];
		std::string output_file = argv[4];

		std::cout << "Loading model file '" << model_file << "'..." << std::endl;
		model.load(model_file);

		bool automated = (strcmp(argv[5], "true") == 0) ? true : false;

		handle_csv_input_output(model, input_file, output_file, automated);

		has_handled = true;
	}

	// Handling ./xu model <input file> <output file>
	if(argc == 4 && strcmp(argv[1], "model") == 0) {
		std::string input_file = argv[2];
		std::string output_file = argv[3];

		generate_model(input_file, output_file);

		has_handled = true;
	}

	// If nothing has been handled yet, show the help menu.
	if(!has_handled) {
		showHelpMenu();
	}
	// Return success.
	return 0;
}