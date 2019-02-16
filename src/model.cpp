#include "headers/model.hpp"

using Sentence = Word2Vec<std::string>::Sentence;
using SentenceP = Word2Vec<std::string>::SentenceP;

// Number of paralel workers.
auto number_of_workers = 6;
// Output Vector Size.
auto max_len = 200;

// Word2Vec model generation from input text file.
// Requires that the input text file be parsed using parse.pl
// Outputs to output.bin & output.txt files.
void generate_model(const std::string input, const std::string output) {
	Word2Vec<std::string> model(max_len);
	model.sample_ = 0;

	std::vector<SentenceP> sentences;

	auto count = 0;

	SentenceP sentence(new Sentence);
	std::ifstream dataFile(input.c_str());

	while(true) {
		std::string str;
		dataFile >> str;

		if(str.empty()) {
			break;
		}

		++count;
		sentence->tokens_.push_back(std::move(str));

		if(count == max_len) {
			count = 0;
			sentence->words_.reserve(sentence->tokens_.size());
			sentences.push_back(std::move(sentence));
			sentence.reset(new Sentence);
		}
	}

	if(!sentence->tokens_.empty()) {
		sentences.push_back(std::move(sentence));
	}

	model.build_vocab(sentences);
	model.train(sentences, number_of_workers);
	model.save(output + ".bin");
	model.save_text(output + ".txt");
}