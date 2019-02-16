#include "headers/xu_math.hpp"

// Takes the average of a set of vectors, and returns the average vector.
Word avg_vec(const std::vector<Word> vecs) {
	
	Word sum(vecs.at(0).size(), 0);
	
	for(auto i = 0; i < vecs.size(); i++) {
		for(auto j = 0; j < vecs.at(i).size(); j++) {
			sum.at(j) += vecs.at(i).at(j);
		}
	}

	for(auto i = 0; i < sum.size(); i++) {
		sum.at(i) /= vecs.size();
	}

	return sum;
}

// Adds two vectors and returns the resultant.
Word add_vec(const Word v1, const Word v2) {
	
	Word resultant;

	if(v1.size() != v2.size()) return v1;
	
	for(auto i = 0; i < v1.size(); i++) {
		resultant.push_back(v1.at(i) + v2.at(i));
	}

	return resultant;
}

// Subtracts two vectors and returns the resultant.
Word sub_vec(const Word v1, const Word v2) {

	Word resultant;

	if(v1.size() != v2.size()) return v1;

	for(auto i = 0; i < v1.size(); i++) {
		resultant.push_back(v1.at(i) - v2.at(i));
	}

	return resultant;
}

// Returns the magnitude of a vector, using an extended version of pythagoras
float mag_vec(const Word vec) {

	auto sum = 0;

	for(auto i = 0; i < vec.size(); i++) {
		sum += pow(vec.at(i), 2);
	}

	return sqrt(sum);
}

// Takes the dot product between two vectors and returns the result.
float dot_vec(const Word v1, const Word v2) {

	auto sum = 0;

	for(auto i = 0; i < v1.size(); i++) {
		sum += v1.at(i) * v2.at(i);
	}

	return sum;
}

// Calculates a "score" for a cluster of vectors, the higher the score, the better they are clustered.
// Magnitude of the summed vectors divided by the sum of the magnitude of all the vectors.
float calculate_score_for_cluster(const Cluster cluster) {

	if(cluster.size() == 0) {
		return 0;
	}

	auto total_individual_mag_sum = 0;

	for(auto i = 0; i < cluster.size(); i++) {
		total_individual_mag_sum += mag_vec(cluster.at(i).second);
	}

	Word vec_sum_total = cluster.at(0).second;
	for(auto i = 1; i < cluster.size(); i++) {
		vec_sum_total = add_vec(vec_sum_total, cluster.at(i).second);
	}

	auto summed_mag = mag_vec(vec_sum_total);


	auto val = summed_mag/total_individual_mag_sum;

	return val;
}

// the vector clustering optimization function.
// splits a cluster of vector into n_groups clusters, using n_iterations.
// As the amount of iterations increase, the accuracy will increase. However, so will runtime.
std::vector<Cluster> cluster_words(const Cluster words, const int n_groups, const int n_iterations) {

	// Uses a trial and error technique.
	// Records the best clustering out of n_iterations of random clustering.
	auto highest_score = 0;
	std::vector<Cluster> best_clustering;

	// For each iteration
	for(auto i = 0; i < n_iterations; i++) {

		std::vector<Cluster> arrangement;
		std::set<int> indexes_left;

		// Populate the set with all indexes of words.
		for(auto j = 0; j < words.size(); j++) {
			indexes_left.insert(indexes_left.end(), j);
		}

		// Create n_groups clusters in the arrangement.
		for(auto j = 0; j < n_groups; j++) {
			Cluster c;
			arrangement.push_back(c);
		}

		// For each index remaining, randomly choose a group for the word and append it to the correct group.
		for(auto index : indexes_left) {
			int group = std::rand() % n_groups;
			arrangement.at(group).push_back(words.at(index));
		}

		// Calculate the score of the arrangement by summing the score of each cluster.
		auto score = 0;
		for(auto cluster : arrangement) {
			auto tmp = calculate_score_for_cluster(cluster);
			score += tmp;
		}

		// If the score is higher than the highest score, update the best clustering and highest score.
		if(score > highest_score) {
			highest_score = score;
			best_clustering = arrangement;
		}

	}
	
	// Return the best clustering.
	return best_clustering;
}
	