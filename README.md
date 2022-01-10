# SVGraph
C++ API to create node graphs (DAGs) for IoT peer-to-peer networks. 

This proyect was made to ease design and creation of directed acycled graphs (DAGs) for my master's degree project.

The original ideaof using SVG, JSON & Regular expressions was sugested by my thesis advisor M.C. Alberto Pacheco. 

The development of this library started in mid September 2021, the first working version was uploaded October 21st 2021. Since then minor modifications were made to improove graphic stetics and show more information in a single graph. 
In January 6 2022 version 1.2 was released, this had mayor modifications to make possible for the project to be used as a command line in windows/MacOS/Linux prompt and terminal. 

This proyect is usefull in showing graphically different test scenarios of mesh networks. It includes some limitations that I included on my network, but it can be easily modified to be used for your specific needs.

## INSTALATION
Download MINGW latest version from https://github.com/brechtsanders/winlibs_mingw/releases

In windows you must set the path to be able to use in the command prompt, 
you access this in control panel/System/Advanced system settings/Environment Variables.
This window should open:

![image](https://user-images.githubusercontent.com/80430510/148432747-4e9b1cdc-3f58-4777-baec-031f4adda099.png)

You will need to clone: vcpkg(https://github.com/Microsoft/vcpkg.git) and jsoncpp(https://github.com/open-source-parsers/jsoncpp) libraries 

You can also create a proyect in Microsoft Visual Studio here's a link to an example of how you can do this: https://www.youtube.com/watch?v=GYauneigGTs

## Example of .exe usage
The way the txt file is to be written is the folowing
You can start by defining the Nodes range: ***range l6 b2***

This indicates that you're setting the range, for LoRa nodes it will be 6 and 2 for BLE nodes. If you do not declare a range the default values are 3 and 1.5 respectively.

To declare a LoRa node you write the following: ***ln 1 [1.7,4]***

"ln" indicates the type of node in this case LoRa node, "1" indicates the number of node & "[1.7,4]" indicates x,y position on the graph. LoRa nodes are considered master nodes, they will not show any sensors even if indicated on text.

To declare a BLE node you write the following: ***bn 2 [1,3] temp***

"bn" indicates the type of node in this case BLE node, "2" indicates the number of node, "[1,3]" indicates x,y position on the graph & every declared sensor after the position will apear as a branch emerging from the node. The type of sensors included are: temperature (**temp**), humidity (**hdty**), proximity (**prox**) & keypad (**keys**).

### .txt example

```
range l6 b2
ln 2 [2,3] prox
bn 7 [1,3] hdty keys
BN 3 [3,3] temp hdty prox
LN 4 [1,1] keys
bn 1 [2,2] prox
```

This .txt file will generate the following .json file:

```
{
"ver" : "1.2",
"LoRa-range"  : [ "6" ],
"BLE-range"   : [ "2" ],
"LoRa-nodes"  : [ "2", "4",  ],
"BLE-nodes"   : [ "7", "3", "1",  ],
"pos"         : { 
    "2" : [2,3],
    "7" : [1,3],
    "3" : [3,3],
    "4" : [1,1],
    "1" : [2,2],
 },
"Temperature" : [ "3",  ],
"Humidity"    : [ "7", "3",  ],
"Proximity"   : [ "2", "3", "1",  ],
"Keypad"      : [ "7", "4",  ]
}
```
Which in turn it will generate the following .svg image:

![image](https://user-images.githubusercontent.com/80430510/148697666-060bebe1-7f6b-4c55-b011-a10d82b3b344.png)

To use the command line for this just acces the folder containing the .exe file and drop your .txt or .json files in it to be processed by the application. The command is as follows:
* SVGraph.exe inputname outputname
* ./SVGraph inputname outputname

The first argument would be the application name (SVGraph), the second argument (inputname) would be the name of the file to be read, and the third argument (outputname) would be the name you want on the generated output files.
If you inlcude only the first argument the default name the application will look for is 'graph01', in case you only wrote the first and second arguments, the output files will be called as the input file with their respective extension names.

### Contributing

If you want to contribute to this project:

* Report bugs and errors
* Ask for enhancements
* Create issues and pull requests
* Tell others about this library
* Contribute new protocols

### Credits
This library is written by Jesús Escobar Mendoza as a tool for making diagrams quickly and with the same format. The idea to develop this library was given by Alberto Pacheco González his dissertation advisor.
