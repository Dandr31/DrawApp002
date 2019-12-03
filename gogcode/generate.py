import math,re
def arcToLine(s_x,s_y,rel_c_x,rel_c_y,e_x,e_y,isCW):
    
    arc_epsilon = 5E-7
    #you can change this to control the segments variable
    arc_tolerance = 0.1#0.002 

    r = math.sqrt(rel_c_x**2+rel_c_y**2)
    
    abs_c_x=s_x + rel_c_x
    abs_c_y=s_y + rel_c_y

   # vector center->start position
   # x1 = abs_c_x - s_x
   # y1 = abs_c_y - s_y
    x1 = -rel_c_x
    y1 = -rel_c_y

    #vector center->end position 
    x2 = e_x - abs_c_x
    y2 = e_y - abs_c_y

    angular_travel = math.atan2(x1*y2 - y1*x2 , x1*x2 + y1*y2)
    
    if(isCW):
        if(angular_travel>= -arc_epsilon ):
            angular_travel -= 2 * math.pi
    else:
        if(angular_travel<= arc_epsilon):
            angular_travel += 2 * math.pi
    segments = math.floor(math.fabs(0.5 - angular_travel * r)/math.sqrt(arc_tolerance * (2 * r - arc_tolerance)))
    if(segments==0):
        segments = 1
    theta_per_segment = angular_travel /segments
    
    cos_T = 2.0- theta_per_segment**2
    sin_T = theta_per_segment * 0.16666667*(cos_T + 4.0)

    cos_T *= 0.5

    cos_Ti = 0
    sin_Ti = 0

    rel_new_vector_y =0
    
    g = ""
    space = " "

    count = 0
    for i in range(int(segments)):
        if(False):
            rel_new_vector_y =  rel_vector_x *sin_T + rel_vector_x * cos_T
            rel_vector_x = rel_vector_x *cos_T - rel_vector_y *sin_T
            rel_vector_y = rel_new_vector_y
            count += 1
        else:
            cos_Ti = math.cos( i * theta_per_segment)
            sin_Ti = math.sin( i * theta_per_segment)
            rel_vector_x = -(rel_c_x)*cos_Ti  + (rel_c_y)*sin_Ti
            rel_vector_y = -(rel_c_x)*sin_Ti  - (rel_c_y)*cos_Ti
            count = 0
        new_e_x = abs_c_x + rel_vector_x
        new_e_y = abs_c_y + rel_vector_y
        
        g+= "G01"+space +"X" + str(new_e_x) + space+ "Y" + str(new_e_y) + "\n"
    return g;
def l(c,start,isCW):
        c= [c[i] if i<len(c) else None for i in  range(6)]
        s_x = start[0]
        s_y = start[1]
        c_x = c[3]
        c_y = c[4]
        e_x = c[0]
        e_y = c[1]
        e_z = c[2]
        #print(s_x,s_y,c_x,c_y,e_x,e_y,isCW)
        return arcToLine(s_x,s_y,c_x,c_y,e_x,e_y,isCW)
################################################################################
###		Point (x,y) operations
################################################################################
class P:
	def __init__(self, x, y=None):
		if not y==None:
			self.x, self.y = float(x), float(y)
		else:
			self.x, self.y = float(x[0]), float(x[1])
	def __add__(self, other): return P(self.x + other.x, self.y + other.y)
	def __sub__(self, other): return P(self.x - other.x, self.y - other.y)
	def __neg__(self): return P(-self.x, -self.y)
	def __mul__(self, other):
		if isinstance(other, P):
			return self.x * other.x + self.y * other.y
		return P(self.x * other, self.y * other)
	__rmul__ = __mul__
	def __div__(self, other): return P(self.x / other, self.y / other)
	def mag(self): return math.hypot(self.x, self.y)
	def unit(self):
		h = self.mag()
		if h: return self / h
		else: return P(0,0)
	def dot(self, other): return self.x * other.x + self.y * other.y
	def rot(self, theta):
		c = math.cos(theta)
		s = math.sin(theta)
		return P(self.x * c - self.y * s,  self.x * s + self.y * c)
	def angle(self): return math.atan2(self.y, self.x)
	def __repr__(self): return '%f,%f' % (self.x, self.y)
	def pr(self): return "%.2f,%.2f" % (self.x, self.y)
	def to_list(self): return [self.x, self.y]	
	def ccw(self): return P(-self.y,self.x)
	def l2(self): return self.x*self.x + self.y*self.y
################################################################################
###     variablie
################################################################################
Zscale=1.0
Zoffset=0.0
Zsafe=0.5
min_arc_radius=.1
################################################################################
###     function
################################################################################
def point_to_point_d2(a,b):
	return (a[0]-b[0])**2 + (a[1]-b[1])**2
def sort_lines(lines):
    if len(lines) == 0 : return []
    lines = [ [key]+lines[key] for key in range(len(lines))]			
    keys = [0]
    end_point = lines[0][3:]
    del lines[0]
    while len(lines)>0:
        dist = [ [point_to_point_d2(end_point,lines[i][1:3]),i] for i in range(len(lines))]
        i = min(dist)[1]
        keys.append(lines[i][0])
        end_point = lines[i][3:]
        del lines[i]
    return keys
    
def sort_curves(curves):
    lines = []
    for curve in curves:
        lines += [curve[0][0][0] + curve[-1][-1][0]]
    return sort_lines(lines)
def atan2(*arg):	
	if len(arg)==1 and ( type(arg[0]) == type([0.,0.]) or type(arg[0])==type((0.,0.)) ) :
		return (math.pi/2 - math.atan2(arg[0][0], arg[0][1]) ) % math.pi2
	elif len(arg)==2 :
		
		return (math.pi/2 - math.atan2(arg[0],arg[1]) ) % math.pi2
	else :
		raise ValueError, "Bad argumets for atan! (%s)" % arg  
tool={'feed': 400.0, 
'diameter': 10.0,
'passing feed': '800',
'sog': '', 
'penetration feed': 100.0,
'gcode after path': '',
'shape': '10', 
'depth step': 1.0,
'4th axis meaning': '',
'spinlde rpm': '',
'id': 'default tool',
'name': 'Default tool',
'in trajectotry': '', 
'style': 
    {
       'stroke-width': '1px', 
       'stroke': '#444444', 
       'fill-opacity': '0.5', 
       'fill': '#00ff00'
    },
'gcode before path': '', 'out trajectotry': '',
'fine feed': '800', 'tool change gcode': '', 
'4th axis scale': 1.0,
'4th axis offset': 0.0, 
'CW or CCW': '',
'penetration angle': 90.0}
################################################################################
###
###		Generate Gcode
###		Generates Gcode on given curve.
###
###		Curve definition [start point, type = {'arc','line','move','end'}, arc center, arc angle, end point, [zstart, zend]]		
###
################################################################################
def generate_gcode(curve, depth):
    Zauto_scale = 1
    g = ""

    def c(c):
        c = [c[i] if i<len(c) else None for i in range(6)]  
        if c[5] == 0 : c[5]=None
        s,s1 = [" X", " Y", " Z", " I", " J", " K"], ["","","","","",""]
        m,a = [1,1,Zscale*Zauto_scale,1,1,Zscale*Zauto_scale], [0,0,Zoffset,0,0,0]
        r = ''	
        for i in range(6):
            if c[i]!=None:
                r += s[i] + ("%f" % (c[i]*m[i]+a[i])) + s1[i]
        return r
    
        
    def calculate_angle(a, current_a):
        return  min(					
                    [abs(a-current_a%math.pi2+math.pi2), a+current_a-current_a%math.pi2+math.pi2],
                    [abs(a-current_a%math.pi2-math.pi2), a+current_a-current_a%math.pi2-math.pi2],
                    [abs(a-current_a%math.pi2),			 a+current_a-current_a%math.pi2])[1]
    if len(curve)==0 : return ""	
            
    
    
    lg, zs, f =  'G00',Zsafe, " F%f"%tool['feed'] 
    current_a = 0
    go_to_safe_distance = "G00" + c([None,None,zs]) + "\n" 
    penetration_feed = " F%s"%tool['penetration feed'] 
    for i in range(1,len(curve)):
    #	Creating Gcode for curve between s=curve[i-1] and si=curve[i] start at s[0] end at s[4]=si[0]
        s, si = curve[i-1], curve[i]
        feed = f if lg not in ['G01','G02','G03'] else ''
        if s[1]	== 'move':
            g += go_to_safe_distance + "G00" + c(si[0]) + "\n" + tool['gcode before path'] + "\n"
            lg = 'G00'
        elif s[1] == 'end':
            g += go_to_safe_distance + tool['gcode after path'] + "\n"
            lg = 'G00'
        elif s[1] == 'line':
            if tool['4th axis meaning'] == "tangent knife" : 
                a = atan2(si[0][0]-s[0][0],si[0][1]-s[0][1])
                a = calculate_angle(a, current_a)
                g+="G01 A%s\n" % (a*tool['4th axis scale']+tool['4th axis offset'])
                current_a = a
            if lg=="G00": g += "G01" + c([None,None,s[5][0]+depth]) + penetration_feed +"(Penetrate)\n"	
            g += "G01" +c(si[0]+[s[5][1]+depth]) + feed + "\n"
            lg = 'G01'
        elif s[1] == 'arc':
            r = [(s[2][0]-s[0][0]), (s[2][1]-s[0][1])]
            if tool['4th axis meaning'] == "tangent knife" : 
                if s[3]<0 : # CW
                    a1 = atan2(s[2][1]-s[0][1],-s[2][0]+s[0][0]) + math.pi 
                else: #CCW
                    a1 = atan2(-s[2][1]+s[0][1],s[2][0]-s[0][0]) + math.pi
                a = calculate_angle(a1, current_a)
                g+="G01 A%s\n" % (a*tool['4th axis scale']+tool['4th axis offset'])
                current_a = a
                axis4 = " A%s"%((current_a+s[3])*tool['4th axis scale']+tool['4th axis offset'])
                current_a = current_a+s[3]
            else : axis4 = ""
            if lg=="G00": g += "G01" + c([None,None,s[5][0]+depth]) + penetration_feed + "(Penetrate)\n"				
            if (r[0]**2 + r[1]**2)>min_arc_radius**2:
                r1, r2 = (P(s[0])-P(s[2])), (P(si[0])-P(s[2]))
                if abs(r1.mag()-r2.mag()) < 0.001 :
                    #g += ("G02" if s[3]<0 else "G03") + c(si[0]+[ s[5][1]+depth, (s[2][0]-s[0][0]),(s[2][1]-s[0][1])  ]) + feed + axis4 + "\n"
                    g+=l(si[0]+[ s[5][1]+depth, (s[2][0]-s[0][0]),(s[2][1]-s[0][1])  ],s[0],(True if s[3]<0 else False) )
                else:
                    r = (r1.mag()+r2.mag())/2
                    g += ("G02" if s[3]<0 else "G03") + c(si[0]+[s[5][1]+depth]) + " R%f" % (r) + feed  + axis4 + "\n"
                lg = 'G02'
            else:
                if tool['4th axis meaning'] == "tangent knife" : 
                    a = atan2(si[0][0]-s[0][0],si[0][1]-s[0][1]) + math.pi
                    a = calculate_angle(a, current_a)
                    g+="G01 A%s\n" % (a*tool['4th axis scale']+tool['4th axis offset'])
                    current_a = a
                g += "G01" +c(si[0]+[s[5][1]+depth]) + feed + "\n"
                lg = 'G01'
    if si[1] == 'end':
        g += go_to_safe_distance + tool['gcode after path'] + "\n"
    return g

            
    