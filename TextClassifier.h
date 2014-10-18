#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

#include "Classifier.h"
#include "SingleCharacterSeparater.h"


void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char);



class CTextClassifier
{
private:
	const std::string working_path;
	const codingtype::SeparaterType encoding_type;
	const int features_num;
	const classifiertype::ClassifierType classifier_type;
  
	short count_classnum;
	int count_training_set;

	CClassifier* classifier;

	char first_trainfile_line[128];
	
	mutable SingleCharacterSeparater separater;
public:
	CTextClassifier(
    const std::string& work_path = "", 
    int featurenum = 0, 
		codingtype::SeparaterType encoding_t = codingtype::SeparaterType::GBK, 
    classifiertype::ClassifierType classifier_t = classifiertype::ClassifierType::Bayesian
    );
	~CTextClassifier();

	void add_training_data(const std::string& data);
	void prepare_training_file();
	void classifier_train();
	bool load_data();
	void add_classname(const std::string& classname);
	void add_train_data(const std::string& classname, const std::string& buffer);
};