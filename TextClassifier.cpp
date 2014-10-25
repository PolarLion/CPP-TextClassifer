
#include "TextClassifier.h"
#include "BayesianTextClassifier.h"

#include <cmath>
#include <ctime>
#include <string>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <cstdio>
#include <stdlib.h>
#include <sstream>
#include <string.h>
#include <algorithm>
#include <vector>


#ifdef __linux__
  const char* training_file_path = "io/train.txt";
  const char* model_file_path = "io/model.txt";
  const char* gbk_features_file_path = "io/gbkfeatures.txt";
  const char* utf8_features_file_path = "io/utf8features.txt";
  const char* result_file_path = "io/result.txt";
  const char* classes_file_path = "io/classes.txt";
#else
  const char* training_file_path = "io\\train.txt";
  const char* model_file_path = "io\\model.txt";
  const char* gbk_features_file_path = "io\\gbkfeatures.txt";
  const char* utf8_features_file_path = "io\\utf8features.txt";
  const char* result_file_path = "io\\result.txt";
  const char* classes_file_path = "io\\classes.txt"
#endif


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


TextClassifier::TextClassifier(
  int featurenum, 
  codingtype::SeparaterType encoding_t, 
  classifiertype::ClassifierType classifier_t,
  const char* classifier_config_f 
  )
  : features_num(featurenum)
  , encoding_type(encoding_t)
  , classifier_type(classifier_t)
  , classifier_config_file(classifier_config_f)
  , count_classnum(0)
  , count_training_set(0)
  , prepare_cts(false)
  , classifier(NULL)
{
  if ( classifiertype::ClassifierType::Bayesian == classifier_type ) {
    classifier = new BayesianTextClassifier();
  }
  else {
    //do nothing
  }

  if ( NULL == classifier ) {
    printf ("TextClassifier::TextClassifier(): can't allocate memory for classifier\n");
  }
  load_features();
  std::ofstream outfile(training_file_path);
  if ( outfile.fail() ) {
    printf("TextClassifier::TextClassifier(): error in opening &s\n", training_file_path);
  }
  outfile.close();
}


TextClassifier::~TextClassifier()
{
  classifier->free_model();
  if ( NULL != classifier )
    delete classifier;
  classifier = NULL;
}

void TextClassifier::load_features()
{
  std::ifstream infile;
  if (codingtype::UTF8 == encoding_type)
    infile.open(utf8_features_file_path);
  else if(codingtype::GBK == encoding_type)
    infile.open(gbk_features_file_path);

  if (infile.fail()) {
    printf("TextClassifier::load_features(): error open %s\n", gbk_features_file_path);
    return;
  }
  int i = 0;
  while (!infile.eof() && i < features_num) {
    std::string line;
    std::getline(infile, line);
    std::vector<std::string> v;
    separater(line, v, encoding_type);
    for (auto p = v.begin(); p != v.end(); ++p) {
      if (p->size() > 0) {
        features[i] = *p;
        ++i;
        break;
      }
    }
  }
  printf ("TextClassifier::load_features(): features num = %d, size of features = %d\n", features_num, i);
  infile.close();
}

void TextClassifier::train()
{
  classifier->train_on_file (training_file_path);
  printf ("TextClassifier::training_on_file() : finished training on file\n");
  classifier->save_model (model_file_path);
  printf ("TextClassifier::training_on_file() : finished saving model\n");
  classifier->free_model ();
  printf ("TextClassifier::training_on_file() : finished free model\n");
}

bool TextClassifier::load_data()
{
  load_features();
  classifier->load_model (model_file_path);
  load_classes();
  return true;
}

void TextClassifier::prepare_classname_to_string()
{
  for (int i = 0; i < count_classnum; ++i) {
    std::string str;
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

void TextClassifier::add_train_data(const std::string& classname, const std::string& buffer)
{
  if ( !prepare_cts) {
    prepare_classname_to_string();
  }
  std::unordered_map<std::string, int> bag;
  std::vector<std::string> tok;
  separater(buffer, tok, encoding_type);
  int count_word = 0;
  std::for_each (tok.begin(), tok.end(), [&bag, &count_word](std::string s){
    bag[s]++;
    count_word++;});

  //printf ("%d, %d\n", bag.size(), count_word);

  std::ofstream outfile(training_file_path, std::ios::app);
  if ( outfile.fail() ) {
    printf("TextClassifier::add_train_data: error in opening &s\n", training_file_path);
    return;
  }

  for (int i = 0; i < features_num; ++i) {
    auto tp = bag.find(features[i]);
    if (tp != bag.end()) {
      outfile << tp->second << " ";// / (double)count_word << " ";
      // std::cout << tp->second / (double)count_word<< " ";
    }
    else {
      outfile << 0 << " ";
    }
  }
  outfile << std::endl << class_to_string_map[classname_to_int(classname)] << std::endl;
  count_training_set++;
  outfile.close();
}

void TextClassifier::preprocessor()
{
  std::ifstream infile(training_file_path);
  if (infile.fail()) {
    printf("TextClassifier::preprocessor: error in opening train.txt\n");
    return;
  }
  infile.seekg (0, infile.end);
  int length = infile.tellg();
  infile.seekg (0, infile.beg);
  char *buffer = new char[length];
  if (nullptr == buffer) {
    printf("TextClassifier::preprocessor:allocate memory error\n");
    return;
  }
  infile.read(buffer, length);
  infile.close();

  std::ofstream outfile(training_file_path, std::fstream::binary);
  if (outfile.fail()) {
    printf("TextClassifier::preprocessor: error in opening train.txt\n");
    delete buffer;
    buffer = nullptr;
    return;
  }
  sprintf(first_trainfile_line, "%d %d %d\n", count_training_set, features_num, count_classnum);
  outfile.write(first_trainfile_line, strlen(first_trainfile_line));
  outfile.write(buffer, length);
  outfile.close();
  printf("finish making train.txt\n");
  printf("finish free memory\n");
  printf("clear training set\n");
  save_classes();
  if (nullptr != buffer) {
    delete buffer;
    printf("delete buffer successed\n");
  }
}

void TextClassifier::save_classes() const
{
  if (classname_int.size() < 1) {
    printf ("TextClassifier::save_classes : no classes\n");
    return;
  }
  std::ofstream outfile(classes_file_path);
  if (outfile.fail()) {
    printf ("TextClassifier::save_classes: can't open file %s\n", classes_file_path);
    return;
  }

  std::for_each(classname_int.begin(), classname_int.end(), 
    [&outfile](std::pair<std::string, int> p){ outfile << p.first << " " << p.second << std::endl;});
  outfile.close();
  printf("save classes successed\n");
}

void TextClassifier::add_classname(const std::string& classname)
{
  if (classname_int.find(classname) != classname_int.end()) 
    return;
  int_classname[count_classnum] = classname;
  classname_int[classname] = count_classnum;
  count_classnum++;
}

bool TextClassifier::load_classes()
{
  int_classname.clear();
  classname_int.clear();
  std::ifstream infile(classes_file_path);
  if (infile.fail()) {
    printf("can't open classesfile : %s\n", classes_file_path);
    return false;
  }
  while (!infile.eof()) {
    std::string line;
    std::getline(infile, line);
    if (line.size() < 1)
      break;
    // printf ("%s\n", line.c_str());
    std::string s1;
    std::string s2;
    int num = -1;
    bool state = 0;
    for (auto p = line.begin(); p != line.end(); ++p) {
      if (0 == state) {
        if (*p != 32)
          s1 += *p;
        else
          state = 1;
      }
      else if (1 == state) {
        if (*p >= '0' && *p <= '9')
          s2 += *p;
        else {
          std::stringstream ss(s2);
          ss >> num;
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
      classname_int[s1] = num;
      int_classname[num] = s1;
      // printf ("%s %d\n", s1.c_str(), num);
    }
  }
  infile.close();
  for (auto p = classname_int.begin(); p != classname_int.end(); ++p) {
    printf ("%s %d\n", (p->first).c_str(), p->second);
  }
  // printf("heeeeeeeeeeeeeeeeeeeeeeeeeee\n");
  return true;
}

const char* TextClassifier::predicted_category(const char* data) const
{
  int res;
  std::unordered_map<std::string, int> bag;
  std::vector<std::string> tok;
  separater(data, tok, encoding_type);
  int count_word = 0;
  for (auto p = tok.begin(); p != tok.end(); ++p) {
    bag[*p]++;
    ++count_word;
  }
  double *fv = new double[features_num];
  for (int i = 0; i < features_num; ++i) {
    auto p = bag.find (features[i]);
    if ( p != bag.end() ) {
      fv[i] = p->second;// / (double)count_word;
    }
    else 
      fv[i] = 0.0;
  }
  classifier->predicted_category (fv, res);
  delete fv;
  return int_to_classname(res).c_str();
}

void TextClassifier::show_model()
{
  classifier->show_model();
}


















// void TextClassifier::add_train_data(const char* classname, char *data)
// {
//   // if ( !prepare_cts) {
//   //   prepare_classname_to_string();
//   // }
//   std::unordered_map<std::string, int> bag;

//   int count_word = 0;
  
//   std::ofstream outfile(training_file_path, std::ios::app);
//   if ( outfile.fail() ) {
//     printf("TextClassifier::add_train_data(): error in opening &strain.txt\n", training_file_path);
//     outfile.close();
//     return;
//   }
//   const char *dropchar = " \t.,!?;-+";
//   char* temp = NULL;
//   char* t2 = NULL;
//   temp = strtok_r(data, dropchar, &t2);
//   while ( NULL != temp ) {
//     bag[temp]++;
//     ++count_word;
//     temp = strtok_r(NULL, dropchar, &t2);
//   }

//   // for (auto p = bag.begin(); p != bag.end(); ++p) {
//   //   std::cout << p->first << " " << p->second << std::endl;
//   // }

//   for (int i = 0; i < features_num; ++i) {
//     std::unordered_map<std::string, int>::iterator p = bag.find(features[i]);
//     if ( p != bag.end()) {
//       outfile << (double)p->second / count_word << " ";
//       std::cout << (double)p->second / count_word << " ";
//     }
//     else {
//       outfile << 0 << " ";
//       std::cout << 0 << " ";
//     }
//   }

//   // outfile << std::endl << class_to_string_map[classname_to_int(classname)] << std::endl;
//   count_training_set++;
//   outfile.close();
// }
