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
  const char* classifier_config_file;
  
  std::string features[FEATURE_NUM];
	short count_classnum;
	int count_training_set;
	bool cts;

	Classifier* classifier;
	char first_trainfile_line[128];
public:
	TextClassifier(
    int featurenum = 0, 
		codingtype::SeparaterType encoding_t = codingtype::SeparaterType::GBK, 
    classifiertype::ClassifierType classifier_t = classifiertype::ClassifierType::Bayesian, 
    const char* classifier_config_f = ""
    );
	~TextClassifier();

	void load_features();
  void training_on_file();
  bool load_data();
  const char* predicted_category(const char* data) const;
	// void set_traing_size(int size);
	// void set_classes(int size, char* classesname);
	// void add_train_data(const char* classname, char *data);
	// void prepare_training_file();
	
	// void add_classname(const std::string& classname);
};

