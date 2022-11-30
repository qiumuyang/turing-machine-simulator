from tester import Testcase


class Test(Testcase):
    UpperBound = 100

    def rand_testcase(self) -> tuple:
        import random

        s = ''.join(
            str(random.randint(0, 1)) for _ in range(random.randint(1, self.UpperBound))
        )
        shift = s[-1] + s[:-1]
        return s, shift


if __name__ == '__main__':
    Test.parse_args()
