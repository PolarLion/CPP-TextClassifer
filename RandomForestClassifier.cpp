// #pragma once

#include "RandomForestClassifier.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <cmath>

using namespace randomforestc;

RandomForestClassifer::RandomForestClassifer()
	: n_trees (0)
{
	printf("RandomForestClassifer\n");
}

RandomForestClassifer::RandomForestClassifer(const char* config_file)
	: n_trees (0)
{
	init_model (config_file);
}

RandomForestClassifer::~RandomForestClassifer()
{
	free_model();
	printf ("RandomForestClassifer::~RandomForestClassifer()\n");
}

void RandomForestClassifer::init_model(const char* config_file)
{
	// printf ("RandomForestClassifer:init_model() \n");
	//nothing to be inited
}

void RandomForestClassifer::train_on_file(const char* training_file)
{
	printf("CRandomForestClassifer::train_on_file() : start reading training file\n");
	free_model();
	alglib::real_2d_array xy;
	alglib::dfreport dfrep;
	alglib::ae_int_t info;
	
	FILE * pfile = fopen(training_file, "r");
	if ( NULL == pfile ) { 
		printf ("RandomForestClassifer::train_on_file() error in opening %s\n", training_file);
		exit(1);
	}

	char* pend = NULL;

	//get first line : trainging size, features number, class number
	char first_line[200] = {0};
	if ( NULL != fgets (first_line, 200, pfile) ) {
		training_size = strtol (first_line, &pend, 10);
		features_num = strtol (pend, &pend, 10);
		class_num = strtol (pend, NULL, 10);
		printf ("RandomForestClassifer::train_on_file() :\
			training_size = %d, features_num = %d, class_num = %d\n", training_size, features_num, class_num);
		is_free = false;
	}
	else {
		printf ("RandomForestClassifer::train_on_file() : wrong traing file ---- error in \
			reading first line\n");
		exit(1);
	}

	double *alldata = (double*) malloc (sizeof(double)*(features_num + class_num)*training_size);
	if ( NULL == alldata ) {
		printf ("RandomForestClassifer::train_on_file() can't allocate memory for alldata\n");
		exit(1);
	}
	// //edit the bayesiantable 
	const int features_line_size = features_num * 10+ 1;
	const int class_line_size = class_num * 2 + 1;
	char *features_line = new char[features_line_size];
	char *class_line = new char[class_line_size];
	if ( NULL == features_line || NULL == class_line ) {
		printf ("RandomForestClassifer::train_on_file() error allocate memory for char[]\n");
		exit (1);
	}


	int count_line = 1;
	int current_index = 0;
	while ( NULL != fgets (features_line, features_line_size, pfile)
	 	&& count_line < training_size * 2 + 1) {
		count_line++;
		if ( NULL == fgets (class_line, class_line_size, pfile) ) {
			printf ("RandomForestClassifer::train_on_file() : wrong traing file\
				read line %d\n", count_line);
			break;
		}
		else { 
			count_line++; 
		}
		int class_index = 0;
			alldata[current_index++] = strtod (features_line, &pend);
		for (int j = 1; j < features_num - 1; ++j) {
			alldata[current_index++] = strtod (pend, &pend);
		}
		alldata[current_index++] = strtod (pend, NULL);

		alldata[current_index++] = strtod (class_line, &pend);
		for (int i = 1; i < class_num - 1; ++i) {
			alldata[current_index++] = strtod (pend, &pend);
		}
		alldata[current_index++] = strtod (pend, NULL);
		//get every feature value
	}

	xy.setcontent (training_size, features_num+class_num, alldata);
	printf("CRandomForestClassifer::train_on_file() : start training training file\n");

	alglib::dfbuildrandomdecisionforest(
    xy, //real_2d_array xy,
    training_size, //ae_int_t npoints,
    features_num, //ae_int_t nvars,
    class_num, //ae_int_t nclasses,
    n_trees, //ae_int_t ntrees,
    0.8, //double r,
    info,
    df, //decisionforest& df,
    dfrep //dfreport& rep
  );
 //  // for (int i = 0; i < 2; ++i) {
 //  //   for (int j = 0; j < class_num; ++j) {
 //  //     printf ("%f ", bayesiantable[i].v[j]);
 //  //   }
 //  //   printf ("\n");
 //  // }
	
	// //p(ti|ci) = (1 + tf(ti, ci)) / (features size + sum of tf(tj, ci));
	// double *denominator = new double[class_num];
	// if ( NULL == denominator ) {
	// 	printf ("RandomForestClassifer::train_on_file() error allocate memory for double[]\n");
	// 	exit(1);
	// }
	// for (int i = 0; i < class_num; ++i) {
	// 	denominator[i] = 0;
	// 	for (int j = 0; j < features_num; ++j) {
	// 		denominator[i] += bayesiantable[j].v[i];
	// 	}
	// 	denominator[i] += features_num;
	// }
	// for (int i = 0; i < features_num; ++i) {
	// 	for (int j = 0; j < class_num; ++j) {
	// 		bayesiantable[i].v[j] += 1.0;
	// 		bayesiantable[i].v[j] /= denominator[j];
	// 		// printf ("%f ", bayesiantable[i].v[j]);
	// 	}
	// 	// printf ("\n");
	// }
	// is_free = false;
	// fclose(pfile);
	// delete denominator;
	// delete features_line;
	// delete class_line;

	// // for (int i = 0; i < features_num; ++i) {
	// // 	printf ("%d : ", i);
	// // 	for (int j = 0; j < class_num; ++j) {
	// // 		printf ("%f ", bayesiantable[i].v[j]);
	// // 	}
	// // 	printf ("\n");
	// // }
	printf("RandomForestClassifer::train_on_file() finished training !\n");
}

void RandomForestClassifer::save_model(const char* model_file)
{
	FILE * pfile = fopen(model_file, "wb");
	if ( NULL == pfile ) { 
		printf ("RandomForestClassifer::save_model() no such directort %s\n", model_file);
		exit (1);
	}
  int temp[2];
  temp[0] = features_num;
  temp[1] = class_num;
  fwrite (temp, sizeof(int), 2, pfile);
  std::string s_out;
  alglib::dfserialize (df, s_out);
  fwrite (s_out.c_str(), sizeof(char), s_out.size(), pfile);
	fclose (pfile);
	printf ("CRandomForestClassifer::save_model() : saved model successful !!\n");
}

void RandomForestClassifer::load_model(const char* model_file)
{
	free_model();
	FILE * pfile = fopen(model_file, "rb");
	if ( NULL == pfile ) { 
		printf ("RandomForestClassifer::load_model() error in opening %s\n", model_file);
		exit(1);
	}
	// // char *pend = NULL;
  int temp[2];
	if ( 0 != fread (temp, sizeof(int), 2, pfile) ) {
    features_num = temp[0];
    class_num = temp[1];
		printf ("RandomForestClassifer::load_model() \
			features_num = %d, class_num = %d\n", features_num, class_num);
	}
	else {
		printf ("RandomForestClassifer::load_model() : wrong model file ---- error in \
			reading first line\n");
		exit(1);
	}

	char* buffer = NULL;
	const size_t length = ftell (pfile);
  rewind (pfile);
  buffer = (char*) malloc (sizeof(char)*length);
  if (buffer == NULL) {
  	printf("RandomForestClassifer::load_model() : can't allocate memory for buffer\n");
  	exit (1);
  }
  size_t result = fread (buffer, 1, length, pfile);
  if (result != length) {
  	printf ("andomForestClassifer::load_model() : Reading error\n"); 
  	exit (1);
  }
  std::string s_in(buffer);
	alglib::dfunserialize(s_in, df);
	is_free = false;
	free (buffer);
	fclose (pfile);
	printf("RandomForestClassifer::load_model() :  load model successful !!\n");
}

void RandomForestClassifer::predicted_category(const double* features, int& res)
{
	// printf("CRandomForestClassifer::predicted_category()\n");
	// for (int i = 0; i < features_num; ++i) {
	// 	printf("%f ", features[i]);
	// }
	// printf("\n");
	alglib::real_1d_array x, y;
	x.setcontent (features_num, features);

	alglib::dfprocess ( df,/*decisionforest*/ x,/*real_1d_array*/ y/*real_1d_array&*/ );
	res = 0;
	double max = y[2];
	for (int i = 0; i < class_num; ++i) {
		printf ("%f ", y[i]);
		if ( max < y[i] ) {
			max = y[i];
			res = i;
		}
	}

	// printf ("res : %d\n", res);
}

void RandomForestClassifer::free_model()
{
	if ( is_free ) {
		printf("free model successful!\n");
		return;
	}
	training_size = 0;
	features_num = 0;
	class_num = 0;
	is_free = true;
	printf("free model successful!\n");
}