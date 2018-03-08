from math import *
import numpy as np
import matplotlib.pyplot as plt


class Fisher:
    pb = 0.0
    pg = 0.0

    def __init__(self, trains):
        self.trains = trains
        self.feature_num = len(trains[0]) - 1
        self.m1 = np.full((self.feature_num, 1), 0.0)
        self.m2 = np.full((self.feature_num, 1), 0.0)
        self.s1 = np.full((self.feature_num, self.feature_num), 0.0)
        self.s2 = np.full((self.feature_num, self.feature_num), 0.0)
        self.sw = np.full((self.feature_num, self.feature_num), 0.0)
        self.sb = np.full((self.feature_num, self.feature_num), 0.0)
        self.w = np.full((self.feature_num, 1), 0.0)
        self.train()

    def output(self):
        return self.m1, self.m2, self.s1, self.s2, self.sw, self.sb, self.w, self.pb, self.pg

    def train(self):
        boys_num = 0
        girls_num = 0
        for person in self.trains:
            if person[-1] == 1:
                boys_num += 1
                for i in range(self.feature_num):
                    self.m1[i] += person[i]
            elif person[-1] == 0:
                girls_num += 1
                for i in range(self.feature_num):
                    self.m2[i] += person[i]
        self.m1 = self.m1 / boys_num
        self.m2 = self.m2 / girls_num
        self.pb = boys_num / self.trains.__len__()
        self.pg = 1 - self.pb
        for person in self.trains:
            if person[-1] == 1:
                person_data = np.array([[person[i]] for i in range(self.feature_num)])
                self.s1 += np.dot((person_data - self.m1), np.transpose(person_data - self.m1))
            elif person[-1] == 0:
                person_data = np.array([[person[i]] for i in range(self.feature_num)])
                self.s2 += np.dot((person_data - self.m2), np.transpose(person_data - self.m2))
        self.s1 = self.s1 / boys_num
        self.s2 = self.s2 / girls_num
        self.sw = self.pb * self.s1 + self.pg * self.s2
        self.sb = np.dot((self.m1 - self.m2), np.transpose((self.m1 - self.m2)))
        self.w = np.dot(np.linalg.inv(self.sw), (self.m1 - self.m2))

    def fisher(self, person):
        gx = np.dot(np.transpose(self.w), (person - 0.5 * (self.m1 + self.m2))) - log(self.pg / self.pb)
        return gx[0][0]

    def judge(self, person):
        gx = np.dot(np.transpose(self.w), (person - 0.5 * (self.m1 + self.m2))) - log(self.pg/self.pb)
        if gx[0][0] > 0:
            return 1
        else:
            return 0

    def ROC(self, data):
        TPR = [0.0]
        FPR = [0.0]
        for j in range(data.__len__()):
            TP = 0
            FP = 0
            FN = 0
            TN = 0
            for i in range(j):
                if data[i][0] == 1 and data[i][1] == 1:
                    TP += 1
                if data[i][0] == 0 and data[i][1] == 0:
                    TN += 1
                if data[i][0] == 0 and data[i][1] == 1:
                    FP += 1
                if data[i][0] == 1 and data[i][1] == 0:
                    FN += 1
            if TP == 0:
                TPR.append(0)
            else:
                TPR.append(TP / (TP + FN))
            if FP == 0:
                FPR.append(0)
            else:
                FPR.append(FP / (FP + TN))
        TPR.append(1.0)
        FPR.append(1.0)
        return TPR, FPR

    def paint(self, data, colors):
        test = []
        for person in data:
            person_data = np.array([[person[i] for i in range(self.feature_num)]])
            test.append([person[-1], self.judge(person_data), self.fisher(person_data)])
        test = sorted(test, key=lambda x: x[-1])
        tpr, fpr = self.ROC(test)
        plt.plot(fpr, tpr, color=colors)
        # plt.plot(fpr, tpr, 'ro')

    def paint_line(self, data, color):
        min_x = 999
        max_x = 0
        for person in data:
            if person[-1] == 1:
                plt.plot(person[0], person[1], 'bo')
                if min_x > person[0][0]:
                    min_x = person[0][0]
                if max_x < person[0][0]:
                    max_x = person[0][0]
            elif person[-1] == 0:
                plt.plot(person[0], person[1], 'go')
                if min_x > person[0][0]:
                    min_x = person[0][0]
                if max_x < person[0][0]:
                    max_x = person[0][0]

        def fisher_line(x):
            return (-self.w[0][0] * x + 0.5 * self.w[0][0] * (self.m1[0][0] + self.m2[0][0]) +
                    log(self.pg/self.pb)) / self.w[1][0] + 0.5 * (self.m1[1][0] + self.m2[1][0])
        x = [min_x, max_x]
        y = [fisher_line(x[0]), fisher_line(x[1])]
        plt.plot(x, y, color)
