#include <json/json.h>
#include <iostream>
#include <tuple> // tie
#include <utility> // pair
#include <map>
#include <cmath>

using namespace std;
using namespace Json;

using  PosPair = pair<float,float>;
using EdgePair = pair<string,string>;
using    Nodes = vector<string>;
using    Edges = vector<EdgePair>;
using   PosMap = map<string, PosPair>;

bool notNode(Nodes &nodes, string &node) { return count( nodes.begin(), nodes.end(), node) < 1; }

string GraphJSON = R"({
    "version" : "1.1",
    "design_key" : [
        "LoRa-fill",
        "BLE-fill",
        "Border",
        "LoRa-range",
        "BLE-range",
    ],
    "design" :
    {
        "LoRa-fill" : "yellow",
        "BLE-fill"  : "#33ccff",
        "Border"    : 1,
        "LoRa-range": 2.5,
        "BLE-range": 1,
    },
    "LoRa-nodes" : ["2", "3", "5", "8", "9", "14"],
    "BLE-nodes"  : ["1", "4", "6", "7","10"],
    "pos" :{
        "1"     : [1, 1],
        "2"     : [1, 2],
        "3"     : [3, 2],
        "4"     : [1.5, 0.5],
        "5"     : [1, 3],
        "6"     : [2.25, 1.75],
        "7"     : [0.5, 2.5],
        "8"     : [2, 4],
        "9"     : [4, 2],
        "10"    : [3.5, 2.5],
        "14"    : [2, 1],
    },
})";

void makeJSON(Value &root, string &text) {
    CharReaderBuilder builder;
    builder["collectComments"] = false;
    const unique_ptr<CharReader> reader(builder.newCharReader());
    string err;
    if (!reader->parse(text.c_str(), text.c_str() + text.length(), &root, &err)) {
      cerr << "error" << endl;
      exit(EXIT_FAILURE);
    }
}

int main() {
    Nodes  nodes;
    Nodes  l_nodes;
    Nodes  b_nodes;
    Edges  edges;
    PosMap nodepos;
    Value  root;

    float   GRID    = 100;
    string  text_fill = "#000";
    string  ellipse_fill = "#ff2";
    string  square_fill = "#ff2";
    string  path_fill = "none";
    string  stroke = "#000";
    float   stroke_width = 2;
    float   ellipse_width = 2;
    float   square_width = 2;
    string  font = "Helvetica";
    float   font_size = 16;
    float   RAD = 30;
    float   LEN = 20;
    float   LoRa_range = 2.0;
    float   BLE_range = 1.0;
    float   width = 1000;
    float   height = 1000;

    int       ns = 0; // nodes size for edge finding
    int       lns = 0; // nodes size for edge finding
    int       bns = 0; // nodes size for edge finding

    makeJSON(root, GraphJSON);

    for (const auto &d: root["design_key"]) {
        auto key = d.asString();
        auto val = root["design"][key];
        cout << key << ":\t";
        if(!key.compare("LoRa-fill")){
            ellipse_fill = val.asString();
            cout << ellipse_fill << endl;
        } else if(!key.compare("BLE-fill")){
            square_fill = val.asString();
            cout << square_fill << endl;
        } else if(!key.compare("Border")){
            ellipse_width = val.asFloat();
            cout << "\t" << ellipse_width << endl;
        } else if(!key.compare("LoRa-range")){
            LoRa_range = val.asFloat();
            cout << LoRa_range << endl;
        } else if(!key.compare("BLE-range")){
            BLE_range = val.asFloat();
            cout << BLE_range << endl;
        }
    }

    cout << "Nodes: ";
    cout << endl << "\tLoRa-nodes: ";
    for (const auto &n: root["LoRa-nodes"]) {
        auto node = n.asString();
        nodes.push_back(node);
        l_nodes.push_back(node);
        cout << node << " ";
    }
    cout << endl <<"\tBLE-nodes: ";
    for (const auto &n: root["BLE-nodes"]) {
        auto node = n.asString();
        nodes.push_back(node);
        b_nodes.push_back(node);
        cout << node << " ";
    }

    lns = l_nodes.size();
    bns = b_nodes.size();
    ns = lns + bns;
    cout << endl << "Total nodes: " << ns << endl;

    cout << "Positions:" << endl;
    for (const auto &node: root["LoRa-nodes"]) {
        auto key = node.asString();
        if (notNode(l_nodes, key)) continue;
        cout << "  Node_" << key << ": (";
        auto arr = root["pos"][key];
        PosPair pos = make_pair(arr[0].asFloat(), arr[1].asFloat());
        nodepos[key] = pos;
        cout << pos.first << "," << pos.second << ")" << endl;
    }

    for (const auto &node: root["BLE-nodes"]) {
        auto key = node.asString();
        if (notNode(b_nodes, key)) continue;
        cout << "  Node_" << key << ": (";
        auto arr = root["pos"][key];
        PosPair pos = make_pair(arr[0].asFloat(), arr[1].asFloat());
        nodepos[key] = pos;
        cout << pos.first << "," << pos.second << ")" << endl;
    }

    ostringstream svg;
    const string version = root["version"].asString();

    svg << "<?xml version='1.0'  encoding='UTF-8' standalone='no'?> <!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'"
    svg<< "\n\t'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>"

    svg << "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink' version='"
    << version <<"' width='" << width << "' height='" << height << "' viewBox='0 0 " << width << " " << height << "'>\n";

    //svg << "<rect width='100%' height='100%' fill='url(#grid)'  />" << endl;

    svg << "<style type='text/css'>\n\ttext {\n\t\tfont-family: " << font << ";\n\t\tfill: " << text_fill << ";\n\t\tfont-size: " << font_size
    << ";\n\t\ttext-anchor: middle;\n\t\tfont-weight: bold;\n\t}" << endl;

    svg << "\tellipse {\n\t\tfill: " << ellipse_fill << ";\n\t\tfill-opacity: 90%;\n\t\tstroke: " << stroke
    << ";\n\t\tstroke-width: " << ellipse_width << ";\n\t}" << endl;

    svg << "\trect {\n\t\tfill: " << square_fill << ";\n\t\tfill-opacity: 90%;\n\t\tstroke: " << stroke
    << ";\n\t\tstroke-width: " << square_width << ";\n\t\twidth: " << LEN << ";\n\t\theight: " << LEN << ";\n\t}" << endl;

    svg << "\tpath {\n\t\tfill: " << path_fill << ";\n\t\tstroke-width: " << stroke_width << ";\n\t}\n</style>" << endl;

    svg << "<defs>\n\t<pattern id='grid' width='" << GRID << "' height='" << GRID << "' patternUnits='userSpaceOnUse'>"
    << "\n\t\t<path d='M 100 0 L 0 0 0 100' stroke-dasharray='2 3' stroke='gray' stroke-width='1px'/>"
    << "\n\t</pattern>\n</defs>" << endl;

    for (const auto &node: l_nodes) {
        auto [x, y] = nodepos[node];
        svg << "<g id='node_" << node << "' >" << endl;
        svg << "\t<ellipse" << " cx='" << x*GRID << "' cy='" << y*GRID << "' rx='" << RAD << "' ry='" << RAD << "' />" << endl;
        svg << "\t<text x='" << (x*GRID-5) << "' y='" << (y*GRID+5) << "' >"<< node << "</text>" << endl;
        svg << "</g>" << endl;
    }

    for (const auto &node: b_nodes) {
        auto [x, y] = nodepos[node];
        svg << "<g id='node_" << node << "' >" << endl;
        svg << "\t<rect" << " x='" << x*GRID-LEN/2 << "' y='" << y*GRID-LEN/2 << "' width='" << LEN << "' height='" << LEN << "' />" << endl;
        svg << "\t<text x='" << (x*GRID-5) << "' y='" << (y*GRID+5) << "' >"<< node << "</text>" << endl;
        svg << "</g>" << endl;
    }

    for(int i = 0; i<lns; i++) {
        for( int j= i+1; j<lns; j++) {
            EdgePair p {l_nodes[i], l_nodes[j]};
            auto [a, b] = p;
            if (notNode(l_nodes,a)) continue;
            if (notNode(l_nodes,b)) continue;
            auto apos = root["pos"][a];
            auto bpos = root["pos"][b];
            auto [x, y] = nodepos[a];
            auto [x2, y2] = nodepos[b];
            if((sqrt(pow(abs(x2-x),2)+pow(abs(y2-y),2)))<LoRa_range) {
                edges.push_back(p);
                float y3, y4, x3, x4, alfa;
                alfa = atan(abs((y2-y)/(x2-x)));
                if(y<y2) {
                    y3 = GRID*y+RAD*sin(alfa);
                    y4 = GRID*y2-RAD*sin(alfa);
                } else {
                    y3 = GRID*y-RAD*sin(alfa);
                    y4 = GRID*y2+RAD*sin(alfa);
                    alfa = alfa*-1;
                }
                if(x<x2) {
                    x3 = GRID*x+RAD*cos(alfa);
                    x4 = GRID*x2-RAD*cos(alfa);
                } else {
                    x3 = GRID*x-RAD*cos(alfa);
                    x4 = GRID*x2+RAD*cos(alfa);
                    alfa = (180/57.2958)-alfa;
                }
                svg << "<path d='M " << x3 << " " << y3 << " L "
                << x4 << " " << y4 << "' stroke='" << stroke << "'  />" << endl;
                svg << "<path d='M " << x4-10 << " " << y4+4 << " l 8 -4 l -8 -4' stroke='" << stroke
                << "' transform=' rotate(" << alfa*57.2958 << " " << x4 << " " << y4 << ")'"<< " />" << endl;
            }
        }
    }
    for(int i = 0; i<bns; i++) {
        int count = 1;
        for( int j= 0; j<lns; j++) {
            EdgePair p {b_nodes[i], l_nodes[j]};
            auto [a, b] = p;
            if (notNode(b_nodes,a)) continue;
            if (notNode(l_nodes,b)) continue;
            auto apos = root["pos"][a];
            auto bpos = root["pos"][b];
            auto [x, y] = nodepos[a];
            auto [x2, y2] = nodepos[b];

            if((sqrt(pow(abs(x2-x),2)+pow(abs(y2-y),2)))<BLE_range) {
                edges.push_back(p);
                float y3, y4, x3, x4, alfa;
                alfa = atan(abs((y2-y)/(x2-x)));
                if(y<y2) {
                    y3 = GRID*y+(LEN/2)*sin(alfa);
                    y4 = GRID*y2-RAD*sin(alfa);
                } else {
                    y3 = GRID*y-(LEN/2)*sin(alfa);
                    y4 = GRID*y2+RAD*sin(alfa);
                    alfa = alfa*-1;
                }
                if(x<x2) {
                    x3 = GRID*x+(LEN/2)*cos(alfa);
                    x4 = GRID*x2-RAD*cos(alfa);
                } else {
                    x3 = GRID*x-(LEN/2)*cos(alfa);
                    x4 = GRID*x2+RAD*cos(alfa);
                    alfa = (180/57.2958)-alfa;
                }
                if(count < 2) {
                    svg << "<path d='M " << x3 << " " << y3 << " L "
                    << x4 << " " << y4 << "' stroke='" << stroke << "'  />" << endl;
                    svg << "<path d='M " << x4-10 << " " << y4+4 << " l 8 -4 l -8 -4' stroke='" << stroke
                    << "' transform=' rotate(" << alfa*57.2958 << " " << x4 << " " << y4 << ")'"<< " />" << endl;
                } else {
                    svg << "<path d='M " << x3 << " " << y3 << " L "
                    << x4 << " " << y4 << "' stroke='#f22222'  />" << endl;
                    svg << "<path d='M " << x4-10 << " " << y4+4 << " l 8 -4 l -8 -4' stroke='#f22222' transform=' rotate("
                    << alfa*57.2958 << " " << x4 << " " << y4 << ")'"<< " />" << endl;
                }
                count++;
            }
        }
    }

    for(int i = 0; i<bns; i++) {
        for( int j= i+1; j<bns; j++) {
            EdgePair p {b_nodes[i], b_nodes[j]};
            auto [a, b] = p;
            if (notNode(b_nodes,a)) continue;
            if (notNode(b_nodes,b)) continue;
            auto apos = root["pos"][a];
            auto bpos = root["pos"][b];
            auto [x, y] = nodepos[a];
            auto [x2, y2] = nodepos[b];
            if((sqrt(pow(abs(x2-x),2)+pow(abs(y2-y),2)))<BLE_range) {
                edges.push_back(p);
                float y3, y4, x3, x4, alfa;
                alfa = atan(abs((y2-y)/(x2-x)));
                if(y<y2) {
                    y3 = GRID*y+(LEN/2)*sin(alfa);
                    y4 = GRID*y2-(LEN/2)*sin(alfa);
                } else {
                    y3 = GRID*y-(LEN/2)*sin(alfa);
                    y4 = GRID*y2+(LEN/2)*sin(alfa);
                    alfa = alfa*-1;
                }
                if(x<x2) {
                    x3 = GRID*x+(LEN/2)*cos(alfa);
                    x4 = GRID*x2-(LEN/2)*cos(alfa);
                } else {
                    x3 = GRID*x-(LEN/2)*cos(alfa);
                    x4 = GRID*x2+(LEN/2)*cos(alfa);
                    alfa = (180/57.2958)-alfa;
                }
                svg << "<path d='M " << x3 << " " << y3 << " L "
                << x4 << " " << y4 << "' stroke='#f22222'  />" << endl;
                svg << "<path d='M " << x4-10 << " " << y4+4 << " l 8 -4 l -8 -4' stroke='#f22222' transform=' rotate("
                << alfa*57.2958 << " " << x4 << " " << y4 << ")'"<< " />" << endl;
            }
        }
    }
    svg << "</svg>\n";
    cout << endl << svg.str();
}
