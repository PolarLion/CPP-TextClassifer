#pragma once

#include "Classifier.h"


class BayesianTextClassifier:public Classifier 
{
public:
	void train_on_file(const char* training_file);
	void save_model(const char* model_file);
	void predicted_category(const char* input_file);
  void free_model();
};