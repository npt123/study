from bayes import Bayes
from fisher import Fisher
from kNN import kNN
import matplotlib.pyplot as plt

trains3 = []
trainshw = []
trainshs = []
trainsws = []


def get_data(file, gender):
    global trains3, trainshw, trainshs, trainsws
    if gender == 'boy':
        try:
            data = open(file)
            for line in data.readlines():
                height, weight, shoe_size = line.split()
                trains3.append([float(height), float(weight), float(shoe_size), 1])
                trainshw.append([float(height), float(weight), 1])
                trainshs.append([float(height), float(shoe_size), 1])
                trainsws.append([float(weight), float(shoe_size), 1])
        except IOError:
            return
    elif gender == 'girl':
        try:
            data = open(file)
            for line in data.readlines():
                height, weight, shoe_size = line.split()
                trains3.append([float(height), float(weight), float(shoe_size), 0])
                trainshw.append([float(height), float(weight), 0])
                trainshs.append([float(height), float(shoe_size), 0])
                trainsws.append([float(weight), float(shoe_size), 0])
        except IOError:
            return


def main():
    get_data('data/boy82.txt', 'boy')
    get_data('data/boy83.txt', 'boy')
    get_data('data/boynew.txt', 'boy')
    get_data('data/girl35.txt', 'girl')
    get_data('data/girl42.txt', 'girl')
    get_data('data/girlnew.txt', 'girl')
    test3 = []
    testhw = []
    tesths = []
    testws = []
    boys = open('data/boy.txt')
    girls = open('data/girl.txt')
    print("1) Bayes")
    print("2) Fisher")
    print("3) kNN")
    choice = input("Input the algorithm: ")
    if choice == '1':
        for line in boys.readlines():
            height, weight, shoe_size = line.split()
            test3.append([[float(height)], [float(weight)], [float(shoe_size)], 1])
            testhw.append([[float(height)], [float(weight)], 1])
            tesths.append([[float(height)], [float(shoe_size)], 1])
            testws.append([[float(weight)], [float(shoe_size)], 1])
        for line in girls.readlines():
            height, weight, shoe_size = line.split()
            test3.append([[float(height)], [float(weight)], [float(shoe_size)], 0])
            testhw.append([[float(height)], [float(weight)], 0])
            tesths.append([[float(height)], [float(shoe_size)], 0])
            testws.append([[float(weight)], [float(shoe_size)], 0])
        plt.xlim(0, 1.0)
        plt.ylim(0, 1.0)
        plt.plot([0, 1.0], [0, 1.0], color='red')
        b3 = Bayes(trains3)
        b3.paint(test3, 'b')
        bhw = Bayes(trainshw)
        bhw.paint(testhw, 'g')
        bhs = Bayes(trainshs)
        bhs.paint(tesths, 'r')
        bws = Bayes(trainsws)
        bws.paint(testws, 'y')
    elif choice == '2':
        for line in boys.readlines():
            height, weight, shoe_size = line.split()
            test3.append([[float(height)], [float(weight)], [float(shoe_size)], 1])
            testhw.append([[float(height)], [float(weight)], 1])
            tesths.append([[float(height)], [float(shoe_size)], 1])
            testws.append([[float(weight)], [float(shoe_size)], 1])
        for line in girls.readlines():
            height, weight, shoe_size = line.split()
            test3.append([[float(height)], [float(weight)], [float(shoe_size)], 0])
            testhw.append([[float(height)], [float(weight)], 0])
            tesths.append([[float(height)], [float(shoe_size)], 0])
            testws.append([[float(weight)], [float(shoe_size)], 0])
        f3 = Fisher(trains3)
        fhw = Fisher(trainshw)
        fhs = Fisher(trainshs)
        fws = Fisher(trainsws)
        print("1) ROC")
        print("2) Line")
        choice = input("Choose: ")
        if choice == '1':
            f3.paint(test3, 'b')
            fhw.paint(testhw, 'g')
            fhs.paint(tesths, 'r')
            fws.paint(testws, 'y')
        elif choice == '2':
            print("1) Height And Weight")
            print("2) Height And Shoe Size")
            print("3) Weight And Shoe Size")
            choice = input("Choose: ")
            if choice == '1':
                fhw.paint_line(testhw, 'c')
            elif choice == '2':
                fhs.paint_line(tesths, 'r')
            elif choice == '3':
                fws.paint_line(testws, 'y')
    elif choice == '3':
        for line in boys.readlines():
            height, weight, shoe_size = line.split()
            test3.append([float(height), float(weight), float(shoe_size), 1])
            testhw.append([float(height), float(weight), 1])
            tesths.append([float(height), float(shoe_size), 1])
            testws.append([float(weight), float(shoe_size), 1])
        for line in girls.readlines():
            height, weight, shoe_size = line.split()
            test3.append([float(height), float(weight), float(shoe_size), 0])
            testhw.append([float(height), float(weight), 0])
            tesths.append([float(height), float(shoe_size), 0])
            testws.append([float(weight), float(shoe_size), 0])
        choice = input("Input K:")
        if choice == '1':
            k1 = kNN(trainshs, 1)
            print(k1.test(tesths))
            k1.paint()
        elif choice == '3':
            k3 = kNN(trainshs, 3)
            print(k3.test(tesths))
            k3.paint()
        elif choice == '5':
            k5 = kNN(trainshs, 5)
            print(k5.test(tesths))
            k5.paint()
    plt.show()


if __name__ == '__main__':
    main()
