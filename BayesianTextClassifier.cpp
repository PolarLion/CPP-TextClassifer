#pragma once

#include "BayesianTextClassifier.h"
#include <stdio.h>
#include <string.h>

BayesianTextClassifier::BayesianTextClassifier()
	: class_num(0)
	, features_num(0)
	, training_size(0)
	, bayesiantable(NULL)
{
}

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
	printf ("BayesianTextClassifier:init_model() \n");
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
	char* pend = NULL;

	//get first line : trainging size, features number, class number
	char first_line[200] = {0};
	if ( NULL != fgets(first_line, 200, pfile) ) {
		training_size = strtol(first_line, &pend, 10);
		features_num = strtol(pend, &pend, 10);
		class_num = strtol(pend, NULL, 10);
		printf ("BayesianTextClassifier::train_on_file() training_size = %d, \
			features_num = %d, class_num = %d", training_size, features_num, class_num);
		bayesiantable = new Item[features_num];
		if ( NULL == bayesiantable ) {
			printf ("BayesianTextClassifier::train_on_file() can't allocate memory for bayesiantable\n");
			eixt(1);
		}
		for (int i = 0; i < features_num; ++i) {
			bayesiantable[i].v = new double[class_num];
			if ( NULL == bayesiantable[i].v ) {
				printf ("BayesianTextClassifier::train_on_file() can't allocate memory for bayesiantable[%d]\n", i);
				eixt(1);
			}
		}
	}
	else {
		printf ("BayesianTextClassifier::train_on_file() : wrong traing file ---- error in \
			reading first line\n");
		exit(1);
	}

	const int features_line_size = features_num * 5 + 1;
	const int class_line_size = class_num * 2 + 1;
	char *features_line = new char[features_line_size];
	char *class_line = new char[class_line_size];
	if ( NULL == features_line || NULL == class_line ) {
		printf ("BayesianTextClassifier::train_on_file() error allocate memory for char[]\n", );
		exit (1);
	}

	int i = 0;
	while ( fgets (features_line, features_line_size, pfile) != NULL ) {
		if ( NULL == fgets (class_line, class_line_size, pfile) ) {
			printf ("BayesianTextClassifier::train_on_file() : wrong traing file\n");
			break;
		}

		int class_index = 0;
		if ( strtol (class_line, &pend) != 1) {
			int ii = 1;
			for (ii = 1; ii < class_num - 1; ++ii) {
				if ( strtol (pend, &pend ) == 1 ) {
					class_index = ii;
					break;
				}
			}
			if ( class_num - 1 == ii && strtol (pend, NULL) == 1 )
				class_inex = ii;
		}
		int j = 0;
		//get every feature value
		bayesiantable[j].v[class_inex] += strtod(line, &pend);
		for (j = 1; j < features_num-1; ++j) {
			bayesiantable[j].v[class_inex] = strtod(pend, &pend);
		}
		bayesiantable[j].v[class_inex] = strtod(pend, NULL);
		++i;
	}
	
	for (int j = 0; j < features_num; ++j) {

	}
	fclose(pfile);
	delete class_line;
	delete line;
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