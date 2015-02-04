
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <ctime>
#include <cmath>
#include <chrono>
#include "scanfile.h"
// #include "TextClassifier.h"
#include <stdio.h>

#include "RunTimeLog.h"
#include "TextClassifier.h"
#include "BayesianTextClassifier.h"
#include "RandomForestClassifier.h"
#include "LogitRegressionClassifier.h"



void keep_window_open()
{
	char c;
	printf("enter a character to exit\n");
	std::cin >> c;
}

void test_ttc (const char* features_file, const std::string& s = "", double r = 0.8);

void test_features (const char* filename) {
	Features t;
	t.load_features (filename);
}


int main(int argc, char **argv)
{
  //printf("%s\n", argv[0]);
	if (argc < 2) {
		printf ("miss param\n");
		return 0;
	}
	printf ("%d\n", argc);
  //long num = strtol (argv[1], &pend, 10);
	if ( argc == 2) {
		test_features (argv[1]);
		//test_ttc (argv[1]);
	}
	else if (argc == 3) {
		test_ttc (argv[1], std::string(argv[2]));
	}
	else if (argc == 4) {
		char *pend = NULL;
		float r = strtof (argv[3], &pend);
		test_ttc (argv[1], std::string(argv[2]), r);
	}
  return 0;
}



void test_ttc (const char* features_file, const std::string& s, double r)
{
  TextClassifier *tc = new TextClassifier( EncodingType::GBK, classifiertype::ClassifierType::RandomForest, "", features_file);
	if ( s.empty ()) {
		// string path = "../training_set/easy_train/";
		std::string path  = "../training_set/sougou_train/";
		tc->auto_test (path, "io/result.xml", r);
	}
  //TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::LogitRegression, "");
  tc->auto_test (s, "io/result.xml", r);
  delete tc;
}





