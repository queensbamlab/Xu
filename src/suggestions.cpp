#include "headers/suggestions.hpp"

// Callback for curl request.
std::size_t callback(const char* in, std::size_t size, std::size_t num, std::string* out) {
	const std::size_t totalBytes(size * num);
	out->append(in, totalBytes);
	return totalBytes;
}

// query_datamuse_for_word queries an api for word suggestions. Parses the Json and returns a large vector of suggestions.
std::vector<std::string> query_datamuse_for_word(std::string word) {

	std::replace(word.begin(), word.end(), ' ', '+');

	const auto url = "https://api.datamuse.com/words?ml=" + word;
	std::vector<std::string> words;

	CURL* curl = curl_easy_init();
	curl_easy_setopt(curl, CURLOPT_IPRESOLVE, CURL_IPRESOLVE_V4);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

	int httpCode(0);
	std::unique_ptr<std::string> httpData(new std::string());

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());

	curl_easy_perform(curl);
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpCode);
	curl_easy_cleanup(curl);

	if(httpCode == 200) {
		Json::Value jsonData;
		Json::Reader jsonReader;

		if(jsonReader.parse(*httpData, jsonData))
			for(auto i = 0; i < jsonData.size(); i++) {
				if(i > 30) break;
				words.push_back(jsonData.get(i, "word")["word"].asString());
			}
	}

	return words;
}

std::vector<std::string> get_suggestions(Word2Vec<std::string> &model, const std::string word) {

	std::vector<std::pair<std::string, float>> suggestions;

	auto inital_word_vector = model.word_vector(word);

	for(auto suggestion : query_datamuse_for_word(word)) {
		std::pair<std::string, float> p(suggestion, mag_vec(sub_vec(model.word_vector(suggestion), inital_word_vector)));
		if(p.second != 0) suggestions.push_back(p);
	}

	auto did_swap = true;

	while(did_swap) {
		did_swap = false;
		for(auto i = 1; i < suggestions.size(); i++) {
			if(suggestions.at(i-1).second > suggestions.at(i).second) {
				std::pair<std::string, float> tmp = suggestions.at(i-1);
				suggestions.at(i-1) = suggestions.at(i);
				suggestions.at(i) = tmp;
				did_swap = true;
			}
		}
	}

	std::vector<std::string> ret;

	for(auto i = 0; i < suggestions.size(); i++) {
		if(i > 25) break;
		ret.push_back(suggestions.at(i).first);
	}

	return ret;
}
