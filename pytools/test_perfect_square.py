from tester import Testcase


class Test(Testcase):
    UpperBound = 128

    def rand_testcase(self) -> tuple:
        import random

        s = '1' * random.randint(1, self.UpperBound)
        return s, int(len(s) ** 0.5) ** 2 == len(s)


if __name__ == '__main__':
    Test.parse_args()
