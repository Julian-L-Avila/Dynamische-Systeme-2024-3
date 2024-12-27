#include <gnuplot-iostream/gnuplot-iostream.h>
#include <cmath>
#include "../Data/parameters.h"

constexpr char kDataFile[] = "../Data/electric-10.dat";
constexpr char kErrorDataFile[] = "../Data/error.dat";
constexpr char kOutputFile[] = "fit-plots.tex";

void ConfigureGnuplot(Gnuplot &gp) {
  gp << "set term tikz standalone header '\\usepackage{siunitx}'\n";
  gp << "set output '" << kOutputFile << "'\n";
  gp << "set grid\n";
  gp << "set key Left left top spacing 1.1\n";
  gp << "set xlabel '$t \\, [\\si{s}]$'\n";
  gp << "set ylabel '$T \\, [\\si{C}]$'\n";
  gp << "set xrange [0:1200]\n";
  gp << "set yrange [0:250]\n";
}

void DefineFunctions(Gnuplot &gp) {
  gp << "heaviside(x) = (x >= 0)\n";
  gp << "OutputFOD(Amplitude, OnTime, Offset, Kp, Tp, Td, x) = "
    "Amplitude * Kp * heaviside(x - (Td + OnTime)) * (1.0 - exp(-(x - (Td + OnTime)) / Tp)) + Offset\n";

  gp << "HF1(x) = OutputFOD(" << kAmplitude << ", " << kOnTime << ", " << kOffset << ", "
    << kKpF1 << ", " << kTpF1 << ", " << kTdF1 << ", x)\n";
  gp << "HF2(x) = OutputFOD(" << kAmplitude << ", " << kOnTime << ", " << kOffset << ", "
    << kKpF2 << ", " << kTpF2 << ", " << kTdF2 << ", x)\n";
  gp << "MF(x) = OutputFOD(" << kAmplitude << ", " << kOnTime << ", " << kOffset << ", "
    << kKpFM << ", " << kTpFM << ", " << kTdFM << ", x)\n";

  gp << "OutputSOD(Amplitude, OnTime, Offset, Kp, Tp1, Tp2, Td, x) = "
    "Amplitude * Kp * heaviside(x - (Td + OnTime)) * (1.0 - Tp1 * exp(-(x - (Td + OnTime)) / Tp1) / (Tp1 - Tp2)"
    "+ Tp2 * exp(-(x - (Td + OnTime)) / Tp2) / (Tp1 - Tp2)) + Offset\n";

  gp << "HS1(x) = OutputSOD(" << kAmplitude << ", " << kOnTime << ", " << kOffset << ", "
    << kKpS1 << ", " << kTp1S1 << "," << kTp2S1 << "," << kTdS1 << ", x)\n";
  gp << "HS2(x) = OutputSOD(" << kAmplitude << ", " << kOnTime << ", " << kOffset << ", "
    << kKpS2 << ", " << kTp1S2 << "," << kTp2S2 << "," << kTdS2 << ", x)\n";
  gp << "MS(x) = OutputSOD(" << kAmplitude << ", " << kOnTime << ", " << kOffset << ", "
    << kKpSM << ", " << kTp1SM << "," << kTp2SM << "," << kTdSM << ", x)\n";
}

void PlotData(Gnuplot &gp) {
  gp << "set title 'Modelo de Primer Orden con Delay'\n";
  gp << "plot '" << kDataFile << "' title 'Data' with linespoints lc -1, "
    "HF1(x) title '$H_{f_1}$' with lines lc 1 dt 2 lw 2, "
    "HF2(x) title '$H_{f_2}$' with lines lc 2 dt 2 lw 2, "
    "MF(x) title '$M_{f}$' with lines lc 3 lw 1\n";

  gp << "set title 'Modelo de Segundo Orden con Delay'\n";
  gp << "plot '" << kDataFile << "' title 'Data' with linespoints lc -1, "
    "HS1(x) title '$H_{s_1}$' with lines lc 4 dt 2 lw 2, "
    "HS2(x) title '$H_{s_2}$' with lines lc 5 dt 2 lw 2, "
    "MS(x) title '$M_{s}$' with lines lc 6 lw 1\n";
}

int main() {
  Gnuplot gp;
  ConfigureGnuplot(gp);
  DefineFunctions(gp);
  PlotData(gp);
  return 0;
}

