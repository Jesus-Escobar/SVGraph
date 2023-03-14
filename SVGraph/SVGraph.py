###############################################################################
# SVGraph - Graph generation tool
# Jesús Escobar Mendoza jjesus-escobar@hotmail.com
# Migrated to python on (7/October/2022)
###############################################################################

###############################################################################
#  TODO:
#   - Add routes (Highlight shortest path from node to node)
#   - Add faults (Crossout and disable node)
#   - Correctly define .cpp and .h files (.h declarations, .cpp definitions)
#   - Add function to each node 
#   - Add labels to nodes as comments (On/Off comments)
#   - Add interactive range (appear/disappear when clicked)
#   - Define color palet and change with an argument
#   - Make sensors universal, dont use name in txt file use number of sensors
###############################################################################

import os       # File manipulation
import sys      # Arguments
import shutil   # Copy files
import json     # JSON parsing
import re       # RegEx regular expression parsing

###############################################################################
# Constants
###############################################################################

SCRIPT_VERSION: int = 1
# Text formatting
_RED_ON:    str = '\033[91m'
_GREEN_ON:  str = '\033[92m'
_BLUE_ON:   str = '\033[94m'
_CYAN_ON:   str = '\033[96m'
_COLOR_OFF: str = '\033[0m'
# Misc
SEPARATOR:  str = f"{_CYAN_ON}###############################################################################{_COLOR_OFF}"
TEXT_EXT:   str = ".txt"
SVG_EXT:    str = ".svg"
JSON_EXT:   str = ".json"

DIST:       int = 100
GRID:       int = 50
NODEA:      int = 30    # Node A is a circle, this value represents radious
NODEB:      int = 25    # Node B is a square, this value represents the length of a side
FONT_SIZE:  int = 20    # Text size
# Color pallete More to be searched https://www.color-hex.com/color-palettes/popular.php - https://coolors.co/f8f9fa-e9ecef-dee2e6-ced4da-adb5bd-6c757d-495057-343a40-212529

EERIE_BLACK: str = "#212529"     # Eerie Black
BLACK:  str = "#000000"
WHITE:  str = "#FFFFFF" 

### Gray scale
GSTEXTA:    str = "#F8F9FA"     # Cultured
GSTEXTB:    str = "#DEE2E6"     # Gainsboro
GSNODEA:    str = "#ADB5BD"     # Cadet Blue Crayola
GSNODEB:    str = "#6C757D"     # Sonic Silver
GSRF:       str = "#CED4DA"     # Light Gray



###############################################################################
# Functions
###############################################################################

"""Print messages to terminal, with color options"""
def log(message: str, debug_msg: bool = False, success_msg: bool = False, error_msg: bool = False):
    if (debug_msg):
        message = f"{_BLUE_ON}{message}{_COLOR_OFF}"
    elif (success_msg):
        message = f"{_GREEN_ON}{message}{_COLOR_OFF}"
    elif (error_msg):
        message = f"{_RED_ON}{message}{_COLOR_OFF}"
    print(message)


"""Exit script depending on result"""
def exit_script(process_ok: bool):
    status: int = 0
    exit_message: str = f"SVG graph generation tool"
    log(SEPARATOR)
    if (process_ok):
        log(f"{exit_message} done!", success_msg=True)
    else:
        log(f"{exit_message} failed", error_msg=True)
        status = 1
    sys.exit(status)

###############################################################################
# Txt parsing
###############################################################################

def parse_text():

    #range:  str = R'(range|RANGE)\s+[a|A]+([\d\.])\s+[b|B]+([\d\.])\s+[c|C]+([\d\.])'
    #tokens: str = R'(an|AN|bn|BN|cn|CN)\s+([\d]+)\s+(\[[\d.]+,[\d.]+\])'

    range_dict = {
        'range': re.compile(r'range = (?P<range>.*)\n'),
        'range': re.compile(r'range = (?P<range>.*)\n'),
    }

    rangeA: str = "8"
    rangeB: str = "4"
    rangeC: str = "2"
    nodeA_ID: str = ""
    nodeB_ID: str = ""
    nodeC_ID: str = ""
    position: str = "\n"

    graph_json: str = R'''{
        "ver" : "#V"
        "NodeA-range"   : [ #a ],
        "NodeB-range"   : [ #b ],
        "Nodec-range"   : [ #c ],
        "A-nodes"       : [ #A ],
        "B-nodes"       : [ #B ],
        "C-nodes"       : [ #C ],
        "pos"   : { #D }
        }'''
###############################################################################
# TODO: add fault to any node ' "Fault"     :   [ #F ]
#       this should not generate any arrows and color the node differently
#       show node range in gray
###############################################################################    

###############################################################################
# JSON generator --- example --- https://www.vipinajayakumar.com/parsing-text-with-python/
###############################################################################
    def _parse_line(line):
        for key, range in range_dict.items():
            match = range.search(line)
            if match:
                return key, match
        # if there are no matches
        return None, None


###############################################################################
# JSON parsing
###############################################################################

###############################################################################
# Script starts
###############################################################################

if __name__ == "__main__":
    log(SEPARATOR)
    log(f"SVG graph generator tool v{SCRIPT_VERSION}")

    input_file: str = sys.argv[1]
    log(f"Using file {input_file}")

    # Check if file exists
    if (not os.path.exists(input_file)):
        log(f"File does not exists - {input_file}", error_msg = True)
        exit_script(False)

    # Don´t think this is needed
    # Make copy of original file to avoid altering the input file
    #input_copy: str = f"{input_file[:-4]}_graph{TEXT_EXT}"
    #log(f"Creating new file {input_copy}") 
    #shutil.copy2(input_file, input_copy)





    exit_script(True)