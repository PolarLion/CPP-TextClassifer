#include "BayesianTextClassifier.h"
#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <cmath>
#include <fstream>


#define FIRST_LINE_SIZE 1000000

using namespace bayesianc;

BayesianTextClassifier::BayesianTextClassifier(const char* config_file)
{
}

BayesianTextClassifier::BayesianTextClassifier()
	: bayesiantable(NULL)
{
}

BayesianTextClassifier::~BayesianTextClassifier()
{
	free_model();
	printf ("BayesianTC::~BayesianTextClassifier()\n");
}

bool BayesianTextClassifier::show_bayesiantable() 
{
	if (NULL == bayesiantable) 
		return false;
  for ( long i = 0; i < features_num; ++i ) {
    printf("%ld : \n", i);
    for ( int j = 0; j < class_num; ++j) {
      printf("%f  ", bayesiantable[i].v[j]);
    }
    printf("\n");
  }
  return true;
}

bool BayesianTextClassifier::init_bayesiantable(int f, int c)
{
  bayesiantable = new Item[f];
  if ( NULL != bayesiantable ) {
    is_free = false;
    for (int i = 0; i < f; ++i) {
      bayesiantable[i].v = (double*) malloc (sizeof(double)*c);
      if ( NULL == bayesiantable[i].v ) {
        printf ("BayesianTC::init_bayesiantable() can't allocate memory for bayesiantable[%d].v\n", i);
        return false;
      }
      for (int j = 0; j < c; ++j) {
        bayesiantable[i].v[j] = 0.0;
      }
    }
  } else {
    printf ("BayesianTC::init_bayesiantable() can't allocate memory for bayesiantable[%d]\n", f);
    return false;
  }
  return true;
}

bool BayesianTextClassifier::train_on_file(const char* training_file)
{
	printf("BayesianTC::train_on_file() : start reading training file\n");
	free_model();
	std::ifstream infile (training_file);
	if (infile.fail()) {
		printf ("BayesianTC::train_on_file() error in opening %s\n", training_file);
		return false;
	}

	char* pend = NULL;
	//get first line : trainging size, features number, class number
	//and init the bayesiantable
	char first_line[FIRST_LINE_SIZE];
  if ( !(infile.getline (first_line, FIRST_LINE_SIZE)).fail()) {
		training_size = strtol (first_line, &pend, 10);
		features_num = strtol (pend, &pend, 10);
		class_num = strtol (pend, NULL, 10);
    // free (first_line);
    if ( training_size > 0 && features_num > 0 && class_num > 0 ) {
      printf ("BayesianTC::train_on_file() :\
        training_size = %ld, features_num = %ld, class_num = %ld\n", training_size, features_num, class_num);
    } else {
      printf ("BayesianTC::train_on_file() : error there is a zero in :\
        training_size = %ld, features_num = %ld, class_num = %ld\n", training_size, features_num, class_num);
      return false;
    }
		init_bayesiantable(features_num, class_num);
	}
	else {
		printf ("BayesianTC::train_on_file() : wrong traing file -- error in \
			reading first line\n");
		return false;
	}

	//edit the bayesiantable 
	// size_t features_line_size = 0;
	// size_t class_line_size = 0;
	char features_line[FIRST_LINE_SIZE];
	char class_line[FIRST_LINE_SIZE];
  int *count_class = new int[class_num];
	if ( NULL == count_class) {
		printf ("BayesianTC::train_on_file() error allocate memory for count_class array\n");
		return false;
	}
  for (int i = 0; i < class_num; ++i) { count_class[i] = 0; }

	printf("BayesianTC::train_on_file() : start training training file\n");
	int count_line = 1;
	while ( !(infile.getline (features_line, FIRST_LINE_SIZE)).fail()
	 	&& count_line < training_size * 2 + 1) {
		count_line++;
		if ( (infile.getline (class_line, FIRST_LINE_SIZE)).fail() ){
			printf ("BayesianTC::train_on_file() : wrong traing file\
				read line %d\n", count_line);
			break;
		}
		else { 
			count_line++; 
		}
		int class_index = 0;
		if ( strtol (class_line, &pend, 10) != 1) {
			int i ;
			for (i = 1; i < class_num - 1; ++i) {
				if ( strtol (pend, &pend, 10) == 1 ) {
					class_index = i;
					break;
				}
			}
			if ( class_num - 1 == i && strtol (pend, NULL, 10) == 1 )
				class_index = i;
		}
    // count_class[class_index]+;
		int j = 0;
		//get every feature value
    int d = strtol (features_line, &pend, 10);
    count_class[class_index] += d;
		bayesiantable[j].v[class_index] += d;
		for (j = 1; j < features_num-1; ++j) {
      d = strtol (pend, &pend, 10);
			bayesiantable[j].v[class_index] += d;
      count_class[class_index] += d;
		}
    d = strtol (pend, NULL, 10);
		bayesiantable[j].v[class_index] += d;
    count_class[class_index] += d;
	}

  // free (features_line);
  // free (class_line);
  // show_bayesiantable();
	//p(ti|ci) = (1 + tf(ti, ci)) / (features size + sum of tf(tj, ci));
	double *denominator = new double[class_num];
	if ( NULL == denominator ) {
		printf ("BayesianTC::train_on_file() error allocate memory for double[]\n");
		return false;
	}
	for (int i = 0; i < class_num; ++i) {
		denominator[i] = 0;
		for (int j = 0; j < features_num; ++j) {
      bayesiantable[j].v[i] /= count_class[i];
			denominator[i] += bayesiantable[j].v[i];
		}
		denominator[i] += features_num;
	}
	for (int i = 0; i < features_num; ++i) {
		for (int j = 0; j < class_num; ++j) {
			bayesiantable[i].v[j] += 1.0;
			bayesiantable[i].v[j] /= denominator[j];
		}
	}
	is_free = false;
	// fclose(pfile);
	delete denominator;
  delete count_class;
	printf("BayesianTC::train_on_file() finished training !\n");
	return true;
}

bool BayesianTextClassifier::save_model(const char* model_file)
{
	FILE * pfile = fopen(model_file, "wb");
	if ( NULL == pfile ) { 
		printf ("BayesianTC::save_model() no such directort %s\n", model_file);
		return false;
	}
  int temp[2];
  temp[0] = features_num;
  temp[1] = class_num;
  fwrite (temp, sizeof(int), 2, pfile);
	for (int i = 0; i < features_num; ++i) {
		fwrite (bayesiantable[i].v, sizeof(double), class_num, pfile);
	}
	fclose(pfile);
	printf("CBayesianTextClassifier::save_model() : saved model successful !!\n");
	return true;
}

bool BayesianTextClassifier::load_model(const char* model_file)
{
	free_model();
	FILE * pfile = fopen(model_file, "rb");
	if ( NULL == pfile ) { 
		printf ("BayesianTC::load_model() error in opening %s\n", model_file);
		return false;
	}
	// char *pend = NULL;
  int temp[2];
	if ( 2 == fread (temp, sizeof(int), 2, pfile) ) {
    features_num = temp[0];
    class_num = temp[1];
		printf ("BayesianTC::load_model() \
			features_num = %ld, class_num = %ld\n", features_num, class_num);
		if ( !init_bayesiantable(features_num, class_num) ) {
      printf ("BayesianTC::load_model() : init_bayesiantable() error\n");
      return false;
    }
	}
	else {
		printf ("BayesianTC::load_model() : wrong model file ---- error in \
			reading first line\n");
		return false;
	}

	for (int i = 0; i < features_num; ++i) {
		if ( fread (bayesiantable[i].v, sizeof(double), class_num, pfile) != (size_t)class_num ) {
			printf ("BayesianTC::load_model() : can't get values for line %d v\n", i);
			return false;
		}
	}

	// for (int i = 0; i < features_num; ++i) {
	// 	printf ("%d : ", i);
	// 	for (int j = 0; j < class_num; ++j) {
	// 		printf ("%f ", bayesiantable[i].v[j]);
	// 	}
	// 	printf ("\n");
	// }
	is_free = false;
	fclose(pfile);
	printf("BayesianTC::load_model() :  load model successful !!\n");
	return true;
}

bool BayesianTextClassifier::predicted_category(const double* features, int& res)
{
  if ( class_num <= 1 || features_num < 1 || NULL == bayesiantable ) {
    printf ("BayesianTC::predicted_category() model hasn't been inited yet\n");
    return false;
  }
  double count_word = 0;
  double max = 0.0;
  res = 0;
  for (int j = 0; j < features_num; ++j) {
    count_word += features[j];
  }
  for (int j = 0; j < features_num; ++j) {
    max += log(pow(bayesiantable[j].v[0], features[j] / count_word));
  }

	for (int i = 1; i < class_num; ++i) {
		double temp = 0.0;
		for (int j = 0; j < features_num; ++j) {
			temp += log(pow(bayesiantable[j].v[i], features[j] / count_word));
		}
    if ( max < temp ) {
      max = temp;
      res = i;
    }
	}
	return true;
	// printf ("res : %d\n", res);
}

bool BayesianTextClassifier::free_model()
{
	if ( is_free && NULL == bayesiantable ) {
		printf("free model successful!\n");
		return true;
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
	if ( NULL == bayesiantable ) {
		printf("free model successful!\n");
	}
	return true;
}

bool BayesianTextClassifier::show_model() 
{
  return show_bayesiantable();
}
