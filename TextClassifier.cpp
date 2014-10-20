#pragma once

#include "TextClassifier.h"
#include "BayesianTextClassifier.h"

#include <cmath>
#include <ctime>
#include <string>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <algorithm>
#include <vector>

//#include <boost/tokenizer.hpp>

const char SPACE = 32;


// void char_separator(std::vector<std::string>& tokens, const char* str, const char* drop_char)
// {
//   bool drop[256] = {false};
//   int length = sizeof(drop_char);
//   for (int i = 0; i < length; i++) {
//     drop[(int)drop_char[i]] = true;
//   }
//   length = sizeof(str);
//   std::string temp;
//   for (int i = 0; i < length; i++) {
//     if (drop[(int)str[i]] && !temp.empty()) {
//       tokens.push_back(temp);
//       temp.clear();
//     }
//     else {
//     }
//   }
// }

#ifdef __linux__
  const char* training_file_path = "io/train.txt";
  const char* model_file_path = "io/model.txt";
  const char* gbk_features_file_path = "io/gbkfeatrues.txt";
  const char* utf8_features_file_path = "io/utf8features.txt";
  const char* result_file_path = "io/result.txt";
#else
  const char* training_file_path = "io\\train.txt";
  const char* model_file_path = "io\\model.txt";
  const char* gbk_features_file_path = "io\\gbkfeatrues.txt";
  const char* utf8_features_file_path = "io\\utf8features.txt";
  const char* result_file_path = "io\\result.txt";
#endif


TextClassifier::TextClassifier(
  int featurenum, 
  codingtype::SeparaterType encoding_t, 
  classifiertype::ClassifierType classifier_t
  )
  : features_num(featurenum)
  , encoding_type(encoding_t)
  , classifier_type(classifier_t)
  , count_classnum(0)
  , count_training_set(0)
  , cts(false)
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
    printf("TextClassifier::TextClassifier(): error in opening &strain.txt\n", training_file_path);
  }
  outfile.close();
}


TextClassifier::~TextClassifier()
{
  printf ("TextClassifier::~TextClassifier()\n");
  if ( NULL != classifier )
    delete classifier;
  classifier = NULL;
  printf ("TextClassifier::~TextClassifier()\n");
}


void TextClassifier::load_features()
{
  std::ifstream infile;
  if (codingtype::UTF8 == encoding_type)
    infile.open(utf8_features_file_path);
  else if(codingtype::GBK == encoding_type)
    infile.open(gbk_features_file_path);

  if (infile.fail()) {
    printf("TextClassifier::load_features(): error open features.txt\n");
    return;
  }
  int i = 0;
  while (!infile.eof() && i < features_num) {
    std::string line;
    std::getline(infile, line);
    if (line.size() > 0){
      features[i] = line;
      ++i;
    }
  }
  printf ("out TextClassifier::load_features(): features num = %d, size of features = %d\n", features_num, i);
  // std::cout << "feature : "<< features.size() << " features num : " << features_num << endl;
  infile.close();
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


void TextClassifier::add_train_data(const char* classname, char *data)
{
  // if ( !prepare_cts) {
  //   prepare_classname_to_string();
  // }
  std::unordered_map<std::string, int> bag;

  int count_word = 0;
  
  std::ofstream outfile(training_file_path, std::ios::app);
  if ( outfile.fail() ) {
    printf("TextClassifier::add_train_data(): error in opening &strain.txt\n", training_file_path);
    outfile.close();
    return;
  }
  const char *dropchar = " \t.,!?;-+";
  char* temp = NULL;
  char* t2 = NULL;
  temp = strtok_r(data, dropchar, &t2);
  while ( NULL != temp ) {
    bag[temp]++;
    ++count_word;
    temp = strtok_r(NULL, dropchar, &t2);
  }

  // for (auto p = bag.begin(); p != bag.end(); ++p) {
  //   std::cout << p->first << " " << p->second << std::endl;
  // }

  for (int i = 0; i < features_num; ++i) {
    std::unordered_map<std::string, int>::iterator p = bag.find(features[i]);
    if ( p != bag.end()) {
      outfile << (double)p->second / count_word << " ";
      std::cout << (double)p->second / count_word << " ";
    }
    else {
      outfile << 0 << " ";
      std::cout << 0 << " ";
    }
  }

  // outfile << std::endl << class_to_string_map[classname_to_int(classname)] << std::endl;
  count_training_set++;
  outfile.close();
}
