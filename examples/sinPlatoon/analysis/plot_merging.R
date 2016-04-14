#load omnet helper function read .vec files
source('omnet_helpers.R')
#load ggplot for quick and dirty plotting
library(ggplot2)
library(grid)


#function to load data needed for plotting
prepare.vector <- function(vecFile) {
	ds1 <- loadVectorsShaped(vecFile,
		add('vector', select='
(
module(scenario.node[*].prot)
			AND (
				name(nodeId)
				OR name(distance)
				OR name(relativeSpeed)
				OR name(speed)
				OR name(posx)
				OR name(posy)
				OR name(gap_to_fwd)
			)
)
'))
}

#load simulation output files
#accCloseData <- prepare.vector('../results/Sinusoidal_0_0.3_0.vec')
#accFarData <- prepare.vector('../results/Sinusoidal_0_1.2_0.vec')
platoonMergingData <- prepare.vector('../results/Sinusoidal_1_0.3_0_true.vec')

#add a column to distinguish them before merging
#accCloseData$controller <- "ACC (0.3s)"
#accFarData$controller <- "ACC (1.2s)"
platoonMergingData$controller <- "CACC"

#merge all data together
#allData <- rbind(accCloseData, accFarData, caccData, ploegData)
allData <- platoonMergingData

#increase font size
#cex.lab = 2, cex.axis = 2, cex.main = 2, cex.sub = 2

#plot speed as function of time for different controllers
p1 <-	#ggplot(allData, aes(x=time, y=speed)) +
		#arguments of two lines below should match number of vehicles in simulation
		#geom_line(aes(linetype=factor(nodeId, labels=c("V0, ", "V1, ","V2, ","V3, ", "V4")))) +
		#scale_linetype_manual(values=c("solid", "dotted","dashed","twodash", "12345678")) +
		#labs(linetype=expression("speed of the vehicle : ")) +
		#theme(legend.position = "top", legend.box = "horizontal") +
		#Use below if you want colorful plot
		#---------------------------------------------------------------------------------------
		ggplot(allData, aes(x=time, y=speed, col=factor(nodeId))) +
		geom_line() +
		#---------------------------------------------------------------------------------------
		scale_x_continuous(breaks=c(0,25,40,50,60,75,100,120)) +
		#xlim(c(0, 120)) +
		labs(y=expression("speed" ~ ("m/s"))) +
		labs(col=expression("vehicle")) +
		labs(x=expression("time" ~ ("second"))) +
		#ylim(c(24,31)) +
		theme(text = element_text(size=35)) +
		geom_vline(x = 40, color = "black", size=1.5) +
		geom_vline(x = 100, color = "black", size=1.5) +
		geom_vline(x = 60, color = "black", size=1.5) +
		facet_grid(controller~.) + 
		theme(panel.margin = unit(2, "lines")) 
		#scale_x_continuous(breaks=c(25,40,50,60,75,90,120))
#print(p1)
ggsave('speed.pdf', p1, width=16, height=9)

#plot distance as function of time for different controllers
ss <-	subset(allData, nodeId != 0, nodeId != 1)
ss2 <-	subset(ss, nodeId != 1)
p2 <-	#ggplot(ss, aes(x=time, y=distance)) +
		#geom_line(aes(linetype=factor(nodeId, labels=c("V0-V1, ", "V1-V2, ","V2-V3, ","V3-V4 ")))) +
		#scale_linetype_manual(values=c("solid", "dotted","dashed","twodash")) +
		#labs(linetype=expression("gap between vehicles : ")) +
		#theme(legend.position = "top", legend.box = "horizontal") +
		#Use below if you want colorful plot
		#---------------------------------------------------------------------------------------
		#ggplot(ss, aes(x=time, y=distance, col=factor(nodeId, labels=c("0-1", "1-2","2-3","3-4")))) +
		ggplot(ss2, aes(x=time, y=distance, col=factor(nodeId))) +
		geom_line() +
		labs(col=expression("gap")) +
		#---------------------------------------------------------------------------------------
		scale_x_continuous(breaks=c(0,25,40,50,70,100,125,150)) +
		scale_y_continuous(breaks=c(0,10,20,40,60,80)) +
		#xlim(c(0, 120)) +
		labs(y=expression("distance" ~ ("m"))) +
		labs(x=expression("time" ~ ("second"))) +
		theme(text = element_text(size=35)) +
		#geom_hline(y = 10, color = "black", size=0.5) +
		geom_vline(x = 40, color = "black", size=0.5) +
		geom_vline(x = 100, color = "black", size=0.5) +
		facet_grid(controller~., scales="free_y") + 
		theme(panel.margin = unit(2, "lines"))
#print(p2)
ggsave('distance.pdf', p2, width=16, height=9)
