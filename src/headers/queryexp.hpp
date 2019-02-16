#ifndef XU_QUERY_EXP
#define XU_QUERY_EXP

#include "xu_math.hpp"
#include "model.hpp"
#include "suggestions.hpp"

std::string join_synonyms(const Cluster);
std::string generate_query_exp(Word2Vec<std::string> &model, const std::string, const int, const int);
std::vector<std::string> handle_phrase(Word2Vec<std::string> &model, const std::string, const bool);

#endif