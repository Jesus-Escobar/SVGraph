/*
  SVGraph.cpp generates svg for mesh connections with 2 different
nodes and RF singal & range.
  Autor (15/09/21):
    Jesús Escobar Mendoza jjesus-escobar@hotmail.com


This program takes json files as input and generates SVG files
*/


#include "SVGraph.h"

string  filename = "graph01";
string  out_file = filename;
string   in_file = filename + ".txt";
string json_file = filename + ".json";
string  svg_file = filename + ".svg";

int main (int argc, char** argv) {
    if (argc == 2) {
         filename = argv[1];
          in_file = filename + ".txt";
        json_file = filename + ".json";
         svg_file = filename + ".svg";
    } 
    else if (argc == 3) {
         filename = argv[1];
          in_file = filename + ".txt";
         out_file = argv[2];
        json_file = out_file + ".json";
         svg_file = out_file + ".svg";
    }
    else {
        cout << "Error: no input file name specified used default \"graph01.txt\" \nUse: SVGraph.exe inputfilename outputfilename" << endl;
    }
    SVGraph svgraph(in_file);
    svgraph.save(json_file);
    cout << "JSON file saved: " << json_file << endl;
    svgraph.save(svg_file);
    cout << "SVG file saved: " << svg_file << "(" << svgraph.size() << " bytes)" << endl;
    return 0;
}
