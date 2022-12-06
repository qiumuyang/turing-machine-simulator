from tester import Testcase


class Test(Testcase):
    UpperBound = 0xFFFF

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(0, self.UpperBound)
        y = random.randint(0, self.UpperBound)
        x_bin = bin(x)[2:]
        y_bin = bin(y)[2:]
        mul_bin = bin(x * y)[2:]
        return f'{x_bin}x{y_bin}', mul_bin


if __name__ == '__main__':
    Test.parse_args()
