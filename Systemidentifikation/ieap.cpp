#include <cmath>
#include <fstream>
#include <string>

double Heaviside(double t, double tau = 0.0) {
  return (t >= tau) ? 1.0 : 0.0;
}

double OutputFunctionFirstOrder(double t, double amplitude, double initialt, double Kp, double Tp, double Td, double offset) {
  double delay = Td + initialt;
  double timeFactor = t - delay;
  return amplitude * Kp * Heaviside(timeFactor) * (1.0 - std::exp(-timeFactor / Tp)) + offset;
}

double EAP(double plant, double model) {
  return std::abs(model - plant);
}

void DataFile(const std::string& inputFileName, const std::string& outputFileName,
    double amplitude, double initialt, double Kp, double Tp, double Td, double offset,
    double modelKp, double modelTp, double modelTd) {
  std::ifstream dataFile(inputFileName);
  std::ofstream outFile(outputFileName);

  if (!dataFile || !outFile) return;

  double t, yplant;
  while (dataFile >> t >> yplant) {
    double ymodelH = OutputFunctionFirstOrder(t, amplitude, initialt, Kp, Tp, Td, offset);
    double ymodelM = OutputFunctionFirstOrder(t, amplitude, initialt, modelKp, modelTp, modelTd, offset);

    outFile << t << '\t' << EAP(yplant, ymodelH) << '\t' << EAP(yplant, ymodelM) << '\n';
  }
}

int main() {
  const std::string inputFileName = "./Data/Electric-10.csv";
  const std::string outputFileName = "./Data/error.dat";

  const double amplitude = 110.0;
  const double initialt = 170.0;
  const double Kp = 2.0;
  const double Tp = 303.0;
  const double Td = 30.0;
  const double offset = 20.0;

  const double modelKp = 2.000635678404079;
  const double modelTp = 298.8828592591657;
  const double modelTd = 36.91;

  DataFile(inputFileName, outputFileName, amplitude, initialt, Kp, Tp, Td, offset, modelKp, modelTp, modelTd);
  return 0;
}
