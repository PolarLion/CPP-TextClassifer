#pragma once

#include "Classifier.h"
#include <unordered_map>

struct Item {
	double *v;
};


class BayesianTextClassifier:public Classifier 
{
private:
	int class_num;
	int features_num;
  int training_size;
	Item *bayesiantable;
  void train_model();
public:
	BayesianTextClassifier(const char* config_file);
	~BayesianTextClassifier();
  void init_model(const char* config_file);
  void train_on_file(const char* training_file);
  void save_model(const char* model_file);
  void load_model(const char* model_file);
  void predicted_category(const char* input_file);
  void free_model();
};