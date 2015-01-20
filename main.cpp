
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

using namespace std;
using std::chrono::system_clock;
// using std::chrono::system_clock;


void keep_window_open()
{
	char c;
	printf("enter a character to exit\n");
	cin >> c;
}

using std::ifstream;

void test_tc (int num);
void fool_test ();
void test_Bayesizn ();
void test_RFC ();
void test_LRC ();
void test_TC (int num);
void test_ttc (long num);
void test_log ();

int main(int argc, char **argv)
{
	//test_log ();
  // test_tc(10);
  char *pend = NULL;
  //printf("%s\n", argv[0]);
  long num = strtol (argv[1], &pend, 10);
//  for (size_t i = 100; i < 3500; i += 100) {
//    test_ttc (i);
//  }
  // test_tc(num);
  test_ttc (num);
  // test_LRC();
  // test_TC (num);
  // fool_test();
  // test_RFC();
  // keep_window_open();
  // printf ("%f\n", pow(0.5, 0.5));
  // test_Bayesizn();
  return 0;
}


void test_log ()
{
	RunTimeLog t("io/log.txt");
	const char* str = "doges";
	int n = 12;
	t.write_log (t.LOGTYPE_ERROR, "This are %d %s", n, str);

}


void test_ttc (long num)
{
  string path = "../training_set/easy_train/";
  // string path = "../training_set/test_train/";

  // string path = "../training_set/sougou_train/";
  //TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::RandomForest, "");
  TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::LogitRegression, "");
  tc->auto_test (path, "io/result.xml", 0.5);
  delete tc;
}


void test_TC (int num)
{
  TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::RandomForest, "");
  string trpath = "../training_set/easy_train/";
  string tepath = "../testing_set/easy_test/";
  tc->add_training_set (trpath);
  tc->preprocessor ();
  tc->train ();
  tc->load_data ();
  tc->batch_predict (tepath, "io/result.xml");
  delete tc;
}


void test_LRC() 
{
  logitregressionc::LogitRegressionClassifier * p = new logitregressionc::LogitRegressionClassifier();
  printf("1\n");
  p->train_on_file("io/train.txt");

  printf("2\n");
  p->save_model("io/model.txt");
  p->free_model();
  p->load_model("io/model.txt");
  double d1[] = {0.9, 0.8, 0.1, 0.3};
  double d2[] ={0.1, 0.2, 0.9, 0.9};
  int c = 0;
  p->predicted_category(d1, c);
  printf ("predicted category is %d\n", c);
  c = 0;
  p->predicted_category(d2, c);
  printf ("predicted category is %d\n", c);
  delete p;
}


void test_RFC()
{
  randomforestc::RandomForestClassifier* p = new randomforestc::RandomForestClassifier();
  p->train_on_file("io/train.txt");
  p->save_model("io/model.txt");
  p->free_model();
  p->load_model("io/model.txt");
  double d1[] = {0.9, 0.8, 0.4, 0.3};
  double d2[] ={0.1, 0.2, 0.9, 0.4};
  int c = 0;
  p->predicted_category(d1, c);
  printf ("predicted category is %d\n", c);
  c = 0;
  p->predicted_category(d2, c);
  printf ("predicted category is %d\n", c);
  delete p;
}

void test_Bayesizn()
{
  Classifier *p = new bayesianc::BayesianTextClassifier();
  p->train_on_file("io/train.txt");
  p->save_model("io/model.txt");
  p->free_model();
  p->load_model("io/model.txt");
  // double d1[] = {0.4, 0.2, 0.4, 0.3, 0.1};
  // double d2[] ={0.1, 0.2, 0.2, 0.4, 0.6};
  // int c;
  // p->predicted_category(d1, c);
  // printf ("predicted category is %d\n", c);
  // p->predicted_category(d2, c);
  // printf ("predicted category is %d\n", c);
  delete p;
}

void fool_test()
{

}


void test_tc(int num)
{
  const double ratio = 0.7;
  // TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::Bayesian, "");
  TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::RandomForest, "");
  // TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::LogitRegression, "");
  printf ("1\n");
  const string working_path = "io/";
  unordered_map<string, vector<string>> class_map;
  // string path = "../training_set/easy_train/";
  // string path = "../training_set/test_train/";

  string path = "../training_set/sougou_train/";

  vector<string> dirs;
  get_dirs(path, dirs);
  for (auto p = dirs.begin(); p != dirs.end(); ++p) {
  	tc->add_classname(*p);	
  	string spath = path+*p+"/";
  	cout << "process : "<< spath << " : ";
  	vector<string> files;
  	get_files(spath, files);
  	class_map[*p] = files;
  	cout << files.size() << endl;
  }
  printf ("2\n");
  dirs.clear();
  for (auto p = class_map.begin(); p != class_map.end(); ++p) {
  	// int count = 0;
    // printf("%d\n", count);
  	const int maxindex = p->second.size() * ratio;
	  for (int i = 0; i < maxindex; ++i) {
	    string spath = path + p->first + "/" + p->second[i];
	    ifstream infile(spath);
	    //cout << "process : "<< spath<< endl;
	    if (infile.fail()) {
  		  printf("no such file %s\n", p->second[i].c_str());
  		  return;
      }
      infile.seekg(0, infile.end);
      int length = infile.tellg();
      infile.seekg(0, infile.beg);
      char* buffer = new char[length+1];
      buffer[length] = 0;
      infile.read(buffer, length);
      // cout << buffer << endl;
      tc->add_train_data(p->first, buffer);
      // ++count;
      delete buffer;
      infile.close();
  	}
  }
  printf ("3\n");
  tc->preprocessor();
  printf ("4\n");
  tc->train();
  printf ("5\n");
  // tc->save_model();
  tc->load_data();

 //  printf ("hehehehehehehe\n");

  //记录被标记为该类别的样本数（用于计算precision）
  unordered_map<string, int> c_info1;
  //记录被正确标注的样本数
  unordered_map<string, int> c_info2;
  unordered_map<string, double> recall;
  int count_all_right = 0;
  int count_all = 0;
  for (auto p = class_map.begin(); p != class_map.end(); ++p) {
	   c_info2[p->first] = 0;
  }

  for (auto p = class_map.begin(); p != class_map.end(); ++p) {
  	int count_right = 0;
  	int count = 0;
  	const int min = p->second.size() * ratio;
  	for (int i = min; i < p->second.size(); ++i) {
  	// for (int i = 0; i < min; ++i) {
	    string spath = path + p->first + "/" + p->second[i];
	    ifstream infile(spath);
	    if (infile.fail()) {
    		printf("no such file %s\n", p->second[i].c_str());
    		return;
      }
	    infile.seekg(0, infile.end);
	    int length = infile.tellg();
	    infile.seekg(0, infile.beg);
	    char* buffer = new char[length+1];
	    buffer[length] = 0;
	    infile.read(buffer, length);

	    string sc = tc->predicted_category(buffer);
	    ++count;
	    ++count_all;
	    c_info1[sc]++;
	    //cout << p->first << endl;
	    if(p->first == sc) {
    		++count_right;
    		++count_all_right;
	    }
	    delete buffer;
	    infile.close();
    }
  	c_info2[p->first] = count_right;
  	cout << endl;
  	cout << p->first <<  " ： right number : " << count_right << endl;
  	//cout << "Recall Rate : " << count_right / (double)vf.size() << endl;
  	recall[p->first] = count_right / (double)count;
	}
	int all = 0;
	cout << c_info1.size() << endl;
	ofstream outfile(working_path+"result.txt", ios::app);
	if (outfile.fail()) {
	    printf("open outifle error\n");
	    return;
	}
	system_clock::time_point today = system_clock::now();
	std::time_t tt = system_clock::to_time_t (today);
	outfile << string(ctime(&tt)) << "features : \t" << num << endl;
	outfile << "class name\t"<<"training set\t"<<"testing set\t"<<"precision\t"<<"recall rate\t"<<"F value\n";
	for (auto p = c_info1.begin(); p != c_info1.end(); ++p) {
	    all += p->second;
	    const double precision = c_info2[p->first] / (double)p->second;
	    cout << "class " << p->first << " : \n";
	    cout << "precision : " << precision << endl;
	    cout << "recall rate : " << recall[p->first] << endl;
	    cout << "F : " << precision * recall[p->first] * 2 /(recall[p->first] + precision) << endl;
	    outfile << p->first << "\t" << int(class_map[p->first].size() * ratio) << "\t"
		<< (int) (class_map[p->first].size() * (1.0-ratio)) << "\t" << precision << "\t" << recall[p->first] << "\t"
		<< precision * recall[p->first] * 2 /(recall[p->first] + precision) << endl;
	}
	const double drecall = count_all_right / (double)count_all;
	outfile << "total :\t" << drecall << endl << endl;
	//const double precision = count_all_right / (double)all;
	//cout << "total recall rate : " << drecall << endl;
	//cout << "total precision : " << precision << endl;
	//cout << "total F : " << 2 * precision * drecall /(precision + drecall) << endl;
	cout << "total accuracy :\t" << drecall << endl;


  // tc->show_model();
	return ;
 }



