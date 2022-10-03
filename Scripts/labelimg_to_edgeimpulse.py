#!/usr/bin/env python

# LabelImg to Edge Impulse
# -------------------------
# 
# File structure:
# images/
#   img-001.jpg
#   img-001.xml
#   ...
#   img-00n.jpg
#   img-00n.xml
#   bounding_boxes.labels (exported from script)
# labelimg_to_edgeimpulse.py
# data.json (exported from script)
#
# Exported files:
# bounding_boxes.labels in the DATASET directory
# data.json in the main directory
#
# By: Erin K - Oct. 3, 2022
# RobotMissions.org
#
# License: MIT (https://opensource.org/licenses/MIT)

import os
import xml.etree.ElementTree as ET
import json


DATASET = "images/"
xml_files = []

# ------- open the files -------
if os.path.exists(DATASET) == False:
    print("Dataset path does not exist")
    exit()

for file in os.listdir(DATASET):
    if file.endswith(".xml"):
        #print(os.path.join("/mydir", file))
        xml_files.append(file)

print("Found %d files" % len(xml_files))
# -------


# ------- setup output data -------
data = {
    "version": 1,
    "type": "bounding-box-labels",
    "boundingBoxes": {}
}
# -------


# ------- read through xml files -------
for file in xml_files:

    # open file
    root = ET.parse(DATASET + file).getroot()
    #root = ET.parse("images/image-005.xml").getroot() # test file

    # data dict related
    imgname = ""
    imglabels = []

    # each node in the xml
    for child in root:

        #print(child.tag, child.attrib)

        if child.tag == "filename":
            imgname = child.text
            #print("imgname: %s" % imgname)

        # the labeled objects
        if child.tag == "object":

            xmin = 0
            xmax = 0
            ymin = 0
            ymax = 0
            width = 0
            height = 0

            labeldict = {
                "label": "",
                "x": 0,
                "y": 0,
                "width": 0,
                "height": 0
            }

            # go through each of the objects
            for obj in child:
                
                #print("\t", obj.tag, obj.attrib)
                
                if obj.tag == "name":
                    labeldict["label"] = obj.text

                if obj.tag == "bndbox":
                    box = obj

                    for dim in box:
                        if dim.tag == "xmin":
                            xmin = (int)(dim.text)
                        if dim.tag == "xmax":
                            xmax = (int)(dim.text)
                        if dim.tag == "ymin":
                            ymin = (int)(dim.text)
                        if dim.tag == "ymax":
                            ymax = (int)(dim.text)

            width = xmax-xmin
            height = ymax-ymin

            labeldict["x"] = xmin
            labeldict["y"] = ymin
            labeldict["width"] = width
            labeldict["height"] = height

            imglabels.append(labeldict)

            # debug prints...
            #print("%s: (%d, %d, %d, %d) W: %d H: %d" % (labeldict["label"], xmin, xmax, ymin, ymax, labeldict["width"], labeldict["height"]))
            # for item in imglabels:
            #     print(item)

    # create the dictionary of boxes
    add_bb = {
        imgname: []
    }
    add_bb[imgname] = imglabels
    #print(add_bb)

    # update the data
    data["boundingBoxes"].update(add_bb)
# -------


# ------- export -------
with open('data.json', 'w', encoding='utf-8') as f:
    json.dump(data, f, ensure_ascii=False, indent=4)

with open((DATASET + 'bounding_boxes.labels'), 'w', encoding='utf-8') as f:
    json.dump(data, f, ensure_ascii=False, indent=4)

print("Export complete")
# -------








