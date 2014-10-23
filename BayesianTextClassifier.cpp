#pragma once

#include "BayesianTextClassifier.h"
#include <stdio.h>
#include <string.h>
#include <cmath>

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
	, is_free(true)
	, bayesiantable(NULL)
{
	init_model (config_file);
}

BayesianTextClassifier::~BayesianTextClassifier()
{
	free_model();
	printf ("BayesianTextClassifier::~BayesianTextClassifier()\n");
}

void BayesianTextClassifier::init_model(const char* config_file)
{
	printf ("BayesianTextClassifier:init_model() \n");
	//nothing to be inited
}

void BayesianTextClassifier::train_on_file(const char* training_file)
{
	printf("CBayesianTextClassifier::train_on_file() : start reading training file\n");
	free_model();
	FILE * pfile = fopen(training_file, "r");
	if ( NULL == pfile ) { 
		printf ("BayesianTextClassifier::train_on_file() error in opening %s\n", training_file);
		exit(1);
	}

	char* pend = NULL;

	//get first line : trainging size, features number, class number
	//and init the bayesiantable
	char first_line[200] = {0};
	if ( NULL != fgets(first_line, 200, pfile) ) {
		training_size = strtol (first_line, &pend, 10);
		features_num = strtol (pend, &pend, 10);
		class_num = strtol (pend, NULL, 10);
		printf ("BayesianTextClassifier::train_on_file() :\
			training_size = %d, features_num = %d, class_num = %d\n", training_size, features_num, class_num);
		bayesiantable = new Item[features_num];
		if ( NULL == bayesiantable ) {
			printf ("BayesianTextClassifier::train_on_file() can't allocate memory for bayesiantable\n");
			exit(1);
		}
		is_free = false;
		for (int i = 0; i < features_num; ++i) {
			bayesiantable[i].v = new double[class_num];
			if ( NULL == bayesiantable[i].v ) {
				printf ("BayesianTextClassifier::train_on_file() can't allocate memory for bayesiantable[%d]\n", i);
				exit(1);
			}
			memset(bayesiantable[i].v, 0.0, class_num);
		}
	}
	else {
		printf ("BayesianTextClassifier::train_on_file() : wrong traing file ---- error in \
			reading first line\n");
		exit(1);
	}

	//edit the bayesiantable 
	const int features_line_size = features_num * 5 + 1;
	const int class_line_size = class_num * 2 + 1;
	char *features_line = new char[features_line_size];
	char *class_line = new char[class_line_size];
	if ( NULL == features_line || NULL == class_line ) {
		printf ("BayesianTextClassifier::train_on_file() error allocate memory for char[]\n");
		exit (1);
	}

	printf("CBayesianTextClassifier::train_on_file() : start training training file\n");
	while ( NULL != fgets (features_line, features_line_size, pfile) ) {
		if ( NULL == fgets (class_line, class_line_size, pfile) ) {
			printf ("BayesianTextClassifier::train_on_file() : wrong traing file\n");
			break;
		}
		int class_index = 0;
		if ( strtol (class_line, &pend, 10) != 1) {
			int i = 1;
			for (i; i < class_num - 1; ++i) {
				if ( strtol (pend, &pend, 10) == 1 ) {
					class_index = i;
					break;
				}
			}
			if ( class_num - 1 == i && strtol (pend, NULL, 10) == 1 )
				class_index = i;
		}
		int j = 0;
		//get every feature value
		bayesiantable[j].v[class_index] += strtod (features_line, &pend);
		for (j = 1; j < features_num-1; ++j) {
			bayesiantable[j].v[class_index] += strtod (pend, &pend);
		}
		bayesiantable[j].v[class_index] += strtod (pend, NULL);
	}
	
	//p(ti|ci) = (1 + tf(ti, ci)) / (features size + sum of tf(tj, ci));
	double *denominator = new double[class_num];
	if ( NULL == denominator ) {
		printf ("BayesianTextClassifier::train_on_file() error allocate memory for double[]\n");
		exit(1);
	}
	for (int i = 0; i < class_num; ++i) {
		denominator[i] = 0;
		for (int j = 0; j < features_num; ++j) {
			denominator[i] += bayesiantable[j].v[i];
		}
		denominator[i] += features_num;
	}
	for (int i = 0; i < features_num; ++i) {
		for (int j = 0; j < class_num; ++j) {
			bayesiantable[i].v[j] += 1.0;
			bayesiantable[i].v[j] /= denominator[j];
			// printf ("%f ", bayesiantable[i].v[j]);
		}
		// printf ("\n");
	}
	is_free = false;
	fclose(pfile);
	delete denominator;
	delete features_line;
	delete class_line;
	printf("CBayesianTextClassifier::train_on_file() finished training !\n");
}

void BayesianTextClassifier::save_model(const char* model_file)
{
	FILE * pfile = fopen(model_file, "wb");
	if ( NULL == pfile ) { 
		printf ("BayesianTextClassifier::save_model() no such directort %s\n", model_file);
		exit(1);
	}
	fprintf(pfile, "%d %d %d\n", training_size, features_num, class_num);	
	for (int i = 0; i < features_num; ++i) {
		fwrite (bayesiantable[i].v, sizeof(double), class_num, pfile);
	}
	fclose(pfile);
	printf("CBayesianTextClassifier::save_model() : saved model successful !!\n");
}

void BayesianTextClassifier::load_model(const char* model_file)
{
	free_model();
	FILE * pfile = fopen(model_file, "r");
	if ( NULL == pfile ) { 
		printf ("BayesianTextClassifier::load_model() error in opening %s\n", model_file);
		exit(1);
	}
	char *pend = NULL;
	char first_line[200] = {0};
	if ( NULL != fgets (first_line, 200, pfile) ) {
		training_size = strtol (first_line, &pend, 10);
		features_num = strtol (pend, &pend, 10);
		class_num = strtol (pend, NULL, 10);
		printf ("BayesianTextClassifier::load_model() \
			training_size = %d, features_num = %d, class_num = %d\n", training_size, features_num, class_num);
		bayesiantable = new Item[features_num];
		if ( NULL == bayesiantable ) {
			printf ("BayesianTextClassifier::load_model() can't allocate memory for bayesiantable\n");
			exit(1);
		}
		for (int i = 0; i < features_num; ++i) {
			bayesiantable[i].v = new double[class_num];
			if ( NULL == bayesiantable[i].v ) {
				printf ("BayesianTextClassifier::load_model() can't allocate memory for bayesiantable[%d]\n", i);
				exit(1);
			}
			memset(bayesiantable[i].v, 0.0, class_num);
		}
	}
	else {
		printf ("BayesianTextClassifier::load_model() : wrong model file ---- error in \
			reading first line\n");
		exit(1);
	}

	for (int i = 0; i < features_num; ++i) {
		fread (bayesiantable[i].v, sizeof(double), class_num, pfile);
	}

	// for (int i = 0; i < features_num; ++i) {
	// 	for (int j = 0; j < class_num; ++j) {
	// 		printf ("%f ", bayesiantable[i].v[j]);
	// 	}
	// 	printf ("\n");
	// }
	fclose(pfile);
	printf("CBayesianTextClassifier::load_model() :  load model successful !!\n");
}

void BayesianTextClassifier::predicted_category(const double* features, int& res)
{
	printf("CBayesianTextClassifier::predicted_category()\n");
	double max = 0.0;
	res = 0;
	for (int i = 0; i < class_num; ++i) {
		double temp = 0.0;
		for (int j = 0; j < features_num; ++j) {
			temp += log(pow(bayesiantable[j].v[i], features[j]));
		}
		if ( 0 == i ) {
			max = temp;
		}else if ( temp > max ) {
			max = temp;
			res = i;
		}
	}
}

void BayesianTextClassifier::free_model()
{
	if ( is_free ) {
		return;
	}
	else if ( NULL != bayesiantable) {
		for (int i = 0; i < features_num; ++i) {
			if ( NULL != bayesiantable[i].v ) {
				delete bayesiantable[i].v;
				bayesiantable[i].v = NULL;
			}
		}//end for
		delete bayesiantable;
		bayesiantable = NULL;
	}//end if
	training_size = 0;
	features_num = 0;
	class_num = 0;
	is_free = true;
}