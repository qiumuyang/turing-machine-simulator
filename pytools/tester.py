import subprocess
import time


class Testcase:
    def __init__(self, executable: str, tm_file: str):
        self.executable = executable
        self.tm_file = tm_file

    def rand_testcase(self) -> tuple:
        raise NotImplementedError

    def test_many(self, n: int) -> None:
        tik = time.perf_counter()
        for _ in range(n):
            input_, expect = self.rand_testcase()
            self.test(input_, expect)
        tok = time.perf_counter()
        print(f'{n} tests passed, {tok - tik:.3f} seconds elapsed')

    def test(self, input_: str, expect: str) -> None:
        proc = subprocess.Popen(
            [self.executable, self.tm_file, input_], stdout=subprocess.PIPE
        )
        if isinstance(expect, bool):
            expect = 'true' if expect else 'false'
        elif not isinstance(expect, str):
            expect = str(expect)
        output = proc.stdout.read().decode('ascii').strip()
        if output != expect:
            print('input: ', input_)
            print('expect:', repr(expect))
            print('output:', repr(output))
            raise AssertionError('test failed')

    @classmethod
    def parse_args(cls):
        import argparse

        parser = argparse.ArgumentParser()
        parser.add_argument('exec')
        parser.add_argument('tm')
        parser.add_argument('-n', type=int, default=1)
        args = parser.parse_args()
        cls(args.exec, args.tm).test_many(args.n)
