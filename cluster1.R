memory.limit(96000)
library(cluster)
setwd("C:\\Users\\Brian\\Documents\\R\\RSandbox")
data <- read.csv("INPUTFILE", sep=";", stringsAsFactors=FALSE)

data <- within(data, rm(email, encrypted_password, reset_password_token, reset_password_sent_at,
      confirmation_sent_at, remember_created_at, confirmation_token, confirmed_at, unconfirmed_email, created_at,
      updated_at, latitude, longitude, email_notifications, apns_notifications, avatar_id, skill,
      facebook_token, pusher_channel, badges, first_name, last_name, username, geo_coordinates_from_device,
      last_ip_address, invited_friends, time_zone_name, paypal_email, cash_balance, 
      orders_count, loc_country_code, loc_state_name, gender, role, currency_code))
#head(data)


##########  set facebook membership to binary flag   ##################
data$facebook = 1
for (i in 1:dim(data)[1]){data$facebook[i] <- ifelse(data$facebook_id[i] == "",0,1)}

data <- within(data, rm(id, facebook_id))
colnames(data)
#table(data$is_facebook_login, data$facebook)  #not perfect correlation


####  fix empty country names  -> XX   ####
y <- which(is.na(data$country_code)==TRUE)       
data$country_code[y] <- "XX"     
#table(data$country_code, data$facebook)


######  t/f -> 1/0  ######
data$interested_in_real_trading[which(data$interested_in_real_trading =="t")] <- 1
data$interested_in_real_trading[which(data$interested_in_real_trading =="f")] <- 0
data$is_facebook_login[which(data$is_facebook_login =="t")] <- 1
data$is_facebook_login[which(data$is_facebook_login =="f")] <- 0

#table(data$interested_in_real_trading, data$is_facebook_login)



#######   explore other columns, check for nulls   ###
colnames(data)[colSums(is.na(data)) > 0]#columns with na
aggregate(x = data$positions_count, by = list(data$positions_count), FUN = length)

plot(data$completed_trade_academy_steps, data$open_app_session_count)
plot(data$completed_trade_academy_steps)
hist(data$completed_trade_academy_steps,main="Academy Steps",xlab="Steps",border="blue",col="green",xlim=c(0,10),las=1,breaks=10)
hist(data$open_app_session_count,main="Open Sessions",xlab="Sessions",border="blue",col="green",xlim=c(0,500),las=1,breaks=1000)



#######  cluster   ####
data <- within(data, rm(country_code))#can't cluster on country because its not a number
colnames(data)

temp1 <- data.frame(data$open_app_session_count, data$completed_trade_academy_steps,  data$positions_count,  
         data$country_code)
temp2 <- head(temp1,250)
temp3 <- data.frame(data$open_app_session_count, data$positions_count)
temp4 <- data.frame(data$open_app_session_count, data$completed_trade_academy_steps,  data$positions_count)
temp5 <- head(temp4,250)

plot(temp1)

#pamx <- pam(temp5, 2, diss=FALSE, stand=TRUE, do.swap = FALSE, keep.diss=FALSE, keep.data=FALSE)
#pamx 
#summary(pamx)
#pamx$clustering
#plot(pamx)
#plot (temp5, col = pamx$clustering)

#dv <- diana(temp1, metric = "manhattan", stand = TRUE)
#print(dv)
#plot(dv)

#ai <- agnes(temp3)
#plot(temp3)
#meplot(ai)

#data2 <- within(data, rm(interested_in_real_trading))#try this - no change

clr <- clara(data, 3, stand=TRUE, samples=100, sampsize=20, correct.d=FALSE)
plot (clr, col = clr$clustering)
plot(clr, ylim=c(-5,5), xlim=c(-3,3))
clr$medoids
plot (data, col = clr$clustering)
plot (data, col = clr$clustering, ylim=c(-4,10), xlim=c(-160,50))

aggregate(x = clr$cluster, by = list(clr$cluster), FUN = length)


############  invesetigate clusters   ######################
cluster1 <- data[which(clr$clustering == 1),]
cluster2 <- data[which(clr$clustering == 2),]


plot(cluster1$positions_count, cluster2$positions_count[1:dim(cluster1)[1]])
plot(cluster1$positions_count, cluster1$open_app_session_count)
plot(cluster2$positions_count, cluster2$open_app_session_count)
plot(cluster1$positions_count)
plot(cluster2$positions_count)


################   split into groups using 10 session threshold  ##################
short <- data[(data$open_app_session_count <= 10),]
long <- data[(data$open_app_session_count > 10),]

aggregate(x = short$facebook, by = list(short$facebook), FUN = length)
aggregate(x = long$facebook, by = list(long$facebook), FUN = length)

plot(short$completed_trade_academy_steps, short$reset_portfolio_count)
hist(data$completed_trade_academy_steps,main="Academy Steps",xlab="Steps",border="blue",col="green",xlim=c(0,10),las=1,breaks=10)

table(data$contest_first_rank_count, data$completed_trade_academy_steps)
table(short$contest_first_rank_count)
table(long$contest_first_rank_count)
table(short$contest_second_rank_count)
table(long$contest_second_rank_count)
table(short$i_follow_count, short$followers_count)
table(long$i_follow_count, long$followers_count)

## remove all possible fields to get lowest dimension (4) for clustering
short <- within(short, rm(country_code, interested_in_real_trading))#country not number, intrest field too sparsely populated
long <- within(long, rm(country_code, interested_in_real_trading))
colnames(short)
short <- within(short, rm(positions_count, open_app_session_count))#drop fields already accounted for in group deffinition
long <- within(long, rm(positions_count, open_app_session_count))
short <- within(short, rm(facebook, followers_count))#no differnce between groups for facebook, drop one follow field
long <- within(long, rm(facebook, followers_count))
short <- within(short, rm(contest_second_rank_count, contest_third_rank_count))#one competition field as surogate for all three
long <- within(long, rm(contest_second_rank_count, contest_third_rank_count))

dd <- within(data, rm(interested_in_real_trading, positions_count, open_app_session_count, facebook, followers_count,
                        contest_second_rank_count, contest_third_rank_count, is_facebook_login))

#########  cluster  ############
clrs <- clara(dd, 3, stand=TRUE, samples=100, sampsize=20)
plot (clrs, col = clrs$clustering)
clrs$medoids

cluster1_dd <- data[which(clrs$clustering == 1),]
cluster2_dd <- data[which(clrs$clustering == 2),]
cluster3_dd <- data[which(clrs$clustering == 3),]
mean(cluster1_dd$positions_count)
mean(cluster2_dd$positions_count)
mean(cluster3_dd$positions_count)

clr <- clara(long, 3, stand=TRUE, samples=100, sampsize=20)
plot (clr, col = clr$clustering)


plot (clr, col = clr$clustering, ylim=c(0,1000), xlim=c(0,1000))
plot(clr)
plot(clr, ylim=c(-5,5), xlim=c(-3,3))

