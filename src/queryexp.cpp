#include "headers/queryexp.hpp"

#include <sstream>
#include <iostream>
#include <utility>

// Join Synonyms joins a cluster of words with OR statements and outputs a resulting string.
std::string join_synonyms(const Cluster c) {

	if(c.size() == 0) {
		return "";
	}

	std::string tmp = "'" + c.at(0).first;

	for(auto i = 1; i < c.size(); i++) {
		tmp += ("' OR '" + c.at(i).first);
	}

	tmp += "'";

	return tmp;
}

// Join Groups joins a vector of strings together using (_) AND (_) AND ...
std::string join_groups(const std::vector<std::string> groups) {
	
	if(groups.size() == 0) {
		return "";
	}

	std::string tmp = "(" + groups.at(0);

	for(auto i = 1; i < groups.size(); i++) {
		tmp += (") AND (" + groups.at(i));
	}

	tmp += ")";

	return tmp;
}

// generate_query_exp generates the query expansion.
std::string generate_query_exp(Word2Vec<std::string> &model, const std::string word, const int n_groups, const int n_iterations) {
	// Get word suggestions from datamuse api
	auto suggestions = get_suggestions(model, word);
	suggestions.push_back(word);

	// Create vector of words, by retrieving the vector representation of the word from the wikipedia trained Word2Vec model.
	std::vector<Word> words;
	for(auto i = 0; i < suggestions.size(); i++) {
		words.push_back(model.word_vector(suggestions.at(i)));
	}

	// Calculate the average vector of the words.
	Word avg = avg_vec(words);
	// Calculate each vector from the average to the respective word.
	Cluster c;
	for(auto i = 0; i < suggestions.size(); i++) {
		std::pair<std::string, Word> p(suggestions.at(i), sub_vec(words.at(i), avg));
		c.push_back(p);
	}

	// Call the cluster_words function, with 4 groups and 500 iterations.
	auto clusters = cluster_words(c, n_groups, n_iterations);

	// For each cluster, join the cluster words together using OR.
	std::vector<std::string> groups;
	for(auto cluster : clusters) {

		/*
			Query expansion for 'photography':
			('landscape') AND ('prints' OR 'exhibition' OR 'painting' OR 'shoot' OR 'portrait' OR 'lenses' OR 'imaging') AND ('filmed' OR 'medium' OR 'printing' OR 'pictures' OR 'scenes' OR 'art' OR 'studio') AND ('gallery' OR 'cinematography' OR 'visual' OR 'installations' OR 'documentary' OR 'photograph' OR 'photography')

			The removal of individual clusters with one word not equal to the inital word produces stronger results, and removes the possibility of the above being generated.
		*/
		if(cluster.size() == 1 && cluster.at(0).first != word) {
			continue; 
		}

		groups.push_back(join_synonyms(cluster));
	}

	// Join the cluster groups together using AND.
	return join_groups(groups);
}

std::vector<std::string> handle_phrase(Word2Vec<std::string> &model, const std::string phrase, const bool automated) {
	std::istringstream stream(phrase);
	std::vector<std::string> tokens;
	std::copy(std::istream_iterator<std::string>(stream),
     std::istream_iterator<std::string>(),
     std::back_inserter(tokens));


	std::vector<std::string> possibilities;
	std::vector<std::pair<int, int>> trials = {std::make_pair(2, 500), std::make_pair(2, 10000), std::make_pair(3, 1000), std::make_pair(3, 10000), std::make_pair(4, 10000), std::make_pair(4, 50000), std::make_pair(5, 10000)};

	for(auto p : trials) {
		std::string tmp = "";
		std::string query_expansion = "";
		auto needs_ending = false;
		for(auto i = 0; i < tokens.size(); i++) {
			if(tmp == "") { 
				tmp = tokens.at(i);
			} else {
				tmp += " " + tokens.at(i);
			}
			if(i+1 == tokens.size()) {
				if(query_expansion == "") {
					query_expansion = generate_query_exp(model, tmp, p.first, p.second);
				} else {
					query_expansion += " " + generate_query_exp(model, tmp, p.first, p.second);
				}
				continue;
			}


			if(tokens.at(i) == "or") {
				query_expansion += "(" + generate_query_exp(model, tmp, p.first, p.second) + ") OR (";
				tmp = "";
				needs_ending = true;
			}

			if(tokens.at(i) == "and") {
				query_expansion += "(" + generate_query_exp(model, tmp, p.first, p.second) + ") AND (";
				tmp = "";
				needs_ending = true;
			}

		}

		if(needs_ending) query_expansion += ")";
		if(query_expansion.size() < 15) continue;

		possibilities.push_back(query_expansion);

		if(automated) return possibilities;
	}

	return possibilities;
}
