#pragma once

#include "Classifier.h"


struct Item {
	char *key;
	double *value;
};


class BayesianTextClassifier:public Classifier 
{
private:
	int class_num;
	int features_num;
	Item *table;
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