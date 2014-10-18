#pragma once

#include "TextClassifier.h"
#include "BayesianTextClassifier.h"

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


CTextClassifier::CTextClassifier(
  const std::string& work_path, 
  int featurenum, 
  codingtype::SeparaterType encoding_t, 
  classifiertype::ClassifierType classifier_t
  )
  : working_path(work_path)
  , features_num(featurenum)
  , encoding_type(encoding_t)
  , classifier_type(classifier_t)
  , count_classnum(0)
  , count_training_set(0)
  , classifier(NULL)
{
  printf ("in CTextClassifier:CTextClassifier()\n");
  if ( classifiertype::ClassifierType::Bayesian == classifier_type ) {
    classifier = new CBayesianTextClassifier();
  }
  else {
    //do nothing
  }

  if ( NULL == classifier ) {
    printf ("can't allocate memory for classifier\n");
    exit(0);
  }
  printf ("out CTextClassifier::CTextClassifier()\n");
}

CTextClassifier::~CTextClassifier()
{
  printf ("in CTextClassifier::~CTextClassifier()\n");
  if ( NULL != classifier )
    delete classifier;
  classifier = NULL;
  printf ("out CTextClassifier::~CTextClassifier()\n");
}

void CTextClassifier::add_training_data(const std::string& data)
{
  
}
