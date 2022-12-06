from tester import Testcase


class Test(Testcase):
    UpperBound = 512

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(1, self.UpperBound)
        y = random.randint(1, self.UpperBound)
        z1 = random.randint(1, self.UpperBound)
        z2 = random.randint(1, self.UpperBound)
        input_ = '0' * z1 + '1' * x + '0' * z2 + '1' * y
        output = '0' * z1 + 'x' * x + '0' * z2 + '1' * (x + y)
        return input_, output


if __name__ == '__main__':
    Test.parse_args()
