# Constant-volume (Otto) cycle, CR 8.2, same heat release per mole, burned gas as
# (a) sim: air-like (N2/O2 rigid rotor + vibration) and (b) real rich products
# (CO2, H2O, CO, H2, N2; harmonic oscillators, no dissociation). Isentropic
# compression of air-like charge, combustion at TDC, isentropic expansion.
import math
R=8.314
def vib(th,T): x=th/T; return x*x*math.exp(x)/(math.exp(x)-1)**2 if x<50 else 0.0
species={ # (rot dof/2 * R contribution in units of R, vib thetas)
 'N2':(2.5,[3353]),'O2':(2.5,[2239]),'CO':(2.5,[3084]),'H2':(2.5,[5986]),
 'CO2':(2.5,[960,960,1997,3380]),'H2O':(3.0,[2295,5262,5404])}
def cv(mix,T): return R*sum(f*(species[s][0]+sum(vib(t,T) for t in species[s][1])) for s,f in mix.items())
def isentrope(mix,T,ratio):   # volume ratio V2/V1, dT = -(R T / cv) dlnV
    n=2000; dl=math.log(ratio)/n
    for _ in range(n): T-= R*T/cv(mix,T)*dl
    return T
def u(mix,T):   # internal energy from 300 K
    n=400; s=0; dT=(T-300)/n
    for i in range(n): s+=cv(mix,300+(i+.5)*dT)*dT
    return s
def T_from_u(mix,U):
    lo,hi=300,6000
    for _ in range(60):
        m=(lo+hi)/2
        if u(mix,m)<U: lo=m
        else: hi=m
    return lo
air={'N2':0.79,'O2':0.21}
# rich products, lambda 0.85, CH1.87, WGS K=3.5: per mole C
lam=0.85; hc=1.87; o2=lam*(1+hc/4); K=3.5
# solve a (CO2) : O balance 2a+(1-a)+h2o = 2*o2, h2o from WGS
lo,hi=0.0,1.0
for _ in range(60):
    a=(lo+hi)/2; h2o=K*a*(hc/2)/((1-a)+K*a)
    if 2*a+(1-a)+h2o<2*o2: lo=a
    else: hi=a
h2=hc/2-h2o; n2=o2*3.76
tot=a+(1-a)+h2o+h2+n2
prod={'CO2':a/tot,'CO':(1-a)/tot,'H2O':h2o/tot,'H2':h2/tot,'N2':n2/tot}
print('products (mole fr):',{k:round(v,3) for k,v in prod.items()})
CR=8.2; T1=330.0
T2=isentrope(air,T1,1/CR); U2=u(air,T2)
for q in (60e3,75e3):
  res=[]
  for name,mix in (('sim air-like',air),('real products',prod)):
    T3=T_from_u(mix,u(mix,T2)+q)          # same heat per mole (mole change neglected)
    T4=isentrope(mix,T3,CR)
    w=(u(mix,T3)-u(mix,T4))-(U2-u(air,T1))
    res.append((name,T3,w/q))
  print('q %.0f kJ/mol: '%(q/1e3)+'; '.join('%s: Tpeak %.0f K eta %.3f'%r for r in res)+'; ratio %.3f'%(res[1][2]/res[0][2]))
