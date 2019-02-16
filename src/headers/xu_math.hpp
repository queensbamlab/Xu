#ifndef XU_VECTOR_MATH
#define XU_VECTOR_MATH

#include <math.h>
#include <vector>
#include <string>
#include <random>
#include <set>

typedef std::vector<float> Word;
typedef std::vector<std::pair<std::string, Word>> Cluster;

Word avg_vec(const std::vector<Word>);
Word add_vec(const Word, const Word);
Word sub_vec(const Word, const Word);
float mag_vec(const Word);
float dot_vec(const Word, const Word);

float calculate_score_for_cluster(const Cluster);
std::vector<Cluster> cluster_words(const Cluster, const int, const int);

#endif