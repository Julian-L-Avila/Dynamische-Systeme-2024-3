#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

class Model {
  public:
    Model(double amplitude, double initial_time, double kp, double tp1, double tp2, double td, double offset)
      : amplitude_(amplitude),
      initial_time_(initial_time),
      kp_(kp),
      tp1_(tp1),
      tp2_(tp2),
      td_(td),
      offset_(offset) {}

    double OutputFunction(double t) const {
      const double time_factor = t - (td_ + initial_time_);
      return amplitude_ * kp_ * Heaviside(time_factor) * (1.0 - std::exp(-time_factor / tp1_)) + offset_;
    }

    double OutputFunctionSecond(double t) const {
      const double time_factor = t - (td_ + initial_time_);
      return amplitude_ * kp_ * Heaviside(time_factor) *
        (1.0 - tp1_ * std::exp(-time_factor / tp1_) / (tp1_ - tp2_) +
         tp2_ * std::exp(-time_factor / tp2_) / (tp1_ - tp2_)) +
        offset_;
    }

  private:
    static constexpr double Heaviside(double t, double tau = 0.0) { return t >= tau; }

    const double amplitude_, initial_time_, kp_, tp1_, tp2_, td_, offset_;
};

class TrapezoidalIntegrator {
  public:
    double Integrate(double t, double prev_t, double curr_y, double prev_y) {
      return sum_ += (t - prev_t) * (prev_y + curr_y) * 0.5;
    }

    double GetSum() const { return sum_; }

  private:
    double sum_ = 0.0;
};

double EAP(double plant, double model) { return std::abs(model - plant); }

void ProcessData(const std::string& input_file, const std::string& output_file,
    const Model& model_h, const Model& model_m, const Model& model_ms) {
  std::ifstream data_file(input_file);
  if (!data_file) return;

  std::ofstream out_file(output_file);
  if (!out_file) return;

  TrapezoidalIntegrator integrator_h, integrator_m, integrator_ms;
  double t = 0.0, prev_t = 0.0, y_plant = 0.0;
  double prev_h = 0.0, prev_m = 0.0, prev_ms = 0.0;

  auto integrate_and_write = [&](double eap_h, double eap_m, double eap_ms) {
    out_file << t << '\t'
      << integrator_h.Integrate(t, prev_t, eap_h, prev_h) << '\t'
      << integrator_m.Integrate(t, prev_t, eap_m, prev_m) << '\t'
      << '3' << '\t'
      << '4' << '\t'
      << integrator_ms.Integrate(t, prev_t, eap_ms, prev_ms) << '\n';
    prev_h = eap_h;
    prev_m = eap_m;
    prev_ms = eap_ms;
    prev_t = t;
  };

  while (data_file >> t >> y_plant) {
    y_plant -= 20.0;
    integrate_and_write(EAP(y_plant, model_h.OutputFunction(t)),
        EAP(y_plant, model_m.OutputFunction(t)),
        EAP(y_plant, model_ms.OutputFunctionSecond(t)));
  }

  std::cout << std::fixed << std::setprecision(15)
    << "Integrated EAP (Model Hf): " << integrator_h.GetSum() << '\n'
    << "Integrated EAP (Model Mf): " << integrator_m.GetSum() << '\n'
    << "Integrated EAP (Model Ms): " << integrator_ms.GetSum() << '\n';
}

int main() {
  constexpr std::string_view input_file = "./Data/electric-10.dat";
  constexpr std::string_view output_file = "./Data/error.dat";

  constexpr double amplitude = 110.0, initial_time = 170.0, offset = 0.0;
  constexpr double kp_h = 2.0, tp_h = 303.0, td_h = 30.0;
  constexpr double kp_m = 2.000635678404079, tp_m = 298.8828592591657, td_m = 36.91;
  constexpr double kp_ms = 1.989015963633353, tp1_ms = 25.57774292268281, tp2_ms = 290.5659847649038, td_ms = 14.49;

  const Model model_h(amplitude, initial_time, kp_h, tp_h, 0.0, td_h, offset);
  const Model model_m(amplitude, initial_time, kp_m, tp_m, 0.0, td_m, offset);
  const Model model_ms(amplitude, initial_time, kp_ms, tp1_ms, tp2_ms, td_ms, offset);

  ProcessData(input_file.data(), output_file.data(), model_h, model_m, model_ms);
  return 0;
}

