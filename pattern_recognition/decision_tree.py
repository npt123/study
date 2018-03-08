from math import *
import matplotlib.pyplot as plt


class DecisionTree:
    root = {}
    classList = []
    labels = []
    trains = []

    def __init__(self, dataSet, labels, max_depth, min_size):
        self.trains = dataSet
        self.labels = labels
        self.max_depth = max_depth
        self.min_size = min_size

#     def calcShannonEnt(self, groups):
#         shannonEnt = 0.0
#         for classValue in self.classList:
#             for group in groups:
#                 size = len(group)
#                 if size == 0:
#                     continue
#                 else:
#                     prob = [row[-1] for row in group].count(classValue) / float(size)
#                     shannonEnt += prob * log(prob)
#         return shannonEnt

    def calcGiniEnt(self, groups):
        giniEnt = 0.0
        for classValue in self.classList:
            for group in groups:
                size = len(group)
                if size == 0:
                    continue
                else:
                    prob = [row[-1] for row in group].count(classValue) / float(size)
                    giniEnt += prob * (1 - prob)
        return giniEnt

    def splitDataSet(self, dataSet, axis, value):
        left, right = [], []
        for row in dataSet:
            if row[axis] < value:
                left.append(row)
            else:
                right.append(row)
        return left, right

    def chooseBestFeatureToSplit(self, dataSet):
        numFeatures = len(dataSet[0]) - 1
        bestFeature, bestValue, bestGini, bestSplit = 999, 999, 999, None
        for i in range(numFeatures):
            for row in dataSet:
                groups = self.splitDataSet(dataSet, i, row[i])
                gini = self.calcGiniEnt(groups)
                if gini < bestGini:
                    bestFeature, bestValue, bestGini, bestSplit = i, row[i], gini, groups
        return {'axis': bestFeature, 'value': bestValue, 'groups': groups}

    def toTerminal(self, group):
        results = [row[-1] for row in group]
        return max(set(results), key=results.count)

    def split(self, node, depth):
        left, right = node['groups']
        del(node['groups'])
        if not left or not right:
            node['left'] = node['right'] = self.toTerminal(left + right)
            return
        if depth >= self.max_depth:
            node['left'] = self.toTerminal(left)
            node['right'] = self.toTerminal(right)
            return
        if len(left) <= self.min_size:
            node['left'] = self.toTerminal(left)
        else:
            node['left'] = self.chooseBestFeatureToSplit(left)
            self.split(node['left'], depth+1)
        if len(right) <= self.min_size:
            node['right'] = self.toTerminal(right)
        else:
            node['right'] = self.chooseBestFeatureToSplit(right)
            self.split(node['right'], depth+1)

    def buildTree(self):
        for person in trains:
            if person[-1] not in self.classList:
                self.classList.append(person[-1])
        self.root = self.chooseBestFeatureToSplit(self.trains)
        self.split(self.root, 1)


trains = []


def get_data(file, gender):
    global trains
    try:
        data = open(file)
        for line in data.readlines():
            height, weight, shoe_size = line.split()
            trains.append([float(height), float(weight), float(shoe_size), gender])
    except IOError:
        return


def get_bank_data():
    global trains
    f = open('data/data_banknote_authentication.txt')
    for line in f.readlines():
        variance, skewness, curtosis, entropy, category = line.split(',')
        trains.append([float(variance), float(skewness), float(curtosis), float(entropy), int(category)])


def main():
    template = input('Choose the template:')
    if template == 'gender':
        get_data('data/boy.txt', 'boy')
        get_data('data/boy82.txt', 'boy')
        get_data('data/boy83.txt', 'boy')
        get_data('data/boynew.txt', 'boy')
        get_data('data/girl.txt', 'girl')
        get_data('data/girl35.txt', 'girl')
        get_data('data/girl42.txt', 'girl')
        get_data('data/girlnew.txt', 'girl')
        labels = ['height', 'weight', 'shoe_size']
    elif template == 'banknote':
        get_bank_data()
        labels = ['variance', 'skewness', 'curtosis', 'entropy']
    tree = DecisionTree(trains, labels, 100, 20)
    tree.buildTree()
    print(tree.root)


if __name__ == '__main__':
    main()
