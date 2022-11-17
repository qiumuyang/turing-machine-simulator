from tester import Testcase


class Test(Testcase):
    UpperBound = 0xFFFFFFFFFFFF

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(0, self.UpperBound)
        y = random.randint(0, self.UpperBound)
        x_bin = bin(x)[2:]
        y_bin = bin(y)[2:]
        add_bin = bin(x + y)[2:]
        return f'{x_bin}#{y_bin}', add_bin


Test.parse_args()
