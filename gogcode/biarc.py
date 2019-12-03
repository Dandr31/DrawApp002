import math
import bezmisc
################################################################################
###     variablie
################################################################################
'''
('options.biarc_max_split_depth', 4)
('straight_distance_tolerance', 0.0001)
('straight_tolerance', 0.0001)
('EMC_TOLERANCE_EQUAL', 1e-05)
'''
biarc_max_split_depth =4
options={biarc_max_split_depth}
straight_distance_tolerance=0.0001
straight_tolerance= 0.0001
EMC_TOLERANCE_EQUAL=1e-05
min_arc_radius=.1
math.pi2 = math.pi*2
biarc_tolerance=1
################################################################################
###     function
################################################################################
def csp_split(sp1,sp2,t=.5) :
	[x1,y1],[x2,y2],[x3,y3],[x4,y4] = sp1[1], sp1[2], sp2[0], sp2[1] 
	x12 = x1+(x2-x1)*t
	y12 = y1+(y2-y1)*t
	x23 = x2+(x3-x2)*t
	y23 = y2+(y3-y2)*t
	x34 = x3+(x4-x3)*t
	y34 = y3+(y4-y3)*t
	x1223 = x12+(x23-x12)*t
	y1223 = y12+(y23-y12)*t
	x2334 = x23+(x34-x23)*t
	y2334 = y23+(y34-y23)*t
	x = x1223+(x2334-x1223)*t
	y = y1223+(y2334-y1223)*t
	return [sp1[0],sp1[1],[x12,y12]], [[x1223,y1223],[x,y],[x2334,y2334]], [[x34,y34],sp2[1],sp2[2]]
def cspseglength(sp1,sp2, tolerance = 0.01):
	bez = (sp1[1][:],sp1[2][:],sp2[0][:],sp2[1][:])
	return bezmisc.bezierlength(bez, tolerance)	

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

def between(c,x,y):
		return x-straight_tolerance<=c<=y+straight_tolerance or y-straight_tolerance<=c<=x+straight_tolerance
def point_to_arc_distance(p, arc):
	###		Distance calculattion from point to arc
	P0,P2,c,a = arc
	dist = None
	p = P(p)
	r = (P0-c).mag()
	if r>0 :
		i = c + (p-c).unit()*r
		alpha = ((i-c).angle() - (P0-c).angle())
		if a*alpha<0: 
			if alpha>0:	alpha = alpha-math.pi2
			else: alpha = math.pi2+alpha
		if between(alpha,0,a) or min(abs(alpha),abs(alpha-a))<straight_tolerance : 
			return (p-i).mag(), [i.x, i.y]
		else : 
			d1, d2 = (p-P0).mag(), (p-P2).mag()
			if d1<d2 : 
				return (d1, [P0.x,P0.y])
			else :
				return (d2, [P2.x,P2.y])

def csp_to_arc_distance(sp1,sp2, arc1, arc2, tolerance = 0.01 ): # arc = [start,end,center,alpha]
	n, i = 10, 0
	d, d1, dl = (0,(0,0)), (0,(0,0)), 0
	while i<1 or (abs(d1[0]-dl[0])>tolerance and i<4):
		i += 1
		dl = d1*1	
		for j in range(n+1):
			t = float(j)/n
			p = csp_at_t(sp1,sp2,t) 
			d = min(point_to_arc_distance(p,arc1), point_to_arc_distance(p,arc2))
			d1 = max(d1,d)
		n=n*2
	return d1[0]
def csp_at_t(sp1,sp2,t):
	ax,bx,cx,dx = sp1[1][0], sp1[2][0], sp2[0][0], sp2[1][0]
	ay,by,cy,dy = sp1[1][1], sp1[2][1], sp2[0][1], sp2[1][1]

	x1, y1 = ax+(bx-ax)*t, ay+(by-ay)*t	
	x2, y2 = bx+(cx-bx)*t, by+(cy-by)*t	
	x3, y3 = cx+(dx-cx)*t, cy+(dy-cy)*t	
	
	x4,y4 = x1+(x2-x1)*t, y1+(y2-y1)*t 
	x5,y5 = x2+(x3-x2)*t, y2+(y3-y2)*t 
	
	x,y = x4+(x5-x4)*t, y4+(y5-y4)*t 
	return [x,y]

################################################################################
###
###		Biarc function
###
###		Calculates biarc approximation of cubic super path segment
###		splits segment if needed or approximates it with straight line
###
################################################################################
def biarc(sp1, sp2, z1, z2, depth=0):
	
	def biarc_split(sp1,sp2, z1, z2, depth): 
		if depth<biarc_max_split_depth:
			sp1,sp2,sp3 = csp_split(sp1,sp2)
			l1, l2 = cspseglength(sp1,sp2), cspseglength(sp2,sp3)
			if l1+l2 == 0 : zm = z1
			else : zm = z1+(z2-z1)*l1/(l1+l2)
			return biarc(sp1,sp2,z1,zm,depth+1)+biarc(sp2,sp3,zm,z2,depth+1)
		else: return [ [sp1[1],'line', 0, 0, sp2[1], [z1,z2]] ]

	P0, P4 = P(sp1[1]), P(sp2[1])
	TS, TE, v = (P(sp1[2])-P0), -(P(sp2[0])-P4), P0 - P4
	tsa, tea, va = TS.angle(), TE.angle(), v.angle()
	if TE.mag()<straight_distance_tolerance and TS.mag()<straight_distance_tolerance:	
		# Both tangents are zerro - line straight
		return [ [sp1[1],'line', 0, 0, sp2[1], [z1,z2]] ]
	if TE.mag() < straight_distance_tolerance:
		TE = -(TS+v).unit()
		r = TS.mag()/v.mag()*2
	elif TS.mag() < straight_distance_tolerance:
		TS = -(TE+v).unit()
		r = 1/( TE.mag()/v.mag()*2 )
	else:	
		r=TS.mag()/TE.mag()
	TS, TE = TS.unit(), TE.unit()
	tang_are_parallel = ((tsa-tea)%math.pi<straight_tolerance or math.pi-(tsa-tea)%math.pi<straight_tolerance )
	if ( tang_are_parallel  and 
				((v.mag()<straight_distance_tolerance or TE.mag()<straight_distance_tolerance or TS.mag()<straight_distance_tolerance) or
					1-abs(TS*v/(TS.mag()*v.mag()))<straight_tolerance)	):
				# Both tangents are parallel and start and end are the same - line straight
				# or one of tangents still smaller then tollerance

				# Both tangents and v are parallel - line straight
		return [ [sp1[1],'line', 0, 0, sp2[1], [z1,z2]] ]

	c,b,a = v*v, 2*v*(r*TS+TE), 2*r*(TS*TE-1)
	if v.mag()==0:
		return biarc_split(sp1, sp2, z1, z2, depth)
	asmall, bsmall, csmall = abs(a)<10**-10,abs(b)<10**-10,abs(c)<10**-10 
	if 		asmall and b!=0:	beta = -c/b
	elif 	csmall and a!=0:	beta = -b/a 
	elif not asmall:	 
		discr = b*b-4*a*c
		if discr < 0:	raise ValueError, (a,b,c,discr)
		disq = discr**.5
		beta1 = (-b - disq) / 2 / a
		beta2 = (-b + disq) / 2 / a
		if beta1*beta2 > 0 :	raise ValueError, (a,b,c,disq,beta1,beta2)
		beta = max(beta1, beta2)
	elif	asmall and bsmall:	
		return biarc_split(sp1, sp2, z1, z2, depth)
	alpha = beta * r
	ab = alpha + beta 
	P1 = P0 + alpha * TS
	P3 = P4 - beta * TE
	P2 = (beta / ab)  * P1 + (alpha / ab) * P3


	def calculate_arc_params(P0,P1,P2):
		D = (P0+P2)/2
		if (D-P1).mag()==0: return None, None
		R = D - ( (D-P0).mag()**2/(D-P1).mag() )*(P1-D).unit()
		p0a, p1a, p2a = (P0-R).angle()%(2*math.pi), (P1-R).angle()%(2*math.pi), (P2-R).angle()%(2*math.pi)
		alpha =  (p2a - p0a) % (2*math.pi)					
		if (p0a<p2a and  (p1a<p0a or p2a<p1a))	or	(p2a<p1a<p0a) : 
			alpha = -2*math.pi+alpha 
		if abs(R.x)>1000000 or abs(R.y)>1000000  or (R-P0).mag<min_arc_radius**2 :
			return None, None
		else :	
			return  R, alpha
	R1,a1 = calculate_arc_params(P0,P1,P2)
	R2,a2 = calculate_arc_params(P2,P3,P4)
	if R1==None or R2==None or (R1-P0).mag()<straight_tolerance or (R2-P2).mag()<straight_tolerance	: return [ [sp1[1],'line', 0, 0, sp2[1], [z1,z2]] ]
	
	d = csp_to_arc_distance(sp1,sp2, [P0,P2,R1,a1],[P2,P4,R2,a2])
	if d > biarc_tolerance and depth<biarc_max_split_depth	 : return biarc_split(sp1, sp2, z1, z2, depth)
	else:
		if R2.mag()*a2 == 0 : zm = z2
		else : zm  = z1 + (z2-z1)*(abs(R1.mag()*a1))/(abs(R2.mag()*a2)+abs(R1.mag()*a1)) 

		l = (P0-P2).l2()
		if  l < EMC_TOLERANCE_EQUAL**2 or l<EMC_TOLERANCE_EQUAL**2 * R1.l2() /100 :
			# arc should be straight otherwise it could be threated as full circle
			arc1 = [ sp1[1], 'line', 0, 0, [P2.x,P2.y], [z1,zm] ] 
		else :
			arc1 = [ sp1[1], 'arc', [R1.x,R1.y], a1, [P2.x,P2.y], [z1,zm] ] 

		l = (P4-P2).l2()
		if  l < EMC_TOLERANCE_EQUAL**2 or l<EMC_TOLERANCE_EQUAL**2 * R2.l2() /100 :
			# arc should be straight otherwise it could be threated as full circle
			arc2 = [ [P2.x,P2.y], 'line', 0, 0, [P4.x,P4.y], [zm,z2] ] 
		else :
			arc2 = [ [P2.x,P2.y], 'arc', [R2.x,R2.y], a2, [P4.x,P4.y], [zm,z2] ]
		
		return [ arc1, arc2 ]


def biarc_curve_segment_length(seg):
	if seg[1] == "arc" :
		return math.sqrt((seg[0][0]-seg[2][0])**2+(seg[0][1]-seg[2][1])**2)*seg[3]
	elif seg[1] == "line" :	
		return math.sqrt((seg[0][0]-seg[4][0])**2+(seg[0][1]-seg[4][1])**2)
	else: 
		return 0	
#transform each point to another coordinate
def transform(source_point, reverse=False):
	scaleFactor = 0.01
	x,y = source_point[0], source_point[1]
	t=[[0.28222222386412843, 0.0, -0.0], [0.0, -0.28222222386412843, 263.87777931296006], [0.0, 0.0, 1.0]]
	#return [t[0][0]*x+t[0][1]*y+t[0][2], t[1][0]*x+t[1][1]*y+t[1][2]]#transform each points g.gcode
	#return [t[1][1]*x+t[1][0]*y+t[0][2], t[0][1]*x+t[0][0]*y+t[1][2]]#transform each points gg.gcode
	#return [t[0][0]*x+t[0][1]*y+t[0][2], t[0][1]*x+t[0][0]*y+t[1][2]]#transform each points ggg.gcode
	return [(t[0][0]*x+t[0][1]*y+t[0][2])*scaleFactor, (t[0][1]*x+t[0][0]*y+t[1][2])*scaleFactor]#transform each points ggg.gcode

def transform_csp( csp_, reverse = False):
		csp = [  [ [csp_[i][j][0][:],csp_[i][j][1][:],csp_[i][j][2][:]]  for j in range(len(csp_[i])) ]   for i in range(len(csp_)) ]
		for i in xrange(len(csp)):
			for j in xrange(len(csp[i])): 
				for k in xrange(len(csp[i][j])): 
					csp[i][j][k] = transform(csp[i][j][k], reverse)
		return csp
def parse_curve(p,w = None, f = None): 
    c = []
    if len(p)==0 : 
        return []
    p =transform_csp(p)
#    p = self.transform_csp(p, layer)#transform all points  
#    print("self.transform_csp \n")
#    print(p)

    ### Sort to reduce Rapid distance	
    k = range(1,len(p))
    keys = [0]
    while len(k)>0:
        end = p[keys[-1]][-1][1]
        dist = None
        for i in range(len(k)):
            start = p[k[i]][0][1]
            dist = max(   ( -( ( end[0]-start[0])**2+(end[1]-start[1])**2 ) ,i)	,   dist )
        keys += [k[dist[1]]]
        del k[dist[1]]
    for k in keys:
        subpath = p[k]
        c += [ [	[subpath[0][1][0],subpath[0][1][1]]   , 'move', 0, 0] ]
        for i in range(1,len(subpath)):
            sp1 = [  [subpath[i-1][j][0], subpath[i-1][j][1]] for j in range(3)]
            sp2 = [  [subpath[i  ][j][0], subpath[i  ][j][1]] for j in range(3)]
            c += biarc(sp1,sp2,0,0) if w==None else biarc(sp1,sp2,-f(w[k][i-1]),-f(w[k][i]))
#					l1 = biarc(sp1,sp2,0,0) if w==None else biarc(sp1,sp2,-f(w[k][i-1]),-f(w[k][i]))
#					print_((-f(w[k][i-1]),-f(w[k][i]), [i1[5] for i1 in l1]) )
        c += [ [ [subpath[-1][1][0],subpath[-1][1][1]]  ,'end',0,0] ]
    return c

'''
import pathC,pahtd
csp = pathC.parsePath(pathd.getStrD())
curves=[]
curves += [parse_curve([subpath]) for subpath in csp  ]

import generate
gcode=""
for curve in curves:
    gcode += generate.generate_gcode(curve, 1.0)  
print(gcode)
'''