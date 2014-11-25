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
	virtual void train_on_file(const char* training_file) = 0;
	virtual void save_model(const char* model_file) = 0;
  virtual void load_model(const char* model_file) = 0;
	virtual void predicted_category(const double* features, int& res) = 0;
  virtual void free_model() = 0;
  virtual void show_model() {}
};