
#include "LogitRegressionClassifier.h"
#include <string>
#include <stdio.h>
#include <string.h>
#include <cmath>

using namespace logitregressionc;

LogitRegressionClassifier::LogitRegressionClassifier()
{
	printf("LogitRegressionClassifier\n");
}

LogitRegressionClassifier::LogitRegressionClassifier(const char* config_file)
{
	init_model (config_file);
}

LogitRegressionClassifier::~LogitRegressionClassifier()
{
	free_model();
	printf ("LogitRegressionClassifier::~LogitRegressionClassifier()\n");
}

void LogitRegressionClassifier::init_model(const char* config_file)
{
	// printf ("LogitRegressionClassifier:init_model() \n");
	//nothing to be inited
}

void LogitRegressionClassifier::train_on_file(const char* training_file)
{
	printf("CLogitRegressionClassifier::train_on_file() : start reading training file\n");
	free_model();
	alglib::real_2d_array xy;
	alglib::mnlreport lmrep;
	alglib::ae_int_t info;
	
	FILE * pfile = fopen(training_file, "r");
	if ( NULL == pfile ) { 
		printf ("LogitRegressionClassifier::train_on_file() error in opening %s\n", training_file);
		exit(1);
	}

	char* pend = NULL;

	//get first line : trainging size, features number, class number
	char first_line[200] = {0};
	if ( NULL != fgets (first_line, 200, pfile) ) {
		training_size = strtol (first_line, &pend, 10);
		features_num = strtol (pend, &pend, 10);
		class_num = strtol (pend, NULL, 10);
		printf ("LogitRegressionClassifier::train_on_file() :\
			training_size = %ld, features_num = %ld, class_num = %ld\n", training_size, features_num, class_num);
		is_free = false;
	}
	else {
		printf ("LogitRegressionClassifier::train_on_file() : wrong traing file ---- error in \
			reading first line\n");
		exit (1);
	}

	double *alldata = (double*) malloc (sizeof(double)*(features_num + 1)*training_size);
	if ( NULL == alldata ) {
		printf ("LogitRegressionClassifier::train_on_file() can't allocate memory for alldata\n");
		exit (1);
	}
	// //edit the bayesiantable 
	size_t features_line_size = features_num * 10+ 1;
	size_t class_line_size = class_num * 2 + 1;
	char *features_line = new char[features_line_size];
	char *class_line = new char[class_line_size];
	if ( NULL == features_line || NULL == class_line ) {
		printf ("LogitRegressionClassifier::train_on_file() error allocate memory for char[]\n");
		exit (1);
	}

	long count_line = 1;
	int current_index = 0;
	printf("LogitRegressionClassifier::train_on_file() : start training training file\n");
	while ( -1 != getline (&features_line, &features_line_size, pfile)
	 	&& count_line < training_size * 2 + 1) {
		// printf("features_line %s\n", features_line);
		count_line++;
		if ( -1 == getline (&class_line, &class_line_size, pfile) ) {
			printf ("LogitRegressionClassifier::train_on_file() : wrong traing file\
				read line %ld\n", count_line);
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
		// printf("train  %ld %s= %ld\n", count_line, class_line, class_index);
		alldata[current_index++] = (double)class_index;
		//get every feature value
	}

	xy.setcontent (training_size, features_num+1, alldata);

	// for (int i = 0; i < training_size; ++i) {
	// 	for (int j = 0; j < features_num+1; ++j) {
	// 		/* code */
	// 		printf("%f ", xy[i][j]);
	// 	}
	// 	printf("\n");
	// }

	alglib::mnltrainh(
    xy, //real_2d_array xy,
    training_size, //ae_int_t npoints,
    features_num, //ae_int_t nvars,
    class_num, //ae_int_t nclasses,
    info,
    lm, //decisionforest& lm,
    lmrep //lmreport& rep
  );
  if ( -2 == info ) {
  	printf("LogitRegressionClassifier::train_on_file() \
  		there is a point with class number outside of [0..NClasses-1].\n");
  	exit (1);
  }
  else if ( -1 == info ) {
  	printf("LogitRegressionClassifier::train_on_file() \
  		incorrect parameters was passed (NPoints<NVars+2, NVars<1, NClasses<2).\n");
  	exit (1);
  }
	is_free = false;
	fclose(pfile);
	delete features_line;
	delete class_line;
	printf("LogitRegressionClassifier::train_on_file() finished training !\n");
}

void LogitRegressionClassifier::save_model(const char* model_file)
{
	alglib::real_2d_array a;
	FILE * pfile = fopen(model_file, "wb");
	if ( NULL == pfile ) { 
		printf ("LogitRegressionClassifier::save_model() no such directort %s\n", model_file);
		exit (1);
	}
  long temp[2];
  temp[0] = features_num;
  temp[1] = class_num;
  fwrite (temp, sizeof(long), 2, pfile);
  alglib::mnlunpack (lm, a, features_num, class_num);
  // printf("%ld\n", a.rows());
  // fwrite (s_out.c_str(), sizeof(char), s_out.size(), pfile);
  // int i = 0;
  // printf("%ld %ld\n", a.rows(), a.cols());
  long length = ( class_num - 1 ) * ( features_num + 1 );

  // printf("save model length = %ld features_num = %ld\n", length, features_num);

  double *temp2 = (double*) malloc (sizeof(double) * length);
  if ( NULL == temp2 ) {
  	printf("LogitRegressionClassifier::save_model() can't allocate memory\n");
  	exit (1);
  }
  int index = 0;
  for (int i = 0; i < class_num-1; ++i) {
  	for (int j = 0; j < features_num+1; ++j) {
  		temp2[index++] = a[i][j];
  		// printf("%f ", a[i][j]);
  	}
  	// printf("\n");
  }
  // printf("\n");
  // for (int i = 0; i < length; ++i) {
  // 	printf("%f ", temp2[i]);
  // }
  // printf("\n");

  fwrite (temp2, sizeof (double), length, pfile);
  free (temp2);
	fclose (pfile);
	printf ("CLogitRegressionClassifier::save_model() : saved model successful !!\n");
}

void LogitRegressionClassifier::load_model(const char* model_file)
{
	free_model();
	alglib::real_2d_array a;
	FILE * pfile = fopen(model_file, "rb");
	if ( NULL == pfile ) { 
		printf ("LogitRegressionClassifier::load_model() error in opening %s\n", model_file);
		exit (1);
	}

  long temp[2];
	if ( 0 != fread (temp, sizeof(long), 2, pfile) ) {
    features_num = temp[0];
    class_num = temp[1];
		printf ("LogitRegressionClassifier::load_model() \
			features_num = %ld, class_num = %ld\n", features_num, class_num);
	}
	else {
		printf ("LogitRegressionClassifier::load_model() : wrong model file ---- error in \
			reading first line\n");
		exit (1);
	}

	size_t length = (features_num+1) * (class_num-1);
	double* temp2 = (double*) malloc (sizeof(double) * length);

	// printf("save model length = %ld features_num = %ld\n", length, features_num);

	if ( 0 != fread (temp2, sizeof (double), length, pfile) ) {
		a.setcontent (class_num-1, features_num+1, temp2);
	}
	 // for (int i = 0; i < class_num-1; ++i) {
  // 	for (int j = 0; j < features_num+1; ++j) {
  // 		// temp2[i * (class_num-1) + j] = a[i][j];
  // 		printf("%f ", a[i][j]);
  // 	}
  // 	printf("\n");
  // }


  // std::string s_in(buffer);
  alglib::mnlpack (a, features_num, class_num, lm);
	is_free = false;
	free (temp2);
	fclose (pfile);
	printf("LogitRegressionClassifier::load_model() :  load model successful !!\n");
}

void LogitRegressionClassifier::predicted_category(const double* features, int& res)
{
	// printf("CLogitRegressionClassifier::predicted_category()\n");
	// for (int i = 0; i < features_num; ++i) {
	// 	printf("%f ", features[i]);
	// }
	// printf("\n");
	alglib::real_1d_array x, y;
	double *yp = new double[class_num];
	// for (int i = 0; i < class_num; ++i) {
	// 	yp[i] = 0.0;
	// }
	y.setcontent (class_num, yp);
	x.setcontent (features_num, features);
	// for (int i = 0; i < features_num; ++i) {
	// 	printf("%f ", x[i]);
	// }
	// printf("\n");
	alglib::mnlprocess (lm,/*decisionforest*/ x,/*real_1d_array*/ y/*real_1d_array&*/ );
	// printf("x.length = %ld, y.length = %ld\n", x.length(), y.length());
	res = 0;
	double max = y[0];
	for (int i = 0; i < class_num; ++i) {
		// printf ("%f ", y[i]);
		if ( max < y[i] ) {
			max = y[i];
			res = i;
		}
	}

	// printf ("res : %ld\n", res);
	delete yp;
}

void LogitRegressionClassifier::free_model()
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