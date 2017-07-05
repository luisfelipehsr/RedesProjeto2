import random
import sys

def main():
    fparameters = open("carParameters", "w")
    cars = int(sys.argv[1])
    for i in range(cars):
        if i < cars/2:
            x = random.randrange(-300, 301, 20)
            if x < 0:
                sentido = 1
            else:
                sentido = -1
            y = 0
            direction = 0
        else:
            x = 0
            y = random.randrange(-300, 301, 20)
            if y < 0:
                sentido = 1
            else:
                sentido = -1
            direction = 1

        app = random.randrange(0, 2, 1)
        url = random.randrange(0, 2, 1)
        vel = random.randrange(10, 21, 5)
        size = random.randrange(3, 6, 1)
        if sys.argv[2] == '1':
            reckless = random.randrange(0, 2, 1)
        else:
            reckless = 0

        car = "{:4}{:4}{:4}{:4}{:4}{:4}{:4}{:4}{:4}\n".format(
            x, y, vel, direction, sentido, size, app, url, reckless)
        fparameters.write(car)
    fparameters.close()

if __name__ == "__main__":
    main()