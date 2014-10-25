#pragma once

#include "Classifier.h"

struct Item {
	double *v;
};


class BayesianTextClassifier:public Classifier 
{
private:
	int class_num;
	int features_num;
  int training_size;
  bool is_free;
	Item *bayesiantable;
  
  void show_bayesiantable();
  bool init_bayesiantable(int f, int c);
public:
  BayesianTextClassifier();
	~BayesianTextClassifier();
  // void init_model(const char* config_file);
  void train_on_file(const char* training_file);
  void save_model(const char* model_file);
  void load_model(const char* model_file);
  void predicted_category(const double* features, int& res);
  void free_model();
  void show_model();
};