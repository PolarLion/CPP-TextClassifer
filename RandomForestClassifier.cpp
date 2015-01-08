// #pragma once

#include "RandomForestClassifier.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <fstream>

#define FIRST_LINE_SIZE 1000000
using namespace randomforestc;

RandomForestClassifier::RandomForestClassifier()
	: n_trees (50)
{
}

RandomForestClassifier::RandomForestClassifier(const char* config_file)
	: n_trees (50)
{
	init_model (config_file);
}

RandomForestClassifier::~RandomForestClassifier()
{
	free_model ();
	printf ("RandomForestClassifier::~RandomForestClassifier()\n");
}

bool RandomForestClassifier::init_model(const char* config_file)
{
	// printf ("RandomForestClassifier:init_model() \n")
	//nothing to be inited
	return true;
}

bool RandomForestClassifier::train_on_file(const char* training_file)
{
	printf("RandomForestClassifier::train_on_file() : start reading training file\n");
	free_model();
	alglib::real_2d_array xy;
	alglib::dfreport dfrep;
	alglib::ae_int_t info;
	
	std::ifstream infile (training_file);
	if (infile.fail()) {
		printf ("BayesianTC::train_on_file() error in opening %s\n", training_file);
		return false;
	}

	char* pend = NULL;

	//get first line : trainging size, features number, class number
	char first_line[FIRST_LINE_SIZE] = {0};
	if ( !(infile.getline (first_line, FIRST_LINE_SIZE)).fail() ) {
		training_size = strtol (first_line, &pend, 10);
		features_num = strtol (pend, &pend, 10);
		class_num = strtol (pend, NULL, 10);
		printf ("RandomForestClassifier::train_on_file() :\
			training_size = %ld, features_num = %ld, class_num = %ld\n", training_size, features_num, class_num);
		is_free = false;
	}
	else {
		printf ("RandomForestClassifier::train_on_file() : wrong traing file ---- error in \
			reading first line\n");
		return false;
	}

	double *alldata = (double*) malloc (sizeof(double)*(features_num + 1)*training_size);
	if ( NULL == alldata ) {
		printf ("RandomForestClassifier::train_on_file() can't allocate memory for alldata\n");
		return false;
	}
	// //edit the bayesiantable 
	size_t features_line_size = features_num * 10+ 1;
	size_t class_line_size = class_num * 2 + 1;
	char *features_line = new char[features_line_size];
	char *class_line = new char[class_line_size];
	if ( NULL == features_line || NULL == class_line ) {
		printf ("RandomForestClassifier::train_on_file() error allocate memory for char[]\n");
		return false;
	}

	int count_line = 1;
	int current_index = 0;
	printf("RandomForestClassifier::train_on_file() : start training training file\n");
	while ( !(infile.getline (features_line, FIRST_LINE_SIZE)).fail()
	 	&& count_line < training_size * 2 + 1) {
		count_line++;
		if (  (infile.getline (class_line, FIRST_LINE_SIZE)).fail() ) {
			printf ("RandomForestClassifier::train_on_file() : wrong traing file\
				read line %d\n", count_line);
			break;
		}
		else { 
			count_line++; 
		}

		// printf("class_line %s\n", class_line);
		alldata[current_index++] = strtod (features_line, &pend);
		for (int j = 1; j < features_num - 1; ++j) {
			alldata[current_index++] = strtod (pend, &pend);
		}
		alldata[current_index++] = strtod (pend, NULL);

		pend = NULL;
		int class_index = 0;
		if ( strtol (class_line, &pend, 10) != 1) {
			int i = 1;
			for (i = 1; i < class_num - 1; ++i) {
				if ( strtol (pend, &pend, 10) == 1 ) {
					class_index = i;
					break;
				}
			}
			if ( class_num - 1 == i && strtol (pend, NULL, 10) == 1 )
				class_index = i;
		}
		// printf("train  %d %s= %d\n", count_line, class_line, class_index);
		alldata[current_index++] = (double)class_index;
		//get every feature value
	}

	xy.setcontent (training_size, features_num+1, alldata);

	alglib::dfbuildrandomdecisionforest(
    xy, //real_2d_array xy,
    training_size, //ae_int_t npoints,
    features_num, //ae_int_t nvars,
    class_num, //ae_int_t nclasses,
    n_trees, //ae_int_t ntrees,
    1.0, //double r,
    info,
    df, //decisionforest& df,
    dfrep //dfreport& rep
  );

	is_free = false;
	// fclose(pfile);
	delete features_line;
	delete class_line;

	printf("RandomForestClassifier::train_on_file() finished training !\n");
	return true;
}

bool RandomForestClassifier::save_model(const char* model_file)
{
	FILE * pfile = fopen(model_file, "wb");
	if  ( NULL == pfile ) { 
		printf ("RandomForestClassifier::save_model() no such directort %s\n", model_file);
		return false;
	}
  int temp[2];
  temp[0] = features_num;
  temp[1] = class_num;
  fwrite (temp, sizeof(int), 2, pfile);
  std::string s_out;
  alglib::dfserialize (df, s_out);
  fwrite (s_out.c_str(), sizeof(char), s_out.size(), pfile);
	fclose (pfile);
	printf ("RandomForestClassifier::save_model() : saved model successful !!\n");
	return true;
}

bool RandomForestClassifier::load_model(const char* model_file)
{
	free_model();
	FILE * pfile = fopen(model_file, "rb");
	if ( NULL == pfile ) { 
		printf ("RandomForestClassifier::load_model() error in opening %s\n", model_file);
		return false;
	}
	// // char *pend = NULL;
	char* buffer = NULL;
	// const size_t lp = ftell (pfile);
	fseek (pfile, 0, SEEK_END);
	const size_t length = ftell (pfile);
  rewind (pfile);
  buffer = (char*) malloc (sizeof(char)*length);
  if (buffer == NULL) {
  	printf("RandomForestClassifier::load_model() : can't allocate memory for buffer\n");
  	exit (1);
  }

  int temp[2];
	if ( 0 != fread (temp, sizeof(int), 2, pfile) ) {
    features_num = temp[0];
    class_num = temp[1];
		printf ("RandomForestClassifier::load_model() \
			features_num = %ld, class_num = %ld\n", features_num, class_num);
	}
	else {
		printf ("RandomForestClassifier::load_model() : wrong model file ---- error in \
			reading first line\n");
		return false;
	}

  size_t result = fread (buffer, 1, length, pfile);
  if (result > length) {
  	printf ("andomForestClassifer::load_model() : Reading error\n"); 
  	return false;
  }
  std::string s_in(buffer);
	alglib::dfunserialize(s_in, df);
	is_free = false;
	free (buffer);
	fclose (pfile);
	printf("RandomForestClassifier::load_model() :  load model successful !!\n");
	return true;
}

bool RandomForestClassifier::predicted_category(const double* features, int& res)
{
	alglib::real_1d_array x, y;
	double *yp = new double[class_num];
	y.setcontent (class_num, yp);
	x.setcontent (features_num, features);
	alglib::dfprocess ( df,/*decisionforest*/ x,/*real_1d_array*/ y/*real_1d_array&*/ );
	// printf("x.length = %d, y.length = %d\n", x.length(), y.length());
	res = 0;
	double max = y[0];
	for (int i = 0; i < class_num; ++i) {
		// printf ("%f ", y[i]);
		if ( max < y[i] ) {
			max = y[i];
			res = i;
		}
	}
	delete yp;
	return true;
}

bool RandomForestClassifier::free_model()
{
	if ( is_free ) {
		printf("free model successful!\n");
		return false;
	}
	training_size = 0;
	features_num = 0;
	class_num = 0;
	is_free = true;
	printf("free model successful!\n");
	return is_free;
}
