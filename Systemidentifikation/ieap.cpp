#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>

class Model {
 public:
  Model(double amplitude, double initial_time, double kp, double tp1, double tp2, double td, double offset, bool is_second_order = false)
      : amplitude_(amplitude), initial_time_(initial_time), kp_(kp), tp1_(tp1), tp2_(tp2), td_(td), offset_(offset), is_second_order_(is_second_order) {}

  double Output(double t) const {
    const double time_factor = t - (td_ + initial_time_);
    return amplitude_ * kp_ * Heaviside(time_factor) *
           (is_second_order_ ? secondOrderResponse(time_factor) : firstOrderResponse(time_factor)) + offset_;
  }

 private:
  static double Heaviside(double t) { return t >= 0.0 ? 1.0 : 0.0; }

  double firstOrderResponse(double time_factor) const {
    return 1.0 - std::exp(-time_factor / tp1_);
  }

  double secondOrderResponse(double time_factor) const {
    return 1.0 - tp1_ * std::exp(-time_factor / tp1_) / (tp1_ - tp2_) +
           tp2_ * std::exp(-time_factor / tp2_) / (tp1_ - tp2_);
  }

  const double amplitude_, initial_time_, kp_, tp1_, tp2_, td_, offset_;
  const bool is_second_order_;
};

class TrapezoidalIntegrator {
 public:
  double Integrate(double t, double prev_t, double curr_y, double prev_y) {
    sum_ += (t - prev_t) * (prev_y + curr_y) * 0.5;
    return sum_;
  }

  double GetSum() const { return sum_; }

 private:
  double sum_ = 0.0;
};

double EAP(double plant, double model) { return std::abs(model - plant); }
double EAC(double plant, double model) { return (model - plant) * (model - plant); }

void ProcessData(std::string_view input_file, std::string_view output_file, const std::vector<Model>& models) {
  std::ifstream data_file(input_file.data());
  if (!data_file) return;

  std::ofstream out_file(output_file.data());
  if (!out_file) return;

  std::vector<TrapezoidalIntegrator> integrators(models.size());
  std::vector<TrapezoidalIntegrator> integrators_eac(models.size());

  double t = 0.0, prev_t = 0.0, y_plant = 0.0;
  std::vector<double> prev_outputs(models.size(), 0.0);
  std::vector<double> prev_eac(models.size(), 0.0);

  auto integrate_and_write = [&](const std::vector<double>& eap, const std::vector<double>& eac) {
    out_file << t;
    for (size_t i = 0; i < models.size(); ++i) {
      out_file << '\t' << integrators[i].Integrate(t, prev_t, eap[i], prev_outputs[i]);
      integrators_eac[i].Integrate(t, prev_t, eac[i], prev_eac[i]);
    }
    out_file << '\n';

    prev_outputs = eap;
    prev_eac = eac;
    prev_t = t;
  };

  while (data_file >> t >> y_plant) {
    y_plant -= 20.0;

    std::vector<double> eap(models.size());
    std::vector<double> eac(models.size());
    for (size_t i = 0; i < models.size(); ++i) {
      eap[i] = EAP(y_plant, models[i].Output(t));
      eac[i] = EAC(y_plant, models[i].Output(t));
    }

    integrate_and_write(eap, eac);
  }

  std::cout << std::fixed << std::setprecision(15);
  for (size_t i = 0; i < models.size(); ++i) {
    std::cout << "Integrated EAP (Model " << i << "): " << integrators[i].GetSum() << '\n'
              << "Integrated EAC (Model " << i << "): " << integrators_eac[i].GetSum() << '\n';
  }
}

int main() {
  constexpr std::string_view input_file = "./Data/electric-10.dat";
  constexpr std::string_view output_file = "./Data/error.dat";

  constexpr double amplitude = 110.0, initial_time = 170.0, offset = 0.0;

  std::vector<Model> models = {
      {amplitude, initial_time, 2.0, 303.0, 0.0, 30.0, offset},  // Model Hf
      {amplitude, initial_time, 2.000635678404079, 298.8828592591657, 0.0, 36.91, offset},  // Model Mf
      {amplitude, initial_time, 1.989015963633353, 25.0, 290.5659847649038, 14.49, offset, true},  // Model Hs1
      {amplitude, initial_time, 1.989015963633353, 25.57774292268281, 290.0, 14.49, offset, true},  // Model Hs2
      {amplitude, initial_time, 1.989015963633353, 25.57774292268281, 290.5659847649038, 14.49, offset, true}  // Model Ms
  };

  ProcessData(input_file, output_file, models);
  return 0;
}

