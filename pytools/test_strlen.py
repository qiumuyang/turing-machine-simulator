from tester import Testcase


class Test(Testcase):
    ResultBound = 128

    def rand_testcase(self) -> tuple:
        import random

        strlen = random.randint(1, self.ResultBound)

        return ''.join(random.choice('ab') for _ in range(strlen)), bin(strlen)[2:]


if __name__ == '__main__':
    Test.parse_args()
