#include "SVMTextClassifier.h"

#include "svm.h"
#include <cmath>
#include <ctime>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <chrono>
#include <cstdio>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
//#include <boost/tokenizer.hpp>

using std::string;
using std::cout;
using std::endl;
using std::vector;

using std::unordered_map;

const char SPACE = 32;

void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char)
{
  std::unordered_set<char> drops;
  std::for_each(drop_char.begin(), drop_char.end(), [&drops](char c){drops.insert(c);});
  std::string temp;
  for(auto p = s.begin(); p != s.end(); ++p) {
    if ( drops.end() != drops.find(*p)) {
      if ( !temp.empty() )
        tokens.push_back(temp);
      temp.clear();
    }
    else {
      temp += *p;
    }
  }
  if ( !temp.empty() )
    tokens.push_back(temp);
  temp.clear();
}


SVMTextClassifier::SVMTextClassifier(const string& working_path, int featuresnum, codingtype::SeparaterType encoding_type)
  : work_path(working_path)
  , features_num(featuresnum)
  , encoding(encoding_type)
  , count_training_set(0)
  , prepare_cts(false)
  , model(nullptr)
  , count_classnum(0)
{
  load_features();
  std::ofstream outfile(work_path+"train.txt");
  if ( outfile.fail() ) {
    printf("SVMTextClassifier::add_train_data: error in opening &strain.txt\n", work_path.c_str());
    return;
  }
  outfile.close();
}

SVMTextClassifier::SVMTextClassifier(const string& working_path, codingtype::SeparaterType encoding_type)
  : work_path(working_path)
  , features_num(FEATURE_NUMBER)
  , encoding(encoding_type)
  , count_training_set(0)
  , prepare_cts(false)
  , model(nullptr)
  , count_classnum(0)
{
  load_features();
  std::ofstream outfile(work_path+"train.txt");
  if ( outfile.fail() ) {
    printf("SVMTextClassifier::add_train_data: error in opening &strain.txt\n", work_path.c_str());
    return;
  }
  outfile.close();
}

SVMTextClassifier::~SVMTextClassifier()
{
  printf("~SVMTextClassifier()\n");
  svm_free_and_destroy_model(&model);
}

bool SVMTextClassifier::load_features()
{
  if (features.size() > 0)
    features.clear();
  std::ifstream infile;
  if (codingtype::UTF8 == encoding)
    infile.open(work_path+"utf8features.txt");
  else if(codingtype::GBK == encoding)
    infile.open(work_path+"gbkfeatures.txt");
  if (infile.fail()) {
    printf("SVMTextClassifier::load_features error open features.txt\n");
    return false;
  }

  while (!infile.eof() && features.size() < features_num) {
    string line;
    std::getline(infile, line);
    vector<string> v;
    separater(line, v, encoding);
    for (auto p = v.begin(); p != v.end(); ++p) {
      if (p->size() > 0) {
        features.push_back(*p);
        break;
      }
    }
  }
  cout << "feature : "<< features.size() << " features num : " << features_num << endl;
  infile.close();
  return true;
}

void SVMTextClassifier::prepare_classname_to_string()
{
  for (int i = 0; i < count_classnum; ++i) {
    string str;
    for (int j = 0; j < count_classnum; ++j) {
      if (i == j) {
        str += "1 ";
      }
      else {
        str += "0 ";
      }
    }
    class_to_string_map[i] = str;
  }
  prepare_cts = true;
}

void SVMTextClassifier::add_train_data(const string& classname, const string& buffer)
{
  if ( !prepare_cts) {
    prepare_classname_to_string();
  }
  std::unordered_map<string, int> bag;
  vector<string> tok;
  separater(buffer, tok, encoding);
  int count_word = 0;
  std::for_each (tok.begin(), tok.end(), [&bag, &count_word](std::string s){
    bag[s]++;
    count_word++;});

  std::ofstream outfile(work_path+"train.txt", std::ios::app);
  if ( outfile.fail() ) {
    printf("SVMTextClassifier::add_train_data: error in opening &strain.txt\n", work_path.c_str());
    return;
  }

  int i = 0;
  for (auto p = features.begin(); p != features.end(); ++p) {
    auto tp = bag.find(*p);
    if (tp != bag.end()) {
      outfile << tp->second << " ";
    }
    else {
      outfile << 0 << " ";
    }
    ++i;
  }
  outfile << std::endl << class_to_string_map[classname_to_int(classname)] << std::endl;
  count_training_set++;
  outfile.close();
}

void SVMTextClassifier::preprocessor()
{
  std::ifstream infile(work_path+"train.txt");
  if (infile.fail()) {
    printf("SVMTextClassifier::preprocessor: error in opening train.txt\n");
    return;
  }
  infile.seekg (0, infile.end);
  int length = infile.tellg();
  infile.seekg (0, infile.beg);
  char *buffer = new char[length];
  if (nullptr == buffer) {
    printf("SVMTextClassifier::preprocessor:allocate memory error\n");
    return;
  }
  infile.read(buffer, length);
  infile.close();

  std::ofstream outfile(work_path+"train.txt", std::fstream::binary);
  if (outfile.fail()) {
    printf("SVMTextClassifier::preprocessor: error in opening train.txt\n");
    delete buffer;
    buffer = nullptr;
    return;
  }
  sprintf(first_trainfile_line, "%d %d %d\n", count_training_set, features.size(), count_classnum);
  outfile.write(first_trainfile_line, strlen(first_trainfile_line));
  outfile.write(buffer, length);
  outfile.close();
  printf("finish making train.txt\n");
  printf("finish free memory\n");
  printf("clear training set\n");
  features.clear();
  save_classes();
  if (nullptr != buffer) {
    delete buffer;
    printf("delete buffer successed\n");
  }
}

bool SVMTextClassifier::train_on_file(const std::string filename, struct svm_parameter* param)
{
  struct svm_problem prob;
  std::ifstream infile(filename);
  if (infile.fail()) {
    printf("SVMTextClassifier::train_on_file:open file error %s\n", filename.c_str());
    return false;
  }
  std::string seq(" .,\n\r\t");
  int dimension = 0;
  int class_num = 0;
  string line;
  getline(infile, line);
  printf("SVMTextClassifier::train_on_file: first line is %s\n", line.c_str());
  std::vector<std::string> tok;
  char_separator(tok, line, seq);
  auto p = tok.begin();
  if (p != tok.end()) {
    prob.l = atoi(p->c_str());
    printf("SVMTextClassifier::train_on_file: number of training_set: %d\n", prob.l);
    if (prob.l <= 0) {
      printf("SVMTextClassifier::train_on_file: number of training set is lower than 0\n");
    }
    else {
      prob.x = new svm_node*[prob.l];
      if (nullptr == prob.x) {
        printf("can't allocate memory\n");
      }
    }
  }
  else {
    printf("wrong train file\n");
  }
  p++;
  if (p != tok.end()) {
    dimension = atoi(p->c_str());
    printf("SVMTextClassifier::train_on_file: features dimension %d\n", dimension);
    if (dimension <= 0) {
      printf("number of training set is lower than 0\n");
    }
  }
  else {
    printf("wrong train file\n");
  }
  p++;
  if (p != tok.end()) {
    class_num = atoi(p->c_str());
    cout << "number of classes : " << class_num << endl; 
    if (class_num <= 0) {
      printf("number of classes is lower than 0\n");
    }
    else {
      prob.y = new double[prob.l];
      if (nullptr == prob.y) {
        printf("can't allocate memory\n");
      }
    }
  }
  else {
    printf("wrong train file\n");
  }
  int current_index = 0;
  while (!infile.eof()) {
    string line;
    getline(infile, line);
    if (line.size() < 2) {
      continue;
    }
    std::vector<std::string> tok0;
    char_separator(tok0, line, seq);
    struct svm_node *x_space = new svm_node[dimension+1];
    auto p = tok0.begin();
    for (int i = 0; i < dimension; ++i) {
      x_space[i].index = i+1;
      x_space[i].value = atof(p->c_str());
      ++p;
    }
    x_space[dimension].index = -1;
    prob.x[current_index] = x_space;
    getline(infile, line);
    std::vector<std::string> tok1;
    char_separator(tok1, line, seq);
    p = tok1.begin();
    for (int i = 0; i < class_num; ++i) {
      if (1 == atoi(p->c_str())) {
        prob.y[current_index] = i;
      }
      ++p;
    }
    ++current_index;
  }
  infile.close();
  printf("infile close\n");
  printf("starting training \n");
  model = svm_train(&prob, param);
  return true;
}

void SVMTextClassifier::train_svm()
{
  struct svm_parameter param; //Ä£ÐÍ²ÎÊý
  param.svm_type = C_SVC;
  //param.kernel_type = LINEAR;
  param.kernel_type = RBF;
  param.degree = 3;
  param.gamma = 0.0001;
  //param.nu = 0.5;
  param.nu = 0;
  param.cache_size = 100;
  //param.C = 0.001;
  param.C = 15;
  //param.
  param.eps = 1e-5;
  param.p = 0.1;
  param.shrinking = 1;
  param.probability = 0;
  param.nr_weight = 0;
  param.weight_label = NULL;
  param.weight = NULL;
  //struct svm_problem prob;

  train_on_file(work_path+"train.txt", &param);
  svm_save_model((work_path+"svm_model.txt").c_str(), model);
  svm_destroy_param(&param);
  svm_free_and_destroy_model(&model);
  printf("svm free model\n");
  model = nullptr;
}

bool SVMTextClassifier::load_svm()
{
  model = svm_load_model((work_path+"svm_model.txt").c_str());
  if (nullptr == model) {
    printf("load svm model error\n");
    return false;
  }
  return true;
}

bool SVMTextClassifier::load_classes()
{
  int_classname.clear();
  classname_int.clear();
  std::ifstream infile(work_path+"classes.txt");
  if (infile.fail()) {
    printf("can't open classesfile : %s\n", (work_path+"classes.txt").c_str());
    return false;
  }
  while (!infile.eof()) {
    string line;
    std::getline(infile, line);
//cout << line << endl;
    string s1;
    string s2;
    short num = -1;
    bool state = 0;
    for (auto p = line.begin(); p != line.end(); ++p) {
      if (0 == state) {
        if (*p != SPACE)
          s1 += *p;
        else
          state = 1;
      }
      else if (1 == state) {
//cout << "p " << *p << " ";
        if (*p >= '0' && *p <= '9')
          s2 += *p;
        else {
          std::stringstream ss(s2);
          ss >> num;
//cout << "nun : " << num << endl;
          break;
        }
      }
      else
        break;
    }
    if (num < 0) {
      std::stringstream ss(s2);
      ss >> num;
    }
    if (s1.size() > 0 && num > -1) {
//cout << s1 << endl;
      classname_int[s1] = num;
      int_classname[num] = s1;
    }
  }
  infile.close();
//cout << "classes number : " << classname_int.size() << endl;
  return true;
}

bool SVMTextClassifier::load_data()
{
  if (load_features() && load_svm() && load_classes()) {
    printf("load all data success\n");
    return true;
  }
  return false;
}

string SVMTextClassifier::classification(const string& buffer) const
{
  short type = 0;
  std::unordered_map<string, int> bag;
  vector<string> tok;
  separater(buffer, tok, encoding);
  int count_word = 0;
  for (auto p = tok.begin(); p != tok.end(); ++p) {
    bag[*p]++;
    ++count_word;
  }
  struct svm_node *x_space = new svm_node[features.size()+1];
  int i = 0;
  for (auto p = features.begin(); p != features.end(); ++p) {
    auto tp = bag.find(*p);
    if (tp != bag.end()) {
      x_space[i].index = i+1;
      x_space[i].value = tp->second;
    }
    else {
      x_space[i].index = i+1;
      x_space[i].value = 0.0;
    }
    i++;
  }
  x_space[features.size()].index = -1;
  string res = int_to_classname(svm_predict(model, x_space));
  if (nullptr != x_space)
    delete x_space;
  return res;
}

void SVMTextClassifier::save_classes() const
{
  if (classname_int.size() < 1) {
    printf ("SVMTextClassifier::save_classes : no classes\n");
    return;
  }
  std::ofstream outfile(work_path+"classes.txt");
  if (outfile.fail()) {
    printf ("SVMTextClassifier::save_classes: can't open file %s\n", (work_path+"classes.txt").c_str());
    return;
  }

  std::for_each(classname_int.begin(), classname_int.end(), 
    [&outfile](std::pair<string, int> p){ outfile << p.first << SPACE << p.second << endl;});
  outfile.close();
  printf("save classes successed\n");
}

void SVMTextClassifier::add_classname(const string& classname)
{
  if (classname_int.find(classname) != classname_int.end()) 
    return;
  int_classname[count_classnum] = classname;
  classname_int[classname] = count_classnum;
  count_classnum++;
}
