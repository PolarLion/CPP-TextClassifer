#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>


#include "Classifier.h"
//#include "SingleCharacterSeparater.h"
#include "RunTimeLog.h"
#include "Features.h"


enum EncodingType {
	GBK = 0,
	UTF_8 = 1
};


class TextClassifier
{
private:
	const EncodingType encoding_type;
	const classifiertype::ClassifierType classifier_type;
  const char* classifier_config_file;
	const char* features_file;

	long count_classnum;
	long count_training_set;
	bool prepare_cts;

	Classifier* classifier;
  char first_trainfile_line[128];
  std::unordered_map<int, std::string> int_classname;
  std::unordered_map<std::string, int> classname_int;
  std::unordered_map<int, std::string> class_to_string_map;
	mutable RunTimeLog runtime_log;
	Features features;
  
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

  bool prepare_classname_to_string();

  bool save_classes() const;
  bool load_classes();
public:
	TextClassifier(
		EncodingType encoding_t = EncodingType::GBK, 
    classifiertype::ClassifierType classifier_t = classifiertype::ClassifierType::Bayesian, 
    const char* classifier_config_f = "",
		const char* features_f = ""
    );
	~TextClassifier();

	bool load_features();
  // void train();
  bool load_data();
  const char* predicted_category(const char* data) const;

  bool preprocessor();
  bool add_classname(const std::string& classname);
  bool add_train_data(const std::string& classname, const std::string& buffer);
  void show_model();

  bool add_training_set (const std::string& train_dir);

  bool train ();

  bool batch_predict (const std::string& dir, const std::string& outfile);

  bool auto_test (const std::string& train_dir, const std::string& resfile, const double ratio);

  long get_features_number () const {
    return features.get_features_dim ();
  }

	long get_training_size () const {
    return count_training_set;
  }
  long get_classes_number () const {
    return count_classnum;
  }

}; 

