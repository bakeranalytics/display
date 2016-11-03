import pandas as pd
import numpy as np
import scipy
from sklearn.ensemble import RandomForestClassifier
from sklearn import svm
from sklearn import utils
from numbers import Number
from sklearn import preprocessing

data = pd.read_csv('C:\\Users\\Brian\\Desktop\\python\\test1\\testData.csv')

data = data.fillna(data.mean())
#print(data.head())

#### aggregate to find non-numeric values that must be replaced and summary stats ########
#names = data.groupby('PERFORMANCERATINGCODE').aggregate({'PREVPBCRATINGCODE': pd.Series.count})
#print(names)
#names = data.groupby('SERVICEINYEARSDOH').aggregate({'TENUREGRP': pd.Series.mean})
#stats = data.groupby('PERFORMANCERATINGCODE').agg([np.sum, np.mean, np.std, len])
#print(names, stats)

#replace N with text-7, then recast into int
data['PRIORPREVPBCRATINGCODE'] = data['PRIORPREVPBCRATINGCODE'].str.replace('N','7')
data['PRIORPREVPBCRATINGCODE'] = [int(x) for x in data['PRIORPREVPBCRATINGCODE']]
data['PREVPBCRATINGCODE'] = data['PREVPBCRATINGCODE'].str.replace('N','7')
data['PREVPBCRATINGCODE'] = [int(x) for x in data['PREVPBCRATINGCODE']]
data['PERFORMANCERATINGCODE'] = data['PERFORMANCERATINGCODE'].str.replace('N','7')
data['PERFORMANCERATINGCODE'] = [int(x) for x in data['PERFORMANCERATINGCODE']]

data = data.sample(frac=1, random_state=3).reset_index(drop=True)  #shuffle rows
data['stillHere'] = pd.isnull(data.LOSSDATE) * 1  # *1 converts T/F into 1/0
target = data.stillHere
data = data.drop('stillHere', axis=1)
data = data.drop('LOSSDATE', axis=1)

trainData = data.iloc[:300000,:]
trainTarget = target.iloc[:300000]
testData = data.iloc[300000:,:]
testTarget = target.iloc[300000:]


print("td",len(trainData))
print("tt",len(trainTarget))
print(len(testData))
print(len(testTarget))
print("___")

#Standardization: remove mean value of each feature, then scale by dividing by standard deviation.
scaler = preprocessing.StandardScaler().fit(trainData)
trainData = pd.DataFrame(scaler.transform(trainData))
testData = pd.DataFrame(scaler.transform(testData))

trainTarget = trainTarget.ravel()
testTarget = testTarget.ravel()

#model = svm.SVC(kernel = 'sigmoid', gamma=0.05, C = 0.2, probability ='true', random_state=314)
#model = svm.SVC() - bad -> try differnt kernel
#model.fit(trainData,trainTarget)

model = RandomForestClassifier()
model.fit(trainData, trainTarget)

preds = model.predict(testData)
confMatrix = pd.crosstab(testTarget, preds, rownames=['actual'], colnames=['preds'])
print(confMatrix)

#print(data)
print('done:')
