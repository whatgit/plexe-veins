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
ss <-	subset(allData, nodeId != 0)
ss2 <-	subset(ss, nodeId != 1)
ss3 <-  subset(ss2,nodeId != 2)
#increase font size
#cex.lab = 2, cex.axis = 2, cex.main = 2, cex.sub = 2

#plot speed as function of time for different controllers
p1 <-	ggplot(ss3, aes(x=time, y=speed)) +
		#arguments of two lines below should match number of vehicles in simulation
		geom_line(aes(linetype=factor(nodeId, labels=c("3, ", "4, ","5, ","6, ", "7, ", "8")))) +
		scale_linetype_manual(values=c("solid", "dashed", "dotted", "dotdash", "longdash", "124242")) +
		labs(linetype=expression("speed of the vehicle : ")) +
		theme(legend.position = "top", legend.box = "horizontal") +
		#Use below if you want colorful plot
		#---------------------------------------------------------------------------------------
		#ggplot(ss3, aes(x=time, y=speed, col=factor(nodeId))) +
		#labs(col=expression("vehicle")) +
		#geom_line() +
		#---------------------------------------------------------------------------------------
		scale_x_continuous(breaks=c(0,25,50,75,100,125,150,175,200)) +
		scale_y_continuous(breaks=c(0,5,10,15,20,25,30)) +
		#xlim(c(0, 120)) +
		labs(y=expression("speed" ~ ("m/s"))) +
		labs(x=expression("time" ~ ("second"))) +
		#ylim(c(24,31)) +
		theme(text = element_text(size=35)) +
		#geom_vline(x = 50, color = "black", size=0.5) +
		#geom_vline(x = 150, color = "black", size=0.5) +
		facet_grid(controller~.) + 
		theme(panel.margin = unit(2, "lines")) 
		#scale_x_continuous(breaks=c(25,40,50,60,75,90,120))
#print(p1)
ggsave('speed.pdf', p1, width=16, height=9)

#plot distance as function of time for different controllers
#ss <-	subset(allData, nodeId != 0)
#ss2 <-	subset(ss, nodeId != 1)
#ss3 <-  subset(ss2,nodeId != 2)
p2 <-	ggplot(ss3, aes(x=time, y=distance)) +
		geom_line(aes(linetype=factor(nodeId, labels=c("3, ", "4, ","5 , ","6 , ","7 , ","8")))) +
		scale_linetype_manual(values=c("solid", "dashed", "dotted", "dotdash", "longdash", "124242")) +
		labs(linetype=expression("distance gap to MIO of vehicle id : ")) +
		theme(legend.position = "top", legend.box = "horizontal") +
		#Use below if you want colorful plot
		#---------------------------------------------------------------------------------------
		#ggplot(ss, aes(x=time, y=distance, col=factor(nodeId, labels=c("0-1", "1-2","2-3","3-4")))) +
		#ggplot(ss3, aes(x=time, y=distance, col=factor(nodeId))) +
		#geom_line() +
		#theme(legend.position = "top", legend.box = "horizontal") +
		#labs(col=expression("gap")) +
		#---------------------------------------------------------------------------------------
		scale_x_continuous(breaks=c(0,25,50,75,100,125,150,175,200)) +
		scale_y_continuous(breaks=c(0,10,16,20,40,60,80)) +
		#xlim(c(0, 120)) +
		labs(y=expression("distance" ~ ("m"))) +
		labs(x=expression("time" ~ ("second"))) +
		theme(text = element_text(size=35)) +
		#geom_hline(y = 10, color = "black", size=0.5) +
		#geom_vline(x = 150, color = "black", size=0.5) +
		#geom_vline(x = 50, color = "black", size=0.5) +
		facet_grid(controller~., scales="free_y") + 
		theme(panel.margin = unit(2, "lines"))
#print(p2)
ggsave('distance.pdf', p2, width=16, height=9)
