__author__ = 'Kevin'
import os
import argparse
import errno
import xml.etree.ElementTree as ET
from PIL import Image, ImageDraw

height = 500
width = 500

class inklingPoint(object):

    def __init__(self, x, y):
        self.x = float(x)
        self.y = float(y)

def create_directory(path):
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

def split_file(path, outputFolder):
    global height, width
    reading = ET.parse(path)
    r_root = reading.getroot()
    pointsInStroke = []
    numberOfDrawings = 0
    for child in r_root:
        pressure = float(child.find("cord_p").text)
        if(pressure > 0.001):
          cord_x = float(child.find("cord_x").text)
          cord_y = float(child.find("cord_y").text)
          point = inklingPoint(cord_x, cord_y)
          pointsInStroke.append(point)
        else:
            if (len(pointsInStroke) > 0):
                adjustedPoints = x_y_range(pointsInStroke)
                drawImage(str(outputFolder + str(numberOfDrawings) + ".png"), adjustedPoints)
                numberOfDrawings += 1
                pointsInStroke = []

    #draw the final image
    if (len(pointsInStroke) > 0):
        adjustedPoints = x_y_range(pointsInStroke)
        drawImage(str(outputFolder + str(numberOfDrawings) + ".png"), adjustedPoints)
    return

def x_y_range(points):
    min_x = -1.0;
    max_x = -1.0;
    min_y = -1.0;
    max_y = -1.0;

    for i in range(0, len(points)):
        point = points[i]
        if(point.x < min_x or min_x == -1.0):
            min_x = point.x
        if(point.x > max_x or max_x == -1.0):
            max_x = point.x
        if(point.y < min_y or min_y == -1.0):
            min_y = point.y
        if(point.y > max_y or max_y == -1.0):
            max_y = point.y


    scale_x = (max_x - min_x)
    scale_y = (max_y - min_y)
    for i in range(0, len(points)):
        if(scale_x != 0):
          points[i].x = (points[i].x - min_x) / scale_x
        else:
          points[i].x = (points[i].x - min_x)

        if(scale_y != 0):
          points[i].y = (points[i].y - min_y) / scale_y
        else:
          points[i].y = (points[i].y - min_y)



    return points

def drawImage(path, points):
    global height, width
    im = Image.new("RGB", (height,width), "white")
    draw = ImageDraw.Draw(im)
    for i in range(0, len(points) - 1):
        point1 = points[i]
        point2 = points[i + 1]
        x_start = point1.x * width
        x_end = point2.x * width
        y_start = point1.y * height
        y_end = point2.y * height

        draw.line((x_start, y_start, x_end, y_end), fill=3, width=3)
    del draw
    im.save(path, "PNG")

parser = argparse.ArgumentParser()
parser.add_argument('--name')
args = parser.parse_args()
file_name = args.name
create_directory(file_name)
split_file(file_name + ".xml", file_name + "/")