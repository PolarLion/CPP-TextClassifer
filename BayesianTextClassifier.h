#pragma once

#include "Classifier.h"
#include <stdio.h>

class CBayesianTextClassifier:public CClassifier 
{
public:
	void train_on_file(const char* training_file);
	void save_model(const char* model_file);
	void predicted_category(const char* input_file);
};