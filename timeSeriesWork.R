########  set up environment  ############
memory.limit(96000)
setwd("C:\\Users\\Brian\\Documents\\R\\cars")  ### change this to the appropriate directory
#install.packages("readxl")  #### run these 2 lines only the first time
#install.packages("forecast")

library(readxl)
library("forecast") # load the "forecast" R library


##################   import data  #########################
# Different car types (make/model) can require different data import processes, esp. for salesdate.
# Pre-processing in Excel is sometimes necessary.  Text-to-column was used to split the date field components 
# all the way down to year, month before importing in one instance.  In another case, the data was foramted such
# that all the work could be done cleanly within R.  
# 
# Test it step-wise through the import to ensure the data has been accurately imported.  The template commands
# below will faciliate this process, but all commands will not have to be run.

###  filter by the desired make & model in Excel and then remove all columns(if desired) 
# keeping the columns: grossprofit,modelname,	salesdate (and any columns derived from sales date)
### data import may need to come from an txt, xls or an xlsx file.  
# If salesdate looks like this, 41846, instead of a date, resave it as xls or txt and try again.  If it still
# isn't working, try splitting the data components out using the Excel tool text-to-column.

#use one of these, depending on file type
data <- read_excel("pilot.xlsx")
#data <- read_excel("pilot2.xls")  ### xls format - one tab only in sheet
#data <- read.table("pilot.txt", sep="\t", header=TRUE)

###  check data types
#lapply(data['grossprofit'], class)
#lapply(data['salesdate'], class) ## if this is 'posix' the date imported successfully and can be converted into a date
#head(data['salesdate'])

# year and month should be extracted from the date here if the date field wouldl import as a date.
# if not, and if year and month were parsed in Excel, these fields should already exist from import
data['year'] <- format(data['salesdate'], "%Y")
data['month'] <- format(data['salesdate'], "%m")


############  data prep  ####################
# monthIndex is a count of months from the earliest to the latest. 
# It is the time index for the the time series model.
# This section of code creates and populates the monthIndex column.
data['monthIndex'] <- vector(,dim(data['year'])[1])
for (i in 1:dim(data['year'])[1]){
  if (is.na(data['year'][i,1])){
    data['monthIndex'][i,1] = NaN
  }
  else if (data['year'][i,1] == "2014"){
    data['monthIndex'][i,1] = as.numeric(data['month'][i,1])
  }
  else if (data['year'][i,1] == "2015"){
    data['monthIndex'][i,1] = as.numeric(data['month'][i,1]) + 12
  }
  else if (data['year'][i,1] == "2016"){
    data['monthIndex'][i,1] = as.numeric(data['month'][i,1]) + 24
  }
  else if (data['year'][i,1] == "2017"){
    data['monthIndex'][i,1] = as.numeric(data['month'][i,1]) + 36
  }
}

###  create the indata dataframe to hold the final modeling data
temp1 <-aggregate(data["grossprofit"],by=data[("monthIndex")], FUN=mean)
tempCount <-aggregate(data["grossprofit"],by=data[("monthIndex")], FUN=length)
indata <- temp1
indata$count <- tempCount$grossprofit
indata <- data.matrix(indata)## convert all to numeric
indata <- transform(indata, ratio = grossprofit/count)##  create ratio column
#head(indata)



###########  model   #############
# a good reference for the time series modeling is:
# https://a-little-book-of-r-for-time-series.readthedocs.io/en/latest/src/timeseries.html
# The ARIMA section holds most of the necessary information.
# The first step in the process is to select the arima parameters.  This is done interactively
# by plotting the output of several functions (differencing, acf, pacf)
# This process is to be done (repeated) for margin/n, n, margin.  The code for the first process is
# documented and the same procedures and methods should be applied to the latter two.

plot.ts(indata$count)
plot.ts(indata$grossprofit)
plot.ts(indata$ratio)

###### forecasting for margin/n  #############
# find the appropiate differencing for each data set.  Take the one that has the most uniform
# mean and standard deviation throughout the time series.  If there isn't one with relatively 
# uniform mean & SD, the results should be taken with some skeptisism.
# the differencing level becomes the 2nd number in the arima parameter set (see below)

plot.ts(indata$ratio)

diff1 <- diff(indata$ratio, differences=1)
plot.ts(diff1)

diff2 <- diff(indata$ratio, differences=2)
plot.ts(diff2)

diff3 <- diff(indata$ratio, differences=3)
plot.ts(diff3)


# the auto correlation lag number becomes the 3rd number in the arima parameter set
# It is the number of significant (above or below the blue noise line) peaks
# the input is the raw data series or the difference series selected from the above step.  For example,
# if diff1 were selected then the acf functions using diff1 would be used and those using indata$ratio 
# or others would be ignored.
acf(indata$ratio, lag.max=20)
acf(indata$ratio, lag.max=20, plot = FALSE)

acf(diff1, lag.max=20)
acf(diff1, lag.max=20, plot = FALSE)


# the partial auto correlation lag number becomes the 1st number in the arima parameter set
# It is the number of significant (above or below the blue noise line) peaks
# the input is the raw data series or the difference series selected from the above step
# Note that if the initial (low lag count) peaks are below the blue noise line and one peak happens
# to pop up above (or below) it, then it is probably just noise and should be ignored.  The lag number
# in this case would still be zero.
pacf(indata$ratio, lag.max=20)
pacf(indata$ratio, lag.max=20, plot = FALSE)

pacf(diff1, lag.max=20)
pacf(diff1, lag.max=20, plot = FALSE)


## here you fit the arima model.  The order input field is the parameter set determined above.
# you will need to edit this to the parameter set you determined to be appropriate for your new data set.
ratiotModel <- arima(indata$ratio, order=c(0,1,1)) # fit an ARIMA(0,1,1) model
ratiotModel #informational, don't need to see this
ratioForecast <- forecast.Arima(ratiotModel, h=6) #time series predictions
ratioForecast
plot.forecast(ratioForecast, main="Margin / n", xlab = "Months", ylab = "Ratio") #plot of the time series and its predictions
accuracy(ratioForecast)



###### forecasting for count (n)  #############
plot.ts(indata$count)

diff1 <- diff(indata$count, differences=1) 
plot.ts(diff1)

diff2 <- diff(indata$count, differences=2) 
plot.ts(diff2)

acf(indata$count, lag.max=20)
acf(indata$count, lag.max=20, plot = FALSE)

acf(diff1, lag.max=20)
acf(diff1, lag.max=20, plot = FALSE)

pacf(indata$count, lag.max=20)
pacf(indata$count, lag.max=20, plot = FALSE)

pacf(diff1, lag.max=20)
pacf(diff1, lag.max=20, plot = FALSE)

countModel <- arima(indata$count, order=c(1,1,2)) # fit an ARIMA(1,1,2) model
countModel
countForecast <- forecast.Arima(countModel, h=6)
countForecast
plot.forecast(countForecast, main="Count (n)", xlab = "Months", ylab = "Quantity Sold")
accuracy(countForecast)



###### forecasting for count (n)  #############
indata$grossprofit
plot.ts(indata$grossprofit)

diff1 <- diff(indata$grossprofit, differences=1)  #use diff1
plot.ts(diff1)

acf(indata$grossprofit, lag.max=20)
acf(indata$grossprofit, lag.max=20, plot = FALSE)

pacf(indata$grossprofit, lag.max=20)
pacf(indata$grossprofit, lag.max=20, plot = FALSE)

acf(diff1, lag.max=20)
acf(diff1, lag.max=20, plot = FALSE)

pacf(diff1, lag.max=20)
pacf(diff1, lag.max=20, plot = FALSE)

profitModel <- arima(indata$grossprofit, order=c(0,0,1)) # fit an ARIMA(0,0,1) model
profitModel
profitForecast <- forecast.Arima(profitModel, h=6)
profitForecast
plot.forecast(profitForecast, main="Margin", xlab = "Months", ylab = "Margin")

accuracy(profitForecast)




