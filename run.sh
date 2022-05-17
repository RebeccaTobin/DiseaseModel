number=1 # number of simulated individuals
R=3.0 #Gamma+/Gamma-
gammap=7.5 #damage rate constant
gamman=6.5 #repair rate constant
N=10000 #network size
nd=2
alpha=2.27 #scale free exponent
avgdeg=4 #average degree (must be even for scalefree and smallworld)
Mortality="AND" #mortality condition: "OR" or "AND"
Folder="Local" #"Local" uses the Data/ folder. Otherwise use scratch space on a computing cluster.
TopologyID=1
pA=0.0 #if > 0, will modify the assortativity of the network. https://en.wikipedia.org/wiki/Assortativity
Tag=0
StartTime=0.0183
tau=0.00183 # 1yr
k_min=0
k_max=10000  # k_min <= k <= k_max
knn_min=0
knn_max=10000  # knn_min <= knn < knn_max
m=0.02
r=1.0

make

# loop through parameter values
for StartTime in 0.0366 0.0549 0.0732 0.0915 0.1098 0.1281 0.1464 0.1647 0.183 # onset ages: 20,30,40,50,60,70,80,90,100
do
    ./main $number $R $gammap $gamman $N $nd $alpha $avgdeg $Mortality $Folder $TopologyID $pA $Tag $StartTime $tau $k_min $k_max $knn_min $knn_max $m $r D
    ./main $number $R $gammap $gamman $N $nd $alpha $avgdeg $Mortality $Folder $TopologyID $pA $Tag $StartTime $tau $k_min $k_max $knn_min $knn_max $m $r C
done


