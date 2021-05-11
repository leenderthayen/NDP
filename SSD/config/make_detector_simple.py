# -*- coding: utf-8 -*-

import json
import numpy as np

def find_centers(N,R,s):    
    centers = [(0,0)]
    o_centers = [(0,0)]
    
    r = (np.sqrt(3)*R+s)
    theta = np.radians(np.arange(0,360,60))
    dx = np.round(r*np.cos(theta), 11)
    dy = np.round(r*np.sin(theta),11)
    
    while len(centers) < N:
        new_centers = []
        for center in o_centers:
            oldx, oldy = center
            for i in range(len(theta)):
                new_center = (np.round(oldx+dx[i], 8), np.round(oldy+dy[i], 8))
                if new_center not in centers:
                    new_centers.append(new_center)
                    centers.append(new_center)
        o_centers = new_centers
    return centers[:N]

#determine how many rings needed to fit N pixels
def find_rings(N):
    rings= 0
    pixels = 1
    while pixels < N:
        rings += 1
        pixels = rings * 6 + pixels
    return rings

def tuple_average(tuple_list):
    x= 0
    y = 0
    n = len(tuple_list)
    for tup in tuple_list:
        x += tup[0]
        y += tup[1]
    return (x/n,y/n)

def max_dist(center, point_list):
    max_dist = 0
    for point in point_list:
        dist = np.sqrt(point[0]**2 + point[1]**2)
        if dist > max_dist:
            max_dist = dist
    return max_dist

#make bulk N type silicon tube large enough to fit pixels
def make_bulk(N, R, s, H, T=140):
    centers = find_centers(N,R,s)
    
    #place center in center of hexagon centers
    bulk_center = tuple_average(centers)
    
    #determine radius  
    L = max_dist(bulk_center, centers) + R + s + 2
        
    bulk = {
        "type": "semiconductor",
		"material": "Si",
		"bulk_type": "n",
		"temperature": T,
		"geometry": {
			"type": "tube",
			"r": {
				"from": 0.0,
				"to": L
				},
			"h": H,
			"translate": {
                    "x": bulk_center[0],
                    "y": bulk_center[1],
					"z": 0.0
				}
			}
        }
    
    return bulk


def make_pcontact(N, R, s):
    centers = find_centers(N,R,s)
    
    #place center in center of hexagon centers
    bulk_center = tuple_average(centers)
    
    #determine radius  
    L = max_dist(bulk_center, centers) + R + s + 2
        
    pcontact = {
        "type": "contact",
		"material": "Al",
		"name": "p+ contact",
		"potential": -150.0,
		"channel": 1,
		"geometry": {
			"type": "tube",
			"r": {
				"from": 0.0,
				"to": L
                },
				"h": 1e-2,
				"translate": {
                    "x": bulk_center[0],
                    "y": bulk_center[1],
					"z": -1e-2
				}
			}
        }
    
    return pcontact

def make_pixel_contact(R, center, n, H):
    x,y = center
    name = "pixel contact" + str(n+1)
    pixel = {
 			"type": "contact",
 			"material": "Al",
            "name": name,
            "potential": 0.0,
 			"channel": n+2,
 			"geometry": {
				"type": "HexagonalPrism",
				"r": {
 					"from": 0.0,
 					"to": R
				},
				"h": 1e-2,
				"translate": {
                    "x": x,
                    "y": y,
 					"z": H + 0.5e-2
				}
 			}
        }
    return pixel

def make_pixel_contacts(N,R,s, H):    
    centers = find_centers(N,R,s)
        
    pixels = []
    for n in range(N):
        pixels.append(make_pixel_contact(R,centers[n],n, H))
        
    return pixels

def main():
    N = input("Number of pixels: ")
    if N == "":
        N=1
    else:
        N = int(N.strip())
        
    H = input("Detector height(mm): ")
    if H == "":
        H=2
    else:
        H = float(H.strip())
        
    D = input("Pixel size(mm): ")
    if D == "":
        D=10.3
    else:
        D = float(D.strip())
    R = D/2
    
    s = input("Pixel seperation: ")
    if s == "":
        s=0.1
    else:
        s = float(s.strip())
    
    grid = input("grid size(xstart,xend,ystart,yend): ")
    if grid == "":
        centers = find_centers(N,R,s)
        bulk_center = tuple_average(centers)
        L = max_dist(bulk_center, centers) + R + s + 2
        
        xfrom = -L + bulk_center[0] - 1
        xto = L + bulk_center[0] + 1
        yfrom = -L + bulk_center[1] - 1
        yto= L + bulk_center[1] + 1
    else:
        grid = grid.split(',')
        xfrom = float(grid[0].strip())
        xto = float(grid[1].strip())
        yfrom = float(grid[2].strip())
        yto = float(grid[3].strip())
        
    zfrom = -1
    zto = H + 1
        
    #naming scheme "Si NHexagon H-D-s"
    name = "Si " + str(N) + "Hexagon " + str(H) +"-" + str(D) +"-" + str(s)
    
    dictionary = {
        "name": name,
        "units": {
            "length": "mm",
            "angle": "deg",
    		"potential": "V",
    		"temperature": "K"
            },
        "grid": {
		"coordinates": "cartesian",
		"axes": {
            "x": {
                "from": xfrom,
                "to": xto,
                "boundaries": {
                    "left": "inf",
                    "right": "inf"
                }
            },
            "y": {
                "from": yfrom,
                "to": yto,
				"boundaries": {
                    "left": "inf",
                    "right": "inf"
                }
            },
            "z": {
                "from": zfrom,
                "to": zto,
                "boundaries": {
                    "left": "inf",
                    "right": "inf"
                    }
                }
            }
        },
        "medium": "vacuum"  
    }
    
    dictionary["objects"] = [make_bulk(N,R,s,H), make_pcontact(N,R,s)] + make_pixel_contacts(N,R,s,H)
    
    json_object = json.dumps(dictionary, indent = 2)
    
    with open(name + ".json", "w") as outfile:
        outfile.write(json_object)
        
if __name__ == "__main__":
    main()
