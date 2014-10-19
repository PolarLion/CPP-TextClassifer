#pragma once

#include "BayesianTextClassifier.h"
#include <stdio.h>

void BayesianTextClassifier::train_on_file(const char* training_file)
{
	printf("in CBayesianTextClassifier::train_on_file\n");
}


void BayesianTextClassifier::save_model(const char* model_file)
{
	printf("in CBayesianTextClassifier::save_model\n");
}

void BayesianTextClassifier::predicted_category(const char* input_file)
{
	printf("in CBayesianTextClassifier::predicted_category\n");
}

void BayesianTextClassifier::free_model()
{

}