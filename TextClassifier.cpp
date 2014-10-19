#pragma once

#include "TextClassifier.h"
#include "BayesianTextClassifier.h"

#include <cmath>
#include <ctime>
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <cstdio>
#include <stdlib.h>
#include <algorithm>
#include <vector>

//#include <boost/tokenizer.hpp>

const char SPACE = 32;

void char_separator(std::vector<std::string>& tokens, const std::string& s, const std::string& drop_char)
{
  std::unordered_set<char> drops;
  std::for_each(drop_char.begin(), drop_char.end(), [&drops](char c){drops.insert(c);});
  std::string temp;
  for(std::string::const_iterator p = s.begin(); p != s.end(); ++p) {
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

#ifdef __linux__
  const char* training_file_path = "io/train.txt";
  const char* model_file_path = "io/model.txt";
  const char* features_file_path = "io/featrues.txt";
  const char* result_file_path = "io/result.txt";
#else
  const char* training_file_path = "io\\train.txt";
  const char* model_file_path = "io\\model.txt";
  const char* features_file_path = "io\\featrues.txt";
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
  printf ("in TextClassifier::~TextClassifier()\n");
  if ( NULL != classifier )
    delete classifier;
  classifier = NULL;
  printf ("out TextClassifier::~TextClassifier()\n");
}


void TextClassifier::load_features()
{

}


void TextClassifier::add_train_data(const char* classname, const char* buffer)
{

  // if ( !prepare_cts) {
  //   prepare_classname_to_string();
  // }
  // std::unordered_map<string, int> bag;
  // vector<string> tok;
  // separater(buffer, tok, encoding);
  // int count_word = 0;
  // std::for_each (tok.begin(), tok.end(), [&bag, &count_word](std::string s){
  //   bag[s]++;
  //   count_word++;});

  std::ofstream outfile(training_file_path);
  if ( outfile.fail() ) {
    printf("TextClassifier::add_train_data(): error in opening &strain.txt\n", training_file_path;
    return;
  }

  // int i = 0;
  // for (auto p = features.begin(); p != features.end(); ++p) {
  //   auto tp = bag.find(*p);
  //   if (tp != bag.end()) {
  //     outfile << tp->second << " ";
  //   }
  //   else {
  //     outfile << 0 << " ";
  //   }
  //   ++i;
  // }
  // outfile << std::endl << class_to_string_map[classname_to_int(classname)] << std::endl;
  // count_training_set++;
  outfile.close();
}
