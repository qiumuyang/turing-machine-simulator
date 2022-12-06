from tester import Testcase


class Test(Testcase):
    UpperBound = 1024

    def rand_testcase(self) -> tuple:
        import random

        x = random.randint(0, self.UpperBound)
        input_ = bin(x)[2:]
        output = str(x)
        return input_, output


if __name__ == '__main__':
    Test.parse_args()
