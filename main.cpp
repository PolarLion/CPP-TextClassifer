
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

#include "TextClassifier.h"
#include "BayesianTextClassifier.h"
#include "RandomForestClassifer.h"


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

void test_tc(int num);
void fool_test();
void test_Bayesizn();
void test_RFC();
void test_TC();

int main(int argc, char **argv)
{
  // test_tc(10);
  char *pend = NULL;
  // printf("%s\n", argv[0]);
  int num = strtol (argv[1], &pend, 10);
  test_tc(num);
  // test_TC();
  // fool_test();
  test_RFC();
  // keep_window_open();
  // printf ("%f\n", pow(0.5, 0.5));
  // test_Bayesizn();
  return 0;
}

void test_TC()
{
  TextClassifier tc(4, codingtype::SeparaterType::GBK, classifiertype::ClassifierType::Bayesian);
}

void test_RFC()
{
  randomforestc::RandomForestClassifer* p = new randomforestc::RandomForestClassifer();
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
  const double ratio = 0.8;
  TextClassifier *tc = new TextClassifier(num, codingtype::GBK, classifiertype::ClassifierType::Bayesian, "");
  printf ("1\n");
  const string working_path = "io/";
  unordered_map<string, vector<string>> class_map;
  string path = "../training_set/easy_train/";

  // string path = "../training_set/sougou_train/";

  vector<string> dirs;
  get_dirs(path, dirs);
  for (auto p = dirs.begin(); p != dirs.end(); ++p) {
  	tc->add_classname(*p);	
  	string spath = path+*p+"/";
  	cout << "process : "<< spath << endl;
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
  printf ("6\n");

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
  	// for (int i = min; i < p->second.size(); ++i) {
  	for (int i = 0; i < min; ++i) {
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



