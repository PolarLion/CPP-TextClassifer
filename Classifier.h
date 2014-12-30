#pragma once


namespace classifiertype{
  enum ClassifierType{
    Bayesian = 0,
    RandomForest, 
    LogitRegression, 
    SVM,
    NeuralNetwork
  };
}


class Classifier
{
protected:
  long class_num;
  long features_num;
  long training_size;
  bool is_free;
public:
  Classifier() : class_num (0), features_num(0), training_size(0), is_free(true) {}
  virtual ~Classifier(){}
  // virtual void init_model(const char* config_file) = 0;
	virtual bool train_on_file(const char* training_file) = 0;
	virtual bool save_model(const char* model_file) = 0;
  virtual bool load_model(const char* model_file) = 0;
	virtual bool predicted_category(const double* features, int& res) = 0;
  virtual bool free_model() = 0;
  virtual bool show_model() { return true; }
};