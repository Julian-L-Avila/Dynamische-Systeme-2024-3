#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <vector>
#include <string>

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
      const double time_factor = t - (td_ + initial_time_);
      const double response = is_second_order_ ? SecondOrderResponse(time_factor) : FirstOrderResponse(time_factor);
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

double EAP(double plant, double model) { return std::abs(model - plant); }
double EAC(double plant, double model) { return (model - plant) * (model - plant); }

void ProcessData(std::string_view input_file, std::string_view output_file, const std::vector<Model>& models) {
  std::ifstream data_file(input_file.data());
  if (!data_file) return;

  std::ofstream out_file(output_file.data());
  if (!out_file) return;

  double t = 0.0, y_plant = 0.0;
  std::vector<double> eap(models.size(), 0.0);
  std::vector<double> eac(models.size(), 0.0);

  auto integrate_and_write = [&](double current_t, double y_plant) {
    out_file << current_t;
    for (size_t i = 0; i < models.size(); ++i) {
      const double output = models[i].Output(current_t);
      eap[i] += EAP(y_plant, output);
      eac[i] += EAC(y_plant, output);
      out_file << '\t' << eap[i] * 10.0;
    }
    out_file << '\n';
  };

  while (data_file >> t >> y_plant) {
    integrate_and_write(t, y_plant - 20.0);
  }

  std::cout << std::fixed << std::setprecision(15);
  for (size_t i = 0; i < models.size(); ++i) {
    std::cout << models[i].Name() << ":\tIEAP = " << eap[i] * 10.0 << '\t'
      << "IEAC = " << eac[i] * 10.0 << '\n';
  }
}

int main() {
  constexpr std::string_view input_file = "./Data/electric-10.dat";
  constexpr std::string_view output_file = "./Data/error.dat";

  constexpr double amplitude = 110.0, initial_time = 170.0, offset = 0.0;

  std::vector<Model> models = {
    {amplitude, initial_time, 2.0, 303.0, 0.0, 30.0, offset, "Model Hf"},
    {amplitude, initial_time, 2.000635678404079, 298.8828592591657, 0.0, 36.91, offset, "Model Mf"},
    {amplitude, initial_time, 1.989015963633353, 25.0, 290.5659847649038, 14.49, offset, "Model Hs1", true},
    {amplitude, initial_time, 1.989015963633353, 25.57774292268281, 290.0, 14.49, offset, "Model Hs2", true},
    {amplitude, initial_time, 1.989015963633353, 25.57774292268281, 290.5659847649038, 14.49, offset, "Model Ms", true}
  };

  ProcessData(input_file, output_file, models);
  return 0;
}

