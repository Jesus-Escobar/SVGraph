/*
  SVGraph.cpp generates svg for mesh connections with 2 different
nodes and RF singal & range.
  Autor (15/09/21):
    Jesús Escobar Mendoza jjesus-escobar@hotmail.com


This program takes json files as input and generates SVG files
*/


#include "SVGraph.h"

const string  filename = "graph01";
const string   in_file = filename + ".txt";
const string json_file = filename + ".json";
const string  svg_file = filename + ".svg";

int main () {
  SVGraph svgraph(in_file);
  svgraph.save(json_file);
  cout << "JSON file saved: " << json_file << endl;
  svgraph.save(svg_file);
  cout << "SVG file saved: " << svg_file << "(" << svgraph.size() << " bytes)" << endl;
  return 0;
}
