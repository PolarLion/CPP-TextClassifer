#pragma once

#include <vector>
#include <string>
#include <unordered_map>

class Features
{
private:
	long features_num;
	std::unordered_map<std::string, std::vector<long>> features_index;
public:
	Features () : features_num (0) {}
	std::vector<double> doc_to_vec (const std::vector<std::string>& doc) const;
	double* doc_to_ptr (const std::vector<std::string>& doc) const;
	bool load_features (const char* filename);
	long get_features_num () const {
		return features_num;
	}
	std::vector<long> get_indexs (const std::string& ch) const {
		return features_index.at (ch);
	}
};
