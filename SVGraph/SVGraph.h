/*
  SVGraph.h
  Autor (15/12/2021):
  Jesús Escobar Mendoza jjesus-escobar@hotmail.com

  @TODO:
    -Add routes
    -Add function to each node
    -Add labels to nodes as comments (On/Off comments)
    -Add interactive range (appear/disappear when clicked)

*/

#ifndef SVGRAPH_H_
#define SVGRAPH_H_


#include <json/json.h>
#include <iostream>
#include <tuple> // tie
#include <utility> // pair
#include <map>
#include <cmath>
#include <regex>
#include <fstream>

using namespace std;
using namespace Json;

using TextLines = vector<string>;
using   PosPair = pair<float,float>;
using  EdgePair = pair<string,string>;
using     Nodes = vector<string>;
using     Edges = vector<EdgePair>;
using    PosMap = map<string, PosPair>;

const string  VER   = "1.2";
const float   DIST  = 100;
const float   GRID  = 50;
const float   RAD = 30;
const float   LEN = 25;
const float   SENSERAD = 2.5;
string  text_fill = "#000";
string  lora_fill = "#ff2";
string  ble_fill = "#36f";
string  arrow_fill = "none";
string  stroke = "#000";
float   arrow_width = 1;
float   lora_width = 0;
float   ble_width = 1;
string  font = "Helvetica";
float   font_size = 14;
float   LoRa_range = 2.0;
float   BLE_range = 1.0;
float   width = 1;
float   height = 1;

bool notNode(Nodes &nodes, string &node) { return count( nodes.begin(), nodes.end(), node) < 1; }

void makeJSON(Value& root, string& text) {
    CharReaderBuilder builder;
    builder["collectComments"] = false;
    const unique_ptr<CharReader> reader(builder.newCharReader());
    string err;
    if (!reader->parse(text.c_str(), text.c_str() + text.length(), &root, &err)) {
        cerr << "error" << endl;
        exit(EXIT_FAILURE);
    }
}

inline string trim(string & str) {
    str.erase(0, str.find_first_not_of(" \n\r\t"));
    str.erase(str.find_last_not_of(" \n\r\t") + 1);
    return str;
}

inline ostream &operator <<(ostream &os, const PosPair &p) {
    os << p.first << "," << p.second << " ";
    return os;
}

class SVGraph {
  protected:
    Nodes  nodes;
    Nodes  l_nodes;
    Nodes  b_nodes;
    Nodes  rf_nodes;
    Nodes  temp;
    Nodes  hdty;
    Nodes  prox;
    Nodes  keys;
    Edges  edges;
    PosMap nodepos;
    Value  root;
    string json = "";
    string svg = "";

    int       ns = 0; // nodes size for edge finding
    int       lns = 0; // nodes size for edge finding
    int       bns = 0; // nodes size for edge finding

    void parse_graph(TextLines lines) {
        regex   range(R"((range|RANGE)\s+[l|L]+([\d\.])\s+[b|B]+([\d\.]))");
        regex   tokens(R"((bn|BN|ln|LN)\s+([\d]+)\s+(\[[\d.]+,[\d.]+\]))"); //.7.8 first-clause: id. info [dur]
        regex   sensor(R"((temp|hdty|prox|keys|TEMP|HDTY|PROX|KEYS))"); //.8 second-clause: params
        smatch  matches;
        string  lra = "3";
        string  bra = "1.5";
        string  idl = "";
        string  idb = "";
        string  pos = "\n";
        string  col = "\n";
        string  dur = "\n";
        string temp = "";
        string hdty = "";
        string prox = "";
        string keys = "";

        json = R"({
"ver" : "#V",
"LoRa-range"  : [ #l ],
"BLE-range"   : [ #b ],
"LoRa-nodes"  : [ #L ],
"BLE-nodes"   : [ #B ],
"pos"         : { #D },
"Temperature" : [ #T ],
"Humidity"    : [ #H ],
"Proximity"   : [ #P ],
"Keypad"      : [ #K ]
})";

        for(auto &line : lines) {
                 auto x = line.find("]");
            string toks = line.substr(0, x + 1);
            string pars = line.substr(x + 1);
            if(string::npos == x) {
                toks = line;
                if (toks.length() < 2) continue;
                if(regex_match(toks, matches, range)) {
                    string lrange = matches[2].str();
                    string brange = matches[3].str();
                    lra = ("\"" + lrange + "\"");
                    bra = ("\"" + brange + "\"");
                    continue;
                }
                cerr << "Error: line missing range [start,end] -> " << line << endl;
                continue;
            }
            string id;
            if(regex_match(toks, matches, tokens)) {
                string node = matches[1].str();
                        id = matches[2].str();
                auto    loc = matches[3].str();
                if(!node.compare("bn") || !node.compare("BN")) {
                    idb += ("\"" + id + "\", ");
                } else if(!node.compare("ln") || !node.compare("LN")) {
                    idl += ("\"" + id + "\", ");
                } else {
                    cerr << "Error: line missing node specifier -> " << line << endl;
                    continue;
                }
                pos += ("    \"" + id + "\" : " + loc + ",\n");
            }
            string sen;
            pars = trim(pars);
            if (pars.length() < 2) continue;
            x = pars.find("te");
            if(string::npos != x) {
                temp += ("\"" + id + "\", ");
            }
            x = pars.find("TE");
            if (string::npos != x) {
                temp += ("\"" + id + "\", ");
            }
            x = pars.find("h");
            if(string::npos != x) {
                hdty += ("\"" + id + "\", ");
            }
            x = pars.find("H");
            if (string::npos != x) {
                hdty += ("\"" + id + "\", ");
            }
            x = pars.find("pr");
            if(string::npos != x) {
                prox += ("\"" + id + "\", ");
            }
            x = pars.find("PR");
            if (string::npos != x) {
                prox += ("\"" + id + "\", ");
            }
            x = pars.find("k");
            if(string::npos != x) {
                keys += ("\"" + id + "\", ");
            }
            x = pars.find("K");
            if (string::npos != x) {
                keys += ("\"" + id + "\", ");
            }        
        } //for loop
        //@TODO: Delete last coma & space from strings
        json = regex_replace(json, regex("#V"), VER);
        json = regex_replace(json, regex("#l"), lra);
        json = regex_replace(json, regex("#b"), bra);
        json = regex_replace(json, regex("#L"), idl);
        json = regex_replace(json, regex("#B"), idb);
        json = regex_replace(json, regex("#D"), pos);
        json = regex_replace(json, regex("#T"), temp);
        json = regex_replace(json, regex("#H"), hdty);
        json = regex_replace(json, regex("#P"), prox);
        json = regex_replace(json, regex("#K"), keys);
    } //parse_graph

    void parse_json() {
        makeJSON(root, json);
        //parsing LoRa-BLE ranges
        for (const auto& r : root["LoRa-range"]) {
            auto ran = r.asString();
            LoRa_range = stof(ran);
        }
        for (const auto& r : root["BLE-range"]) {
            auto ran = r.asString();
            BLE_range = stof(ran);
        }
        //LoRa node parsing
        for (const auto& n : root["LoRa-nodes"]) {
            auto node = n.asString();
            nodes.push_back(node);
            l_nodes.push_back(node);
        }
        //BLE node parsing
        for (const auto& n : root["BLE-nodes"]) {
            auto node = n.asString();
            nodes.push_back(node);
            b_nodes.push_back(node);
        }
        /*
        @TODO: Replace this function with interactive SVG, show RF range when clicked
        for (const auto &n: root["RF-on"]) {
            auto node = n.asString();
            nodes.push_back(node);
            rf_nodes.push_back(node);
        }

        @TODO: Add routes with different collor arrows
        for (const auto &arr: root["route"]) {
            auto a = arr[0].asString();
            auto b = arr[1].asString();
            if (notNode(b_nodes, a) && notNode(l_nodes, a)) {
                cerr << "\tSender node not a node " << a << endl;
                continue;
            }
            if (notNode(b_nodes, b) && notNode(l_nodes, b)) {
                cerr << "\tReceiver node not a node " << b << endl;
                continue;
            }
        }
        */
        //Sensor parsing
        for (const auto& s : root["Temperature"]) {
            auto a = s.asString();
            if (notNode(b_nodes, a)) {
                cerr << "\tNode " << a << " not a slave node " << endl;
                continue;
            }
            temp.push_back(a);
        }
        for (const auto& s : root["Humidity"]) {
            auto a = s.asString();
            if (notNode(b_nodes, a)) {
                cerr << "\tNode " << a << " not a slave node " << endl;
                continue;
            }
            hdty.push_back(a);
        }
        for (const auto& s : root["Proximity"]) {
            auto a = s.asString();
            if (notNode(b_nodes, a)) {
                cerr << "\tNode " << a << " not a slave node " << endl;
                continue;
            }
            prox.push_back(a);
        }
        for (const auto& s : root["Keypad"]) {
            auto a = s.asString();
            if (notNode(b_nodes, a)) {
                cerr << "\tNode " << a << " not a slave node " << endl;
                continue;
            }
            keys.push_back(a);
        }

        lns = l_nodes.size(); //Number of LoRa nodes
        bns = b_nodes.size(); //Number of BLE nodes

        //Nodes position parsing & calculating total graph width and height
        for (const auto& node : root["LoRa-nodes"]) {
            auto key = node.asString();
            if (notNode(l_nodes, key)) continue;
            auto arr = root["pos"][key];
            PosPair pos = make_pair(arr[0].asFloat(), arr[1].asFloat());
            nodepos[key] = pos;
            if (pos.first > width) {
                width = pos.first;
            }
            if (pos.second > height) {
                height = pos.second;
            }
        }
        for (const auto& node : root["BLE-nodes"]) {
            auto key = node.asString();
            if (notNode(b_nodes, key)) continue;
            auto arr = root["pos"][key];
            PosPair pos = make_pair(arr[0].asFloat(), arr[1].asFloat());
            nodepos[key] = pos;
            if (pos.first > width) {
                width = pos.first;
            }
            if (pos.second > height) {
                height = pos.second;
            }
        }

        height = round(height);
        width = round(width);

        height = (height) * 100 + 101;  //Graph total height
        width = (width) * 100 + 101;    //Graph total width
    }

    void parse_svg() {
        ostringstream ssvg;
        const string version = root["version"].asString();

        ssvg << "<?xml version='1.0'  encoding='UTF-8' standalone='no'?>" << endl
            << "<!DOCTYPE svg PUBLIC '-//W3C//DTD SVG 1.1//EN'" << endl
            << "\t'http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd'>" << endl;

        ssvg << "<svg xmlns='http://www.w3.org/2000/svg' xmlns:xlink='http://www.w3.org/1999/xlink'" <<
            endl << "version='" << version << "' width='" << width << "' height='" << height
            << "' viewBox='0 0 " << width << " " << height << "'>\n";

        ssvg << "<style type='text/css'>\n\ttext {\n\t\tfont-family: " << font
            << ";\n\t\tfill: " << text_fill << ";\n\t\tfont-size: " << font_size
            << ";\n\t\ttext-anchor: middle;\n\t\tfont-weight: bold;\n\t}" << endl;

        ssvg << "\t.lora {\n\t\tstroke: " << stroke << ";\n\t\tstroke-width: "
            << lora_width << "px;\n\t}" << endl;

        ssvg << "\t.ble {\n\t\tfill: " << ble_fill << ";\n\t\tstroke: " << stroke
            << ";\n\t\tstroke-width: " << ble_width << "px;\n\t}" << endl;

        ssvg << "\t.sensor {\n\t\tstroke: " << stroke << ";\n\t\tstroke-width: "
            << lora_width << "px;\n\t}" << endl;

        ssvg << "\t.arrow {\n\t\tfill: none;\n\t\tstroke-width: " << arrow_width
            << "px;\n\t}\n</style>\n<defs>" << endl;

        ssvg << "\t<radialGradient id='rf'>" << endl
            << "\t\t<stop offset='0.3' stop-color='#429' />" << endl
            << "\t\t\t<stop offset='0.4' stop-color='#35a' />" << endl
            << "\t\t\t<stop offset='0.5' stop-color='#27b' />" << endl
            << "\t\t\t<stop offset='0.6' stop-color='#19c' />" << endl
            << "\t\t\t<stop offset='0.7' stop-color='#0bd' />" << endl
            << "\t\t\t<stop offset='0.8' stop-color='#0ce' />" << endl
            << "\t\t\t<stop offset='0.9' stop-color='#0ef' />" << endl
            << "\t\t\t<stop offset='1.0' stop-color='#fff' />" << endl
            << "\t</radialGradient>" << endl;

        ssvg << "\t<radialGradient id='m3d'>" << endl
            << "\t\t\t<stop offset='0.1' stop-color='#990' />" << endl
            << "\t\t\t<stop offset='0.4' stop-color='#aa0' />" << endl
            << "\t\t\t<stop offset='0.6' stop-color='#bb0' />" << endl
            << "\t\t\t<stop offset='0.7' stop-color='#cc0' />" << endl
            << "\t\t\t<stop offset='0.8' stop-color='#dd0' />" << endl
            << "\t\t\t<stop offset='0.9' stop-color='#ee0' />" << endl
            << "\t\t\t<stop offset='1.0' stop-color='#ff0' />" << endl
            << "\t</radialGradient>" << endl;

        ssvg << "\t<pattern id='GRID' width='" << GRID << "' height='" << GRID
            << "' patternUnits='userSpaceOnUse'>" << "\n\t\t<path d='M " << GRID
            << " 0 L 0 0 0 " << GRID
            << "' stroke-dasharray='2 3' fill='none' stroke='gray'"
            << " stroke-width='1px' opacity='50%' />\n\t</pattern>\n</defs>" << "\n<!-- GRID -->" << endl
            << "<rect x='0' y='0' width='100%' height='100%' fill='url(#GRID)' />" << endl;

        ssvg << "<!-- RF signal BLE-->" << endl;
        for (const auto& node : b_nodes) {
            auto a = node;
            if (notNode(rf_nodes, a)) continue;
            auto [x, y] = nodepos[node];
            ssvg << "<g id='node_" << node << "_range'>" << endl;
            ssvg << "\t<ellipse rx='" << BLE_range * DIST << "' ry='"
                << BLE_range * DIST << "' cx='" << x * DIST << "' cy='" << y * DIST
                << "' fill='url(#rf)' opacity='50%' stroke='none' />" << endl;
            ssvg << "\t<ellipse rx='" << BLE_range * DIST << "' ry='"
                << BLE_range * DIST << "' cx='" << x * DIST << "' cy='" << y * DIST
                << "' fill='none' stroke-dasharray='2 3' stroke='gray' stroke-width='1px' opacity='50%' />" << endl;
            ssvg << "</g>" << endl;
        }

        ssvg << "<!-- RF signal LoRa -->" << endl;
        for (const auto& node : l_nodes) {
            auto a = node;
            if (notNode(rf_nodes, a)) continue;
            auto [x, y] = nodepos[node];
            ssvg << "<g id='node_" << node << "_range'>" << endl;
            ssvg << "\t<ellipse rx='" << LoRa_range * DIST << "' ry='"
                << LoRa_range * DIST << "' cx='" << x * DIST << "' cy='" << y * DIST
                << "' fill='url(#rf)' opacity='50%' stroke='none' />" << endl;
            ssvg << "\t<ellipse rx='" << LoRa_range * DIST << "' ry='"
                << LoRa_range * DIST << "' cx='" << x * DIST << "' cy='" << y * DIST
                << "' fill='none' stroke-dasharray='2 3' stroke='gray' stroke-width='1px' opacity='50%' />" << endl;
            ssvg << "</g>" << endl;
        }

        ssvg << "<!-- ARROWS LORA SIGNAL -->" << endl;

        for (int i = 0; i < lns; i++) {
            for (int j = i + 1; j < lns; j++) {
                EdgePair p{ l_nodes[i], l_nodes[j] };
                auto [a, b] = p;
                if (notNode(l_nodes, a)) continue;
                if (notNode(l_nodes, b)) continue;
                auto apos = root["pos"][a];
                auto bpos = root["pos"][b];
                auto [x, y] = nodepos[a];
                auto [x2, y2] = nodepos[b];
                if ((sqrt(pow(abs(x2 - x), 2) + pow(abs(y2 - y), 2))) < LoRa_range) {
                    edges.push_back(p);
                    float y3, y4, x3, x4, alfa;
                    alfa = atan(abs((y2 - y) / (x2 - x)));
                    if (y < y2) {
                        y3 = DIST * y + RAD * sin(alfa);
                        y4 = DIST * y2 - RAD * sin(alfa);
                    }
                    else {
                        y3 = DIST * y - RAD * sin(alfa);
                        y4 = DIST * y2 + RAD * sin(alfa);
                        alfa = alfa * -1;
                    }
                    if (x < x2) {
                        x3 = DIST * x + RAD * cos(alfa);
                        x4 = DIST * x2 - RAD * cos(alfa);
                    }
                    else {
                        x3 = DIST * x - RAD * cos(alfa);
                        x4 = DIST * x2 + RAD * cos(alfa);
                        alfa = (180 / 57.2958) - alfa;
                    }
                    ssvg << "<path class='arrow' d='M " << x3 << " " << y3 << " L "
                        << x4 << " " << y4 << "' stroke='" << stroke << "'  />" << endl;
                }
            }
        }

        ssvg << "<!-- ARROWS LoRa-BLE SIGNAL -->" << endl;

        for (int i = 0; i < bns; i++) {
            int count = 1;
            for (int j = 0; j < lns; j++) {
                EdgePair p{ b_nodes[i], l_nodes[j] };
                auto [a, b] = p;
                if (notNode(b_nodes, a)) continue;
                if (notNode(l_nodes, b)) continue;
                auto apos = root["pos"][a];
                auto bpos = root["pos"][b];
                auto [x, y] = nodepos[a];
                auto [x2, y2] = nodepos[b];

                if ((sqrt(pow(abs(x2 - x), 2) + pow(abs(y2 - y), 2))) < BLE_range) {
                    edges.push_back(p);
                    float y3, y4, x3, x4, alfa;
                    alfa = atan(abs((y2 - y) / (x2 - x)));
                    if (y < y2) {
                        y3 = DIST * y + (LEN / 2) * sin(alfa);
                        y4 = DIST * y2 - RAD * sin(alfa);
                    }
                    else {
                        y3 = DIST * y - (LEN / 2) * sin(alfa);
                        y4 = DIST * y2 + RAD * sin(alfa);
                        alfa = alfa * -1;
                    }
                    if (x < x2) {
                        x3 = DIST * x + (LEN / 2) * cos(alfa);
                        x4 = DIST * x2 - RAD * cos(alfa);
                    }
                    else {
                        x3 = DIST * x - (LEN / 2) * cos(alfa);
                        x4 = DIST * x2 + RAD * cos(alfa);
                        alfa = (180 / 57.2958) - alfa;
                    }
                    if (count < 2) {
                        ssvg << "<path class='arrow' d='M " << x3 << " " << y3 << " L "
                            << x4 << " " << y4 << "' stroke='" << stroke << "'  />" << endl;
                        ssvg << "<path class='arrow' d='M " << x4 - 10 << " " << y4 + 4 << " l 8 -4 l -8 -4' stroke='" << stroke
                            << "' transform=' rotate(" << alfa * 57.2958 << " " << x4 << " " << y4 << ")'" << " />" << endl;
                    }
                    else {
                        ssvg << "<path class='arrow' d='M " << x3 << " " << y3 << " L "
                            << x4 << " " << y4 << "' stroke='#f22222'  />" << endl;
                        ssvg << "<path class='arrow' d='M " << x4 - 10 << " " << y4 + 4 << " l 8 -4 l -8 -4' stroke='#f22222' transform=' rotate("
                            << alfa * 57.2958 << " " << x4 << " " << y4 << ")'" << " />" << endl;
                    }
                    count++;
                }
            }
        }

        ssvg << "<!-- ARROWS BLE SIGNAL -->" << endl;

        for (int i = 0; i < bns; i++) {
            for (int j = i + 1; j < bns; j++) {
                EdgePair p{ b_nodes[i], b_nodes[j] };
                auto [a, b] = p;
                if (notNode(b_nodes, a)) continue;
                if (notNode(b_nodes, b)) continue;
                auto apos = root["pos"][a];
                auto bpos = root["pos"][b];
                auto [x, y] = nodepos[a];
                auto [x2, y2] = nodepos[b];
                if ((sqrt(pow(abs(x2 - x), 2) + pow(abs(y2 - y), 2))) < BLE_range) {
                    edges.push_back(p);
                    float y3, y4, x3, x4, alfa;
                    alfa = atan(abs((y2 - y) / (x2 - x)));
                    if (y < y2) {
                        y3 = DIST * y + (LEN / 2) * sin(alfa);
                        y4 = DIST * y2 - (LEN / 2) * sin(alfa);
                    }
                    else {
                        y3 = DIST * y - (LEN / 2) * sin(alfa);
                        y4 = DIST * y2 + (LEN / 2) * sin(alfa);
                        alfa = alfa * -1;
                    }
                    if (x < x2) {
                        x3 = DIST * x + (LEN / 2) * cos(alfa);
                        x4 = DIST * x2 - (LEN / 2) * cos(alfa);
                    }
                    else {
                        x3 = DIST * x - (LEN / 2) * cos(alfa);
                        x4 = DIST * x2 + (LEN / 2) * cos(alfa);
                        alfa = (180 / 57.2958) - alfa;
                    }
                    ssvg << "<path class='arrow' d='M " << x3 << " " << y3 << " L "
                        << x4 << " " << y4 << "' stroke='#f22222'  />" << endl;
                    ssvg << "<path class='arrow' d='M " << x4 - 10 << " " << y4 + 4 << " l 8 -4 l -8 -4' stroke='#f22222' transform=' rotate("
                        << alfa * 57.2958 << " " << x4 << " " << y4 << ")'" << " />" << endl;
                }
            }
        }

        ssvg << "<!-- Sensor paths -->" << endl;

        for (int i = 0; i < bns; i++) {
            auto a = b_nodes[i];
            auto apos = root["pos"][a];
            auto [x, y] = nodepos[a];
            int senseNum = 0;
            float xpos;
            float ypos;
            float xendpos;
            float yendpos;
            x = x * DIST;
            y = y * DIST;
            xpos = x - LEN / 2;
            ypos = y - LEN / 3;
            xendpos = xpos - LEN / 2;
            yendpos = ypos;
            if (notNode(b_nodes, a)) continue;
            if (!notNode(temp, a)) {
                ssvg << "<path class='arrow' d='M " << xpos << " " << ypos << " L "
                    << xendpos << " " << yendpos << "' stroke='#099e13'  />" << endl;
                ssvg << "\t<circle class='sensor' " << "cx='" << xendpos << "' cy='"
                    << yendpos << "' r='" << SENSERAD << "' fill='#099e13' />" << endl;
                senseNum++;
            }
            if (!notNode(hdty, a)) {
                if (senseNum == 1) {
                    xpos = x - LEN / 2;
                    ypos = y + LEN / 3;
                    xendpos = xpos - LEN / 2;
                    yendpos = ypos;
                }
                ssvg << "<path class='arrow' d='M " << xpos << " " << ypos << " L "
                    << xendpos << " " << yendpos << "' stroke='#11099e'  />" << endl;
                ssvg << "\t<circle class='sensor' " << "cx='" << xendpos << "' cy='"
                    << yendpos << "' r='" << SENSERAD << "' fill='#11099e' />" << endl;
                senseNum++;
            }
            if (!notNode(prox, a)) {
                if (senseNum == 1) {
                    xpos = x - LEN / 2;
                    ypos = y + LEN / 3;
                    xendpos = xpos - LEN / 2;
                    yendpos = ypos;
                }
                else if (senseNum == 2) {
                    xpos = x + LEN / 2;
                    ypos = y - LEN / 3;
                    xendpos = xpos + LEN / 2;
                    yendpos = ypos;
                }
                ssvg << "<path class='arrow' d='M " << xpos << " " << ypos << " L "
                    << xendpos << " " << yendpos << "' stroke='#bf0ffa'  />" << endl;
                ssvg << "\t<circle class='sensor' " << "cx='" << xendpos << "' cy='"
                    << yendpos << "' r='" << SENSERAD << "' fill='#bf0ffa' />" << endl;
                senseNum++;
            }
            if (!notNode(keys, a)) {
                if (senseNum == 1) {
                    xpos = x - LEN / 2;
                    ypos = y + LEN / 3;
                    xendpos = xpos - LEN / 2;
                    yendpos = ypos;
                }
                else if (senseNum == 2) {
                    xpos = x + LEN / 2;
                    ypos = y - LEN / 3;
                    xendpos = xpos + LEN / 2;
                    yendpos = ypos;
                }
                else if (senseNum == 3) {
                    xpos = x + LEN / 2;
                    ypos = y + LEN / 3;
                    xendpos = xpos + LEN / 2;
                    yendpos = ypos;
                }
                ssvg << "<path class='arrow' d='M " << xpos << " " << ypos << " L "
                    << xendpos << " " << yendpos << "' stroke='#fbff12'  />" << endl;
                ssvg << "\t<circle class='sensor' " << "cx='" << xendpos << "' cy='"
                    << yendpos << "' r='" << SENSERAD << "' fill='#fbff12' />" << endl;
                senseNum++;
            }
        }

        ssvg << "<!-- LoRa nodes -->" << endl;

        for (const auto& node : l_nodes) {
            auto [x, y] = nodepos[node];
            ssvg << "<g id='node_" << node << "'>" << endl;
            ssvg << "\t<ellipse class='lora' rx='" << RAD << "' ry='" << RAD << "' cx='"
                << x * DIST << "' cy='" << y * DIST << "' fill='url(#m3d)' />" << endl;
            ssvg << "\t<text x='" << (x * DIST) << "' y='" << (y * DIST + 5) << "' >"
                << node << "</text>" << endl;
            ssvg << "</g>" << endl;
        }

        ssvg << "<!-- BLE nodes -->" << endl;

        for (const auto& node : b_nodes) {
            auto [x, y] = nodepos[node];
            ssvg << "<g id='node_" << node << "'>" << endl;
            ssvg << "\t<rect class='ble'" << " x='" << x * DIST - LEN / 2 << "' y='" << y * DIST - LEN / 2
                << "' width='" << LEN << "' height='" << LEN << "' rx='5' ry='5' />" << endl;
            ssvg << "\t<text x='" << (x * DIST) << "' y='" << (y * DIST + 5) << "' >"
                << node << "</text>" << endl;
            ssvg << "</g>" << endl;
        }

        ssvg << "</svg>\n";
        svg = ssvg.str();
    }

    inline void parser(TextLines lines) {
        parse_graph(lines); // txt -> json
        parse_json();       // json -> internal memory
        parse_svg();        // mem -> svg
    }

    void _save(const string &filename, const string &data) {
        ofstream fo(filename);
        if (fo.is_open()) {
            fo << data;
            fo.close();
        } else {
            cerr << "Unable to open " << filename << endl;
            exit(EXIT_FAILURE);
        }
    }

  public:

    SVGraph(TextLines lines) {
      parser(lines);
    }

    SVGraph(string filename) {
        ifstream fi(filename);
        if (filename.find(".txt") != string::npos) {
            if (fi.is_open()) { // .txt
                TextLines lines;
                string line;
                while (getline(fi,line)) {
                    if (line.length() > 6) { /// @todo: add remarks
                        lines.push_back(trim(line));
                    }
                }
                parser(lines); 
            } else {
                cerr << "Can't find " << filename << endl;
            }
        } else if (filename.find(".json") != string::npos) {
            if (fi.is_open()) { // .json
                json = ""; // output
                string line;
                while (getline(fi,line)) {
                    json = json + trim(line) + "\n";
                }
                parse_json();       //  json -> internal memory
                parse_svg();        //   mem -> svg
            } else {
                cerr << "Can't find " << filename << endl;
            }
        } else {
            cerr << "Error: bad filename: " << filename << endl;
        }
        fi.close();
    }

    size_t size() { return svg.length(); }

    friend ostream &operator <<(ostream &os, SVGraph &graph) {
        os << graph.svg;
        return os;
    }

    void save(const string &filename) {
        if (filename.find(".json") != string::npos) {
            if (json.length() > 0) {
                _save(filename, json);
            }
        } else if (filename.find(".svg") != string::npos) {
            if (svg.length() > 0) {
                _save(filename, svg);
            }
        } else {
            cerr << "Error: empty or not parsed graph" << endl;
        }
    }

}; //End class SVGraph


#endif //SVGRAPH_H_
