#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Classifier.h"
#include "SingleCharacterSeparater.h"


void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char);


class TextClassifier
{
private:
	const codingtype::SeparaterType encoding_type;
	const int features_num;
	const classifiertype::ClassifierType classifier_type;
  
	short count_classnum;
	int count_training_set;

	Classifier* classifier;

	char first_trainfile_line[128];
	
	mutable SingleCharacterSeparater separater;
public:
	TextClassifier(
    int featurenum = 0, 
		codingtype::SeparaterType encoding_t = codingtype::SeparaterType::GBK, 
    classifiertype::ClassifierType classifier_t = classifiertype::ClassifierType::Bayesian
    );
	~TextClassifier();

	void load_features();
	void add_train_data(const char* classname, char* buffer);

	void prepare_training_file();
	void classifier_train();
	bool load_data();
	// void add_classname(const std::string& classname);
};

