#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Classifier.h"
#include "SingleCharacterSeparater.h"

#define FEATURE_NUM 3500


class TextClassifier
{
private:
	const codingtype::SeparaterType encoding_type;
	const int features_num;
	const classifiertype::ClassifierType classifier_type;
  
  std::string features[FEATURE_NUM];
	short count_classnum;
	int count_training_set;
	bool cts;

	Classifier* classifier;

	char first_trainfile_line[128];

	std::unordered_map<std::string, std::string> class_to_string_map;
	
	mutable SingleCharacterSeparater separater;
public:
	TextClassifier(
    int featurenum = 0, 
		codingtype::SeparaterType encoding_t = codingtype::SeparaterType::GBK, 
    classifiertype::ClassifierType classifier_t = classifiertype::ClassifierType::Bayesian
    );
	~TextClassifier();

	void load_features();
	void add_train_data(const char* classname, char *data);

	void prepare_training_file();
	void classifier_train();
	bool load_data();
	// void add_classname(const std::string& classname);
};

