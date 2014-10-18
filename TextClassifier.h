#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include "svm.h"    //包涵本目录下的SVM.H头文件
#include "SingleCharacterSeparater.h"

const int FEATURE_NUMBER = 1000;

void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char);


class SVMTextClassifier
{
private:
	const std::string work_path;
	const codingtype::SeparaterType encoding;
	const int features_num;
	short count_classnum;
	bool prepare_cts;
	int count_training_set;
	struct svm_model *model; //
	char first_trainfile_line[128];
	
	//std::vector<TrainingData*> training_set;
	std::vector<std::string> features;
	std::unordered_map<short, std::string> int_classname;
	std::unordered_map<std::string, short> classname_int;
	std::unordered_map<int, std::string> class_to_string_map;
	mutable SingleCharacterSeparater separater;
	
	short classname_to_int(const std::string& classname) const {
		if (classname_int.find(classname) != classname_int.end()) {
			return classname_int.at(classname);
		}
		return -1;
	}

	std::string int_to_classname(short integer) const {
		if (int_classname.find(integer) != int_classname.end()) {
			return int_classname.at(integer);
		}
		return std::string();
	}

	void prepare_classname_to_string();

	void save_classes() const;
	bool load_classes();
	bool load_features();
	bool load_svm();
	bool train_on_file(const std::string filename, struct svm_parameter* param);
public:
	SVMTextClassifier(const std::string& working_path, int featurenum, codingtype::SeparaterType encoding_type);
	SVMTextClassifier(const std::string& working_path, codingtype::SeparaterType encoding_type);
	~SVMTextClassifier();

	void preprocessor();
	void train_svm();
	std::string classification(const std::string& buffer) const;
	bool load_data();
	void add_classname(const std::string& classname);
	void add_train_data(const std::string& classname, const std::string& buffer);
};