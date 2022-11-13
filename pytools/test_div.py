from tester import Testcase


class Test(Testcase):
    def rand_testcase(self) -> tuple:
        import random
        import math

        x = random.randint(0, 0xFFFFFFFFF)
        y = math.ceil(x / 2)
        input_ = bin(x)[2:]
        expect = bin(y)[2:]
        return input_, expect


Test.parse_args()
