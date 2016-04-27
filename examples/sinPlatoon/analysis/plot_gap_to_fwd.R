#load omnet helper function read .vec files
source('omnet_helpers.R')
#load ggplot for quick and dirty plotting
library(ggplot2)
library(grid)


#function to load data needed for plotting
prepare.vectorApp <- function(vecFile) {
	ds1 <- loadVectorsShaped(vecFile,
		add('vector', select='
(
module(scenario.node[*].appl)
			AND (
				name(nodeId)
				OR name(gap_to_fwd)
			)
)
'))
}
#load simulation output files
#accCloseData <- prepare.vector('../results/Sinusoidal_0_0.3_0.vec')
#accFarData <- prepare.vector('../results/Sinusoidal_0_1.2_0.vec')
platoonMergingDataApp <- prepare.vectorApp('../results/Sinusoidal_1_0.3_0_true.vec')
#platoonMergingDataApp <- prepare.vectorApp('../results/archive/Sinusoidal_1_0.3_0_true.vec')

#add a column to distinguish them before merging
#accCloseData$controller <- "ACC (0.3s)"
#accFarData$controller <- "ACC (1.2s)"
platoonMergingDataApp$controller <- "CACC"

#merge all data together
#allData <- rbind(accCloseData, accFarData, caccData, ploegData)

allData <- rbind(platoonMergingDataApp)

#increase font size
#cex.lab = 2, cex.axis = 2, cex.main = 2, cex.sub = 2

#plot speed as function of time for different controllers
p1 <-	ggplot(allData, aes(x=time, y=gap_to_fwd)) +
		#arguments of two lines below should match number of vehicles in simulation
		geom_line(aes(linetype=factor(nodeId, labels=c("3, ", "4, ","5, ","6, ", "7 ,", "8")))) +
		scale_linetype_manual(values=c("solid", "dashed", "dotted", "dotdash", "longdash", "124242")) +
		labs(linetype=expression("distance to the forward pair of vehicle : ")) +
		theme(legend.position = "top", legend.box = "horizontal") +
		#Use below if you want colorful plot
		#---------------------------------------------------------------------------------------
		#ggplot(allData, aes(x=time, y=gap_to_fwd, col=factor(nodeId))) +
		#theme(legend.position = "top", legend.box = "horizontal") +
		#geom_line() +
		#---------------------------------------------------------------------------------------
		scale_x_continuous(breaks=c(0,25,40,50,60,75,100,125)) +
		scale_y_continuous(breaks=c(-5,0,5,10,15,16,20)) +
		#xlim(c(0, 120)) +
		labs(y=expression("distance" ~ ("m"))) +
		labs(x=expression("time" ~ ("second"))) +
		#ylim(c(24,31)) +
		theme(text = element_text(size=35)) +
		#geom_vline(x = 50, color = "black", size=0.5) +
		facet_grid(controller~.) + 
		theme(panel.margin = unit(2, "lines")) 
		#scale_x_continuous(breaks=c(25,40,50,60,75,90,120))
#print(p1)
ggsave('gap_to_fwd.pdf', p1, width=16, height=9)