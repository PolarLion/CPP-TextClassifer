#pragma once

#include "BayesianTextClassifier.h"
#include <stdio.h>



BayesianTextClassifier::BayesianTextClassifier(const char* config_file)
	: class_num(0)
	, features_num(0)
	, table(NULL)
{
	init_model (config_file);
}

BayesianTextClassifier::~BayesianTextClassifier()
{
	printf ("BayesianTextClassifier::~BayesianTextClassifier()\n");
}

void BayesianTextClassifier::train_model()
{
	
}

void BayesianTextClassifier::init_model(const char* config_file)
{
}

void BayesianTextClassifier::train_on_file(const char* training_file)
{
	printf("CBayesianTextClassifier::train_on_file()\n");
}

void BayesianTextClassifier::save_model(const char* model_file)
{
	printf("CBayesianTextClassifier::save_model()\n");
}

void BayesianTextClassifier::load_model(const char* model_file)
{
	
}

void BayesianTextClassifier::predicted_category(const char* input_file)
{
	printf("CBayesianTextClassifier::predicted_category()\n");
}

void BayesianTextClassifier::free_model()
{

}