#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>


#include "Classifier.h"
// #include "BayesianTextClassifier.h"
#include "SingleCharacterSeparater.h"
// #include "LogitRegressionClassifier.h"

#define FEATURE_NUM 3500

void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char);

class TextClassifier
{
private:
  const size_t features_num;
	const codingtype::SeparaterType encoding_type;
	const classifiertype::ClassifierType classifier_type;
  const char* classifier_config_file;

  std::string features[FEATURE_NUM];
	size_t count_classnum;
	size_t count_training_set;
	bool prepare_cts;

	Classifier* classifier;
  mutable SingleCharacterSeparater separater;
  char first_trainfile_line[128];
  std::unordered_map<int, std::string> int_classname;
  std::unordered_map<std::string, int> classname_int;
  std::unordered_map<int, std::string> class_to_string_map;
  
  int classname_to_int(const std::string& classname) const {
    if (classname_int.find(classname) != classname_int.end()) {
      return classname_int.at(classname);
    }
    return -1;
  }

  std::string int_to_classname(int integer) const {
    if (int_classname.find(integer) != int_classname.end()) {
      return int_classname.at(integer);
    }
    return std::string();
  }

  void prepare_classname_to_string();

  void save_classes() const;
  bool load_classes();
public:
	TextClassifier(
    long featurenum = 0, 
		codingtype::SeparaterType encoding_t = codingtype::SeparaterType::GBK, 
    classifiertype::ClassifierType classifier_t = classifiertype::ClassifierType::Bayesian, 
    const char* classifier_config_f = ""
    );
	~TextClassifier();

	void load_features();
  // void train();
  bool load_data();
  const char* predicted_category(const char* data) const;

  void preprocessor();
  void add_classname(const std::string& classname);
  void add_train_data(const std::string& classname, const std::string& buffer);
  void show_model();

  bool add_training_set (const std::string& train_dir);

  void train ();

  void batch_predict (const std::string& dir, const std::string& outfile);

  bool auto_test (const std::string& train_dir, const std::string& resfile, const double ratio);

  size_t get_features_number () const {
    return features_num;
  }

  size_t get_training_size () const {
    return count_training_set;
  }

  size_t get_classes_number () const {
    return count_classnum;
  }

};

