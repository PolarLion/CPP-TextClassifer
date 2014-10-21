#pragma once

#include "BayesianTextClassifier.h"
#include <stdio.h>
#include <string.h>



BayesianTextClassifier::BayesianTextClassifier(const char* config_file)
	: class_num(0)
	, features_num(0)
	, training_size(0)
	, bayesiantable(NULL)
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
	FILE * pfile = fopen(training_file, "r");
	if ( NULL == pfile ) { 
		printf ("BayesianTextClassifier::train_on_file() error in opening training file\n");
		exit(1);
	}
	const char *dropchar = " \t.,!?;-+";
	char* line = NULL;
	char* temp = NULL;
	char* pend = NULL;
	fgets(line, features_num * 5, pfile);
	temp = strtok_r(line, dropchar, &pend);
	training_size = strtol(temp, &pend, 10);
	features_num = strtol(pend, &pend, 10);
	class_num = strtol(pend, NULL, 10);

	printf ("BayesianTextClassifier::train_on_file() training_size = %d, features_num = %d, class_num = %d", 
		training_size, features_num, class_num);

	int i = 0;
	while ( fgets(line, features_num * 5, pfile) != NULL ) {
		double *value = new double[features_num];
		if ( NULL == value ) {
			printf ("BayesianTextClassifier::train_on_file() error allocate memory for double[]\n");
			exit(1);
		}
		temp = strtok_r(line, dropchar, &pend);
		pend = temp;
		for (int j = 1; j < features_num-1; ++j) {
			value[j] = strtod(pend, &pend);
		}
		value[features_num-1] = strtod(pend, NULL);
		bayesiantable[i].v = value;
		++i;
	}
	fclose(pfile);
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