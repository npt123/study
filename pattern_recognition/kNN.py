import numpy as np
import matplotlib.pyplot as plt


class kNN:

    def __init__(self, trains, k):
        self.trains = np.array(trains)
        self.k = k

    def judge(self, test):
        difference = np.tile(test, (np.shape(self.trains)[0], 1)) - self.trains[:, :-1]
        difference = difference ** 2
        distance = difference.sum(1)
        distance = distance ** 0.5
        sort_fiddidx = distance.argsort()

        vote = {}
        for i in range(self.k):
            ith_label = self.trains[:, -1][sort_fiddidx[i]]
            vote[ith_label] = vote.get(ith_label, 0) + 1
        sorted_vote = sorted(vote.items(), key=lambda x: x[1], reverse=True)
        return int(sorted_vote[0][0])

    def autoNorm(self, dataSet):
        minVals = dataSet.min(0)
        maxVals = dataSet.max(0)
        ranges = maxVals - minVals
        normDataSet = np.zeros(np.shape(dataSet))
        m = dataSet.shape[0]
        normDataSet = dataSet - np.tile(minVals, (m, 1))
        normDataSet = normDataSet / np.tile(ranges, (m, 1))
        return normDataSet

    def test(self, test):
        right = 0
        false = 0
        for person in test:
            if self.judge(person[:-1]) == person[-1]:
                right += 1
            else:
                false += 1
        return right / (right + false)

    def paint(self):
        x = np.arange(150, 200, 0.1)
        y = np.arange(35, 45, 0.1)

        bx = []
        by = []
        gx = []
        gy = []

        for i in range(len(x)):
            for j in range(len(y)):
                result = self.judge([x[i], y[j]])
                if result == 1:
                    bx.append(x[i])
                    by.append(y[j])
                else:
                    gx.append(x[i])
                    gy.append(y[j])

        plt.plot(bx, by, 'b.', label="男生")
        plt.plot(gx, gy, 'g.', label="女生")
        plt.xlabel('Height')
        plt.ylabel('Weight')
