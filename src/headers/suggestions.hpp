#ifndef XU_SUGGESTIONS_RET
#define XU_SUGGESTIONS_RET

#include <curl/curl.h>
#include <jsoncpp/json/json.h>

#include <vector>
#include <string>
#include <memory>

#include "word2vec.h"
#include "xu_math.hpp"

std::vector<std::string> query_datamuse_for_word(std::string);
std::vector<std::string> get_suggestions(Word2Vec<std::string> &model, const std::string);

#endif