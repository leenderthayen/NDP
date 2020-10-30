# -*- coding: utf-8 -*-

import json
import numpy as np

#determine how many rings needed to fit N pixels
def find_rings(N):
    rings= 0
    pixels = 1
    while pixels < N:
        rings += 1
        pixels = rings * 6 + pixels
    return rings

#make bulk N type silicon tube large enough to fit pixels
def make_bulk(N, R, s, T=140, cdm=None):
    rings = find_rings(N)
    
    #determine distance from center to farthest corner    
    L = (1+rings*2) * np.sqrt(6) * (R+s/2) / (1+np.sqrt(3))
    #add a little bit extra length to L to prevent edge effects
    L += 1
    
    if cdm == None:
        cdm = {
			"name": "linear",
			"r": {
				"init": 0.0,
				"gradient": 0.0
				},
			"phi": {
				"init": 0.0,
				"gradient": 0.0
				},
			"z": {
				"init": 1.9e8,
				"gradient": 0.0
				}
			}
        
    bulk = {
        "type": "semiconductor",
		"material": "Si",
		"bulk_type": "n",
		"temperature": T,
		"charge_density_model": cdm,
		"geometry": {
			"type": "tube",
			"r": {
				"from": 0.0,
				"to": L
				},
			"h": 2.0,
			"translate": {
					"z": 1.0
				}
			}
        }
    
    return bulk

#make p contact completely covering bottom of bulk Si
def make_pcontact(N, R, s, ph):
    rings = find_rings(N)
    
    #determine distance from center to farthest corner    
    L = (1+rings*2) * np.sqrt(6) * (R+s/2)/ (1+np.sqrt(3))
    #add a little bit extra length to L to prevent edge effects
    L += 1
        
    pcontact = {
        "type": "contact",
		"material": "Si",
		"name": "p+ contact",
		"potential": -150.0,
		"channel": 1,
		"geometry": {
			"type": "tube",
			"r": {
				"from": 0.0,
				"to": L
                },
				"h": ph,
				"translate": {
					"z": -5e-3
				}
			}
        }
    
    return pcontact

#make a single pixel
def make_pixel(R, nh, center, n):
    x,y = center
    name = "n+ contact " + str(n+1)
    pixel = {
 			"type": "contact",
 			"material": "Si",
 			"name": name,
 			"potential": 0.0,
 			"channel": n+2,
 			"geometry": {
				"type": "HexagonalPrism",
				"r": {
 					"from": 0.0,
 					"to": R
				},
				"h": nh,
				"translate": {
                    "x": x,
                    "y": y,
 					"z": 2.005
				}
 			}
        }
    return pixel

#make pspray or pstop around a single pixel
def make_pspray(R,s, tw, th, center, n,N):
    x,y = center
    name = "pspray contact " + str(n+1)
    pixel = {
 			"type": "contact",
 			"material": "Si",
 			"name": name,
 			"potential": 0.0,
 			"channel": N + n+2,
 			"geometry": {
				"type": "HexagonalPrism",
				"r": {
 					"from": R+s-tw,
 					"to": R+s
				},
				"h": th,
				"translate": {
                    "x": x,
                    "y": y,
 					"z": 2.005
				}
 			}
        }
    return pixel

#make N pixels starting with center and then moving counterclockwise around
def make_pixels(N,R,s,nh):    
    centers = [(0,0)]
    o_centers = [(0,0)]
    
    r = (2*R+s)
    theta = np.radians(np.arange(0,360,60))
    dx = r*np.cos(theta)
    dy = r*np.sin(theta)
    
    while len(centers) < N:
        new_centers = []
        for center in o_centers:
            oldx, oldy = center
            for i in range(len(theta)):
                new_center = (oldx+dx[i], oldy+dy[i])
                if new_center not in centers:
                    new_centers.append(new_center)
        centers = centers + new_centers
        o_centers = new_centers
        
    pixels = []
    for n in range(N):
        pixels.append(make_pixel(R,nh,centers[n],n))
    
    return pixels

#make pspray/pstop around every pixel
def make_psprays(N,R,s,tw, th):    
    centers = [(0,0)]
    o_centers = [(0,0)]
    
    r = (2*R+s)
    theta = np.radians(np.arange(0,360,60))
    dx = r*np.cos(theta)
    dy = r*np.sin(theta)
    
    while len(centers) < N:
        new_centers = []
        for center in o_centers:
            oldx, oldy = center
            for i in range(len(theta)):
                new_center = (oldx+dx[i], oldy+dy[i])
                if new_center not in centers:
                    new_centers.append(new_center)
        centers = centers + new_centers
        o_centers = new_centers
        
    pixels = []
    for n in range(N):
        pixels.append(make_pspray(R,s,tw,th,centers[n],n,N))
    
    return pixels
    

def main():
    N = input("Number of pixels: ")
    if N == "":
        N=1
    else:
        N = int(N.strip())
        
    D = input("Pixel size(mm): ")
    if D == "":
        D=10.3
    else:
        D = float(D.strip())
    R = D/2

    tw = input("pspray width: ")
    if tw == "":
        tw=0
    else:
        tw = float(tw.strip())
    
    th = input("pspray thickness: ")
    if th == "":
        th=0
    else:
        th = float(th.strip())
        
    s = input("Pixel seperation")
    if s == "":
        s=0.1
    else:
        s = float(s.strip())
    
    ph = input("p+ thickness: ")
    if ph == "":
        ph = 0.01
    else:
        ph = float(ph.strip())
    
    nh = input("n+ thickness: ")
    if nh == "":
        nh = 0.01
    else:
        nh = float(nh.strip())
    
    grid = input("grid size(xstart,xend,ystart,yend): ")
    if grid == "":
        xfrom = -D
        xto = D
        yfrom = -D
        yto= D
    else:
        grid = grid.split(',')
        xfrom = float(grid[0].strip())
        xto = float(grid[1].strip())
        yfrom = float(grid[2].strip())
        yto = float(grid[3].strip())
        
    zfrom = -1
    zto = 3
        
    #naming scheme "Si NHexagon D-twth-sphnh"
    name = "Si " + str(N) + "Hexagon " + str(D) +"-" + str(tw) + str(th) + "-" + str(s) + str(ph) + str(nh)
    
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
    
    if tw > 0:
        dictionary["objects"] = [make_bulk(N,R,s), make_pcontact(N,R,s,ph)] + make_pixels(N,R,s,nh) + make_psprays(N,R,s,th,tw)
    else:
        dictionary["objects"] = [make_bulk(N,R,s), make_pcontact(N,R,s,ph)] + make_pixels(N,R,s,nh)
        
    
    
    json_object = json.dumps(dictionary, indent = 2)
    
    with open(name + ".json", "w") as outfile:
        outfile.write(json_object)
        
if __name__ == "__main__":
    main()