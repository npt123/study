from math import *
import numpy as np
import matplotlib.pylab as plt


class Bayes:
    obi = 0.0
    ogi = 0.0
    pb = 0.0
    pg = 0.0

    def __init__(self, trains):
        self.trains = trains
        self.feature_num = len(trains[0]) - 1
        self.ub = np.full((self.feature_num, 1), 0.0)
        self.ug = np.full((self.feature_num, 1), 0.0)
        self.ob = np.eye(self.feature_num, self.feature_num, dtype=float)
        self.og = np.eye(self.feature_num, self.feature_num, dtype=float)
        self.train()

    def output(self):
        return self.ub, self.ug, self.ob, self.og, self.obi, self.ogi, self.pb, self.pg

    def train(self):
        boys_num = 0
        girls_num = 0
        for person in self.trains:
            if person[-1] == 1:
                boys_num += 1
                for i in range(self.feature_num):
                    self.ub[i][0] += person[i]
            elif person[-1] == 0:
                girls_num += 1
                for i in range(self.feature_num):
                    self.ug[i][0] += person[i]
        self.ub = self.ub / boys_num
        self.ug = self.ug / girls_num
        for person in self.trains:
            if person[-1] == 1:
                for i in range(self.feature_num):
                    self.ob[i][i] += (person[i] - self.ub[i][0]) ** 2
            elif person[-1] == 0:
                for i in range(self.feature_num):
                    self.og[i][i] += (person[i] - self.ub[i][0]) ** 2
        self.ob = self.ob / (boys_num - 1)
        self.og = self.og / (girls_num - 1)
        self.pb = boys_num / (boys_num + girls_num)
        self.pg = 1 - self.pb
        self.obi = np.linalg.inv(self.ob)
        self.ogi = np.linalg.inv(self.og)

    def bayes(self, person):
        gb = -0.5 * np.dot(np.dot(np.transpose(person - self.ub), self.obi), (person - self.ub))\
             - 1.5 * log(2 * pi) - 0.5 * log(
            np.linalg.det(self.ob)) + log(self.pb)
        gg = -0.5 * np.dot(np.dot(np.transpose(person - self.ug), self.obi), (person - self.ug))\
             - 1.5 * log(2 * pi) - 0.5 * log(
            np.linalg.det(self.og)) + log(self.pb)
        return gb[0][0] / (gb[0][0] + gg[0][0])

    def judge(self, person):
        gx = -0.5 * (np.dot(np.dot(np.transpose(person - self.ub), self.obi), (person - self.ub)) - np.dot(np.dot(
            np.transpose(person - self.ug), self.ogi), (person - self.ug))) - 0.5 * log(
            np.linalg.det(self.ob) / np.linalg.det(self.og)) + log(self.pb / self.pg)
        if gx[0][0] < 0:
            return 0
        else:
            return 1

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

    def paint(self, data, color):
        test = []
        for person in data:
            person_data = np.array([[person[i][0]] for i in range(self.feature_num)])
            test.append([person[-1], self.judge(person_data), self.bayes(person_data)])
        test = sorted(test, key=lambda x: x[-1], reverse=True)
        tpr, fpr = self.ROC(test)
        plt.plot(fpr, tpr, color)
        # plt.plot(fpr, tpr, 'ro')

    def paint_line(self, data, color):
        min_x = 999
        max_x = 0
        for person in data:
            if person[-1] == 1:
                plt.plot(person[0], person[1], 'bo')
                if min_x > person[0]:
                    min_x = person[0]
                if max_x < person[0]:
                    max_x = person[0]
            elif person[-1] == 0:
                plt.plot(person[0], person[1], 'go')
                if min_x > person[0]:
                    min_x = person[0]
                if max_x < person[0]:
                    max_x = person[0]

        def bayes_line(x):
            return
        x = []
        y = []
        for i in range(min_x-10, max_x+10):
            x.append(i)
            y.append(bayes_line(x[i]))
        plt.plot(x, y, color)
