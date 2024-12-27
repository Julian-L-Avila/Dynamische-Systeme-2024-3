#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

#include "./Data/parameters.h"

class Model {
  public:
    Model(double amplitude, double initial_time, double kp, double tp1, double tp2, double td, double offset,
        std::string name, bool is_second_order = false)
      : amplitude_(amplitude),
      initial_time_(initial_time),
      kp_(kp),
      tp1_(tp1),
      tp2_(tp2),
      td_(td),
      offset_(offset),
      name_(std::move(name)),
      is_second_order_(is_second_order) {}

    double Output(double t) const {
      double time_factor = t - (td_ + initial_time_);
      double response = is_second_order_ ? SecondOrderResponse(time_factor) : FirstOrderResponse(time_factor);
      return amplitude_ * kp_ * Heaviside(time_factor) * response + offset_;
    }

    const std::string& Name() const { return name_; }

  private:
    static double Heaviside(double t) { return t >= 0.0 ? 1.0 : 0.0; }

    double FirstOrderResponse(double time_factor) const {
      return 1.0 - std::exp(-time_factor / tp1_);
    }

    double SecondOrderResponse(double time_factor) const {
      return 1.0 - tp1_ * std::exp(-time_factor / tp1_) / (tp1_ - tp2_) +
        tp2_ * std::exp(-time_factor / tp2_) / (tp1_ - tp2_);
    }

    const double amplitude_, initial_time_, kp_, tp1_, tp2_, td_, offset_;
    const std::string name_;
    const bool is_second_order_;
};

double ComputeEAP(double plant, double model) {
  return std::abs(model - plant);
}

double ComputeEAC(double plant, double model) {
  return (model - plant) * (model - plant);
}

double ComputePEP(double model, double model_opt) {
  return 100.0 * (model - model_opt) / model_opt;
}

std::vector<double> CalculateErrorMetrics(const std::vector<Model>& models, double t, double y_plant, std::vector<double>& eap, std::vector<double>& eac) {
  std::vector<double> outputs(models.size());
  for (size_t i = 0; i < models.size(); ++i) {
    outputs[i] = models[i].Output(t);
    eap[i] += ComputeEAP(y_plant, outputs[i]);
    eac[i] += ComputeEAC(y_plant, outputs[i]);
  }
  return outputs;
}

void WriteErrorData(std::ofstream& out_file, double t, const std::vector<double>& eap) {
  out_file << t;
  for (const auto& value : eap) {
    out_file << '\t' << value * 10.0;
  }
  out_file << '\n';
}

int FindMinErrorIndex(const std::vector<double>& eap) {
  return std::distance(eap.begin(), std::min_element(eap.begin(), eap.end()));
}

std::vector<double> ComputePEPForAll(const std::vector<double>& eap, int min_index) {
  std::vector<double> pep(eap.size(), 0.0);
  for (size_t i = 0; i < eap.size(); ++i) {
    if (i != static_cast<size_t>(min_index)) {
      pep[i] = ComputePEP(eap[i], eap[min_index]);
    }
  }
  return pep;
}

void WritePerformanceData(const std::vector<Model>& models, const std::vector<double>& eap, const std::vector<double>& eac, const std::vector<double>& pep) {
  std::ofstream performance_file("./Data/performance.dat");
  performance_file << std::fixed << std::setprecision(15);
  performance_file << "Model\tIEAP\tIEAC\tPEP\n";
  for (size_t i = 0; i < models.size(); ++i) {
    performance_file << models[i].Name() << '\t' << eap[i] << '\t'
      << eac[i] << '\t' << pep[i] << '\n';
  }
}

void ProcessData(std::string_view input_file, std::string_view output_file, const std::vector<Model>& models) {
  std::ifstream data_file(input_file.data());
  if (!data_file) return;

  std::ofstream out_file(output_file.data());
  if (!out_file) return;

  double t = 0.0, y_plant = 0.0;
  std::vector<double> eap(models.size(), 0.0);
  std::vector<double> eac(models.size(), 0.0);

  while (data_file >> t >> y_plant) {
    auto outputs = CalculateErrorMetrics(models, t, y_plant, eap, eac);
    WriteErrorData(out_file, t, eap);
  }

  int min_index = FindMinErrorIndex(eap);
  auto pep = ComputePEPForAll(eap, min_index);
  WritePerformanceData(models, eap, eac, pep);
}

int main() {
  constexpr std::string_view input_file = "./Data/electric-10.dat";
  constexpr std::string_view output_file = "./Data/error.dat";

  constexpr double amplitude = 110.0, initial_time = 170.0, offset = 20.0;

  std::vector<Model> models = {
    {kAmplitude, kOnTime, kKpF1, kTpF1, 0.0, kTdF1, kOffset, "Hf1"},
    {kAmplitude, kOnTime, kKpF2, kTpF2, 0.0, kTdF2, kOffset, "Hf2"},
    {kAmplitude, kOnTime, kKpFM, kTpFM, 0.0, kTdFM, kOffset, "Mf"},
    {kAmplitude, kOnTime, kKpS1, kTp1S1, kTp2S1, kTdS1, kOffset, "Hs1", true},
    {kAmplitude, kOnTime, kKpS2, kTp1S2, kTp2S2, kTdS2, kOffset, "Hs2", true},
    {kAmplitude, kOnTime, kKpSM, kTp1SM, kTp2SM, kTdSM, kOffset, "Ms", true},
  };

  ProcessData(input_file, output_file, models);
  return 0;
}

